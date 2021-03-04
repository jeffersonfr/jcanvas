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
static jrect_t<int> sg_previous_bounds;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static Image *sg_jcanvas_icon = nullptr;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case ALLEGRO_KEY_ENTER:
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		case ALLEGRO_KEY_BACKSPACE:
			return jkeyevent_symbol_t::Backspace;
		case ALLEGRO_KEY_TAB:
			return jkeyevent_symbol_t::Tab;
		// case ALLEGRO_KEY_CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case ALLEGRO_KEY_ESCAPE:
			return jkeyevent_symbol_t::Escape;
		case ALLEGRO_KEY_SPACE:
			return jkeyevent_symbol_t::Space;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return jkeyevent_symbol_t::ExclamationMark;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return jkeyevent_symbol_t::Quotation;
		// case ALLEGRO_KEY_HASH:
		// 	return jkeyevent_symbol_t::Hash;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return jkeyevent_symbol_t::Dollar;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return jkeyevent_symbol_t::Percent;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return jkeyevent_symbol_t::Ampersand;
		case ALLEGRO_KEY_QUOTE:
			return jkeyevent_symbol_t::Aposthrophe;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return jkeyevent_symbol_t::ParenthesisLeft;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return jkeyevent_symbol_t::ParenthesisRight;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return jkeyevent_symbol_t::Star;
		// case ALLEGRO_KEY_PLUS:
		// 	return jkeyevent_symbol_t::Plus;
		case ALLEGRO_KEY_COMMA:   
			return jkeyevent_symbol_t::Comma;
		case ALLEGRO_KEY_MINUS:
			return jkeyevent_symbol_t::Minus;
		case ALLEGRO_KEY_FULLSTOP:  
		 	return jkeyevent_symbol_t::Period;
		case ALLEGRO_KEY_SLASH:
			return jkeyevent_symbol_t::Slash;
		case ALLEGRO_KEY_0:     
			return jkeyevent_symbol_t::Number0;
		case ALLEGRO_KEY_1:
			return jkeyevent_symbol_t::Number1;
		case ALLEGRO_KEY_2:
			return jkeyevent_symbol_t::Number2;
		case ALLEGRO_KEY_3:
			return jkeyevent_symbol_t::Number3;
		case ALLEGRO_KEY_4:
			return jkeyevent_symbol_t::Number4;
		case ALLEGRO_KEY_5:
			return jkeyevent_symbol_t::Number5;
		case ALLEGRO_KEY_6:
			return jkeyevent_symbol_t::Number6;
		case ALLEGRO_KEY_7:
			return jkeyevent_symbol_t::Number7;
		case ALLEGRO_KEY_8:
			return jkeyevent_symbol_t::Number8;
		case ALLEGRO_KEY_9:
			return jkeyevent_symbol_t::Number9;
		// case ALLEGRO_KEY_COLON:
		// 	return jkeyevent_symbol_t::Colon;
		case ALLEGRO_KEY_SEMICOLON:
			return jkeyevent_symbol_t::SemiColon;
		// case ALLEGRO_KEY_LESS:
		// 	return jkeyevent_symbol_t::LessThan;
		case ALLEGRO_KEY_EQUALS: 
			return jkeyevent_symbol_t::Equals;
		// case ALLEGRO_KEY_GREATER:
		// 	return jkeyevent_symbol_t::GreaterThan;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return jkeyevent_symbol_t::QuestionMark;
		case ALLEGRO_KEY_AT:
			return jkeyevent_symbol_t::At;
		case ALLEGRO_KEY_A:
			return jkeyevent_symbol_t::a;
		case ALLEGRO_KEY_B:
			return jkeyevent_symbol_t::b;
		case ALLEGRO_KEY_C:
			return jkeyevent_symbol_t::c;
		case ALLEGRO_KEY_D:
			return jkeyevent_symbol_t::d;
		case ALLEGRO_KEY_E:
			return jkeyevent_symbol_t::e;
		case ALLEGRO_KEY_F:
			return jkeyevent_symbol_t::f;
		case ALLEGRO_KEY_G:
			return jkeyevent_symbol_t::g;
		case ALLEGRO_KEY_H:
			return jkeyevent_symbol_t::h;
		case ALLEGRO_KEY_I:
			return jkeyevent_symbol_t::i;
		case ALLEGRO_KEY_J:
			return jkeyevent_symbol_t::j;
		case ALLEGRO_KEY_K:
			return jkeyevent_symbol_t::k;
		case ALLEGRO_KEY_L:
			return jkeyevent_symbol_t::l;
		case ALLEGRO_KEY_M:
			return jkeyevent_symbol_t::m;
		case ALLEGRO_KEY_N:
			return jkeyevent_symbol_t::n;
		case ALLEGRO_KEY_O:
			return jkeyevent_symbol_t::o;
		case ALLEGRO_KEY_P:
			return jkeyevent_symbol_t::p;
		case ALLEGRO_KEY_Q:
			return jkeyevent_symbol_t::q;
		case ALLEGRO_KEY_R:
			return jkeyevent_symbol_t::r;
		case ALLEGRO_KEY_S:
			return jkeyevent_symbol_t::s;
		case ALLEGRO_KEY_T:
			return jkeyevent_symbol_t::t;
		case ALLEGRO_KEY_U:
			return jkeyevent_symbol_t::u;
		case ALLEGRO_KEY_V:
			return jkeyevent_symbol_t::v;
		case ALLEGRO_KEY_W:
			return jkeyevent_symbol_t::w;
		case ALLEGRO_KEY_X:
			return jkeyevent_symbol_t::x;
		case ALLEGRO_KEY_Y:
			return jkeyevent_symbol_t::y;
		case ALLEGRO_KEY_Z:
			return jkeyevent_symbol_t::z;
		case ALLEGRO_KEY_OPENBRACE:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case ALLEGRO_KEY_BACKSLASH:   
			return jkeyevent_symbol_t::BackSlash;
		case ALLEGRO_KEY_CLOSEBRACE:
			return jkeyevent_symbol_t::SquareBracketRight;
		// case ALLEGRO_KEY_CARET:
		// 	return jkeyevent_symbol_t::CircumflexAccent;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return jkeyevent_symbol_t::Underscore;
		case ALLEGRO_KEY_BACKQUOTE:
			return jkeyevent_symbol_t::GraveAccent;
		// case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
		//	return jkeyevent_symbol_t::CurlyBracketLeft;
		// case ALLEGRO_KEY_VERTICAL_BAR:  
		// 	return jkeyevent_symbol_t::VerticalBar;
		// case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
		// 	return jkeyevent_symbol_t::CurlyBracketRight;
		case ALLEGRO_KEY_TILDE:  
			return jkeyevent_symbol_t::Tilde;
		case ALLEGRO_KEY_DELETE:
			return jkeyevent_symbol_t::Delete;
		case ALLEGRO_KEY_LEFT:
			return jkeyevent_symbol_t::CursorLeft;
		case ALLEGRO_KEY_RIGHT:
			return jkeyevent_symbol_t::CursorRight;
		case ALLEGRO_KEY_UP:  
			return jkeyevent_symbol_t::CursorUp;
		case ALLEGRO_KEY_DOWN:
			return jkeyevent_symbol_t::CursorDown;
		case ALLEGRO_KEY_INSERT:  
			return jkeyevent_symbol_t::Insert;
		case ALLEGRO_KEY_HOME:     
			return jkeyevent_symbol_t::Home;
		case ALLEGRO_KEY_END:
			return jkeyevent_symbol_t::End;
		case ALLEGRO_KEY_PGUP:
			return jkeyevent_symbol_t::PageUp;
		case ALLEGRO_KEY_PGDN:
			return jkeyevent_symbol_t::PageDown;
		case ALLEGRO_KEY_PRINTSCREEN:   
			return jkeyevent_symbol_t::Print;
		case ALLEGRO_KEY_PAUSE:
			return jkeyevent_symbol_t::Pause;
		// case ALLEGRO_KEY_RED:
		// 	return jkeyevent_symbol_t::Red;
		// case ALLEGRO_KEY_GREEN:
		// 	return jkeyevent_symbol_t::Green;
		// case ALLEGRO_KEY_YELLOW:
		// 	return jkeyevent_symbol_t::Yellow;
		// case ALLEGRO_KEY_BLUE:
		// 	return jkeyevent_symbol_t::Blue;
		case ALLEGRO_KEY_F1:
		 	return jkeyevent_symbol_t::F1;
		case ALLEGRO_KEY_F2:
		 	return jkeyevent_symbol_t::F2;
		case ALLEGRO_KEY_F3:
			return jkeyevent_symbol_t::F3;
		case ALLEGRO_KEY_F4:
			return jkeyevent_symbol_t::F4;
		case ALLEGRO_KEY_F5:
			return jkeyevent_symbol_t::F5;
		case ALLEGRO_KEY_F6:     
			return jkeyevent_symbol_t::F6;
		case ALLEGRO_KEY_F7:    
		 	return jkeyevent_symbol_t::F7;
		case ALLEGRO_KEY_F8:   
			return jkeyevent_symbol_t::F8;
		case ALLEGRO_KEY_F9:  
			return jkeyevent_symbol_t::F9;
		case ALLEGRO_KEY_F10: 
		 	return jkeyevent_symbol_t::F10;
		case ALLEGRO_KEY_F11:
			return jkeyevent_symbol_t::F11;
		case ALLEGRO_KEY_F12:
		 	return jkeyevent_symbol_t::F12;
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
		 	return jkeyevent_symbol_t::Shift;
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
		 	return jkeyevent_symbol_t::Control;
		case ALLEGRO_KEY_ALT:
		 	return jkeyevent_symbol_t::Alt;
		case ALLEGRO_KEY_ALTGR:
			return jkeyevent_symbol_t::AltGr;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return jkeyevent_symbol_t::Meta;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return jkeyevent_symbol_t::Super;
		// case ALLEGRO_KEY_HYPER:
		// 	return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
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

  BufferedImage
    buffer(surface);
  Graphics 
    *g = buffer.GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcomposite_t::Src);

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

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
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

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
      } else if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
        // SDL_CaptureMouse(true);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
      } else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(jcursor_style_t::Default);

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
      } else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
        InternalPaint();

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Resized));
      } else if (event.type == ALLEGRO_EVENT_DISPLAY_EXPOSE) {
        InternalPaint();
      } else if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

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
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if (sg_key_modifiers[ALLEGRO_KEY_RSHIFT] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if (sg_key_modifiers[ALLEGRO_KEY_LCTRL] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if (sg_key_modifiers[ALLEGRO_KEY_RCTRL] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if (sg_key_modifiers[ALLEGRO_KEY_ALT] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if (sg_key_modifiers[ALLEGRO_KEY_ALTGR] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
        } else if (sg_key_modifiers[ALLEGRO_KEY_LWIN] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        } else if (sg_key_modifiers[ALLEGRO_KEY_RWIN] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        } else if (sg_key_modifiers[ALLEGRO_KEY_CAPSLOCK] == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::CapsLock);
        }

        if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN) {
          type = jkeyevent_type_t::Pressed;
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.keyboard.keycode);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
        static jmouseevent_button_t buttons = jmouseevent_button_t::None;

        jmouseevent_button_t button = jmouseevent_button_t::None;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        sg_mouse_x = event.mouse.x;
        sg_mouse_y = event.mouse.y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
          type = jmouseevent_type_t::Moved;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
          if (event.mouse.button == 1) {
            button = jmouseevent_button_t::Button1;
          } else if (event.mouse.button == 2) {
            button = jmouseevent_button_t::Button2;
          } else if (event.mouse.button == 3) {
            button = jmouseevent_button_t::Button3;
          }

          if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            type = jmouseevent_type_t::Pressed;
            buttons = jenum_t<jmouseevent_button_t>{buttons}.Or(button);
          } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            type = jmouseevent_type_t::Released;
            buttons = jenum_t<jmouseevent_button_t>{buttons}.And(jenum_t<jmouseevent_button_t>{button}.Not());
          }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
          type = jmouseevent_type_t::Rotated;
          mouse_z = event.mouse.dz;
        }

        if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jmouseevent_button_t::None) {
          al_grab_mouse(sg_display);
        } else {
          al_ungrab_mouse();
        }

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
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

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
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

jrect_t<int> WindowAdapter::GetBounds()
{
	jrect_t<int> t;

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

  if (style == jcursor_style_t::Default) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
  } else if (style == jcursor_style_t::Crosshair) {
  } else if (style == jcursor_style_t::East) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E;
  } else if (style == jcursor_style_t::West) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W;
  } else if (style == jcursor_style_t::North) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N;
  } else if (style == jcursor_style_t::South) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S;
  } else if (style == jcursor_style_t::Hand) {
  } else if (style == jcursor_style_t::Move) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
  } else if (style == jcursor_style_t::Vertical) {
  } else if (style == jcursor_style_t::Horizontal) {
  } else if (style == jcursor_style_t::NorthWest) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW;
  } else if (style == jcursor_style_t::NorthEast) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE;
  } else if (style == jcursor_style_t::SouthWest) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW;
  } else if (style == jcursor_style_t::SouthEast) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE;
  } else if (style == jcursor_style_t::Text) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT;
  } else if (style == jcursor_style_t::Wait) {
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
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(Image *image)
{
  sg_jcanvas_icon = image;
}

Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
