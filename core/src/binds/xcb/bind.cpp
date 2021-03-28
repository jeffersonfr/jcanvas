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

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <cairo.h>
#include <cairo-xcb.h>

namespace jcanvas {

/** \brief */
static std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static xcb_connection_t *sg_xcb_connection = nullptr;
/** \brief */
static xcb_screen_t *sg_xcb_screen = nullptr;
/** \brief */
static xcb_window_t sg_xcb_window = 0;
/** \brief */
static xcb_gcontext_t sg_xcb_context = 0;
/** \brief */
static std::shared_ptr<Image> sg_icon = nullptr;
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
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jrect_t<int> sg_previous_bounds;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(xcb_keycode_t symbol)
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
			return jkeyevent_symbol_t::Slash;
		case 0x6a:
			return jkeyevent_symbol_t::Slash;
		case 0x15:
			return jkeyevent_symbol_t::Plus;
		case 0x14:
			return jkeyevent_symbol_t::Minus;
		case 0x52:
			return jkeyevent_symbol_t::Minus;
		case 0x22:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case 0x23:
			return jkeyevent_symbol_t::SquareBracketRight;
		case 0x3b:
			return jkeyevent_symbol_t::Comma;
		case 0x3c:
		case 0x5b:
			return jkeyevent_symbol_t::Period;
		case 0x30:
			return jkeyevent_symbol_t::Aposthrophe;
		case 0x33:
			return jkeyevent_symbol_t::VerticalBar;
		case 0x31:
			return jkeyevent_symbol_t::GraveAccent;
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
			return jkeyevent_symbol_t::Number0;
		case 0x0a:
		// case 0x57:
			return jkeyevent_symbol_t::Number1;
		case 0x0b:
		// case 0x58:
			return jkeyevent_symbol_t::Number2;
		case 0x0c:
		// case 0x59:
			return jkeyevent_symbol_t::Number3;
		case 0x0d:
		// case 0x53:
			return jkeyevent_symbol_t::Number4;
		case 0x0e:
		// case 0x54:
			return jkeyevent_symbol_t::Number5;
		case 0x0f:
		// case 0x55:
			return jkeyevent_symbol_t::Number6;
		case 0x10:
		// case 0x4f:
			return jkeyevent_symbol_t::Number7;
		case 0x11:
		// case 0x50:
			return jkeyevent_symbol_t::Number8;
		case 0x12:
		// case 0x51:
			return jkeyevent_symbol_t::Number9;
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
  sg_xcb_connection = xcb_connect(nullptr,nullptr);

  if (xcb_connection_has_error(sg_xcb_connection)) {
		throw std::runtime_error("Unable to open display");
  }

  // get the first screen
  sg_xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(sg_xcb_connection)).data;

  sg_screen.x = sg_xcb_screen->width_in_pixels;
  sg_screen.y = sg_xcb_screen->height_in_pixels;
  
  sg_quitting = false;
}

static xcb_visualtype_t * find_visual(xcb_connection_t *c, xcb_visualid_t visual)
{
  xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(c));

  for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);

    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
      xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);

      for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
        if (visual == visual_iter.data->visual_id) {
          return visual_iter.data;
        }
    }
  }

  return nullptr;
}

static void InternalPaint()
{
	if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
		return;
	}

  // OPTIMIZE:: cairo_xlib_surface_create(Display, Drawable, Visual, width, height)
  
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

  cairo_surface_t 
    *cairo_surface = g->GetCairoSurface();

  xcb_visualtype_t 
    *vt = find_visual(sg_xcb_connection, sg_xcb_screen->root_visual);

  if (vt == nullptr) {
    return;
  }

  cairo_surface_t 
    *surface = cairo_xcb_surface_create(sg_xcb_connection, sg_xcb_window, vt, bounds.size.x, bounds.size.y);
  cairo_t 
    *cr = cairo_create(surface);

  xcb_flush(sg_xcb_connection);
  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  cairo_surface_finish(surface);

  xcb_flush(sg_xcb_connection);

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

struct my_event_queue_t {
  xcb_generic_event_t *prev = nullptr;
  xcb_generic_event_t *current = nullptr;
  xcb_generic_event_t *next = nullptr;
} event_queue;

void update_event_queue(){
  std::free(event_queue.prev);

  event_queue.prev = event_queue.current;
  event_queue.current = event_queue.next;
  event_queue.next = xcb_poll_for_queued_event(sg_xcb_connection);
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
        } else if (id == XCB_KEY_RELEASE) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->detail);

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
	if (sg_xcb_window != 0) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

	sg_xcb_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

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
  // xcb_change_window_attributes (connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(sg_xcb_connection, sg_xcb_window);
  xcb_flush(sg_xcb_connection);
}

WindowAdapter::~WindowAdapter()
{
  xcb_destroy_window(sg_xcb_connection, sg_xcb_window);
  xcb_disconnect(sg_xcb_connection);
  
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

xcb_intern_atom_cookie_t getCookieForAtom(const char *state_name) 
{
  return xcb_intern_atom(sg_xcb_connection, 0, sizeof(state_name)/sizeof(char), state_name);
}

xcb_atom_t getReplyAtomFromCookie(xcb_intern_atom_cookie_t cookie) 
{
  xcb_generic_error_t 
    *error;
  xcb_intern_atom_reply_t 
    *reply = xcb_intern_atom_reply(sg_xcb_connection, cookie, &error);

  if (error) {
    return 0;
  }

  return reply->atom;
}

void WindowAdapter::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_previous_bounds = GetBounds();

    SetBounds({{0, 0}, sg_screen});

    sg_fullscreen = true;
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
    SetBounds(sg_previous_bounds);
    xcb_map_window(sg_xcb_connection, sg_xcb_window);

    sg_fullscreen = false;
  }
  
  xcb_flush(sg_xcb_connection);

  /*
  xcb_intern_atom_cookie_t wm_state_ck = getCookieForAtom("_NET_WM_STATE");
  xcb_intern_atom_cookie_t wm_state_fs_ck = getCookieForAtom("_NET_WM_STATE_FULLSCREEN");

#define _NET_WM_STATE_REMOVE        0    // remove/unset property
#define _NET_WM_STATE_ADD           1    // add/set property
#define _NET_WM_STATE_TOGGLE        2    // toggle property

  xcb_atom_t atom_from_cookie = getReplyAtomFromCookie(wm_state_ck); 

  if (atom_from_cookie == 0) {
    return;
  }

  xcb_client_message_event_t ev;
  // memset (&ev, 0, sizeof (ev));
  ev.response_type = XCB_CLIENT_MESSAGE;
  ev.type = atom_from_cookie;
  ev.format = 32;
  ev.window = sg_xcb_window;
  // ev.data.data32[0] = sg_fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
  ev.data.data32[0] = _NET_WM_STATE_TOGGLE;
  ev.data.data32[1] = getReplyAtomFromCookie(wm_state_fs_ck);
  ev.data.data32[2] = XCB_ATOM_NONE;
  ev.data.data32[3] = 0;
  ev.data.data32[4] = 0;

  xcb_send_event(
      sg_xcb_connection, 1, sg_xcb_window, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (const char*)(&ev));
      */
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
  const uint32_t 
    values[] = {(uint32_t)bounds.point.x, (uint32_t)bounds.point.y, (uint32_t)bounds.size.x, (uint32_t)bounds.size.y};

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
  // jpoint_t<int>
  //  size = GetSize();

	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > sg_screen.x) {
		x = sg_screen.x;
	}

	if (y > sg_screen.y) {
		y = sg_screen.y;
	}

	// XWarpPointer(_display, None, sg_xcb_window, 0, 0, size.x, size.y, x, y);
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

	// XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

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

	// XDefineCursor(_display, _window, _issg_cursor_enabled);
	// XFlush(_display);
}

bool WindowAdapter::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
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

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

	// XChangeWindowAttributes() this funciontion change attributes like cursor
	//
	
	/*
	if ((void *)shape == nullptr) {
		return;
	}

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
  sg_icon = image;
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return sg_icon;
}

}
