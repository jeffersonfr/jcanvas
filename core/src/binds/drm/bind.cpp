/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindowadapter.h"
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jenum.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#define DRM_DEVICE "/dev/dri/card0"

namespace jcanvas {

struct modeset_buf {
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint32_t size;
  uint32_t handle;
  uint8_t *map;
  uint32_t fb;
};

struct modeset_dev {
  struct modeset_dev *next;
  unsigned int front_buf;
  struct modeset_buf bufs[2];
  drmModeModeInfo mode;
  uint32_t conn;
  int crtc;
  drmModeCrtc *saved_crtc;
  bool cleanup;
  uint8_t r, g, b;
  bool r_up, g_up, b_up;
};

struct cursor_params_t {
  std::shared_ptr<Image> cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
struct modeset_buf;
/** \brief */
struct modeset_dev;

/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_handler = -1;
/** \brief */
static struct modeset_dev *sg_modeset_list = NULL;
/** \brief */
static bool sg_pending = false;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sg_jcanvas_cursors;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static struct cursor_params_t sg_cursor_params;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;

static int modeset_create_fb(struct modeset_buf *buf)
{
  struct drm_mode_create_dumb creq;
  struct drm_mode_destroy_dumb dreq;
  struct drm_mode_map_dumb mreq;
  int ret;

  memset(&creq, 0, sizeof(creq));
  creq.width = buf->width;
  creq.height = buf->height;
  creq.bpp = 32;
  ret = drmIoctl(sg_handler, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
  if (ret < 0) {
    fprintf(stderr, "cannot create dumb buffer (%d): %m\n", errno);
    return -errno;
  }
  buf->stride = creq.pitch;
  buf->size = creq.size;
  buf->handle = creq.handle;

  ret = drmModeAddFB(sg_handler, buf->width, buf->height, 24, 32, buf->stride, buf->handle, &buf->fb);
  if (ret) {
    fprintf(stderr, "cannot create framebuffer (%d): %m\n", errno);
    ret = -errno;
    goto err_destroy;
  }

  memset(&mreq, 0, sizeof(mreq));
  mreq.handle = buf->handle;
  ret = drmIoctl(sg_handler, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
  if (ret) {
    fprintf(stderr, "cannot map dumb buffer (%d): %m\n", errno);
    ret = -errno;
    goto err_fb;
  }

  buf->map = (uint8_t*)mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED, sg_handler, mreq.offset);
  if (buf->map == MAP_FAILED) {
    fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n", errno);
    ret = -errno;
    goto err_fb;
  }

  /* clear the framebuffer to 0 */
  memset(buf->map, 0, buf->size);

  return 0;

err_fb:
  drmModeRmFB(sg_handler, buf->fb);

err_destroy:
  memset(&dreq, 0, sizeof(dreq));
  dreq.handle = buf->handle;
  drmIoctl(sg_handler, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

  return ret;
}

static void modeset_destroy_fb(struct modeset_buf *buf)
{
  struct drm_mode_destroy_dumb dreq;

  munmap(buf->map, buf->size);

  drmModeRmFB(sg_handler, buf->fb);

  memset(&dreq, 0, sizeof(dreq));
  dreq.handle = buf->handle;
  drmIoctl(sg_handler, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}

static void modeset_cleanup()
{
  struct modeset_dev *iter;
  drmEventContext ev;

  memset(&ev, 0, sizeof(ev));
  ev.version = DRM_EVENT_CONTEXT_VERSION;
  ev.page_flip_handler = 0;

  while (sg_modeset_list) {
    iter = sg_modeset_list;
    sg_modeset_list = iter->next;

    drmModeSetCrtc(sg_handler, iter->saved_crtc->crtc_id, iter->saved_crtc->buffer_id, iter->saved_crtc->x, iter->saved_crtc->y, &iter->conn, 1, &iter->saved_crtc->mode);
    drmModeFreeCrtc(iter->saved_crtc);

    modeset_destroy_fb(&iter->bufs[1]);
    modeset_destroy_fb(&iter->bufs[0]);

    free(iter);
  }
}

static int modeset_find_crtc(drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
  drmModeEncoder *enc;
  int i, j;
  int32_t crtc;
  struct modeset_dev *iter;

  if (conn->encoder_id)
    enc = drmModeGetEncoder(sg_handler, conn->encoder_id);
  else
    enc = NULL;

  if (enc) {
    if (enc->crtc_id) {
      crtc = enc->crtc_id;
      for (iter = sg_modeset_list; iter; iter = iter->next) {
        if (iter->crtc == crtc) {
          crtc = -1;
          break;
        }
      }

      if (crtc >= 0) {
        drmModeFreeEncoder(enc);
        dev->crtc = crtc;
        return 0;
      }
    }

    drmModeFreeEncoder(enc);
  }

  for (i = 0; i < conn->count_encoders; ++i) {
    enc = drmModeGetEncoder(sg_handler, conn->encoders[i]);
    if (!enc) {
      fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n", i, conn->encoders[i], errno);

      continue;
    }

    for (j = 0; j < res->count_crtcs; ++j) {
      if (!(enc->possible_crtcs & (1 << j))) {
        continue;
      }

      crtc = res->crtcs[j];
      for (iter = sg_modeset_list; iter; iter = iter->next) {
        if (iter->crtc == crtc) {
          crtc = -1;
          break;
        }
      }

      if (crtc >= 0) {
        drmModeFreeEncoder(enc);
        dev->crtc = crtc;
        return 0;
      }
    }

    drmModeFreeEncoder(enc);
  }

  fprintf(stderr, "cannot find suitable CRTC for connector %u\n", conn->connector_id);

  return -ENOENT;
}

static int modeset_setup_dev(drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
  int ret;

  if (conn->connection != DRM_MODE_CONNECTED) {
    fprintf(stderr, "ignoring unused connector %u\n",
      conn->connector_id);
    return -ENOENT;
  }

  if (conn->count_modes == 0) {
    fprintf(stderr, "no valid mode for connector %u\n", conn->connector_id);
    return -EFAULT;
  }

  memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
  dev->bufs[0].width = conn->modes[0].hdisplay;
  dev->bufs[0].height = conn->modes[0].vdisplay;
  dev->bufs[1].width = conn->modes[0].hdisplay;
  dev->bufs[1].height = conn->modes[0].vdisplay;
  fprintf(stderr, "mode for connector %u is %ux%u\n", conn->connector_id, dev->bufs[0].width, dev->bufs[0].height);

  ret = modeset_find_crtc(res, conn, dev);

  if (ret) {
    fprintf(stderr, "no valid crtc for connector %u\n", conn->connector_id);
    return ret;
  }

  ret = modeset_create_fb(&dev->bufs[0]);
  if (ret) {
    fprintf(stderr, "cannot create framebuffer for connector %u\n", conn->connector_id);
    return ret;
  }

  ret = modeset_create_fb(&dev->bufs[1]);
  if (ret) {
    fprintf(stderr, "cannot create framebuffer for connector %u\n", conn->connector_id);
    modeset_destroy_fb(&dev->bufs[0]);
    return ret;
  }

  return 0;
}

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 0x1c:
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		case 0x0e:
			return jkeyevent_symbol_t::Backspace;
		case 0x0f:
			return jkeyevent_symbol_t::Tab;
		// case SDLK_CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case 0x01:
			return jkeyevent_symbol_t::Escape;
		case 0x39:
			return jkeyevent_symbol_t::Space;
		case 0x29:
			return jkeyevent_symbol_t::Aposthrophe;
		case 0x33:
			return jkeyevent_symbol_t::Comma;
		case 0x0c:
			return jkeyevent_symbol_t::Minus;
		case 0x34:  
			return jkeyevent_symbol_t::Period;
		case 0x59:
			return jkeyevent_symbol_t::Slash;
		case 0x0b:     
			return jkeyevent_symbol_t::Number0;
		case 0x02:
			return jkeyevent_symbol_t::Number1;
		case 0x03:
			return jkeyevent_symbol_t::Number2;
		case 0x04:
			return jkeyevent_symbol_t::Number3;
		case 0x05:
			return jkeyevent_symbol_t::Number4;
		case 0x06:
			return jkeyevent_symbol_t::Number5;
		case 0x07:
			return jkeyevent_symbol_t::Number6;
		case 0x08:
			return jkeyevent_symbol_t::Number7;
		case 0x09:
			return jkeyevent_symbol_t::Number8;
		case 0x0a:
			return jkeyevent_symbol_t::Number9;
		case 0x35:
			return jkeyevent_symbol_t::SemiColon;
		case 0x0d: 
			return jkeyevent_symbol_t::Equals;
		case 0x1b:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case 0x56:   
			return jkeyevent_symbol_t::BackSlash;
		case 0x2b:
			return jkeyevent_symbol_t::SquareBracketRight;
		case 0x1e:       
			return jkeyevent_symbol_t::a;
		case 0x30:
			return jkeyevent_symbol_t::b;
		case 0x2e:
			return jkeyevent_symbol_t::c;
		case 0x20:
			return jkeyevent_symbol_t::d;
		case 0x12:
			return jkeyevent_symbol_t::e;
		case 0x21:
			return jkeyevent_symbol_t::f;
		case 0x22:
			return jkeyevent_symbol_t::g;
		case 0x23:
			return jkeyevent_symbol_t::h;
		case 0x17:
			return jkeyevent_symbol_t::i;
		case 0x24:
			return jkeyevent_symbol_t::j;
		case 0x25:
			return jkeyevent_symbol_t::k;
		case 0x26:
			return jkeyevent_symbol_t::l;
		case 0x32:
			return jkeyevent_symbol_t::m;
		case 0x31:
			return jkeyevent_symbol_t::n;
		case 0x18:
			return jkeyevent_symbol_t::o;
		case 0x19:
			return jkeyevent_symbol_t::p;
		case 0x10:
			return jkeyevent_symbol_t::q;
		case 0x13:
			return jkeyevent_symbol_t::r;
		case 0x1f:
			return jkeyevent_symbol_t::s;
		case 0x14:
			return jkeyevent_symbol_t::t;
		case 0x16:
			return jkeyevent_symbol_t::u;
		case 0x2f:
			return jkeyevent_symbol_t::v;
		case 0x11:
			return jkeyevent_symbol_t::w;
		case 0x2d:
			return jkeyevent_symbol_t::x;
		case 0x15:
			return jkeyevent_symbol_t::y;
		case 0x2c:
			return jkeyevent_symbol_t::z;
		// case SDLK_BACKQUOTE:
		//	return jkeyevent_symbol_t::GraveAccent;
		case 0x28:  
			return jkeyevent_symbol_t::Tilde;
		case 0x6f:
			return jkeyevent_symbol_t::Delete;
		case 0x69:
			return jkeyevent_symbol_t::CursorLeft;
		case 0x6a:
			return jkeyevent_symbol_t::CursorRight;
		case 0x67:  
			return jkeyevent_symbol_t::CursorUp;
		case 0x6c:
			return jkeyevent_symbol_t::CursorDown;
		case 0x6e:  
			return jkeyevent_symbol_t::Insert;
		case 0x66:     
			return jkeyevent_symbol_t::Home;
		case 0x6b:
			return jkeyevent_symbol_t::End;
		case 0x68:
			return jkeyevent_symbol_t::PageUp;
		case 0x6d:
			return jkeyevent_symbol_t::PageDown;
		case 0x63:   
			return jkeyevent_symbol_t::Print;
		case 0x77:
			return jkeyevent_symbol_t::Pause;
		// case SDLK_RED:
		//	return jkeyevent_symbol_t::Red;
		// case SDLK_GREEN:
		//	return jkeyevent_symbol_t::Green;
		// case SDLK_YELLOW:
		//	return jkeyevent_symbol_t::Yellow;
		// case SDLK_BLUE:
		//	return jkeyevent_symbol_t::Blue;
		case 0x3b:
			return jkeyevent_symbol_t::F1;
		case 0x3c:
			return jkeyevent_symbol_t::F2;
		case 0x3d:
			return jkeyevent_symbol_t::F3;
		case 0x3e:
			return jkeyevent_symbol_t::F4;
		case 0x3f:
			return jkeyevent_symbol_t::F5;
		case 0x40:
			return jkeyevent_symbol_t::F6;
		case 0x41:    
			return jkeyevent_symbol_t::F7;
		case 0x42:
			return jkeyevent_symbol_t::F8;
		case 0x43:  
			return jkeyevent_symbol_t::F9;
		case 0x44: 
			return jkeyevent_symbol_t::F10;
		case 0x57:
			return jkeyevent_symbol_t::F11;
		case 0x58:
			return jkeyevent_symbol_t::F12;
		case 0x2a: // left
		case 0x36: // right
			return jkeyevent_symbol_t::Shift;
		case 0x1d: // left
		case 0x61: // right
			return jkeyevent_symbol_t::Control;
		case 0x38: // left
			return jkeyevent_symbol_t::Alt;
		case 0x64: 
		  return jkeyevent_symbol_t::AltGr;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jkeyevent_symbol_t::Meta;
		case 0x7d:
			return jkeyevent_symbol_t::Super;
		// case SDLK_HYPER:
		//	return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

void Application::Init(int argc, char **argv)
{
  struct modeset_dev *iter;
  struct modeset_buf *buf;
  uint64_t has_dumb;
  
  int fd = open(DRM_DEVICE, O_RDWR | O_CLOEXEC);

  if (fd < 0) {
    throw std::runtime_error("Unable to open drm device");
  }

  if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
    close(fd);

    throw std::runtime_error("Unable to get capabilities from device");
  }

  drmModeConnector *conn;
  struct modeset_dev *dev;
  drmModeRes *res = drmModeGetResources(fd);

  if (!res) {
    close(fd);

    throw std::runtime_error("Unable to retrieve DRM resources");
  }

  sg_handler = fd;

  for (int i = 0; i < res->count_connectors; ++i) {
    conn = drmModeGetConnector(fd, res->connectors[i]);

    if (!conn) {
      printf("cannot retrieve DRM connector %u:%u (%d): %m\n", i, res->connectors[i], errno);

      continue;
    }

    dev = (modeset_dev*)malloc(sizeof(*dev));
    memset(dev, 0, sizeof(*dev));
    dev->conn = conn->connector_id;

    int ret = modeset_setup_dev(res, conn, dev);

    if (ret) {
      if (ret != -ENOENT) {
        errno = -ret;

        printf("cannot setup device for connector %u:%u (%d): %m\n", i, res->connectors[i], errno);
      }

      drmModeFreeConnector(conn);
      free(dev);

      continue;
    }

    drmModeFreeConnector(conn);
    dev->next = sg_modeset_list;
    sg_modeset_list = dev;
  }

  drmModeFreeResources(res);

  for (iter = sg_modeset_list; iter; iter = iter->next) {
      iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
      buf = &iter->bufs[iter->front_buf];

      if (drmModeSetCrtc(fd, iter->crtc, buf->fb, 0, 0, &iter->conn, 1, &iter->mode)) {
          fprintf(stderr, "cannot set CRTC for connector %u (%d): %m\n", iter->conn, errno);
      }

      if (iter == sg_modeset_list) { // first element ...
        sg_screen.x = buf->width;
        sg_screen.y = buf->height;
  
        printf("screen size:: %ux%u\n", sg_screen.x, sg_screen.y);
      }
  }

  sg_quitting = false;

#define CURSOR_INIT(type, ix, iy, hotx, hoty) \
	t.cursor = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, {w, h}); \
	t.hot_x = hotx; \
	t.hot_y = hoty; \
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jpoint_t<int>{0, 0}); \
	sg_jcanvas_cursors[type] = t; \

  /*
	struct cursor_params_t t;
	int w = 30,
			h = 30;

	std::shared_ptr<Image> cursors = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(jcursor_style_t::Default, 0, 0, 8, 8);
	CURSOR_INIT(jcursor_style_t::Crosshair, 4, 3, 15, 15);
	CURSOR_INIT(jcursor_style_t::East, 4, 4, 22, 15);
	CURSOR_INIT(jcursor_style_t::West, 5, 4, 9, 15);
	CURSOR_INIT(jcursor_style_t::North, 6, 4, 15, 8);
	CURSOR_INIT(jcursor_style_t::South, 7, 4, 15, 22);
	CURSOR_INIT(jcursor_style_t::Hand, 1, 0, 15, 15);
	CURSOR_INIT(jcursor_style_t::Move, 8, 4, 15, 15);
	CURSOR_INIT(jcursor_style_t::Vertical, 2, 4, 15, 15);
	CURSOR_INIT(jcursor_style_t::Horizontal, 3, 4, 15, 15);
	CURSOR_INIT(jcursor_style_t::NorthWest, 8, 1, 10, 10);
	CURSOR_INIT(jcursor_style_t::NorthEast, 9, 1, 20, 10);
	CURSOR_INIT(jcursor_style_t::SouthWest, 6, 1, 10, 20);
	CURSOR_INIT(jcursor_style_t::SouthEast, 7, 1, 20, 20);
	CURSOR_INIT(jcursor_style_t::Text, 7, 0, 15, 15);
	CURSOR_INIT(jcursor_style_t::Wait, 8, 0, 15, 15);
	
	delete cursors;
  */
}

static void InternalPaint()
{
  if (sg_jcanvas_window == nullptr or sg_jcanvas_window->IsVisible() == false) {
    return;
  }

  jrect_t<int> 
    bounds = sg_jcanvas_window->GetBounds();

  struct modeset_dev
    *dev = sg_modeset_list; // INFO:: first monitor
  struct modeset_buf
    *buf = &dev->bufs[dev->front_buf ^ 1];

  cairo_surface_t
    *surface = cairo_image_surface_create_for_data(
        (uint8_t *)buf->map, CAIRO_FORMAT_RGB24, bounds.size.x, bounds.size.y, cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, bounds.size.x));
  
  BufferedImage
    buffer(surface);
  Graphics 
    *g = buffer.GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcomposite_flags_t::Src);

  sg_jcanvas_window->Paint(g);

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params.cursor, jpoint_t<int>{sg_mouse_x, sg_mouse_y});
  }
  
  g->Flush();
  
  dev->front_buf ^= 1;

  sg_pending = true;

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct input_event ev;
  int mouse_x = 0, mouse_y = 0;
  uint32_t last_mouse_state = 0x00;
  
  int 
    fdk = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  int 
    fdm = open("/dev/input/mice", O_RDONLY);

  if(fdm == -1) {   
    printf("Cannot open the mouse device\n");
  }   

  fcntl(fdm, F_SETFL, O_NONBLOCK);

  while (sg_quitting == false) {
    if (mouse_x != sg_mouse_x or mouse_y != sg_mouse_y) {
      mouse_x = sg_mouse_x;
      mouse_y = sg_mouse_y;

      if (sg_cursor_enabled == true) {
        sg_repaint.store(true);
      }
    }
    
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }
  
    Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());
    
    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

        if (ev.code == 0x2a) { // LSHIFT
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if (ev.code == 0x36) { // RSHIFT
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        /*
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
        } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        	mod = (jkeyevent_modifiers_t)(mod | jkeyevent_modifiers_t::Meta);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
        */
        }

        if (ev.value == 1 or ev.value == 2) {
          type = jkeyevent_type_t::Pressed;
        } else if (ev.value == 0) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      }
    }

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
      
      x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
      y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;

      jmouseevent_button_t button = jmouseevent_button_t::None;
      jmouseevent_type_t type = jmouseevent_type_t::Unknown;
      int mouse_z = 0;

      type = jmouseevent_type_t::Pressed;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jmouseevent_type_t::Moved;
      }

      sg_mouse_x = x;
      sg_mouse_y = y;

      if ((buttonMask & 0x01) == 0 && (last_mouse_state & 0x01)) {
        type = jmouseevent_type_t::Released;
      } else if ((buttonMask & 0x02) == 0 && (last_mouse_state & 0x02)) {
        type = jmouseevent_type_t::Released;
      } else if ((buttonMask & 0x04) == 0 && (last_mouse_state & 0x04)) {
        type = jmouseevent_type_t::Released;
      } 

      if ((buttonMask & 0x01) != (last_mouse_state & 0x01)) {
        button = jmouseevent_button_t::Button1;
      } else if ((buttonMask & 0x02) != (last_mouse_state & 0x02)) {
        button = jmouseevent_button_t::Button3;
      } else if ((buttonMask & 0x04) != (last_mouse_state & 0x04)) {
        button = jmouseevent_button_t::Button2;
      }

      last_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
    }
  }

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));

  sg_quitting = true;
 
  close(fdk);
  close(fdm);

  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
  // if (sg_surface != nullptr) {
  // 	throw std::runtime_error("Cannot create more than one window");
  // }

  // sg_jcanvas_icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

  sg_mouse_x = 0;
  sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  SetCursor(sg_jcanvas_cursors[jcursor_style_t::Default].cursor, sg_jcanvas_cursors[jcursor_style_t::Default].hot_x, sg_jcanvas_cursors[jcursor_style_t::Default].hot_y);
}

WindowAdapter::~WindowAdapter()
{
  if (sg_cursor_params.cursor != nullptr) {
    sg_cursor_params.cursor = nullptr;
  }

  modeset_cleanup();
  close(sg_handler);
  
  sg_jcanvas_window = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
}

void WindowAdapter::SetTitle(std::string title)
{
}

std::string WindowAdapter::GetTitle()
{
  return std::string();
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
	return 1.0;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
}

bool WindowAdapter::IsUndecorated()
{
  return true;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
}

jrect_t<int> WindowAdapter::GetBounds()
{
  return {
    0,
    0,
    sg_screen.x,
    sg_screen.y
  };
}
		
void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return false;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
  sg_mouse_x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = sg_mouse_x;
	p.y = sg_mouse_y;

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
}

bool WindowAdapter::IsVisible()
{
  return sg_quitting == false;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;
}

bool WindowAdapter::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
	sg_jcanvas_cursor = style;

	SetCursor(sg_jcanvas_cursors[style].cursor, sg_jcanvas_cursors[style].hot_x, sg_jcanvas_cursors[style].hot_y);
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

  if (sg_cursor_params.cursor != nullptr) {
    sg_cursor_params.cursor = nullptr;
  }

  sg_cursor_params.cursor = shape->Clone();

  sg_cursor_params.hot_x = hotx;
  sg_cursor_params.hot_y = hoty;
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(std::shared_ptr<Image> image)
{
  sg_jcanvas_icon = image;
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
