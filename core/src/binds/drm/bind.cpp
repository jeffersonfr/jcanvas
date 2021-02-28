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
  jcanvas::Image *cursor;
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
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static struct cursor_params_t sg_cursor_params;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;

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

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 0x1c:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case 0x0e:
			return jcanvas::JKS_BACKSPACE;
		case 0x0f:
			return jcanvas::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case 0x01:
			return jcanvas::JKS_ESCAPE;
		case 0x39:
			return jcanvas::JKS_SPACE;
		case 0x29:
			return jcanvas::JKS_APOSTROPHE;
		case 0x33:
			return jcanvas::JKS_COMMA;
		case 0x0c:
			return jcanvas::JKS_MINUS_SIGN;
		case 0x34:  
			return jcanvas::JKS_PERIOD;
		case 0x59:
			return jcanvas::JKS_SLASH;
		case 0x0b:     
			return jcanvas::JKS_0;
		case 0x02:
			return jcanvas::JKS_1;
		case 0x03:
			return jcanvas::JKS_2;
		case 0x04:
			return jcanvas::JKS_3;
		case 0x05:
			return jcanvas::JKS_4;
		case 0x06:
			return jcanvas::JKS_5;
		case 0x07:
			return jcanvas::JKS_6;
		case 0x08:
			return jcanvas::JKS_7;
		case 0x09:
			return jcanvas::JKS_8;
		case 0x0a:
			return jcanvas::JKS_9;
		case 0x35:
			return jcanvas::JKS_SEMICOLON;
		case 0x0d: 
			return jcanvas::JKS_EQUALS_SIGN;
		case 0x1b:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case 0x56:   
			return jcanvas::JKS_BACKSLASH;
		case 0x2b:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case 0x1e:       
			return jcanvas::JKS_a;
		case 0x30:
			return jcanvas::JKS_b;
		case 0x2e:
			return jcanvas::JKS_c;
		case 0x20:
			return jcanvas::JKS_d;
		case 0x12:
			return jcanvas::JKS_e;
		case 0x21:
			return jcanvas::JKS_f;
		case 0x22:
			return jcanvas::JKS_g;
		case 0x23:
			return jcanvas::JKS_h;
		case 0x17:
			return jcanvas::JKS_i;
		case 0x24:
			return jcanvas::JKS_j;
		case 0x25:
			return jcanvas::JKS_k;
		case 0x26:
			return jcanvas::JKS_l;
		case 0x32:
			return jcanvas::JKS_m;
		case 0x31:
			return jcanvas::JKS_n;
		case 0x18:
			return jcanvas::JKS_o;
		case 0x19:
			return jcanvas::JKS_p;
		case 0x10:
			return jcanvas::JKS_q;
		case 0x13:
			return jcanvas::JKS_r;
		case 0x1f:
			return jcanvas::JKS_s;
		case 0x14:
			return jcanvas::JKS_t;
		case 0x16:
			return jcanvas::JKS_u;
		case 0x2f:
			return jcanvas::JKS_v;
		case 0x11:
			return jcanvas::JKS_w;
		case 0x2d:
			return jcanvas::JKS_x;
		case 0x15:
			return jcanvas::JKS_y;
		case 0x2c:
			return jcanvas::JKS_z;
		// case SDLK_BACKQUOTE:
		//	return jcanvas::JKS_GRAVE_ACCENT;
		case 0x28:  
			return jcanvas::JKS_TILDE;
		case 0x6f:
			return jcanvas::JKS_DELETE;
		case 0x69:
			return jcanvas::JKS_CURSOR_LEFT;
		case 0x6a:
			return jcanvas::JKS_CURSOR_RIGHT;
		case 0x67:  
			return jcanvas::JKS_CURSOR_UP;
		case 0x6c:
			return jcanvas::JKS_CURSOR_DOWN;
		case 0x6e:  
			return jcanvas::JKS_INSERT;
		case 0x66:     
			return jcanvas::JKS_HOME;
		case 0x6b:
			return jcanvas::JKS_END;
		case 0x68:
			return jcanvas::JKS_PAGE_UP;
		case 0x6d:
			return jcanvas::JKS_PAGE_DOWN;
		case 0x63:   
			return jcanvas::JKS_PRINT;
		case 0x77:
			return jcanvas::JKS_PAUSE;
		// case SDLK_RED:
		//	return jcanvas::JKS_RED;
		// case SDLK_GREEN:
		//	return jcanvas::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jcanvas::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jcanvas::JKS_BLUE;
		case 0x3b:
			return jcanvas::JKS_F1;
		case 0x3c:
			return jcanvas::JKS_F2;
		case 0x3d:
			return jcanvas::JKS_F3;
		case 0x3e:
			return jcanvas::JKS_F4;
		case 0x3f:
			return jcanvas::JKS_F5;
		case 0x40:
			return jcanvas::JKS_F6;
		case 0x41:    
			return jcanvas::JKS_F7;
		case 0x42:
			return jcanvas::JKS_F8;
		case 0x43:  
			return jcanvas::JKS_F9;
		case 0x44: 
			return jcanvas::JKS_F10;
		case 0x57:
			return jcanvas::JKS_F11;
		case 0x58:
			return jcanvas::JKS_F12;
		case 0x2a: // left
		case 0x36: // right
			return jcanvas::JKS_SHIFT;
		case 0x1d: // left
		case 0x61: // right
			return jcanvas::JKS_CONTROL;
		case 0x38: // left
			return jcanvas::JKS_ALT;
		case 0x64: 
		  return jcanvas::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jcanvas::JKS_META;
		case 0x7d:
			return jcanvas::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
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
	t.cursor = new jcanvas::BufferedImage(JPF_ARGB, {w, h}); \
	t.hot_x = hotx; \
	t.hot_y = hoty; \
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jcanvas::jpoint_t<int>{0, 0}); \
	sg_jcanvas_cursors[type] = t; \

  /*
	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = new jcanvas::BufferedImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
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
  
  jcanvas::BufferedImage
    buffer(surface);
  jcanvas::Graphics 
    *g = buffer.GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcanvas::JCF_SRC);

  sg_jcanvas_window->Paint(g);

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params.cursor, jcanvas::jpoint_t<int>{sg_mouse_x, sg_mouse_y});
  }
  
  g->Flush();
  
  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  dev->front_buf ^= 1;

  sg_pending = true;

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
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
    
    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if (ev.code == 0x2a) { // LSHIFT
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if (ev.code == 0x36) { // RSHIFT
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        /*
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
        } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        */
        }

        type = jcanvas::JKT_UNKNOWN;

        if (ev.value == 1 or ev.value == 2) {
          type = jcanvas::JKT_PRESSED;
        } else if (ev.value == 0) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
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

      jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
      jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
      int mouse_z = 0;

      type = jcanvas::JMT_PRESSED;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jcanvas::JMT_MOVED;
      }

      sg_mouse_x = x;
      sg_mouse_y = y;

      if ((buttonMask & 0x01) == 0 && (last_mouse_state & 0x01)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (last_mouse_state & 0x02)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (last_mouse_state & 0x04)) {
        type = jcanvas::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (last_mouse_state & 0x01)) {
        button = jcanvas::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (last_mouse_state & 0x02)) {
        button = jcanvas::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (last_mouse_state & 0x04)) {
        button = jcanvas::JMB_BUTTON2;
      }

      last_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
    }
  }

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));

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

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
{
  // if (sg_surface != nullptr) {
  // 	throw std::runtime_error("Cannot create more than one window");
  // }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_mouse_x = 0;
  sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  SetCursor(sg_jcanvas_cursors[JCS_DEFAULT].cursor, sg_jcanvas_cursors[JCS_DEFAULT].hot_x, sg_jcanvas_cursors[JCS_DEFAULT].hot_y);
}

WindowAdapter::~WindowAdapter()
{
  if (sg_cursor_params.cursor != nullptr) {
    delete sg_cursor_params.cursor;
    sg_cursor_params.cursor = nullptr;
  }

  modeset_cleanup();
  close(sg_handler);
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

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
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

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

  if (sg_cursor_params.cursor != nullptr) {
    delete sg_cursor_params.cursor;
    sg_cursor_params.cursor = nullptr;
  }

  sg_cursor_params.cursor = dynamic_cast<jcanvas::Image *>(shape->Clone());

  sg_cursor_params.hot_x = hotx;
  sg_cursor_params.hot_y = hoty;
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jcanvas::JWR_NONE;
}

void WindowAdapter::SetIcon(jcanvas::Image *image)
{
  sg_jcanvas_icon = image;
}

jcanvas::Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
