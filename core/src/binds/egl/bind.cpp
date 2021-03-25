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

namespace jcanvas {
  jkeyevent_modifiers_t jKeyEventModifiersNone = jkeyevent_modifiers_t::None;
  jmouseevent_button_t jMouseEventButtonNone = jmouseevent_button_t::None;
  jmouseevent_button_t jMouseEventButtonButton1 = jmouseevent_button_t::Button1;
  jmouseevent_button_t jMouseEventButtonButton2 = jmouseevent_button_t::Button2;
  jmouseevent_button_t jMouseEventButtonButton3 = jmouseevent_button_t::Button3;
	jwindow_rotation_t jWindowRotationNone = jwindow_rotation_t::None;
}

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include <GL/glu.h>

#include <EGL/egl.h>
// #include <EGL/eglext.h>

#include <GLES2/gl2.h>

#include <xcb/xcb.h>

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <cairo.h>
#include <cairo-xcb.h>

namespace jcanvas {

static Display *sg_xcb_display;
static xcb_connection_t *sg_xcb_connection;
static xcb_screen_t *sg_xcb_screen;
static xcb_window_t sg_xcb_window;
static xcb_gcontext_t sg_xcb_context;

/** \brief */
std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static EGLDisplay sg_egl_display;
/** \brief */
static EGLConfig sg_egl_config;
/** \brief */
static EGLContext sg_egl_context;
/** \brief */
static EGLSurface sg_egl_surface;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static std::string sg_title;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(xcb_keycode_t symbol, bool capital)
{
	switch (symbol) {
    case 0x32:
    case 0x3e:
      return jkeyevent_symbol_t::Shift;
    case 0x25:
    case 0x69:
      return jkeyevent_symbol_t::Control;
    case 0x40:
    case 0x6c:
		  return jkeyevent_symbol_t::Alt;
		//case XK_Super_L:
		//	return jkeyevent_symbol_t::LSYSTEM;
		//case XK_Super_R:
		//	return jkeyevent_symbol_t::RSYSTEM;
		//case XK_Menu:
		//	return jkeyevent_symbol_t::MENU;
		case 0x09:
			return jkeyevent_symbol_t::Escape;
		case 0x2f:
			return jkeyevent_symbol_t::SemiColon;
		case 0x3d:
			return jkeyevent_symbol_t::QuestionMark;
		case 0x6a:
			return jkeyevent_symbol_t::Slash;
		case 0x15:
			return jkeyevent_symbol_t::Equals;
		case 0x14:
			return jkeyevent_symbol_t::Underscore;
		case 0x52:
			return jkeyevent_symbol_t::Minus;
		case 0x22:
			return jkeyevent_symbol_t::CurlyBracketLeft;
		case 0x23:
			return jkeyevent_symbol_t::CurlyBracketRight;
		case 0x3b:
			return jkeyevent_symbol_t::LessThan;
		case 0x3c:
		case 0x5b:
			return jkeyevent_symbol_t::GreaterThan;
		case 0x30:
			return jkeyevent_symbol_t::Quotation;
		case 0x33:
			return jkeyevent_symbol_t::BackSlash;
		case 0x31:
			return jkeyevent_symbol_t::Tilde;
		case 0x41:
			return jkeyevent_symbol_t::Space;
		case 0x24:
		case 0x68:
			return jkeyevent_symbol_t::Enter;
		case 0x16:
			return jkeyevent_symbol_t::Backspace;
		case 0x17:
			return jkeyevent_symbol_t::Tab;
		case 0x51:
			return jkeyevent_symbol_t::PageUp;
		case 0x59:
			return jkeyevent_symbol_t::PageDown;
		case 0x57:
			return jkeyevent_symbol_t::End;
		case 0x4f:
			return jkeyevent_symbol_t::Home;
		case 0x76:
			return jkeyevent_symbol_t::Insert;
		case 0x77:
			return jkeyevent_symbol_t::Delete;
		case 0x56:
			return jkeyevent_symbol_t::Plus;
		case 0x3f:
			return jkeyevent_symbol_t::Star;
		// case XK_Pause:
		//	return jkeyevent_symbol_t::Pause;
		case 0x43:
			return jkeyevent_symbol_t::F1;
		case 0x44:
			return jkeyevent_symbol_t::F2;
		case 0x45:
			return jkeyevent_symbol_t::F3;
		case 0x46:
			return jkeyevent_symbol_t::F4;
		case 0x47:
			return jkeyevent_symbol_t::F5;
		case 0x48:
			return jkeyevent_symbol_t::F6;
		case 0x49:
			return jkeyevent_symbol_t::F7;
		case 0x4a:
			return jkeyevent_symbol_t::F8;
		case 0x4b:
			return jkeyevent_symbol_t::F9;
		case 0x4c:
			return jkeyevent_symbol_t::F10;
		case 0x5f:
			return jkeyevent_symbol_t::F11;
		case 0x60:
			return jkeyevent_symbol_t::F12;
		case 0x71:
			return jkeyevent_symbol_t::CursorLeft;
		case 0x72:
			return jkeyevent_symbol_t::CursorRight;
		case 0x6f:
			return jkeyevent_symbol_t::CursorUp;
		case 0x74:
			return jkeyevent_symbol_t::CursorDown;
		case 0x13:
		case 0x54:
			return jkeyevent_symbol_t::ParenthesisRight;
		case 0x0a:
		// case 0x57:
			return jkeyevent_symbol_t::ExclamationMark;
		case 0x0b:
		// case 0x58:
			return jkeyevent_symbol_t::At;
		case 0x0c:
		// case 0x59:
			return jkeyevent_symbol_t::Hash;
		case 0x0d:
		// case 0x53:
			return jkeyevent_symbol_t::Dollar;
		case 0x0e:
		// case 0x54:
			return jkeyevent_symbol_t::Percent;
		case 0x0f:
		// case 0x55:
			return jkeyevent_symbol_t::CircumflexAccent;
		case 0x10:
		// case 0x4f:
			return jkeyevent_symbol_t::Ampersand;
		case 0x11:
		// case 0x50:
			return jkeyevent_symbol_t::Star;
		case 0x12:
		// case 0x51:
			return jkeyevent_symbol_t::ParenthesisLeft;
		case 0x26:
			return jkeyevent_symbol_t::a;
		case 0x38:
			return jkeyevent_symbol_t::b;
		case 0x36:
			return jkeyevent_symbol_t::c;
		case 0x28:
			return jkeyevent_symbol_t::d;
		case 0x1a:
			return jkeyevent_symbol_t::e;
		case 0x29:
			return jkeyevent_symbol_t::f;
		case 0x2a:
			return jkeyevent_symbol_t::g;
		case 0x2b:
			return jkeyevent_symbol_t::h;
		case 0x1f:
			return jkeyevent_symbol_t::i;
		case 0x2c:
			return jkeyevent_symbol_t::j;
		case 0x2d:
			return jkeyevent_symbol_t::k;
		case 0x2e:
			return jkeyevent_symbol_t::l;
		case 0x3a:
			return jkeyevent_symbol_t::m;
		case 0x39:
			return jkeyevent_symbol_t::n;
		case 0x20:
			return jkeyevent_symbol_t::o;
		case 0x21:
			return jkeyevent_symbol_t::p;
		case 0x18:
			return jkeyevent_symbol_t::q;
		case 0x1b:
			return jkeyevent_symbol_t::r;
		case 0x27:
			return jkeyevent_symbol_t::s;
		case 0x1c:
			return jkeyevent_symbol_t::t;
		case 0x1e:
			return jkeyevent_symbol_t::u;
		case 0x37:
			return jkeyevent_symbol_t::v;
		case 0x35:
			return jkeyevent_symbol_t::w;
		case 0x19:
			return jkeyevent_symbol_t::x;
		case 0x1d:
			return jkeyevent_symbol_t::y;
		case 0x34:
			return jkeyevent_symbol_t::z;
		// case XK_Print:
		//	return jkeyevent_symbol_t::Print;
		case 0x7f:
			return jkeyevent_symbol_t::Break;
		// case XK_acute:
		//	return jkeyevent_symbol_t::AcuteAccent;
		default:
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

void Application::Init(int argc, char **argv)
{
  EGLint num_config;

  sg_xcb_display = XOpenDisplay(nullptr);

  if (!sg_xcb_display) {
    throw std::runtime_error("Unable to open display");
  }

  sg_xcb_connection = XGetXCBConnection(sg_xcb_display);
  // sg_xcb_connection = xcb_connect(nullptr,nullptr);

  if (xcb_connection_has_error(sg_xcb_connection)) {
		throw std::runtime_error("Unable to connect to display");
  }

  // get the first screen
  sg_xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(sg_xcb_connection)).data;

  if (!eglBindAPI(EGL_OPENGL_API)) {
    throw std::runtime_error("Unable to bind opengl api");
  }

  sg_egl_display = eglGetDisplay(sg_xcb_display);

  const EGLint attribute_list[] = {
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

  if (eglChooseConfig(sg_egl_display, attribute_list, &sg_egl_config, 1, &num_config) == EGL_FALSE) {
    throw std::runtime_error("Unable to choose egl configuration");
  }

  sg_egl_context = eglCreateContext(sg_egl_display, sg_egl_config, EGL_NO_CONTEXT, sg_egl_context_attribs); 
  
  if (sg_egl_context == EGL_NO_CONTEXT) {
    throw std::runtime_error("eglCreateContext() failed");
  }

  sg_screen.x = sg_xcb_screen->width_in_pixels;
  sg_screen.y = sg_xcb_screen->height_in_pixels;

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

  g->Flush();

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  GLuint texture;

  glGenTextures(1, &texture);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, bounds.size.x, bounds.size.y, GL_BGRA, GL_UNSIGNED_BYTE, data);

  glViewport(0, -bounds.size.y, bounds.size.x*2, bounds.size.y*2);
  glClearColor(0, 0, 0, 0);
  glMatrixMode(GL_TEXTURE);

  glActiveTexture(texture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-bounds.size.x, bounds.size.y);

  glTexCoord2f(0.0f, bounds.size.y);
  glVertex2f(-bounds.size.x, 0.0f);

  glTexCoord2f(bounds.size.x, bounds.size.y);
  glVertex2f(0, 0.0f);

  glTexCoord2f(bounds.size.x, 0.0f);
  glVertex2f(0, bounds.size.y);
  
  glEnd();

  if (g->IsVerticalSyncEnabled() == false) {
    glFlush();
  } else {
    glFinish();
  }

  eglSwapBuffers(sg_egl_display, sg_egl_surface);

  glDeleteTextures(1, &texture);
  
  sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  xcb_generic_event_t *event;

  xcb_intern_atom_cookie_t 
    cookie = xcb_intern_atom(sg_xcb_connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t
    *reply = xcb_intern_atom_reply(sg_xcb_connection, cookie, 0);

  xcb_intern_atom_cookie_t 
    cookie2 = xcb_intern_atom(sg_xcb_connection, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t 
    *reply2 = xcb_intern_atom_reply(sg_xcb_connection, cookie2, 0);

  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, (*reply).atom, 4, 32, 1, &(*reply2).atom);

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }
  
    Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

    while ((event = xcb_poll_for_event(sg_xcb_connection))) {
    // while (e = xcb_wait_for_event(sg_xcb_connection)) {
      uint32_t id = event->response_type & ~0x80;

      if (id == XCB_EXPOSE) {
        InternalPaint();
      } else if (id == XCB_ENTER_NOTIFY) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
      } else if (id == XCB_LEAVE_NOTIFY) {
        // SetCursor(jcursor_style_t::Default);

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
      } else if (id == XCB_FOCUS_IN) {
      } else if (id == XCB_FOCUS_OUT) {
      } else if (id == XCB_CREATE_NOTIFY) {
      } else if (id == XCB_DESTROY_NOTIFY) {
      } else if (id == XCB_UNMAP_NOTIFY) {
      } else if (id == XCB_MAP_NOTIFY) {
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Opened));
      } else if (id == XCB_RESIZE_REQUEST) {
      } else if (id == XCB_KEY_PRESS || id == XCB_KEY_RELEASE) {
        xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jKeyEventModifiersNone;

        if ((e->state & XCB_MOD_MASK_SHIFT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if ((e->state & XCB_MOD_MASK_CONTROL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((e->state & XCB_MOD_MASK_LOCK) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::CapsLock);
        } else if ((e->state & XCB_MOD_MASK_1) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((e->state & XCB_MOD_MASK_2) != 0) {
        } else if ((e->state & XCB_MOD_MASK_3) != 0) {
        } else if ((e->state & XCB_MOD_MASK_4) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
        } else if ((e->state & XCB_MOD_MASK_5) != 0) {
        }

        if (id == XCB_KEY_PRESS) {
          type = jkeyevent_type_t::Pressed;

          // TODO:: grab pointer events
        } else if (id == XCB_KEY_RELEASE) {
          type = jkeyevent_type_t::Released;

          // TODO:: ungrab pointer events
        }

        int shift = (e->state & XCB_MOD_MASK_SHIFT) != 0;
        int capslock = (e->state & XCB_MOD_MASK_LOCK) != 0;

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->detail, (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE || id == XCB_MOTION_NOTIFY) {
        xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;

        jmouseevent_button_t button = jMouseEventButtonNone;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        sg_mouse_x = e->event_x;
        sg_mouse_y = e->event_y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

        if (id == XCB_MOTION_NOTIFY) {
          type = jmouseevent_type_t::Moved;
        } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE) {
          if (id == XCB_BUTTON_PRESS) {
            type = jmouseevent_type_t::Pressed;
          } else if (id == XCB_BUTTON_RELEASE) {
            type = jmouseevent_type_t::Released;
          }

          if (e->detail == 0x01) {
            button = jMouseEventButtonButton1;
          } else if (e->detail == 0x02) {
            button = jMouseEventButtonButton2;
          } else if (e->detail == 0x03) {
            button = jMouseEventButtonButton3;
          }
        }

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jMouseEventButtonNone, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (id == XCB_CLIENT_MESSAGE) {
        if ((*(xcb_client_message_event_t*)event).data.data32[0] == (*reply2).atom) {
          sg_quitting = true;

          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
        }
      }

      free(event);

      xcb_flush(sg_xcb_connection);
    }
  }

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
  // sg_jcanvas_icon = std::make_shared<BufferedImage>(JCANVAS_RESOURCES_DIR "/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  if (sg_xcb_window != 0) {
	  throw std::runtime_error("Cannot create more than one window");
  }

  uint32_t
    mask,
    values[2];

  sg_xcb_window = xcb_generate_id(sg_xcb_connection);

  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = sg_xcb_screen->black_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | 
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  xcb_create_window(
      sg_xcb_connection, XCB_COPY_FROM_PARENT, sg_xcb_window, sg_xcb_screen->root, bounds.point.x, bounds.point.y, bounds.size.x, bounds.size.y, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, sg_xcb_screen->root_visual, mask, values);

  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = sg_xcb_screen->black_pixel;
  values[1] = 0;

  sg_xcb_context = xcb_generate_id(sg_xcb_connection);

  xcb_create_gc(sg_xcb_connection, sg_xcb_context, sg_xcb_window, mask, values);

  // INFO:: change parameters after the window creation
  // const static uint32_t values[] = { XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS };
  // xcb_change_window_attributes (sg_xcb_connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(sg_xcb_connection, sg_xcb_window);
  xcb_flush(sg_xcb_connection);

  /*
  const EGLint sg_egl_config_attribs[] = {
    EGL_COLOR_BUFFER_TYPE,     EGL_RGB_BUFFER,
    EGL_BUFFER_SIZE,           32,
    EGL_RED_SIZE,              8,
    EGL_GREEN_SIZE,            8,
    EGL_BLUE_SIZE,             8,
    EGL_ALPHA_SIZE,            8,

    EGL_DEPTH_SIZE,            24,
    EGL_STENCIL_SIZE,          8,

    EGL_SAMPLE_BUFFERS,        0,
    EGL_SAMPLES,               0,

    EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE,       EGL_OPENGL_BIT,

    EGL_NONE,
  };
  */

  const EGLint sg_egl_surface_attribs[] = {
    EGL_RENDER_BUFFER, 
    EGL_BACK_BUFFER,
    EGL_NONE,
  };

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, sg_xcb_window, sg_egl_surface_attribs);

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

WindowAdapter::~WindowAdapter()
{
  glClear(GL_COLOR_BUFFER_BIT);

  eglSwapBuffers(sg_egl_display, sg_egl_surface);
  eglMakeCurrent(sg_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(sg_egl_display, sg_egl_surface);
  eglDestroyContext(sg_egl_display, sg_egl_context);
  eglTerminate(sg_egl_display);

  xcb_destroy_window(sg_xcb_connection, sg_xcb_window);
  xcb_disconnect(sg_xcb_connection);

  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  static jrect_t<int> previous_bounds = {
	  0, 0, 0, 0
  };

  if (sg_fullscreen == false) {
    previous_bounds = GetBounds();

    SetBounds({{0, 0}, sg_screen});

    sg_fullscreen = true;
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
    SetBounds(previous_bounds);
    xcb_map_window(sg_xcb_connection, sg_xcb_window);

    sg_fullscreen = false;
  }
  
  xcb_flush(sg_xcb_connection);
}

void WindowAdapter::SetTitle(std::string title)
{
	sg_title = title;
		
  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str());
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
  const uint32_t values[] = {
    (uint32_t)bounds.point.x, 
    (uint32_t)bounds.point.y, 
    (uint32_t)bounds.size.x, 
    (uint32_t)bounds.size.y
  };

  xcb_configure_window(sg_xcb_connection, sg_xcb_window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

jrect_t<int> WindowAdapter::GetBounds()
{
	jrect_t<int> 
    t = {0, 0, 0, 0};

  xcb_get_geometry_cookie_t 
    cookie = xcb_get_geometry(sg_xcb_connection, sg_xcb_window);
  xcb_get_geometry_reply_t 
    *reply = nullptr;

  if ((reply = xcb_get_geometry_reply(sg_xcb_connection, cookie, nullptr))) {
    t = {
      reply->x,
      reply->y,
      reply->width,
      reply->height
    };
  }

  free(reply);

	return t;
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
  // XWarpPointer(_display, None, sg_xcb_window, 0, 0, size.x, size.y, x, y);
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t = {
		.x = 0,
		.y = 0
	};

	// XTranslateCoordinates(_display, sg_xcb_window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == true) {
    xcb_map_window(sg_xcb_connection, sg_xcb_window);
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
  }
  
  xcb_flush(sg_xcb_connection);
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

	// XDefineCursor(_display, sg_xcb_window, sg_cursor_enabled);
	// XFlush(_display);
}

bool WindowAdapter::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  sg_jcanvas_cursor = style;

  int type = XC_arrow;
  
  if (style == jcursor_style_t::Default) {
    type = XC_arrow;
  } else if (style == jcursor_style_t::Crosshair) {
    type = XC_crosshair;
  } else if (style == jcursor_style_t::East) {
    type = XC_sb_right_arrow;
  } else if (style == jcursor_style_t::West) {
    type = XC_sb_left_arrow;
  } else if (style == jcursor_style_t::North) {
    type = XC_sb_up_arrow;
  } else if (style == jcursor_style_t::South) {
    type = XC_sb_down_arrow;
  } else if (style == jcursor_style_t::Hand) {
    type = XC_hand2;
  } else if (style == jcursor_style_t::Move) {
    type = XC_fleur;
  } else if (style == jcursor_style_t::Vertical) {
    type = XC_sb_v_double_arrow;
  } else if (style == jcursor_style_t::Horizontal) {
    type = XC_sb_h_double_arrow;
  } else if (style == jcursor_style_t::NorthWest) {
    type = XC_left_ptr;
  } else if (style == jcursor_style_t::NorthEast) {
    type = XC_right_ptr;
  } else if (style == jcursor_style_t::SouthWest) {
    type = XC_bottom_left_corner;
  } else if (style == jcursor_style_t::SouthEast) {
    type = XC_bottom_right_corner;
  } else if (style == jcursor_style_t::Text) {
    type = XC_xterm;
  } else if (style == jcursor_style_t::Wait) {
    type = XC_watch;
  }

  xcb_font_t font = xcb_generate_id(sg_xcb_connection);
  xcb_cursor_t cursor = xcb_generate_id(sg_xcb_connection);
  xcb_create_glyph_cursor(sg_xcb_connection, cursor, font, font, type, type + 1, 0, 0, 0, 0, 0, 0 );

  uint32_t mask = XCB_CW_CURSOR;
  uint32_t values = cursor;

  xcb_change_window_attributes(sg_xcb_connection, sg_xcb_window, mask, &values);
  xcb_free_cursor(sg_xcb_connection, cursor);
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

	/*
	jpoint_t<int> t = shape->GetSize();
	uint32_t data[t.x*t.y];
	
	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	if (data == nullptr) {
		return;
	}

	// Create the icon pixmap
	int screen = DefaultScreen(_display);
	Visual *visual = DefaultVisual(_display, screen);
	unsigned int depth = DefaultDepth(_display, screen);
	Xstd::shared_ptr<Image> image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, t.x, t.y, 32, 0);
	::Window root_window = XRootWindow(_display, screen);

	if (image == nullptr) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(_display, RootWindow(_display, screen), t.x, t.y, depth);
	GC gc = XCreateGC(_display, pixmap, 0, nullptr);
	
	XPutImage(_display, pixmap, gc, image, 0, 0, 0, 0, t.x, t.y);

	XColor color;

	color.flags = DoRed | DoGreen | DoBlue;
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	Cursor cursor = XCreatePixmapCursor(_display, pixmap, pixmap, &color, &color, 0, 0);

	// XUndefineCursor(_display, root_window);
	XDefineCursor(_display, root_window, cursor);
	XSync(_display, root_window);
	XFreePixmap(_display, pixmap);

	delete [] data;
	*/
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jWindowRotationNone;
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
