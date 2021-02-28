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

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#include <vdpau/vdpau_x11.h>

namespace jcanvas {

/** \brief */
struct cursor_params_t {
  Image *cursor;
  int hot_x;
  int hot_y;
};

static VdpDevice sg_vdp_device;
static VdpPresentationQueueTarget sg_vdp_target;
static VdpPresentationQueue sg_vdp_queue;
static VdpOutputSurface sg_vdp_surface;
static VdpTime sg_vdp_time = 0;
static VdpGetProcAddress *sg_vdp_proc_address;

static VdpPresentationQueueTargetCreateX11 *PresentationQueueTargetCreate = NULL;
static VdpGetErrorString *GetErrorString = NULL;
static VdpDeviceDestroy *DeviceDestroy = NULL;
static VdpGetInformationString *GetInformationString = NULL;
static VdpPresentationQueueTargetDestroy *PresentationQueueTargetDestroy = NULL;
static VdpOutputSurfaceCreate *OutputSurfaceCreate = NULL;
static VdpOutputSurfaceDestroy *OutputSurfaceDestroy = NULL;
static VdpOutputSurfacePutBitsNative *OutputSurfacePutBitsNative = NULL;
static VdpOutputSurfaceRenderOutputSurface *OutputSurfaceRenderOutputSurface = NULL;
static VdpPresentationQueueCreate *PresentationQueueCreate = NULL;
static VdpPresentationQueueDestroy *PresentationQueueDestroy = NULL;
static VdpPresentationQueueDisplay *PresentationQueueDisplay = NULL;
static VdpPresentationQueueGetTime *PresentationQueueGetTime = NULL;
static VdpPresentationQueueBlockUntilSurfaceIdle *PresentationQueueBlockUntilSurfaceIdle = NULL;

static Atom sg_wm_delete_message;

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static ::Display *sg_display = nullptr;
/** \brief */
static ::Window sg_window = 0;
/** \brief */
static ::XEvent sg_lastsg_key_release_event;
/** \brief */
static jcanvas::jrect_t<int> sg_visible_bounds;
/** \brief */
static bool sg_key_repeat;
/** \brief */
static int sg_mouse_x;
/** \brief */
static int sg_mouse_y;
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
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static jcanvas::jrect_t<int> sg_previous_bounds;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(KeySym symbol)
{
	switch (symbol) {
		/*
		case XK_Shift_L:
			return jcanvas::JKS_LShift;
		case XK_Shift_R:
			return jcanvas::JKS_RShift;
		case XK_Control_L:
			return jcanvas::JKS_LControl;
		case XK_Control_R:
			return jcanvas::JKS_RControl;
		case XK_Alt_L:
			return jcanvas::JKS_LAlt;
		case XK_Alt_R:
			return jcanvas::JKS_RAlt;
		case XK_Super_L:
			return jcanvas::JKS_LSystem;
		case XK_Super_R:
			return jcanvas::JKS_RSystem;
		case XK_Menu:
			return jcanvas::JKS_Menu;
		*/
		case XK_Escape:
			return jcanvas::JKS_ESCAPE;
		case XK_semicolon:
			return jcanvas::JKS_SEMICOLON;
		case XK_KP_Divide:
		case XK_slash:
			return jcanvas::JKS_SLASH;
		case XK_equal:
			return jcanvas::JKS_EQUALS_SIGN;
		case XK_KP_Subtract:
		case XK_hyphen:
		case XK_minus:
			return jcanvas::JKS_MINUS_SIGN;
		case XK_bracketleft:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case XK_bracketright:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case XK_comma:
			return jcanvas::JKS_COMMA;
		case XK_KP_Decimal:
		case XK_period:
			return jcanvas::JKS_PERIOD;
		case XK_dead_acute:
			return jcanvas::JKS_APOSTROPHE;
		case XK_backslash:
			return jcanvas::JKS_BACKSLASH;
		case XK_dead_grave:
			return jcanvas::JKS_TILDE;
		case XK_space:
			return jcanvas::JKS_SPACE;
		case XK_KP_Enter:
		case XK_Return:
			return jcanvas::JKS_ENTER;
		case XK_BackSpace:
			return jcanvas::JKS_BACKSPACE;
		case XK_Tab:
			return jcanvas::JKS_TAB;
		case XK_Prior:
			return jcanvas::JKS_PAGE_UP;
		case XK_Next:
			return jcanvas::JKS_PAGE_DOWN;
		case XK_KP_End:
		case XK_End:
			return jcanvas::JKS_END;
		case XK_KP_Home:
		case XK_Home:
			return jcanvas::JKS_HOME;
		case XK_KP_Insert:
		case XK_Insert:
			return jcanvas::JKS_INSERT;
		case XK_KP_Delete:
		case XK_Delete:
			return jcanvas::JKS_DELETE;
		case XK_KP_Add:
		case XK_plus:
			return jcanvas::JKS_PLUS_SIGN;
		case XK_KP_Multiply:
			return jcanvas::JKS_STAR;
		case XK_Pause:
			return jcanvas::JKS_PAUSE;
		case XK_F1:
			return jcanvas::JKS_F1;
		case XK_F2:
			return jcanvas::JKS_F2;
		case XK_F3:
			return jcanvas::JKS_F3;
		case XK_F4:
			return jcanvas::JKS_F4;
		case XK_F5:
			return jcanvas::JKS_F5;
		case XK_F6:
			return jcanvas::JKS_F6;
		case XK_F7:
			return jcanvas::JKS_F7;
		case XK_F8:
			return jcanvas::JKS_F8;
		case XK_F9:
			return jcanvas::JKS_F9;
		case XK_F10:
			return jcanvas::JKS_F10;
		case XK_F11:
			return jcanvas::JKS_F11;
		case XK_F12:
			return jcanvas::JKS_F12;
		case XK_KP_Left:
		case XK_Left:
			return jcanvas::JKS_CURSOR_LEFT;
		case XK_KP_Right:
		case XK_Right:
			return jcanvas::JKS_CURSOR_RIGHT;
		case XK_KP_Up:
		case XK_Up:
			return jcanvas::JKS_CURSOR_UP;
		case XK_KP_Down:
		case XK_Down:
			return jcanvas::JKS_CURSOR_DOWN;
		case XK_KP_0:
		case XK_0:
			return jcanvas::JKS_0;
		case XK_KP_1:
		case XK_1:
			return jcanvas::JKS_1;
		case XK_KP_2:
		case XK_2:
			return jcanvas::JKS_2;
		case XK_KP_3:
		case XK_3:
			return jcanvas::JKS_3;
		case XK_KP_4:
		case XK_4:
			return jcanvas::JKS_4;
		case XK_KP_5:
		case XK_5:
			return jcanvas::JKS_5;
		case XK_KP_6:
		case XK_6:
			return jcanvas::JKS_6;
		case XK_KP_7:
		case XK_7:
			return jcanvas::JKS_7;
		case XK_KP_8:
		case XK_8:
			return jcanvas::JKS_8;
		case XK_KP_9:
		case XK_9:
			return jcanvas::JKS_9;
		case XK_a:
			return jcanvas::JKS_a;
		case XK_b:
			return jcanvas::JKS_b;
		case XK_c:
			return jcanvas::JKS_c;
		case XK_d:
			return jcanvas::JKS_d;
		case XK_e:
			return jcanvas::JKS_e;
		case XK_f:
			return jcanvas::JKS_f;
		case XK_g:
			return jcanvas::JKS_g;
		case XK_h:
			return jcanvas::JKS_h;
		case XK_i:
			return jcanvas::JKS_i;
		case XK_j:
			return jcanvas::JKS_j;
		case XK_k:
			return jcanvas::JKS_k;
		case XK_l:
			return jcanvas::JKS_l;
		case XK_m:
			return jcanvas::JKS_m;
		case XK_n:
			return jcanvas::JKS_n;
		case XK_o:
			return jcanvas::JKS_o;
		case XK_p:
			return jcanvas::JKS_p;
		case XK_q:
			return jcanvas::JKS_q;
		case XK_r:
			return jcanvas::JKS_r;
		case XK_s:
			return jcanvas::JKS_s;
		case XK_t:
			return jcanvas::JKS_t;
		case XK_u:
			return jcanvas::JKS_u;
		case XK_v:
			return jcanvas::JKS_v;
		case XK_x:
			return jcanvas::JKS_x;
		case XK_w:
			return jcanvas::JKS_w;
		case XK_y:
			return jcanvas::JKS_y;
		case XK_z:
			return jcanvas::JKS_z;
		case XK_Print:
			return jcanvas::JKS_PRINT;
		case XK_Break:
			return jcanvas::JKS_BREAK;
		case XK_exclam:
			return jcanvas::JKS_EXCLAMATION_MARK;
		case XK_quotedbl:
			return jcanvas::JKS_QUOTATION;
		case XK_numbersign:
			return jcanvas::JKS_NUMBER_SIGN;
		case XK_dollar:
			return jcanvas::JKS_DOLLAR_SIGN;
		case XK_percent:
			return jcanvas::JKS_PERCENT_SIGN;
		case XK_ampersand:
			return jcanvas::JKS_AMPERSAND;
		case XK_apostrophe:
			return jcanvas::JKS_APOSTROPHE;
		case XK_parenleft:
			return jcanvas::JKS_PARENTHESIS_LEFT;
		case XK_parenright:
			return jcanvas::JKS_PARENTHESIS_RIGHT;
		case XK_asterisk:
			return jcanvas::JKS_STAR;
		case XK_less:
			return jcanvas::JKS_LESS_THAN_SIGN;
		case XK_greater:
			return jcanvas::JKS_GREATER_THAN_SIGN;
		case XK_question:
			return jcanvas::JKS_QUESTION_MARK;
		case XK_at:
			return jcanvas::JKS_AT;
		case XK_asciicircum:
			return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		case XK_grave:
			return jcanvas::JKS_GRAVE_ACCENT;
		case XK_bar:
			return jcanvas::JKS_VERTICAL_BAR;  
		case XK_braceleft:
			return jcanvas::JKS_CURLY_BRACKET_LEFT;
		case XK_braceright:
			return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case XK_asciitilde:
			return jcanvas::JKS_TILDE;
		case XK_underscore:
			return jcanvas::JKS_UNDERSCORE;
		case XK_acute:
			return jcanvas::JKS_ACUTE_ACCENT;
		default:
			break;
	}

	return jcanvas::JKS_UNKNOWN;
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

  g->Flush();

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  uint8_t *data = sg_back_buffer->LockData();

  const void *src[1] = {
    data
  };
  uint32_t pitches[1] = {
    (uint32_t)bounds.size.x*4
  };
  VdpTime 
    this_time = 0;
  uint32_t
    period = 1000000;

  PresentationQueueBlockUntilSurfaceIdle(sg_vdp_queue, sg_vdp_surface, &sg_vdp_time);
  OutputSurfacePutBitsNative(sg_vdp_surface, (void const *const *)src, pitches, nullptr);

  if (!sg_vdp_time) {
    PresentationQueueGetTime(sg_vdp_queue, &sg_vdp_time);
    
    sg_vdp_time += 1000000;
  } else {
    sg_vdp_time += period;
  }

  this_time = sg_vdp_time;

  PresentationQueueDisplay(sg_vdp_queue, sg_vdp_surface, 0, 0, this_time);

  sg_back_buffer->UnlockData();
  
  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
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
        
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
      } else if (event.type == ClientMessage) {
        // CHANGE:: destroynotify ???

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
      } else if (event.type == Expose) {
        InternalPaint();
      } else if (event.type == MapNotify) {
        // WARN:: avoid any draw before MapNotify's event
      } else if (event.type == ExposureMask) {
        InternalPaint();
      } else if (event.type == EnterNotify) {
        // SDL_CaptureMouse(true);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
      } else if (event.type == LeaveNotify) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(JCS_DEFAULT);

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
      } else if (event.type == FocusIn) {
      } else if (event.type == FocusOut) {
      } else if (event.type == ConfigureNotify) {
        sg_visible_bounds = {
          event.xconfigure.x,
          event.xconfigure.y,
          event.xconfigure.x,
          event.xconfigure.y
        };

        OutputSurfaceDestroy(sg_vdp_surface);
        OutputSurfaceCreate(sg_vdp_device, VDP_RGBA_FORMAT_B8G8R8A8, sg_visible_bounds.size.x, sg_visible_bounds.size.y, &sg_vdp_surface);

        InternalPaint();
        
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_RESIZED));
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

              continue;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
            if ((event.type == KeyPress) && !sg_key_repeat &&
                (sg_lastsg_key_release_event.xkey.keycode == event.xkey.keycode) && (sg_lastsg_key_release_event.xkey.time == event.xkey.time)) {
              // continue;
            }
          }
        }

        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if (event.xkey.state & ShiftMask) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        }

        if (event.xkey.state & ControlMask) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        }

        if (event.xkey.state & Mod1Mask) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        }

        type = (jcanvas::jkeyevent_type_t)(0);

        if (event.type == KeyPress) {
          type = jcanvas::JKT_PRESSED;
        } else if (event.type == KeyRelease) {
          type = jcanvas::JKT_RELEASED;
        }

        static XComposeStatus keyboard;

        char buffer[32];
        KeySym sym;

        XLookupString(&event.xkey, buffer, sizeof(buffer), &sym, &keyboard);

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(sym);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == ButtonPress || event.type == ButtonRelease || event.type == MotionNotify) {
        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        if (event.type == MotionNotify) {
          type = jcanvas::JMT_MOVED;

          sg_mouse_x = event.xmotion.x;
          sg_mouse_y = event.xmotion.y;
        } else if (event.type == ButtonPress || event.type == ButtonRelease) {
          if (event.type == ButtonPress) {
            type = jcanvas::JMT_PRESSED;
          } else if (event.type == ButtonRelease) {
            type = jcanvas::JMT_RELEASED;
          }

          sg_mouse_x = event.xbutton.x;
          sg_mouse_y = event.xbutton.y;

          if (event.xbutton.button == Button1) {
            button = jcanvas::JMB_BUTTON1;
          } else if (event.xbutton.button == Button2) {
            button = jcanvas::JMB_BUTTON2;
          } else if (event.xbutton.button == Button3) {
            button = jcanvas::JMB_BUTTON3;
          } else if (event.xbutton.button == Button4) {
            if (type == jcanvas::JMT_RELEASED) {
              return;
            }

            type = jcanvas::JMT_ROTATED;
            button = jcanvas::JMB_WHEEL;
            mouse_z = -1;
          } else if (event.xbutton.button == Button5) {
            if (type == jcanvas::JMT_RELEASED) {
              return;
            }

            type = jcanvas::JMT_ROTATED;
            button = jcanvas::JMB_WHEEL;
            mouse_z = 1;
          }
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
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

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
{
	if (sg_jcanvas_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

  XLockDisplay(sg_display);

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

  sg_visible_bounds = {
    bounds.point.x,
    bounds.point.y,
    bounds.size.x,
    bounds.size.y
  };

  XMapRaised(sg_display, sg_window);

  VdpStatus 
    status = vdp_device_create_x11(sg_display, DefaultScreen(sg_display), &sg_vdp_device, &sg_vdp_proc_address);
  
  if (status) {
    XUnlockDisplay(sg_display);
    
		throw std::runtime_error("Unable to create a vdpau device");
  }

#define CHECK_STATUS(x) \
  if (x) { \
    XUnlockDisplay(sg_display); \
		throw std::runtime_error("Unable to load the current vdpau function"); \
  } \

  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_CREATE_X11, (void **)&PresentationQueueTargetCreate));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_GET_ERROR_STRING, (void **)&GetErrorString));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_GET_INFORMATION_STRING, (void **)&GetInformationString));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_DEVICE_DESTROY, (void **)&DeviceDestroy));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_DESTROY, (void **)&PresentationQueueTargetDestroy));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_OUTPUT_SURFACE_CREATE, (void **)&OutputSurfaceCreate));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_OUTPUT_SURFACE_DESTROY, (void **)&OutputSurfaceDestroy));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_OUTPUT_SURFACE_PUT_BITS_NATIVE, (void **)&OutputSurfacePutBitsNative));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_OUTPUT_SURFACE_RENDER_OUTPUT_SURFACE, (void **)&OutputSurfaceRenderOutputSurface));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_CREATE, (void **)&PresentationQueueCreate));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_CREATE, (void **)&PresentationQueueDestroy));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_DISPLAY, (void **)&PresentationQueueDisplay));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_GET_TIME, (void **)&PresentationQueueGetTime));
  CHECK_STATUS(sg_vdp_proc_address(sg_vdp_device, VDP_FUNC_ID_PRESENTATION_QUEUE_BLOCK_UNTIL_SURFACE_IDLE, (void **)&PresentationQueueBlockUntilSurfaceIdle));
  
  status = PresentationQueueTargetCreate(sg_vdp_device, sg_window, &sg_vdp_target);
  status = PresentationQueueCreate(sg_vdp_device, sg_vdp_target, &sg_vdp_queue);

  if (status) {
    XUnlockDisplay(sg_display);
    
		throw std::runtime_error("Unable to create a presentation queue");
  }

  status = OutputSurfaceCreate(sg_vdp_device, VDP_RGBA_FORMAT_B8G8R8A8, bounds.size.x, bounds.size.y, &sg_vdp_surface);

  if (status) {
    XUnlockDisplay(sg_display);

		throw std::runtime_error("Unable to create an output surface");
  }

  XUnlockDisplay(sg_display);

	XMapWindow(sg_display, sg_window);
}

WindowAdapter::~WindowAdapter()
{
  OutputSurfaceDestroy(sg_vdp_surface);
  PresentationQueueDestroy(sg_vdp_queue);
  PresentationQueueTargetDestroy(sg_vdp_target);
  // DeviceDestroy(sg_vdp_device);

  XUnmapWindow(sg_display, sg_window);
  XDestroyWindow(sg_display, sg_window);
  XFlush(sg_display);
  XSync(sg_display, False);
	XCloseDisplay(sg_display);

  sg_window = 0;
  
  delete sg_back_buffer;
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

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
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
  sg_cursor_enabled = enabled;

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
	return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  if (sg_cursor_enabled == false) {
    return;
  }

  int type = XC_arrow;
  
  if (style == JCS_DEFAULT) {
    type = XC_arrow;
  } else if (style == JCS_CROSSHAIR) {
    type = XC_crosshair;
  } else if (style == JCS_EAST) {
    type = XC_sb_right_arrow;
  } else if (style == JCS_WEST) {
    type = XC_sb_left_arrow;
  } else if (style == JCS_NORTH) {
    type = XC_sb_up_arrow;
  } else if (style == JCS_SOUTH) {
    type = XC_sb_down_arrow;
  } else if (style == JCS_HAND) {
    type = XC_hand2;
  } else if (style == JCS_MOVE) {
    type = XC_fleur;
  } else if (style == JCS_NS) {
    type = XC_sb_v_double_arrow;
  } else if (style == JCS_WE) {
    type = XC_sb_h_double_arrow;
  } else if (style == JCS_NW_CORNER) {
    type = XC_left_ptr;
  } else if (style == JCS_NE_CORNER) {
    type = XC_right_ptr;
  } else if (style == JCS_SW_CORNER) {
    type = XC_bottom_left_corner;
  } else if (style == JCS_SE_CORNER) {
    type = XC_bottom_right_corner;
  } else if (style == JCS_TEXT) {
    type = XC_xterm;
  } else if (style == JCS_WAIT) {
    type = XC_watch;
  }

  Cursor cursor = XCreateFontCursor(sg_display, type);

  XDefineCursor(sg_display, sg_window, cursor);
  XSync(sg_display, False);

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
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
	XImage *image = XCreateImage(sg_display, visual, depth, ZPixmap, 0, (char *)data, t.x, t.y, 32, 0);
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
