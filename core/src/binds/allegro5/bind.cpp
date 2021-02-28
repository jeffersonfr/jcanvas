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

#include <allegro5/allegro.h>

namespace jcanvas {

/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::map<int, int> sg_key_modifiers;
/** \brief */
static ALLEGRO_DISPLAY *sg_display = nullptr;
/** \brief */
static ALLEGRO_BITMAP *sg_surface = nullptr;
/** \brief */
static ALLEGRO_MOUSE_CURSOR *sg_jcanvas_cursor_bitmap = nullptr;
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
static bool sg_jcanvas_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static jcanvas::jrect_t<int> sg_previous_bounds;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case ALLEGRO_KEY_ENTER:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case ALLEGRO_KEY_BACKSPACE:
			return jcanvas::JKS_BACKSPACE;
		case ALLEGRO_KEY_TAB:
			return jcanvas::JKS_TAB;
		// case ALLEGRO_KEY_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case ALLEGRO_KEY_ESCAPE:
			return jcanvas::JKS_ESCAPE;
		case ALLEGRO_KEY_SPACE:
			return jcanvas::JKS_SPACE;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return jcanvas::JKS_EXCLAMATION_MARK;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return jcanvas::JKS_QUOTATION;
		// case ALLEGRO_KEY_HASH:
		// 	return jcanvas::JKS_NUMBER_SIGN;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return jcanvas::JKS_DOLLAR_SIGN;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return jcanvas::JKS_PERCENT_SIGN;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return jcanvas::JKS_AMPERSAND;
		case ALLEGRO_KEY_QUOTE:
			return jcanvas::JKS_APOSTROPHE;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return jcanvas::JKS_PARENTHESIS_LEFT;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return jcanvas::JKS_PARENTHESIS_RIGHT;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return jcanvas::JKS_STAR;
		// case ALLEGRO_KEY_PLUS:
		// 	return jcanvas::JKS_PLUS_SIGN;
		case ALLEGRO_KEY_COMMA:   
			return jcanvas::JKS_COMMA;
		case ALLEGRO_KEY_MINUS:
			return jcanvas::JKS_MINUS_SIGN;
		case ALLEGRO_KEY_FULLSTOP:  
		 	return jcanvas::JKS_PERIOD;
		case ALLEGRO_KEY_SLASH:
			return jcanvas::JKS_SLASH;
		case ALLEGRO_KEY_0:     
			return jcanvas::JKS_0;
		case ALLEGRO_KEY_1:
			return jcanvas::JKS_1;
		case ALLEGRO_KEY_2:
			return jcanvas::JKS_2;
		case ALLEGRO_KEY_3:
			return jcanvas::JKS_3;
		case ALLEGRO_KEY_4:
			return jcanvas::JKS_4;
		case ALLEGRO_KEY_5:
			return jcanvas::JKS_5;
		case ALLEGRO_KEY_6:
			return jcanvas::JKS_6;
		case ALLEGRO_KEY_7:
			return jcanvas::JKS_7;
		case ALLEGRO_KEY_8:
			return jcanvas::JKS_8;
		case ALLEGRO_KEY_9:
			return jcanvas::JKS_9;
		// case ALLEGRO_KEY_COLON:
		// 	return jcanvas::JKS_COLON;
		case ALLEGRO_KEY_SEMICOLON:
			return jcanvas::JKS_SEMICOLON;
		// case ALLEGRO_KEY_LESS:
		// 	return jcanvas::JKS_LESS_THAN_SIGN;
		case ALLEGRO_KEY_EQUALS: 
			return jcanvas::JKS_EQUALS_SIGN;
		// case ALLEGRO_KEY_GREATER:
		// 	return jcanvas::JKS_GREATER_THAN_SIGN;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return jcanvas::JKS_QUESTION_MARK;
		case ALLEGRO_KEY_AT:
			return jcanvas::JKS_AT;
		case ALLEGRO_KEY_A:
			return jcanvas::JKS_a;
		case ALLEGRO_KEY_B:
			return jcanvas::JKS_b;
		case ALLEGRO_KEY_C:
			return jcanvas::JKS_c;
		case ALLEGRO_KEY_D:
			return jcanvas::JKS_d;
		case ALLEGRO_KEY_E:
			return jcanvas::JKS_e;
		case ALLEGRO_KEY_F:
			return jcanvas::JKS_f;
		case ALLEGRO_KEY_G:
			return jcanvas::JKS_g;
		case ALLEGRO_KEY_H:
			return jcanvas::JKS_h;
		case ALLEGRO_KEY_I:
			return jcanvas::JKS_i;
		case ALLEGRO_KEY_J:
			return jcanvas::JKS_j;
		case ALLEGRO_KEY_K:
			return jcanvas::JKS_k;
		case ALLEGRO_KEY_L:
			return jcanvas::JKS_l;
		case ALLEGRO_KEY_M:
			return jcanvas::JKS_m;
		case ALLEGRO_KEY_N:
			return jcanvas::JKS_n;
		case ALLEGRO_KEY_O:
			return jcanvas::JKS_o;
		case ALLEGRO_KEY_P:
			return jcanvas::JKS_p;
		case ALLEGRO_KEY_Q:
			return jcanvas::JKS_q;
		case ALLEGRO_KEY_R:
			return jcanvas::JKS_r;
		case ALLEGRO_KEY_S:
			return jcanvas::JKS_s;
		case ALLEGRO_KEY_T:
			return jcanvas::JKS_t;
		case ALLEGRO_KEY_U:
			return jcanvas::JKS_u;
		case ALLEGRO_KEY_V:
			return jcanvas::JKS_v;
		case ALLEGRO_KEY_W:
			return jcanvas::JKS_w;
		case ALLEGRO_KEY_X:
			return jcanvas::JKS_x;
		case ALLEGRO_KEY_Y:
			return jcanvas::JKS_y;
		case ALLEGRO_KEY_Z:
			return jcanvas::JKS_z;
		case ALLEGRO_KEY_OPENBRACE:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case ALLEGRO_KEY_BACKSLASH:   
			return jcanvas::JKS_BACKSLASH;
		case ALLEGRO_KEY_CLOSEBRACE:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		// case ALLEGRO_KEY_CARET:
		// 	return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return jcanvas::JKS_UNDERSCORE;
		case ALLEGRO_KEY_BACKQUOTE:
			return jcanvas::JKS_GRAVE_ACCENT;
		// case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
		//	return jcanvas::JKS_CURLY_BRACKET_LEFT;
		// case ALLEGRO_KEY_VERTICAL_BAR:  
		// 	return jcanvas::JKS_VERTICAL_BAR;
		// case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
		// 	return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case ALLEGRO_KEY_TILDE:  
			return jcanvas::JKS_TILDE;
		case ALLEGRO_KEY_DELETE:
			return jcanvas::JKS_DELETE;
		case ALLEGRO_KEY_LEFT:
			return jcanvas::JKS_CURSOR_LEFT;
		case ALLEGRO_KEY_RIGHT:
			return jcanvas::JKS_CURSOR_RIGHT;
		case ALLEGRO_KEY_UP:  
			return jcanvas::JKS_CURSOR_UP;
		case ALLEGRO_KEY_DOWN:
			return jcanvas::JKS_CURSOR_DOWN;
		case ALLEGRO_KEY_INSERT:  
			return jcanvas::JKS_INSERT;
		case ALLEGRO_KEY_HOME:     
			return jcanvas::JKS_HOME;
		case ALLEGRO_KEY_END:
			return jcanvas::JKS_END;
		case ALLEGRO_KEY_PGUP:
			return jcanvas::JKS_PAGE_UP;
		case ALLEGRO_KEY_PGDN:
			return jcanvas::JKS_PAGE_DOWN;
		case ALLEGRO_KEY_PRINTSCREEN:   
			return jcanvas::JKS_PRINT;
		case ALLEGRO_KEY_PAUSE:
			return jcanvas::JKS_PAUSE;
		// case ALLEGRO_KEY_RED:
		// 	return jcanvas::JKS_RED;
		// case ALLEGRO_KEY_GREEN:
		// 	return jcanvas::JKS_GREEN;
		// case ALLEGRO_KEY_YELLOW:
		// 	return jcanvas::JKS_YELLOW;
		// case ALLEGRO_KEY_BLUE:
		// 	return jcanvas::JKS_BLUE;
		case ALLEGRO_KEY_F1:
		 	return jcanvas::JKS_F1;
		case ALLEGRO_KEY_F2:
		 	return jcanvas::JKS_F2;
		case ALLEGRO_KEY_F3:
			return jcanvas::JKS_F3;
		case ALLEGRO_KEY_F4:
			return jcanvas::JKS_F4;
		case ALLEGRO_KEY_F5:
			return jcanvas::JKS_F5;
		case ALLEGRO_KEY_F6:     
			return jcanvas::JKS_F6;
		case ALLEGRO_KEY_F7:    
		 	return jcanvas::JKS_F7;
		case ALLEGRO_KEY_F8:   
			return jcanvas::JKS_F8;
		case ALLEGRO_KEY_F9:  
			return jcanvas::JKS_F9;
		case ALLEGRO_KEY_F10: 
		 	return jcanvas::JKS_F10;
		case ALLEGRO_KEY_F11:
			return jcanvas::JKS_F11;
		case ALLEGRO_KEY_F12:
		 	return jcanvas::JKS_F12;
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
		 	return jcanvas::JKS_SHIFT;
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
		 	return jcanvas::JKS_CONTROL;
		case ALLEGRO_KEY_ALT:
		 	return jcanvas::JKS_ALT;
		case ALLEGRO_KEY_ALTGR:
			return jcanvas::JKS_ALTGR;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return jcanvas::JKS_META;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return jcanvas::JKS_SUPER;
		// case ALLEGRO_KEY_HYPER:
		// 	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
	if (al_init() == false) {
		throw std::runtime_error("Problem to init allegro5");
	}

	al_install_keyboard();
	al_install_mouse();
	
	ALLEGRO_DISPLAY_MODE mode;
	
	if (al_get_display_mode(0, &mode) == nullptr) {
		throw std::runtime_error("Could not get screen mode");
	}

	sg_screen.x = mode.width;
	sg_screen.y = mode.height;

	sg_key_modifiers[ALLEGRO_KEY_LSHIFT] = false;
	sg_key_modifiers[ALLEGRO_KEY_RSHIFT] = false;
	sg_key_modifiers[ALLEGRO_KEY_LCTRL] = false;
	sg_key_modifiers[ALLEGRO_KEY_RCTRL] = false;
	sg_key_modifiers[ALLEGRO_KEY_ALT] = false;
	sg_key_modifiers[ALLEGRO_KEY_ALTGR] = false;
	sg_key_modifiers[ALLEGRO_KEY_LWIN] = false;
	sg_key_modifiers[ALLEGRO_KEY_RWIN] = false;
	sg_key_modifiers[ALLEGRO_KEY_MENU] = false;
	sg_key_modifiers[ALLEGRO_KEY_SCROLLLOCK] = false;
	sg_key_modifiers[ALLEGRO_KEY_NUMLOCK] = false;
	sg_key_modifiers[ALLEGRO_KEY_CAPSLOCK] = false;
  
  sg_quitting = false;
}

static void InternalPaint()
{
	if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
		return;
	}

  jrect_t<int> 
    bounds = sg_jcanvas_window->GetBounds();

  ALLEGRO_LOCKED_REGION 
    *lock = al_lock_bitmap(sg_surface, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
  cairo_surface_t
    *surface = cairo_image_surface_create_for_data(
        (uint8_t *)lock->data, CAIRO_FORMAT_RGB24, bounds.size.x, bounds.size.y, cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, bounds.size.x));

  jcanvas::BufferedImage
    buffer(surface);
  jcanvas::Graphics 
    *g = buffer.GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcanvas::JCF_SRC);

  sg_jcanvas_window->Paint(g);

  g->Flush();

	al_unlock_bitmap(sg_surface);

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(sg_surface, 0, 0, 0);
	al_flip_display();
  
  if (g->IsVerticalSyncEnabled() == true) {
    al_wait_for_vsync();
  }

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	ALLEGRO_EVENT event;
  
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

  if (queue == nullptr) {
    return;
  }

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(sg_display));

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    if (al_get_next_event(queue, &event) == true) {
      al_drop_next_event(queue);

      // al_wait_for_event(queue, &event);

      if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        sg_quitting = true;

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
      } else if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
        // SDL_CaptureMouse(true);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
      } else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(JCS_DEFAULT);

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
      } else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
        InternalPaint();

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_RESIZED));
      } else if (event.type == ALLEGRO_EVENT_DISPLAY_EXPOSE) {
        InternalPaint();
      } else if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = jcanvas::JKM_NONE;

        switch (event.keyboard.keycode) {
          case ALLEGRO_KEY_LSHIFT:
          case ALLEGRO_KEY_RSHIFT:
          case ALLEGRO_KEY_LCTRL:
          case ALLEGRO_KEY_RCTRL:
          case ALLEGRO_KEY_ALT:
          case ALLEGRO_KEY_ALTGR:
          case ALLEGRO_KEY_LWIN:
          case ALLEGRO_KEY_RWIN:
          case ALLEGRO_KEY_MENU:
          case ALLEGRO_KEY_SCROLLLOCK:
          case ALLEGRO_KEY_NUMLOCK:
          case ALLEGRO_KEY_CAPSLOCK:
            sg_key_modifiers[event.keyboard.keycode] = (event.type == ALLEGRO_EVENT_KEY_DOWN)?true:false;
          default:
            break;
        };

        if (sg_key_modifiers[ALLEGRO_KEY_LSHIFT] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if (sg_key_modifiers[ALLEGRO_KEY_RSHIFT] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if (sg_key_modifiers[ALLEGRO_KEY_LCTRL] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if (sg_key_modifiers[ALLEGRO_KEY_RCTRL] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if (sg_key_modifiers[ALLEGRO_KEY_ALT] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if (sg_key_modifiers[ALLEGRO_KEY_ALTGR] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALTGR);
        } else if (sg_key_modifiers[ALLEGRO_KEY_LWIN] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_META);
        } else if (sg_key_modifiers[ALLEGRO_KEY_RWIN] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_META);
        } else if (sg_key_modifiers[ALLEGRO_KEY_CAPSLOCK] == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CAPS_LOCK);
        }

        type = jcanvas::JKT_UNKNOWN;

        if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN) {
          type = jcanvas::JKT_PRESSED;
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.keyboard.keycode);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
        static jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event.mouse.x;
        sg_mouse_y = event.mouse.y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
          type = jcanvas::JMT_MOVED;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
          if (event.mouse.button == 1) {
            button = jcanvas::JMB_BUTTON1;
          } else if (event.mouse.button == 2) {
            button = jcanvas::JMB_BUTTON2;
          } else if (event.mouse.button == 3) {
            button = jcanvas::JMB_BUTTON3;
          }

          if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            type = jcanvas::JMT_PRESSED;
            buttons = (jcanvas::jmouseevent_button_t)(buttons | button);
          } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            type = jcanvas::JMT_RELEASED;
            buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
          }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
          type = jcanvas::JMT_ROTATED;
          mouse_z = event.mouse.dz;
        }

        if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
          al_grab_mouse(sg_display);
        } else {
          al_ungrab_mouse();
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      }
    }
  }

  al_destroy_event_queue(queue);
  
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
	if (sg_surface != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_surface = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  // al_set_new_displaysg_repaint_rate(60);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	al_set_new_window_position(bounds.point.x, bounds.point.y);
	al_set_new_display_option(ALLEGRO_UPDATE_DISPLAY_REGION, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	sg_display = al_create_display(bounds.size.x, bounds.size.y);

	if (sg_display == nullptr) {
		throw std::runtime_error("Cannot create a window");
	}

	sg_surface = al_create_bitmap(bounds.size.x, bounds.size.y);
	
	if (sg_surface == nullptr) {
	  al_destroy_display(sg_display);

		throw std::runtime_error("Cannot get a window's surface");
	}
}

WindowAdapter::~WindowAdapter()
{
  al_destroy_bitmap(sg_surface);
  sg_surface = nullptr;

	al_destroy_display(sg_display);
  sg_display = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  bool enabled = (al_get_display_flags(sg_display) & ALLEGRO_FULLSCREEN_WINDOW) != 0;

	if (enabled == false) {
    sg_previous_bounds = GetBounds();

    al_set_display_flag(sg_display, ALLEGRO_FULLSCREEN_WINDOW, true);
    al_set_display_flag(sg_display, ALLEGRO_GENERATE_EXPOSE_EVENTS, true);
    
    SetBounds({{0, 0}, sg_screen});
	} else {
    al_set_display_flag(sg_display, ALLEGRO_FULLSCREEN_WINDOW, false);
    al_set_display_flag(sg_display, ALLEGRO_GENERATE_EXPOSE_EVENTS, true);
    
    SetBounds({sg_previous_bounds.point, sg_previous_bounds.size});
	}

	sg_repaint = true;
}

void WindowAdapter::SetTitle(std::string title)
{
  sg_title = title;

  al_set_window_title(sg_display, title.c_str());
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
  al_set_display_flag(sg_display, ALLEGRO_FRAMELESS, undecorated);
}

bool WindowAdapter::IsUndecorated()
{
  return (al_get_display_flags(sg_display) & ALLEGRO_FRAMELESS) != 0;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
	al_set_window_position(sg_display, bounds.point.x, bounds.point.y);
	al_resize_display(sg_display, bounds.size.x, bounds.size.y);
 
	if (sg_surface != nullptr) { 
		al_destroy_bitmap(sg_surface);
	}

	sg_surface = al_create_bitmap(bounds.size.x, bounds.size.y);
}

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
	jcanvas::jrect_t<int> t;

  t.size.x = al_get_bitmap_width(sg_surface);
  t.size.y = al_get_bitmap_height(sg_surface);

	al_get_window_position(sg_display, &t.point.x, &t.point.y);

	return t;
}
		
void WindowAdapter::SetResizable(bool resizable)
{
  al_set_display_flag(sg_display, ALLEGRO_RESIZABLE, resizable);
}

bool WindowAdapter::IsResizable()
{
  return (al_get_display_flags(sg_display) & ALLEGRO_RESIZABLE) != 0;
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

	al_set_mouse_xy(sg_display, x, y);
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

	al_get_mouse_cursor_position(&t.x, &t.y);
	
	return t;
}

void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;

  // TODO:: delete and create the window
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
  sg_jcanvas_cursor_enabled = enabled;

	if (sg_jcanvas_cursor_enabled == false) {
		al_hide_mouse_cursor(sg_display);
	} else {
		al_show_mouse_cursor(sg_display);
	}
}

bool WindowAdapter::IsCursorEnabled()
{
	return sg_jcanvas_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  ALLEGRO_SYSTEM_MOUSE_CURSOR type = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;

  if (style == JCS_DEFAULT) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
  } else if (style == JCS_CROSSHAIR) {
  } else if (style == JCS_EAST) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E;
  } else if (style == JCS_WEST) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W;
  } else if (style == JCS_NORTH) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N;
  } else if (style == JCS_SOUTH) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S;
  } else if (style == JCS_HAND) {
  } else if (style == JCS_MOVE) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
  } else if (style == JCS_NS) {
  } else if (style == JCS_WE) {
  } else if (style == JCS_NW_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW;
  } else if (style == JCS_NE_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE;
  } else if (style == JCS_SW_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW;
  } else if (style == JCS_SE_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE;
  } else if (style == JCS_TEXT) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT;
  } else if (style == JCS_WAIT) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY;
  }

  al_set_system_mouse_cursor(sg_display, type);

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	jpoint_t<int> t = shape->GetSize();
	uint32_t data[t.x*t.y];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	if (data == nullptr) {
		return;
	}

	if (sg_jcanvas_cursor_bitmap != nullptr) {
		al_destroy_mouse_cursor(sg_jcanvas_cursor_bitmap);
	}

	ALLEGRO_BITMAP *sprite = al_create_bitmap(t.x, t.y);
	ALLEGRO_LOCKED_REGION *lock = al_lock_bitmap(sprite, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

	int size = t.x*t.y;
	uint8_t *src = (uint8_t *)data;
	uint8_t *dst = (uint8_t *)lock->data;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[2];
		dst[1] = src[1];
		dst[0] = src[0];

		src = src + 4;
		dst = dst + 4;
	}

	al_unlock_bitmap(sprite);

	sg_jcanvas_cursor_bitmap = al_create_mouse_cursor(sprite, hotx, hoty);

	al_set_mouse_cursor(sg_display, sg_jcanvas_cursor_bitmap);
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
	// TODO::
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
