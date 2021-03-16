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

extern "C" {
	#include "bcm_host.h"
}

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "VG/openvg.h"

#include <linux/input.h>

#include <termio.h>
#include <fcntl.h>

#include <cairo.h>
#include <cairo-xcb.h>

namespace jcanvas {

static EGLDisplay display;
static EGLSurface surface;
static EGLContext context;
static EGLConfig config;

static EGL_DISPMANX_WINDOW_T nativewindow;
static DISPMANX_DISPLAY_HANDLE_T dispman_display;

static EGLDisplay sg_egl_display;
static EGLConfig sg_egl_config;
static EGLContext sg_egl_context;
static EGLSurface sg_egl_surface;

static DISPMANX_DISPLAY_HANDLE_T sg_dispman_display;
static DISPMANX_ELEMENT_HANDLE_T sg_dispman_element;
static DISPMANX_UPDATE_HANDLE_T sg_dispman_update;
static EGL_DISPMANX_WINDOW_T sg_dispman_window;

struct cursor_params_t {
  std::shared_ptr<Image> cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
static std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sg_cursors;
/** \brief */
static struct cursor_params_t sg_cursor_params;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;
/** \brief */
static bool sg_cursor_enabled = true;

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
  EGLint num_config;

  bcm_host_init();

  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;

  uint32_t sw, sh;

  if (graphics_get_display_size(0, &sw, &sh) < 0) {
    throw std::runtime_error("Unable to get screen size");
  }

  sg_screen.x = sw;
  sg_screen.y = sh;

  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.x = sg_screen.x;
  dst_rect.y = sg_screen.y;

  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.x = sg_screen.x << 16;
  src_rect.y = sg_screen.y << 16;

  sg_dispman_display = vc_dispmanx_display_open(0);
  sg_dispman_update = vc_dispmanx_update_start(0);

  sg_dispman_element = vc_dispmanx_element_add (
		  sg_dispman_update, sg_dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

  sg_dispman_window.element = sg_dispman_element;
  sg_dispman_window.width = sg_screen.x;
  sg_dispman_window.height = sg_screen.y;

  vc_dispmanx_update_submit_sync(sg_dispman_update);

  // INFO:: init dispman window
  /*
  const EGLint surface_attrs[] = {
    EGL_COLOR_BUFFER_TYPE,     EGL_RGB_BUFFER,
    EGL_BUFFER_SIZE,           32,

    EGL_DEPTH_SIZE,            24,
    EGL_STENCIL_SIZE,          8,

    EGL_SAMPLE_BUFFERS,        0,

    EGL_RENDERABLE_TYPE,       EGL_OPENGL_BIT,

    EGL_NONE,
  };
  */
  
  const EGLint surface_attrs[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };


  /*
  const EGLint surface_attrs[] = {
    EGL_RED_SIZE,              8,
    EGL_GREEN_SIZE,            8,
    EGL_BLUE_SIZE,             8,
    EGL_ALPHA_SIZE,            8,

    EGL_LUMINANCE_SIZE,        EGL_DONT_CARE,
    
    EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
    
    EGL_SAMPLES,               1,
    
    EGL_RENDER_BUFFER, 
    // EGL_BACK_BUFFER,
    EGL_NONE,
  };
  */

  sg_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  if (eglInitialize(sg_egl_display, nullptr, nullptr) == EGL_FALSE) {
    throw std::runtime_error("Unable to initialize egl");
  }

  if (!eglBindAPI(EGL_OPENVG_API)) {
    throw std::runtime_error("Unable to bind opengl es api");
  }

  if (eglChooseConfig(sg_egl_display, surface_attrs, &sg_egl_config, 1, &num_config) == EGL_FALSE) {
    throw std::runtime_error("Unable to choose egl configuration");
  }

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, &sg_dispman_window, nullptr);
		 
  if (eglGetError() != EGL_SUCCESS) {
    throw std::runtime_error("eglCreateWindowSurface() failed");
  }

  sg_egl_context = eglCreateContext(sg_egl_display, sg_egl_config, EGL_NO_CONTEXT, nullptr);

  if (eglGetError() != EGL_SUCCESS) {
    throw std::runtime_error("eglCreateContext() failed");
  }

  if (eglMakeCurrent(sg_egl_display, sg_egl_surface, sg_egl_surface, sg_egl_context) == EGL_FALSE) {
    throw std::runtime_error("eglMakeCurrent() failed");
  }

  /*
  if (eglSwapInterval(sg_egl_display, 1) == EGL_FALSE) {
    throw std::runtime_error("eglSwapInterval() failed");
  }
  */

#define CURSOR_INIT(type, ix, iy, hotx, hoty) \
  t.cursor = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{w, h}); \
		\
  t.hot_x = hotx;	\
  t.hot_y = hoty;	\
		\
  t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jpoint_t<int>{0, 0}); \
		\
  sg_jcanvas_cursors[type] = t; \

  struct cursor_params_t t;
  int w = 30, h = 30;

  /*
  std::shared_ptr<Image> cursors = std::make_shared<BufferedImage>(JCANVAS_RESOURCES_DIR "/images/cursors.png");

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
  */

  sg_quitting = false;
}

static void InternalPaint()
{
	if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
		return;
	}

  jrect_t<int> 
    bounds = sg_jcanvas_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jpoint_t<int>
      size = sg_back_buffer->GetSize();

    if (size.x != bounds.size.x or size.y != bounds.size.y) {
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = std::make_shared<BufferedImage>(jpixelformat_t::RGB32, bounds.size);
  }

  Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcomposite_flags_t::Src);

  sg_jcanvas_window->Paint(g);

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params.cursor, jpoint_t<int>{sg_mouse_x, sg_mouse_y});
  }

  g->Flush();

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  vgLoadIdentity();
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  vgScale(1.0f, -1.0f);
  vgTranslate(1.0f, -bounds.size.y);

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  VGImage img = vgCreateImage(VG_sARGB_8888, bounds.size.x, bounds.size.y, VG_IMAGE_QUALITY_BETTER);
  vgImageSubData(img, data, bounds.size.x*4, VG_sARGB_8888, 0, 0, bounds.size.x, bounds.size.y);
  vgDrawImage(img);
  vgDestroyImage(img);

  if (g->IsVerticalSyncEnabled() == false) {
    vgFlush();
  } else {
    vgFinish();
  }

  eglSwapBuffers(sg_egl_display, sg_egl_surface);

  sg_back_buffer->UnlockData();

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
  uint32_t lastsg_mouse_state = 0x00;

  int 
    fdk = open("/dev/input/by-path/platform-3f980000.usb-usb-0:1.4:1.0-event-kbd", O_RDONLY);

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

      sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

      if ((buttonMask & 0x01) == 0 && (lastsg_mouse_state & 0x01)) {
        type = jmouseevent_type_t::Released;
      } else if ((buttonMask & 0x02) == 0 && (lastsg_mouse_state & 0x02)) {
        type = jmouseevent_type_t::Released;
      } else if ((buttonMask & 0x04) == 0 && (lastsg_mouse_state & 0x04)) {
        type = jmouseevent_type_t::Released;
      } 

      if ((buttonMask & 0x01) != (lastsg_mouse_state & 0x01)) {
        button = jmouseevent_button_t::Button1;
      } else if ((buttonMask & 0x02) != (lastsg_mouse_state & 0x02)) {
        button = jmouseevent_button_t::Button3;
      } else if ((buttonMask & 0x04) != (lastsg_mouse_state & 0x04)) {
        button = jmouseevent_button_t::Button2;
      }

      lastsg_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
    }
  
    std::this_thread::yield();
  }

  sg_jcanvas_window->SetVisible(false);
  // TODO:: what for ??
  // sg_jcanvas_window->GrabEvents();
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
  // sg_jcanvas_icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

  sg_jcanvas_window = parent;

  sg_mouse_x = 0;
  sg_mouse_y = 0;
}

WindowAdapter::~WindowAdapter()
{
  eglSwapBuffers(sg_egl_display, sg_egl_surface);
  eglMakeCurrent(sg_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(sg_egl_display, sg_egl_surface);
  eglDestroyContext(sg_egl_display, sg_egl_context);
  eglTerminate(sg_egl_display);
  eglReleaseThread();

  vc_dispmanx_element_remove(sg_dispman_update, sg_dispman_element);
  vc_dispmanx_update_submit_sync(sg_dispman_update);
  vc_dispmanx_display_close(sg_dispman_display);
  
  sg_back_buffer = nullptr;

  bcm_host_deinit();
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_fullscreen = true;
  } else {
    sg_fullscreen = false;
  }
}

void WindowAdapter::SetTitle(std::string title)
{
	sg_title = title;
}

std::string WindowAdapter::GetTitle()
{
	return sg_title;
}

void WindowAdapter::SetOpacity(float opacity)
{
  sg_opacity = opacity;
}

float WindowAdapter::GetOpacity()
{
  return sg_opacity;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
}

bool WindowAdapter::IsUndecorated()
{
  return sg_undecorated;
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
  sg_resizable = resizable;
}

bool WindowAdapter::IsResizable()
{
  return sg_resizable;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
  sg_mouse_x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t = {
		.x = 0,
		.y = 0
	};

	t.x = sg_mouse_x;
	t.y = sg_mouse_y;

	return t;
}

void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;
}

bool WindowAdapter::IsVisible()
{
  return sg_visible;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = enabled;

	// XDefineCursor(_display, _window, sg_jcanvas_cursor_enabled);
	// XFlush(_display);
}

bool WindowAdapter::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  sg_jcanvas_cursor = style;
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
