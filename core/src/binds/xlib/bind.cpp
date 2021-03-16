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

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

namespace jcanvas {

/** \brief */
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
static ::Display *sg_display = nullptr;
/** \brief */
static ::Window sg_window = 0;
/** \brief */
static ::XEvent sg_lastsg_key_release_event;
/** \brief */
static jrect_t<int> sg_visible_bounds;
/** \brief */
static bool sg_key_repeat = false;
/** \brief */
static std::shared_ptr<Image> _icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
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
static jrect_t<int> sg_previous_bounds;
/** \brief */
static Atom sg_wm_delete_message;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;
/** \brief */
static bool sgsg_jcanvas_cursor_enabled = true;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(KeySym symbol)
{
	switch (symbol) {
		/*
		case XK_Shift_L:
			return jkeyevent_symbol_t::LShift;
		case XK_Shift_R:
			return jkeyevent_symbol_t::RShift;
		case XK_Control_L:
			return jkeyevent_symbol_t::LControl;
		case XK_Control_R:
			return jkeyevent_symbol_t::RControl;
		case XK_Alt_L:
			return jkeyevent_symbol_t::LAlt;
		case XK_Alt_R:
			return jkeyevent_symbol_t::RAlt;
		case XK_Super_L:
			return jkeyevent_symbol_t::LSystem;
		case XK_Super_R:
			return jkeyevent_symbol_t::RSystem;
		case XK_Menu:
			return jkeyevent_symbol_t::Menu;
		*/
		case XK_Escape:
			return jkeyevent_symbol_t::Escape;
		case XK_semicolon:
			return jkeyevent_symbol_t::SemiColon;
		case XK_KP_Divide:
		case XK_slash:
			return jkeyevent_symbol_t::Slash;
		case XK_equal:
			return jkeyevent_symbol_t::Equals;
		case XK_KP_Subtract:
		case XK_hyphen:
		case XK_minus:
			return jkeyevent_symbol_t::Minus;
		case XK_bracketleft:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case XK_bracketright:
			return jkeyevent_symbol_t::SquareBracketRight;
		case XK_comma:
			return jkeyevent_symbol_t::Comma;
		case XK_KP_Decimal:
		case XK_period:
			return jkeyevent_symbol_t::Period;
		case XK_dead_acute:
			return jkeyevent_symbol_t::Aposthrophe;
		case XK_backslash:
			return jkeyevent_symbol_t::BackSlash;
		case XK_dead_grave:
			return jkeyevent_symbol_t::Tilde;
		case XK_space:
			return jkeyevent_symbol_t::Space;
		case XK_KP_Enter:
		case XK_Return:
			return jkeyevent_symbol_t::Enter;
		case XK_BackSpace:
			return jkeyevent_symbol_t::Backspace;
		case XK_Tab:
			return jkeyevent_symbol_t::Tab;
		case XK_Prior:
			return jkeyevent_symbol_t::PageUp;
		case XK_Next:
			return jkeyevent_symbol_t::PageDown;
		case XK_KP_End:
		case XK_End:
			return jkeyevent_symbol_t::End;
		case XK_KP_Home:
		case XK_Home:
			return jkeyevent_symbol_t::Home;
		case XK_KP_Insert:
		case XK_Insert:
			return jkeyevent_symbol_t::Insert;
		case XK_KP_Delete:
		case XK_Delete:
			return jkeyevent_symbol_t::Delete;
		case XK_KP_Add:
		case XK_plus:
			return jkeyevent_symbol_t::Plus;
		case XK_KP_Multiply:
			return jkeyevent_symbol_t::Star;
		case XK_Pause:
			return jkeyevent_symbol_t::Pause;
		case XK_F1:
			return jkeyevent_symbol_t::F1;
		case XK_F2:
			return jkeyevent_symbol_t::F2;
		case XK_F3:
			return jkeyevent_symbol_t::F3;
		case XK_F4:
			return jkeyevent_symbol_t::F4;
		case XK_F5:
			return jkeyevent_symbol_t::F5;
		case XK_F6:
			return jkeyevent_symbol_t::F6;
		case XK_F7:
			return jkeyevent_symbol_t::F7;
		case XK_F8:
			return jkeyevent_symbol_t::F8;
		case XK_F9:
			return jkeyevent_symbol_t::F9;
		case XK_F10:
			return jkeyevent_symbol_t::F10;
		case XK_F11:
			return jkeyevent_symbol_t::F11;
		case XK_F12:
			return jkeyevent_symbol_t::F12;
		case XK_KP_Left:
		case XK_Left:
			return jkeyevent_symbol_t::CursorLeft;
		case XK_KP_Right:
		case XK_Right:
			return jkeyevent_symbol_t::CursorRight;
		case XK_KP_Up:
		case XK_Up:
			return jkeyevent_symbol_t::CursorUp;
		case XK_KP_Down:
		case XK_Down:
			return jkeyevent_symbol_t::CursorDown;
		case XK_KP_0:
		case XK_0:
			return jkeyevent_symbol_t::Number0;
		case XK_KP_1:
		case XK_1:
			return jkeyevent_symbol_t::Number1;
		case XK_KP_2:
		case XK_2:
			return jkeyevent_symbol_t::Number2;
		case XK_KP_3:
		case XK_3:
			return jkeyevent_symbol_t::Number3;
		case XK_KP_4:
		case XK_4:
			return jkeyevent_symbol_t::Number4;
		case XK_KP_5:
		case XK_5:
			return jkeyevent_symbol_t::Number5;
		case XK_KP_6:
		case XK_6:
			return jkeyevent_symbol_t::Number6;
		case XK_KP_7:
		case XK_7:
			return jkeyevent_symbol_t::Number7;
		case XK_KP_8:
		case XK_8:
			return jkeyevent_symbol_t::Number8;
		case XK_KP_9:
		case XK_9:
			return jkeyevent_symbol_t::Number9;
		case XK_a:
			return jkeyevent_symbol_t::a;
		case XK_b:
			return jkeyevent_symbol_t::b;
		case XK_c:
			return jkeyevent_symbol_t::c;
		case XK_d:
			return jkeyevent_symbol_t::d;
		case XK_e:
			return jkeyevent_symbol_t::e;
		case XK_f:
			return jkeyevent_symbol_t::f;
		case XK_g:
			return jkeyevent_symbol_t::g;
		case XK_h:
			return jkeyevent_symbol_t::h;
		case XK_i:
			return jkeyevent_symbol_t::i;
		case XK_j:
			return jkeyevent_symbol_t::j;
		case XK_k:
			return jkeyevent_symbol_t::k;
		case XK_l:
			return jkeyevent_symbol_t::l;
		case XK_m:
			return jkeyevent_symbol_t::m;
		case XK_n:
			return jkeyevent_symbol_t::n;
		case XK_o:
			return jkeyevent_symbol_t::o;
		case XK_p:
			return jkeyevent_symbol_t::p;
		case XK_q:
			return jkeyevent_symbol_t::q;
		case XK_r:
			return jkeyevent_symbol_t::r;
		case XK_s:
			return jkeyevent_symbol_t::s;
		case XK_t:
			return jkeyevent_symbol_t::t;
		case XK_u:
			return jkeyevent_symbol_t::u;
		case XK_v:
			return jkeyevent_symbol_t::v;
		case XK_x:
			return jkeyevent_symbol_t::x;
		case XK_w:
			return jkeyevent_symbol_t::w;
		case XK_y:
			return jkeyevent_symbol_t::y;
		case XK_z:
			return jkeyevent_symbol_t::z;
		case XK_Print:
			return jkeyevent_symbol_t::Print;
		case XK_Break:
			return jkeyevent_symbol_t::Break;
		case XK_exclam:
			return jkeyevent_symbol_t::ExclamationMark;
		case XK_quotedbl:
			return jkeyevent_symbol_t::Quotation;
		case XK_numbersign:
			return jkeyevent_symbol_t::Hash;
		case XK_dollar:
			return jkeyevent_symbol_t::Dollar;
		case XK_percent:
			return jkeyevent_symbol_t::Percent;
		case XK_ampersand:
			return jkeyevent_symbol_t::Ampersand;
		case XK_apostrophe:
			return jkeyevent_symbol_t::Aposthrophe;
		case XK_parenleft:
			return jkeyevent_symbol_t::ParenthesisLeft;
		case XK_parenright:
			return jkeyevent_symbol_t::ParenthesisRight;
		case XK_asterisk:
			return jkeyevent_symbol_t::Star;
		case XK_less:
			return jkeyevent_symbol_t::LessThan;
		case XK_greater:
			return jkeyevent_symbol_t::GreaterThan;
		case XK_question:
			return jkeyevent_symbol_t::QuestionMark;
		case XK_at:
			return jkeyevent_symbol_t::At;
		case XK_asciicircum:
			return jkeyevent_symbol_t::CircumflexAccent;
		case XK_grave:
			return jkeyevent_symbol_t::GraveAccent;
		case XK_bar:
			return jkeyevent_symbol_t::VerticalBar;  
		case XK_braceleft:
			return jkeyevent_symbol_t::CurlyBracketLeft;
		case XK_braceright:
			return jkeyevent_symbol_t::CurlyBracketRight;
		case XK_asciitilde:
			return jkeyevent_symbol_t::Tilde;
		case XK_underscore:
			return jkeyevent_symbol_t::Underscore;
		case XK_acute:
			return jkeyevent_symbol_t::AcuteAccent;
		default:
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

void Application::Init(int argc, char **argv)
{
	// Open a connection with the X server
	sg_display = XOpenDisplay(nullptr);

	if (sg_display == nullptr) {
		throw std::runtime_error("Unable to connect with X server");
	}

	int screen = DefaultScreen(sg_display);

	sg_screen.x = DisplayWidth(sg_display, screen);
	sg_screen.y = DisplayHeight(sg_display, screen);

	XInitThreads();
  
  sg_quitting = false;
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

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

	int 
    screen = DefaultScreen(sg_display);
	::Visual 
    *visual = DefaultVisual(sg_display, screen);
	uint32_t 
    depth = DefaultDepth(sg_display, screen);

	XImage *image = XCreateImage(sg_display, visual, depth, ZPixmap, 0, (char *)data, bounds.size.x, bounds.size.y, 32, 0);

	if (image == nullptr) {
		return;
	}

	Pixmap 
    pixmap = XCreatePixmap(sg_display, XRootWindow(sg_display, screen), bounds.size.x, bounds.size.y, depth);
	GC 
    gc = XCreateGC(sg_display, pixmap, 0, nullptr);
	
	// XClearWindow(*(::Window *)_surface);
	
	// draw image to pixmap
	XPutImage(sg_display, pixmap, gc, image, 0, 0, 0, 0, bounds.size.x, bounds.size.y);
	XCopyArea(sg_display, pixmap, sg_window, gc, 0, 0, bounds.size.x, bounds.size.y, 0, 0);

	// XDestroyImage(image);
	XFreePixmap(sg_display, pixmap);

  if (g->IsVerticalSyncEnabled() == false) {
	  XFlush(sg_display);
  } else {
  	// INFO:: wait x11 process all events
	  // True:: discards all events remaing
  	// False:: not discards events remaing
	  XSync(sg_display, False);
  }
    
  sg_back_buffer->UnlockData();
  
  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

// Filter the events received by windows (only allow those matching a specific window)
static Bool check_x11_event(Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast<::Window>(userData);
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	XEvent event;
  
  // This function implements a workaround to properly discard repeated key events when necessary. 
  // The problem is that the system's key events policy doesn't match SFML's one: X server will 
  // generate both repeated KeyPress and KeyRelease events when maintaining a key down, while SFML 
  // only wants repeated KeyPress events. Thus, we have to:
  //   - Discard duplicated KeyRelease events when EnableKeyRepeat is true
  //   - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false
  
	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    while (XCheckIfEvent(sg_display, &event, &check_x11_event, reinterpret_cast<XPointer>(sg_window))) {
      if (event.type == DestroyNotify) {
        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
      } else if (event.type == ClientMessage) {
        // CHANGE:: destroynotify ???

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
      } else if (event.type == Expose) {
        InternalPaint();
      } else if (event.type == MapNotify) {
        // WARN:: avoid any draw before MapNotify's event
      } else if (event.type == ExposureMask) {
        InternalPaint();
      } else if (event.type == EnterNotify) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
      } else if (event.type == LeaveNotify) {
        // SetCursor(jcursor_style_t::Default);

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
      } else if (event.type == FocusIn) {
      } else if (event.type == FocusOut) {
      } else if (event.type == ConfigureNotify) {
        sg_visible_bounds = {
          event.xconfigure.x,
          event.xconfigure.y,
          event.xconfigure.x,
          event.xconfigure.y
        };

        InternalPaint();
        
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Resized));
      } else if (event.type == KeyPress || event.type == KeyRelease) {
        if (event.xkey.keycode < 256) {
          // To detect if it is a repeated key event, we check the current state of the key.
          // - If the state is "down", KeyReleased events must obviously be discarded.
          // - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
          //   and we need to properly forward the first one.
          char keys[32];

          XQueryKeymap(sg_display, keys);

          if (keys[event.xkey.keycode / 8] & (1 << (event.xkey.keycode % 8))) {
            // KeyRelease event + key down = repeated event --> discard
            if (event.type == KeyRelease) {
              sg_lastsg_key_release_event = event;

              std::this_thread::yield();
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
            if ((event.type == KeyPress) && !sg_key_repeat &&
                (sg_lastsg_key_release_event.xkey.keycode == event.xkey.keycode) && (sg_lastsg_key_release_event.xkey.time == event.xkey.time)) {
              // continue;
            }
          }
        }

        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jKeyEventModifiersNone;

        if (event.xkey.state & ShiftMask) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        }

        if (event.xkey.state & ControlMask) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        }

        if (event.xkey.state & Mod1Mask) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        }

        if (event.type == KeyPress) {
          type = jkeyevent_type_t::Pressed;
        } else if (event.type == KeyRelease) {
          type = jkeyevent_type_t::Released;
        }

        static XComposeStatus keyboard;

        char buffer[32];
        KeySym sym;

        XLookupString(&event.xkey, buffer, sizeof(buffer), &sym, &keyboard);

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(sym);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == ButtonPress || event.type == ButtonRelease || event.type == MotionNotify) {
        static jmouseevent_button_t buttons = jMouseEventButtonNone;

        jmouseevent_button_t button = jMouseEventButtonNone;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        if (event.type == MotionNotify) {
          type = jmouseevent_type_t::Moved;

          sg_mouse_x = event.xmotion.x;
          sg_mouse_y = event.xmotion.y;
        } else if (event.type == ButtonPress || event.type == ButtonRelease) {
          sg_mouse_x = event.xbutton.x;
          sg_mouse_y = event.xbutton.y;

          if (event.xbutton.button == Button1) {
            button = jMouseEventButtonButton1;
          } else if (event.xbutton.button == Button2) {
            button = jMouseEventButtonButton2;
          } else if (event.xbutton.button == Button3) {
            button = jMouseEventButtonButton3;
          } else if (event.xbutton.button == Button4) {
            if (type == jmouseevent_type_t::Released) {
              return;
            }

            type = jmouseevent_type_t::Rotated;
            button = jmouseevent_button_t::Wheel;
            mouse_z = -1;
          } else if (event.xbutton.button == Button5) {
            if (type == jmouseevent_type_t::Released) {
              return;
            }

            type = jmouseevent_type_t::Rotated;
            button = jmouseevent_button_t::Wheel;
            mouse_z = 1;
          }
          
          if (event.type == ButtonPress) {
            buttons = jenum_t<jmouseevent_button_t>{buttons}.Or(button);
            type = jmouseevent_type_t::Pressed;
          } else if (event.type == ButtonRelease) {
            buttons = jenum_t<jmouseevent_button_t>{buttons}.And(jenum_t<jmouseevent_button_t>{button}.Not());
            type = jmouseevent_type_t::Released;
          }
        }

        /*
        int 
          screen = DefaultScreen(sg_display);

        if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jmouseevent_button_t::None) {
          Cursor  cursor = XCreateFontCursor(sg_display, XC_arrow);
          
          XGrabPointer(sg_display, XRootWindow(sg_display, screen), False, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeSync, GrabModeAsync, XRootWindow(sg_display, screen), cursor, CurrentTime);
        } else {
          XUngrabPointer(sg_display, CurrentTime);
        }
        */

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      }
    }

    std::this_thread::yield();
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
	if (sg_window != 0) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // _icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

	int 
    screen = DefaultScreen(sg_display);

	sg_window = XCreateWindow(
			sg_display, 
			XRootWindow(sg_display, screen), 
			bounds.point.x, 
			bounds.point.y, 
			bounds.size.x, 
			bounds.size.y, 
			0, 
			DefaultDepth(sg_display, screen), 
			InputOutput, 
			DefaultVisual(sg_display, screen), 
			CWEventMask | CWOverrideRedirect, 
			&attr
	);

	if (sg_window == 0) {
		throw std::runtime_error("Cannot create a window");
	}

  sg_wm_delete_message = XInternAtom(sg_display, "WM_DELETE_WINDOW", False);

  XSetWMProtocols(sg_display, sg_window, &sg_wm_delete_message, 1);

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(sg_display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
		static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

		enum mwm_decor_t {
			MWM_DECOR_ALL         = 1 << 0,
			MWM_DECOR_BORDER      = 1 << 1,
			MWM_DECOR_RESIZEH     = 1 << 2,
			MWM_DECOR_TITLE       = 1 << 3,
			MWM_DECOR_MENU        = 1 << 4,
			MWM_DECOR_MINIMIZE    = 1 << 5,
			MWM_DECOR_MAXIMIZE    = 1 << 6
		};

		enum mwm_func_t {
			MWM_FUNC_ALL          = 1 << 0,
			MWM_FUNC_RESIZE       = 1 << 1,
			MWM_FUNC_MOVE         = 1 << 2,
			MWM_FUNC_MINIMIZE     = 1 << 3,
			MWM_FUNC_MAXIMIZE     = 1 << 4,
			MWM_FUNC_CLOSE        = 1 << 5
		};

		struct WMHints {
			unsigned long Flags;
			unsigned long Functions;
			unsigned long Decorations;
			long          InputMode;
			unsigned long State;
		};

		WMHints hints;

		hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
		hints.Decorations = True;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_RESIZE | MWM_FUNC_CLOSE;

		const uint8_t
      *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(sg_display, sg_window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}

	/*
	// This is a hack to force some windows managers to disable resizing
	XSizeHints sizeHints;

	sizeHints.flags = PMinSize | PMaxSize;
	sizeHints.min_width = sizeHints.max_width  = width;
	sizeHints.min_height = sizeHints.max_height = height;

	XSetWMNormalHints(sg_display, sg_window, &sizeHints); 
	*/

	XSelectInput(
			sg_display, sg_window, ExposureMask | EnterNotify | LeaveNotify | KeyPress | KeyRelease | ButtonPress | ButtonRelease | MotionNotify | PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask
	);

  sg_visible_bounds = bounds;

  XMapRaised(sg_display, sg_window);
	XMapWindow(sg_display, sg_window);
}

WindowAdapter::~WindowAdapter()
{
  XUnmapWindow(sg_display, sg_window);
  XDestroyWindow(sg_display, sg_window);
  XFlush(sg_display);
  XSync(sg_display, False);
	XCloseDisplay(sg_display);

  sg_window = 0;
  
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_previous_bounds = GetBounds();

    Atom atoms[2] = { 
      XInternAtom(sg_display, "_NET_WM_STATE_FULLSCREEN", False), None 
    };

    XMoveResizeWindow(sg_display, sg_window, 0, 0, sg_screen.x, sg_screen.y);
    XChangeProperty(sg_display, sg_window, 
        XInternAtom(sg_display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 1);

    /*
    XMoveResizeWindow(sg_display, sg_window, 0, 0, sg_screen.x, sg_screen.y);

    XEvent xev;

    Atom wm_state = XInternAtom(sg_display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(sg_display, "_NET_WM_STATE_FULLSCREEN", False);

    memset(&xev, 0, sizeof(xev));

    xev.type = ClientMessage;
    xev.xclient.window = sg_window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;

    XSendEvent(sg_display, XRootWindow(sg_display, DefaultScreen(sg_display)), False, SubstructureNotifyMask, &xev);
    */

    sg_fullscreen = true;
  } else {
	  XUnmapWindow(sg_display, sg_window);
    XMoveResizeWindow(sg_display, sg_window, sg_previous_bounds.point.x, sg_previous_bounds.point.y, sg_previous_bounds.size.x, sg_previous_bounds.size.y);
	  XMapWindow(sg_display, sg_window);

    sg_fullscreen = false;
  }
}

void WindowAdapter::SetTitle(std::string title)
{
	sg_title = title;
	
   XStoreName(sg_display, sg_window, title.c_str());
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
	sg_undecorated = undecorated;
  
  // XSetWindowBorderWidth()

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(sg_display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
		static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

		enum mwm_decor_t {
			MWM_DECOR_ALL         = 1 << 0,
			MWM_DECOR_BORDER      = 1 << 1,
			MWM_DECOR_RESIZEH     = 1 << 2,
			MWM_DECOR_TITLE       = 1 << 3,
			MWM_DECOR_MENU        = 1 << 4,
			MWM_DECOR_MINIMIZE    = 1 << 5,
			MWM_DECOR_MAXIMIZE    = 1 << 6
		};

		enum mwm_func_t {
			MWM_FUNC_ALL          = 1 << 0,
			MWM_FUNC_RESIZE       = 1 << 1,
			MWM_FUNC_MOVE         = 1 << 2,
			MWM_FUNC_MINIMIZE     = 1 << 3,
			MWM_FUNC_MAXIMIZE     = 1 << 4,
			MWM_FUNC_CLOSE        = 1 << 5
		};

		struct WMHints {
			unsigned long Flags;
			unsigned long Functions;
			unsigned long Decorations;
			long          InputMode;
			unsigned long State;
		};

		WMHints hints;

		hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
		hints.Decorations = True;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_CLOSE;

		if (sg_undecorated == true) {
			hints.Decorations = False;
		}

		const unsigned char *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(sg_display, sg_window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}
}

bool WindowAdapter::IsUndecorated()
{
  return sg_undecorated;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
	XMoveResizeWindow(sg_display, sg_window, bounds.point.x, bounds.point.y, bounds.size.x, bounds.size.y);
}

jrect_t<int> WindowAdapter::GetBounds()
{
  return sg_visible_bounds;
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

	XWarpPointer(sg_display, None, sg_window, 0, 0, 0, 0, x, y);
	// XWarpPointer(sg_display, None, XRootWindow(sg_display, DefaultScreen(sg_display)), 0, 0, 0, 0, x, y);
	XFlush(sg_display);
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

	::Window child_return;

	XTranslateCoordinates(sg_display, sg_window, XRootWindow(sg_display, DefaultScreen(sg_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == true) {
	  XMapWindow(sg_display, sg_window);
  } else {
	  XUnmapWindow(sg_display, sg_window);
  }
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
  sgsg_jcanvas_cursor_enabled = enabled;

  if (enabled == false) {
    XUndefineCursor(sg_display, sg_window);

    XFlush(sg_display);
    XSync(sg_display, False);
  } else {
    SetCursor(sg_jcanvas_cursor);
  }
}

bool WindowAdapter::IsCursorEnabled()
{
	return sgsg_jcanvas_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  if (sgsg_jcanvas_cursor_enabled == false) {
    return;
  }

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

  Cursor cursor = XCreateFontCursor(sg_display, type);

  XDefineCursor(sg_display, sg_window, cursor);
  XSync(sg_display, False);

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
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
	int screen = DefaultScreen(sg_display);
	Visual *visual = DefaultVisual(sg_display, screen);
	unsigned int depth = DefaultDepth(sg_display, screen);
	Xstd::shared_ptr<Image> image = XCreateImage(sg_display, visual, depth, ZPixmap, 0, (char *)data, t.x, t.y, 32, 0);
	::Window rootsg_window = XRootWindow(sg_display, screen);

	if (image == nullptr) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(sg_display, RootWindow(sg_display, screen), t.x, t.y, depth);
	GC gc = XCreateGC(sg_display, pixmap, 0, nullptr);
	
	XPutImage(sg_display, pixmap, gc, image, 0, 0, 0, 0, t.x, t.y);

	XColor color;

	color.flags = DoRed | DoGreen | DoBlue;
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	Cursor cursor = XCreatePixmapCursor(sg_display, pixmap, pixmap, &color, &color, 0, 0);

	// XUndefineCursor(sg_display, rootsg_window);
	XDefineCursor(sg_display, rootsg_window, cursor);
	XSync(sg_display, rootsg_window);
	XFreePixmap(sg_display, pixmap);

	// XChangeWindowAttributes() this funciontion change attributes like cursor
  
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
  _icon = image;
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return _icon;
}

}
