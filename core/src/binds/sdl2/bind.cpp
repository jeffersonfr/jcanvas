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

#include <SDL2/SDL.h>

namespace jcanvas {

#if SDL_VERSION_ATLEAST(2,0,5)
  // USE::
#endif

/** \brief */
std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static SDL_Window *sg_window = nullptr;
/** \brief */
static SDL_Renderer *sg_renderer = nullptr;
/** \brief */
jmouseevent_button_t sg_button_state = jmouseevent_button_t::None;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol)
{
	switch (symbol.sym) {
		case SDLK_RETURN:
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		case SDLK_BACKSPACE:
			return jkeyevent_symbol_t::Backspace;
		case SDLK_TAB:
			return jkeyevent_symbol_t::Tab;
		// case SDLK_CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case SDLK_ESCAPE:
			return jkeyevent_symbol_t::Escape;
		case SDLK_SPACE:
			return jkeyevent_symbol_t::Space;
		case SDLK_EXCLAIM:
			return jkeyevent_symbol_t::ExclamationMark;
		case SDLK_QUOTEDBL:
			return jkeyevent_symbol_t::Quotation;
		case SDLK_HASH:
			return jkeyevent_symbol_t::Hash;
		case SDLK_DOLLAR:
			return jkeyevent_symbol_t::Dollar;
		// case SDLK_PERCENT_SIGN:
		//	return jkeyevent_symbol_t::Percent;
		case SDLK_AMPERSAND:   
			return jkeyevent_symbol_t::Ampersand;
		case SDLK_QUOTE:
			return jkeyevent_symbol_t::Aposthrophe;
		case SDLK_LEFTPAREN:
			return jkeyevent_symbol_t::ParenthesisLeft;
		case SDLK_RIGHTPAREN:
			return jkeyevent_symbol_t::ParenthesisRight;
		case SDLK_ASTERISK:
			return jkeyevent_symbol_t::Star;
		case SDLK_PLUS:
			return jkeyevent_symbol_t::Plus;
		case SDLK_COMMA:   
			return jkeyevent_symbol_t::Comma;
		case SDLK_MINUS:
			return jkeyevent_symbol_t::Minus;
		case SDLK_PERIOD:  
			return jkeyevent_symbol_t::Period;
		case SDLK_SLASH:
			return jkeyevent_symbol_t::Slash;
		case SDLK_0:     
			return jkeyevent_symbol_t::Number0;
		case SDLK_1:
			return jkeyevent_symbol_t::Number1;
		case SDLK_2:
			return jkeyevent_symbol_t::Number2;
		case SDLK_3:
			return jkeyevent_symbol_t::Number3;
		case SDLK_4:
			return jkeyevent_symbol_t::Number4;
		case SDLK_5:
			return jkeyevent_symbol_t::Number5;
		case SDLK_6:
			return jkeyevent_symbol_t::Number6;
		case SDLK_7:
			return jkeyevent_symbol_t::Number7;
		case SDLK_8:
			return jkeyevent_symbol_t::Number8;
		case SDLK_9:
			return jkeyevent_symbol_t::Number9;
		case SDLK_COLON:
			return jkeyevent_symbol_t::Colon;
		case SDLK_SEMICOLON:
			return jkeyevent_symbol_t::SemiColon;
		case SDLK_LESS:
			return jkeyevent_symbol_t::LessThan;
		case SDLK_EQUALS: 
			return jkeyevent_symbol_t::Equals;
		case SDLK_GREATER:
			return jkeyevent_symbol_t::GreaterThan;
		case SDLK_QUESTION:   
			return jkeyevent_symbol_t::QuestionMark;
		case SDLK_AT:
			return jkeyevent_symbol_t::At;
			/*
		case SDLK_CAPITAL_A:
			return jkeyevent_symbol_t::A;
		case SDLK_CAPITAL_B:
			return jkeyevent_symbol_t::B;
		case SDLK_CAPITAL_C:
			return jkeyevent_symbol_t::C;
		case SDLK_CAPITAL_D:
			return jkeyevent_symbol_t::D;
		case SDLK_CAPITAL_E:
			return jkeyevent_symbol_t::E;
		case SDLK_CAPITAL_F:
			return jkeyevent_symbol_t::F;
		case SDLK_CAPITAL_G:
			return jkeyevent_symbol_t::G;
		case SDLK_CAPITAL_H:
			return jkeyevent_symbol_t::H;
		case SDLK_CAPITAL_I:
			return jkeyevent_symbol_t::I;
		case SDLK_CAPITAL_J:
			return jkeyevent_symbol_t::J;
		case SDLK_CAPITAL_K:
			return jkeyevent_symbol_t::K;
		case SDLK_CAPITAL_L:
			return jkeyevent_symbol_t::L;
		case SDLK_CAPITAL_M:
			return jkeyevent_symbol_t::M;
		case SDLK_CAPITAL_N:
			return jkeyevent_symbol_t::N;
		case SDLK_CAPITAL_O:
			return jkeyevent_symbol_t::O;
		case SDLK_CAPITAL_P:
			return jkeyevent_symbol_t::P;
		case SDLK_CAPITAL_Q:
			return jkeyevent_symbol_t::Q;
		case SDLK_CAPITAL_R:
			return jkeyevent_symbol_t::R;
		case SDLK_CAPITAL_S:
			return jkeyevent_symbol_t::S;
		case SDLK_CAPITAL_T:
			return jkeyevent_symbol_t::T;
		case SDLK_CAPITAL_U:
			return jkeyevent_symbol_t::U;
		case SDLK_CAPITAL_V:
			return jkeyevent_symbol_t::V;
		case SDLK_CAPITAL_W:
			return jkeyevent_symbol_t::W;
		case SDLK_CAPITAL_X:
			return jkeyevent_symbol_t::X;
		case SDLK_CAPITAL_Y:
			return jkeyevent_symbol_t::Y;
		case SDLK_CAPITAL_Z:
			return jkeyevent_symbol_t::Z;
			*/
		case SDLK_LEFTBRACKET:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case SDLK_BACKSLASH:   
			return jkeyevent_symbol_t::BackSlash;
		case SDLK_RIGHTBRACKET:
			return jkeyevent_symbol_t::SquareBracketRight;
		case SDLK_CARET:
			return jkeyevent_symbol_t::CircumflexAccent;
		case SDLK_UNDERSCORE:    
			return jkeyevent_symbol_t::Underscore;
		case SDLK_BACKQUOTE:
			return jkeyevent_symbol_t::GraveAccent;
    case SDLK_a:
			return jkeyevent_symbol_t::a;
    case SDLK_b:
			return jkeyevent_symbol_t::b;
    case SDLK_c:
			return jkeyevent_symbol_t::c;
    case SDLK_d:
			return jkeyevent_symbol_t::d;
    case SDLK_e:
			return jkeyevent_symbol_t::e;
    case SDLK_f:
			return jkeyevent_symbol_t::f;
    case SDLK_g:
			return jkeyevent_symbol_t::g;
    case SDLK_h:
			return jkeyevent_symbol_t::h;
    case SDLK_i:
			return jkeyevent_symbol_t::i;
    case SDLK_j:
			return jkeyevent_symbol_t::j;
    case SDLK_k:
			return jkeyevent_symbol_t::k;
    case SDLK_l:
			return jkeyevent_symbol_t::l;
    case SDLK_m:
			return jkeyevent_symbol_t::m;
    case SDLK_n:
			return jkeyevent_symbol_t::n;
    case SDLK_o:
			return jkeyevent_symbol_t::o;
    case SDLK_p:
			return jkeyevent_symbol_t::p;
    case SDLK_q:
			return jkeyevent_symbol_t::q;
    case SDLK_r:
			return jkeyevent_symbol_t::r;
    case SDLK_s:
			return jkeyevent_symbol_t::s;
    case SDLK_t:
			return jkeyevent_symbol_t::t;
    case SDLK_u:
			return jkeyevent_symbol_t::u;
    case SDLK_v:
			return jkeyevent_symbol_t::v;
    case SDLK_w:
			return jkeyevent_symbol_t::w;
    case SDLK_x:
			return jkeyevent_symbol_t::x;
    case SDLK_y:
			return jkeyevent_symbol_t::y;
    case SDLK_z:
			return jkeyevent_symbol_t::z;
		// case SDLK_CURLY_BRACKET_LEFT:
		//	return jkeyevent_symbol_t::CurlyBracketLeft;
		// case SDLK_VERTICAL_BAR:  
		//	return jkeyevent_symbol_t::VerticalBar;
		// case SDLK_CURLY_BRACKET_RIGHT:
		//	return jkeyevent_symbol_t::CurlyBracketRight;
		// case SDLK_TILDE:  
		//	return jkeyevent_symbol_t::Tilde;
		case SDLK_DELETE:
			return jkeyevent_symbol_t::Delete;
		case SDLK_LEFT:
			return jkeyevent_symbol_t::CursorLeft;
		case SDLK_RIGHT:
			return jkeyevent_symbol_t::CursorRight;
		case SDLK_UP:  
			return jkeyevent_symbol_t::CursorUp;
		case SDLK_DOWN:
			return jkeyevent_symbol_t::CursorDown;
		case SDLK_INSERT:  
			return jkeyevent_symbol_t::Insert;
		case SDLK_HOME:     
			return jkeyevent_symbol_t::Home;
		case SDLK_END:
			return jkeyevent_symbol_t::End;
		case SDLK_PAGEUP:
			return jkeyevent_symbol_t::PageUp;
		case SDLK_PAGEDOWN:
			return jkeyevent_symbol_t::PageDown;
		// case SDLK_PRINT:   
		//	return jkeyevent_symbol_t::Print;
		case SDLK_PAUSE:
			return jkeyevent_symbol_t::Pause;
		// case SDLK_RED:
		//	return jkeyevent_symbol_t::Red;
		// case SDLK_GREEN:
		//	return jkeyevent_symbol_t::Green;
		// case SDLK_YELLOW:
		//	return jkeyevent_symbol_t::Yellow;
		// case SDLK_BLUE:
		//	return jkeyevent_symbol_t::Blue;
		case SDLK_F1:
			return jkeyevent_symbol_t::F1;
		case SDLK_F2:
			return jkeyevent_symbol_t::F2;
		case SDLK_F3:
			return jkeyevent_symbol_t::F3;
		case SDLK_F4:
			return jkeyevent_symbol_t::F4;
		case SDLK_F5:
			return jkeyevent_symbol_t::F5;
		case SDLK_F6:     
			return jkeyevent_symbol_t::F6;
		case SDLK_F7:    
			return jkeyevent_symbol_t::F7;
		case SDLK_F8:   
			return jkeyevent_symbol_t::F8;
		case SDLK_F9:  
			return jkeyevent_symbol_t::F9;
		case SDLK_F10: 
			return jkeyevent_symbol_t::F10;
		case SDLK_F11:
			return jkeyevent_symbol_t::F11;
		case SDLK_F12:
			return jkeyevent_symbol_t::F12;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return jkeyevent_symbol_t::Shift;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return jkeyevent_symbol_t::Control;
		case SDLK_LALT:
		case SDLK_RALT:
			return jkeyevent_symbol_t::Alt;
		// case SDLK_ALTGR:
		//	return jkeyevent_symbol_t::AltGr;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jkeyevent_symbol_t::Meta;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jkeyevent_symbol_t::Super;
		// case SDLK_HYPER:
		//	return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

void Application::Init(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING)) {  
		throw std::runtime_error("Problem to init SDL2");
	}

	SDL_DisplayMode display;

	if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
    // TODO:: release sdl

		throw std::runtime_error("Could not get screen mode");
	}

	sg_screen.x = display.w;
	sg_screen.y = display.h;
  
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
    
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, bounds.size.x, bounds.size.y, 32, bounds.size.x*4, 0, 0, 0, 0);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(sg_renderer, surface);
  
  sg_back_buffer->UnlockData();
  
  // SDL_RenderClear(sg_renderer);
  SDL_RenderCopy(sg_renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(sg_renderer);
  
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	SDL_Event event;

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

    while (SDL_PollEvent(&event)) {
    // while (SDL_WaitEventTimeout(&event, 100)) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_ENTER) {
          // SDL_CaptureMouse(true);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(GetCursor());

          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
        } else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
          // SDL_CaptureMouse(false);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(jcursor_style_t::Default);

          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
        } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Opened));
        } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
        } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          InternalPaint();
        } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Moved));
        } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          InternalPaint();
        
          sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Resized));
        } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        }
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

        if ((event.key.keysym.mod & KMOD_LSHIFT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if ((event.key.keysym.mod & KMOD_RSHIFT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((event.key.keysym.mod & KMOD_LGUI) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
        } else if ((event.key.keysym.mod & KMOD_RGUI) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
        } else if ((event.key.keysym.mod & KMOD_MODE) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
        } else if ((event.key.keysym.mod & KMOD_CAPS) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::CapsLock);
        // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
          // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
          // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        // } else if ((event.key.keysym.mod & ) != 0) {
          // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
        // } else if ((event.key.keysym.mod & ) != 0) {
          // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
        // } else if ((event.key.keysym.mod & ) != 0) {
        }

        if (event.key.state == SDL_PRESSED) {
          type = jkeyevent_type_t::Pressed;
        } else if (event.key.state == SDL_RELEASED) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        if (event.type == SDL_MOUSEMOTION) {
          // e.motion.x/y
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
          // e.button.button == SDL_BUTTON_LEFT
        } else if (event.type == SDL_MOUSEBUTTONUP) {
        } else if (event.type == SDL_MOUSEWHEEL) {
        }

        jmouseevent_button_t button = jmouseevent_button_t::None;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        if (event.type == SDL_MOUSEMOTION) {
          type = jmouseevent_type_t::Moved;
        
          sg_mouse_x = CLAMP(event.motion.x, 0, sg_screen.x-1);
          sg_mouse_y = CLAMP(event.motion.y, 0, sg_screen.y-1);
        } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
          if (event.button.button == SDL_BUTTON_LEFT) {
            button = jmouseevent_button_t::Button1;
          } else if (event.button.button == SDL_BUTTON_MIDDLE) {
            button = jmouseevent_button_t::Button2;
          } else if (event.button.button == SDL_BUTTON_RIGHT) {
            button = jmouseevent_button_t::Button3;
          }
          
          if (event.type == SDL_MOUSEBUTTONDOWN) {
            sg_button_state = jenum_t<jmouseevent_button_t>{sg_button_state}.Or(button);
            type = jmouseevent_type_t::Pressed;
          } else if (event.type == SDL_MOUSEBUTTONUP) {
            sg_button_state = jenum_t<jmouseevent_button_t>{sg_button_state}.And(jenum_t<jmouseevent_button_t>{button}.Not());
            type = jmouseevent_type_t::Released;
          }
        } else if (event.type == SDL_MOUSEWHEEL) {
          type = jmouseevent_type_t::Rotated;
          mouse_z = event.wheel.y;
        }

        if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && sg_button_state != jmouseevent_button_t::None) {
          SDL_CaptureMouse(SDL_TRUE);
        } else {
          SDL_CaptureMouse(SDL_FALSE);
        }
        
        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(sg_window);

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));

        break;
      }
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

void Application::SetVerticalSyncEnabled(bool enabled)
{
}

bool Application::IsVerticalSyncEnabled()
{
  return true;
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	if (sg_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

	int 
    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	
	// INFO:: create the main window
	sg_window = SDL_CreateWindow("Main", bounds.point.x, bounds.point.y, bounds.size.x, bounds.size.y, flags);

	if (sg_window == nullptr) {
		throw std::runtime_error("Cannot create a window");
	}

	sg_renderer = SDL_CreateRenderer(sg_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// sg_renderer = SDL_CreateRenderer(sg_window, 0, SDL_RENDERER_SOFTWARE);

	if (sg_renderer == nullptr) {
		throw std::runtime_error("Cannot get a window's surface");
	}

	// SDL_SetWindowBordered(sg_window, SDL_FALSE);

	SDL_SetWindowMinimumSize(sg_window, 16, 16);
	SDL_SetWindowMaximumSize(sg_window, 16000, 16000);
	
  // (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_SHOWN);
  SDL_ShowWindow(sg_window);
}

WindowAdapter::~WindowAdapter()
{
  SDL_DestroyRenderer(sg_renderer);
  SDL_DestroyWindow(sg_window);
	SDL_Quit();

  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  if (SDL_GetWindowFlags(sg_window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
    SDL_SetWindowFullscreen(sg_window, 0);
  } else {
    SDL_SetWindowFullscreen(sg_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    // SDL_SetWindowFullscreen(sg_window, SDL_WINDOW_FULLSCREEN);
  }
}

void WindowAdapter::SetTitle(std::string title)
{
	SDL_SetWindowTitle(sg_window, title.c_str());
}

std::string WindowAdapter::GetTitle()
{
	return std::string(SDL_GetWindowTitle(sg_window));
}

void WindowAdapter::SetOpacity(float opacity)
{
	// SDL_SetWindowOpacity(sg_window, opacity);
}

float WindowAdapter::GetOpacity()
{
  /*
  float opacity;

	if (SDL_SetWindowOpacity(sg_window, &opacity) == 0) {
    return opacity;
  }
  */

	return 1.0;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
	if (undecorated == true) {
		SDL_SetWindowBordered(sg_window, SDL_FALSE);
	} else {
		SDL_SetWindowBordered(sg_window, SDL_TRUE);
	}
}

bool WindowAdapter::IsUndecorated()
{
  return (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_BORDERLESS);
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
  SDL_SetWindowPosition(sg_window, bounds.point.x, bounds.point.y);
  SDL_SetWindowSize(sg_window, bounds.size.x, bounds.size.y);
}

jrect_t<int> WindowAdapter::GetBounds()
{
	jrect_t<int> t;

  SDL_GetWindowPosition(sg_window, &t.point.x, &t.point.y);
  SDL_GetWindowSize(sg_window, &t.size.x, &t.size.y);

	return t;
}
		
void WindowAdapter::SetResizable(bool resizable)
{
  SDL_SetWindowResizable(sg_window, (SDL_bool)resizable);
}

bool WindowAdapter::IsResizable()
{
  return (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_RESIZABLE);
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
	SDL_WarpMouseInWindow(sg_window, x, y);
	// SDL_WarpMouseGlobal(x, y);
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	SDL_GetMouseState(&p.x, &p.y);
	// SDL_GetGlobalMouseState(&p.x, &p.y);

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
	if (visible == true) {
    SDL_ShowWindow(sg_window);
	} else {
    SDL_HideWindow(sg_window);
  }
}

bool WindowAdapter::IsVisible()
{
  return (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_SHOWN);
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
	SDL_ShowCursor((enabled == false)?SDL_DISABLE:SDL_ENABLE);
}

bool WindowAdapter::IsCursorEnabled()
{
	return (bool)SDL_ShowCursor(SDL_QUERY);
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  SDL_SystemCursor type = SDL_SYSTEM_CURSOR_ARROW;

  if (style == jcursor_style_t::Default) {
    type = SDL_SYSTEM_CURSOR_ARROW;
  } else if (style == jcursor_style_t::Crosshair) {
    type = SDL_SYSTEM_CURSOR_CROSSHAIR;
  } else if (style == jcursor_style_t::East) {
  } else if (style == jcursor_style_t::West) {
  } else if (style == jcursor_style_t::North) {
  } else if (style == jcursor_style_t::South) {
  } else if (style == jcursor_style_t::Hand) {
    type = SDL_SYSTEM_CURSOR_HAND;
  } else if (style == jcursor_style_t::Move) {
    type = SDL_SYSTEM_CURSOR_SIZEALL;
  } else if (style == jcursor_style_t::Vertical) {
    type = SDL_SYSTEM_CURSOR_SIZENS;
  } else if (style == jcursor_style_t::Horizontal) {
    type = SDL_SYSTEM_CURSOR_SIZEWE;
  } else if (style == jcursor_style_t::NorthWest) {
  } else if (style == jcursor_style_t::NorthEast) {
  } else if (style == jcursor_style_t::SouthWest) {
  } else if (style == jcursor_style_t::SouthEast) {
  } else if (style == jcursor_style_t::Text) {
    type = SDL_SYSTEM_CURSOR_IBEAM;
  } else if (style == jcursor_style_t::Wait) {
    type = SDL_SYSTEM_CURSOR_WAIT;
  }

  SDL_Cursor
    *cursor = SDL_CreateSystemCursor(type);

  SDL_SetCursor(cursor);
  // TODO:: SDL_FreeCursor(cursor);

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

	jpoint_t<int> 
    t = shape->GetSize();
	uint32_t 
    data[t.x*t.y];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	SDL_Surface 
    *surface = SDL_CreateRGBSurfaceFrom(data, t.x, t.y, 32, t.x*4, 0, 0, 0, 0);

	if (surface == nullptr) {
		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
	// TODO::
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(std::shared_ptr<Image> image)
{
  if (image == nullptr) {
    return;
  }

  jpoint_t<int> 
    size = image->GetSize();
  uint32_t 
    *data = (uint32_t *)image->LockData();

  sg_jcanvas_icon = image;

  SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(data, size.x, size.y, 32, size.x*4, 0, 0, 0, 0);

  if (nullptr == icon) {
    return;
  }

  SDL_SetWindowIcon(sg_window, icon);
  SDL_FreeSurface(icon);
    
  image->UnlockData();
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
