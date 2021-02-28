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
#include "../include/nativewindow.h"

#include "jcanvas/core/jbufferedimage.h"

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

static DISPMANX_DISPLAY_HANDLE_T sg_dispman_display;
static DISPMANX_ELEMENT_HANDLE_T sg_dispman_element;
static DISPMANX_UPDATE_HANDLE_T sg_dispman_update;
static EGL_DISPMANX_WINDOW_T sg_dispman_window;

struct cursor_params_t {
  jcanvas::Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sg_cursors;
/** \brief */
static struct cursor_params_t sg_cursor_params;
/** \brief */
static EGLDisplay sg_egl_display;
/** \brief */
static EGLConfig sg_egl_config;
/** \brief */
static EGLContext sg_egl_context;
/** \brief */
static EGLSurface sg_egl_surface;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
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
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;
/** \brief */
static bool sg_cursor_enabled = true;

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
  EGLint num_config;

  bcm_host_init();

  sg_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#define CURSOR_INIT(type, ix, iy, hotx, hoty)\
	t.cursor = new jcanvas::BufferedImage(JPF_ARGB, {w, h});\
	t.hot_x = hotx;\
	t.hot_y = hoty;\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jcanvas::jpoint_t<int>{0, 0});\
	sg_cursors[type] = t;\

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

  const EGLint attribute_list[] = {
    EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };

  const EGLint sg_egl_context_attribs[] = {
    EGL_NONE,
  };

  if (sg_egl_display == EGL_NO_DISPLAY) {
    throw std::runtime_error("Unable to get egl display");
  }

  if (eglInitialize(sg_egl_display, nullptr, nullptr) == EGL_FALSE) {
    throw std::runtime_error("Unable to initialize egl");
  }

  if (!eglBindAPI(EGL_OPENVG_API)) {
    throw std::runtime_error("Unable to bind opengl es api");
  }

  if (eglChooseConfig(sg_egl_display, attribute_list, &sg_egl_config, 1, &num_config) == EGL_FALSE) {
    throw std::runtime_error("Unable to choose egl configuration");
  }

  sg_egl_context = eglCreateContext(sg_egl_display, sg_egl_config, EGL_NO_CONTEXT, sg_egl_context_attribs); 
  
  if (sg_egl_context == EGL_NO_CONTEXT) {
    throw std::runtime_error("eglCreateContext() failed");
  }

  uint32_t sw, sh;

  if (graphics_get_display_size(0, &sw, &sh) < 0) {
    throw std::runtime_error("Unable to get screen size");
  }

  sg_screen.x = sw;
  sg_screen.y = sh;
  
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
    jcanvas::jpoint_t<int>
      size = sg_back_buffer->GetSize();

    if (size.x != bounds.size.x or size.y != bounds.size.y) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jcanvas::BufferedImage(jcanvas::JPF_RGB32, bounds.size);
  }

  jcanvas::Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcanvas::JCF_SRC);

  sg_jcanvas_window->Paint(g);

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params.cursor, jcanvas::jpoint_t<int>{sg_mouse_x, sg_mouse_y});
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

      sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

      if ((buttonMask & 0x01) == 0 && (lastsg_mouse_state & 0x01)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (lastsg_mouse_state & 0x02)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (lastsg_mouse_state & 0x04)) {
        type = jcanvas::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (lastsg_mouse_state & 0x01)) {
        button = jcanvas::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (lastsg_mouse_state & 0x02)) {
        button = jcanvas::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (lastsg_mouse_state & 0x04)) {
        button = jcanvas::JMB_BUTTON2;
      }

      lastsg_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
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

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
{
  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;

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
  sg_dispman_window.x = sg_screen.x;
  sg_dispman_window.y = sg_screen.y;

  vc_dispmanx_update_submit_sync(sg_dispman_update);

  const EGLint sg_egl_surface_attribs[] = {
    // EGL_RENDER_BUFFER, 
    // EGL_BACK_BUFFER,
    EGL_NONE,
  };

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, &sg_dispman_window, sg_egl_surface_attribs);

  if (!sg_egl_surface) {
    throw std::runtime_error("eglCreateWindowSurface() failed");
  }

  if (eglMakeCurrent(sg_egl_display, sg_egl_surface, sg_egl_surface, sg_egl_context) == EGL_FALSE) {
    throw std::runtime_error("eglMakeCurrent() failed");
  }

  if (eglSwapInterval(sg_egl_display, 1) == EGL_FALSE) {
    throw std::runtime_error("eglSwapInterval() failed");
  }
}

NativeWindow::~NativeWindow()
{
  eglSwapBuffers(sg_egl_display, sg_egl_surface);
  eglMakeCurrent(sg_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(sg_egl_display, sg_egl_surface);
  eglDestroyContext(sg_egl_display, sg_egl_context);
  eglTerminate(sg_egl_display);
  
  vc_dispmanx_element_remove(sg_dispman_update, sg_dispman_element);
  vc_dispmanx_update_submit_sync(sg_dispman_update);
  vc_dispmanx_display_close(sg_dispman_display);
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint()
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_fullscreen = true;
  } else {
    sg_fullscreen = false;
  }
}

void NativeWindow::SetTitle(std::string title)
{
	sg_title = title;
}

std::string NativeWindow::GetTitle()
{
	return sg_title;
}

void NativeWindow::SetOpacity(float opacity)
{
  sg_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return sg_opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return sg_undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
	return {
    0,
    0,
    sg_screen.x,
	  sg_screen.y
  };
}
		
void NativeWindow::SetResizable(bool resizable)
{
  sg_resizable = resizable;
}

bool NativeWindow::IsResizable()
{
  return sg_resizable;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
  sg_mouse_x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t = {
		.x = 0,
		.y = 0
	};

	t.x = sg_mouse_x;
	t.y = sg_mouse_y;

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;
}

bool NativeWindow::IsVisible()
{
  return sg_visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jcanvas_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = enabled;

	// XDefineCursor(_display, _window, sg_jcanvas_cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  sg_jcanvas_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
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

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jcanvas::JWR_NONE;
}

void NativeWindow::SetIcon(jcanvas::Image *image)
{
  sg_jcanvas_icon = image;
}

jcanvas::Image * NativeWindow::GetIcon()
{
  return sg_jcanvas_icon;
}

}
