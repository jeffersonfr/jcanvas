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

#include <SDL2/SDL.h>

namespace jcanvas {

#if SDL_VERSION_ATLEAST(2,0,5)
  // USE::
#endif

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static SDL_Window *sg_window = nullptr;
/** \brief */
static SDL_Renderer *sg_renderer = nullptr;
/** \brief */
jcanvas::jmouseevent_button_t sg_button_state = jcanvas::JMB_NONE;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol)
{
	switch (symbol.sym) {
		case SDLK_RETURN:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case SDLK_BACKSPACE:
			return jcanvas::JKS_BACKSPACE;
		case SDLK_TAB:
			return jcanvas::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case SDLK_ESCAPE:
			return jcanvas::JKS_ESCAPE;
		case SDLK_SPACE:
			return jcanvas::JKS_SPACE;
		case SDLK_EXCLAIM:
			return jcanvas::JKS_EXCLAMATION_MARK;
		case SDLK_QUOTEDBL:
			return jcanvas::JKS_QUOTATION;
		case SDLK_HASH:
			return jcanvas::JKS_NUMBER_SIGN;
		case SDLK_DOLLAR:
			return jcanvas::JKS_DOLLAR_SIGN;
		// case SDLK_PERCENT_SIGN:
		//	return jcanvas::JKS_PERCENT_SIGN;
		case SDLK_AMPERSAND:   
			return jcanvas::JKS_AMPERSAND;
		case SDLK_QUOTE:
			return jcanvas::JKS_APOSTROPHE;
		case SDLK_LEFTPAREN:
			return jcanvas::JKS_PARENTHESIS_LEFT;
		case SDLK_RIGHTPAREN:
			return jcanvas::JKS_PARENTHESIS_RIGHT;
		case SDLK_ASTERISK:
			return jcanvas::JKS_STAR;
		case SDLK_PLUS:
			return jcanvas::JKS_PLUS_SIGN;
		case SDLK_COMMA:   
			return jcanvas::JKS_COMMA;
		case SDLK_MINUS:
			return jcanvas::JKS_MINUS_SIGN;
		case SDLK_PERIOD:  
			return jcanvas::JKS_PERIOD;
		case SDLK_SLASH:
			return jcanvas::JKS_SLASH;
		case SDLK_0:     
			return jcanvas::JKS_0;
		case SDLK_1:
			return jcanvas::JKS_1;
		case SDLK_2:
			return jcanvas::JKS_2;
		case SDLK_3:
			return jcanvas::JKS_3;
		case SDLK_4:
			return jcanvas::JKS_4;
		case SDLK_5:
			return jcanvas::JKS_5;
		case SDLK_6:
			return jcanvas::JKS_6;
		case SDLK_7:
			return jcanvas::JKS_7;
		case SDLK_8:
			return jcanvas::JKS_8;
		case SDLK_9:
			return jcanvas::JKS_9;
		case SDLK_COLON:
			return jcanvas::JKS_COLON;
		case SDLK_SEMICOLON:
			return jcanvas::JKS_SEMICOLON;
		case SDLK_LESS:
			return jcanvas::JKS_LESS_THAN_SIGN;
		case SDLK_EQUALS: 
			return jcanvas::JKS_EQUALS_SIGN;
		case SDLK_GREATER:
			return jcanvas::JKS_GREATER_THAN_SIGN;
		case SDLK_QUESTION:   
			return jcanvas::JKS_QUESTION_MARK;
		case SDLK_AT:
			return jcanvas::JKS_AT;
			/*
		case SDLK_CAPITAL_A:
			return jcanvas::JKS_A;
		case SDLK_CAPITAL_B:
			return jcanvas::JKS_B;
		case SDLK_CAPITAL_C:
			return jcanvas::JKS_C;
		case SDLK_CAPITAL_D:
			return jcanvas::JKS_D;
		case SDLK_CAPITAL_E:
			return jcanvas::JKS_E;
		case SDLK_CAPITAL_F:
			return jcanvas::JKS_F;
		case SDLK_CAPITAL_G:
			return jcanvas::JKS_G;
		case SDLK_CAPITAL_H:
			return jcanvas::JKS_H;
		case SDLK_CAPITAL_I:
			return jcanvas::JKS_I;
		case SDLK_CAPITAL_J:
			return jcanvas::JKS_J;
		case SDLK_CAPITAL_K:
			return jcanvas::JKS_K;
		case SDLK_CAPITAL_L:
			return jcanvas::JKS_L;
		case SDLK_CAPITAL_M:
			return jcanvas::JKS_M;
		case SDLK_CAPITAL_N:
			return jcanvas::JKS_N;
		case SDLK_CAPITAL_O:
			return jcanvas::JKS_O;
		case SDLK_CAPITAL_P:
			return jcanvas::JKS_P;
		case SDLK_CAPITAL_Q:
			return jcanvas::JKS_Q;
		case SDLK_CAPITAL_R:
			return jcanvas::JKS_R;
		case SDLK_CAPITAL_S:
			return jcanvas::JKS_S;
		case SDLK_CAPITAL_T:
			return jcanvas::JKS_T;
		case SDLK_CAPITAL_U:
			return jcanvas::JKS_U;
		case SDLK_CAPITAL_V:
			return jcanvas::JKS_V;
		case SDLK_CAPITAL_W:
			return jcanvas::JKS_W;
		case SDLK_CAPITAL_X:
			return jcanvas::JKS_X;
		case SDLK_CAPITAL_Y:
			return jcanvas::JKS_Y;
		case SDLK_CAPITAL_Z:
			return jcanvas::JKS_Z;
			*/
		case SDLK_LEFTBRACKET:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case SDLK_BACKSLASH:   
			return jcanvas::JKS_BACKSLASH;
		case SDLK_RIGHTBRACKET:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case SDLK_CARET:
			return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		case SDLK_UNDERSCORE:    
			return jcanvas::JKS_UNDERSCORE;
		case SDLK_BACKQUOTE:
			return jcanvas::JKS_GRAVE_ACCENT;
    case SDLK_a:
			return jcanvas::JKS_a;
    case SDLK_b:
			return jcanvas::JKS_b;
    case SDLK_c:
			return jcanvas::JKS_c;
    case SDLK_d:
			return jcanvas::JKS_d;
    case SDLK_e:
			return jcanvas::JKS_e;
    case SDLK_f:
			return jcanvas::JKS_f;
    case SDLK_g:
			return jcanvas::JKS_g;
    case SDLK_h:
			return jcanvas::JKS_h;
    case SDLK_i:
			return jcanvas::JKS_i;
    case SDLK_j:
			return jcanvas::JKS_j;
    case SDLK_k:
			return jcanvas::JKS_k;
    case SDLK_l:
			return jcanvas::JKS_l;
    case SDLK_m:
			return jcanvas::JKS_m;
    case SDLK_n:
			return jcanvas::JKS_n;
    case SDLK_o:
			return jcanvas::JKS_o;
    case SDLK_p:
			return jcanvas::JKS_p;
    case SDLK_q:
			return jcanvas::JKS_q;
    case SDLK_r:
			return jcanvas::JKS_r;
    case SDLK_s:
			return jcanvas::JKS_s;
    case SDLK_t:
			return jcanvas::JKS_t;
    case SDLK_u:
			return jcanvas::JKS_u;
    case SDLK_v:
			return jcanvas::JKS_v;
    case SDLK_w:
			return jcanvas::JKS_w;
    case SDLK_x:
			return jcanvas::JKS_x;
    case SDLK_y:
			return jcanvas::JKS_y;
    case SDLK_z:
			return jcanvas::JKS_z;
		// case SDLK_CURLY_BRACKET_LEFT:
		//	return jcanvas::JKS_CURLY_BRACKET_LEFT;
		// case SDLK_VERTICAL_BAR:  
		//	return jcanvas::JKS_VERTICAL_BAR;
		// case SDLK_CURLY_BRACKET_RIGHT:
		//	return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		// case SDLK_TILDE:  
		//	return jcanvas::JKS_TILDE;
		case SDLK_DELETE:
			return jcanvas::JKS_DELETE;
		case SDLK_LEFT:
			return jcanvas::JKS_CURSOR_LEFT;
		case SDLK_RIGHT:
			return jcanvas::JKS_CURSOR_RIGHT;
		case SDLK_UP:  
			return jcanvas::JKS_CURSOR_UP;
		case SDLK_DOWN:
			return jcanvas::JKS_CURSOR_DOWN;
		case SDLK_INSERT:  
			return jcanvas::JKS_INSERT;
		case SDLK_HOME:     
			return jcanvas::JKS_HOME;
		case SDLK_END:
			return jcanvas::JKS_END;
		case SDLK_PAGEUP:
			return jcanvas::JKS_PAGE_UP;
		case SDLK_PAGEDOWN:
			return jcanvas::JKS_PAGE_DOWN;
		// case SDLK_PRINT:   
		//	return jcanvas::JKS_PRINT;
		case SDLK_PAUSE:
			return jcanvas::JKS_PAUSE;
		// case SDLK_RED:
		//	return jcanvas::JKS_RED;
		// case SDLK_GREEN:
		//	return jcanvas::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jcanvas::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jcanvas::JKS_BLUE;
		case SDLK_F1:
			return jcanvas::JKS_F1;
		case SDLK_F2:
			return jcanvas::JKS_F2;
		case SDLK_F3:
			return jcanvas::JKS_F3;
		case SDLK_F4:
			return jcanvas::JKS_F4;
		case SDLK_F5:
			return jcanvas::JKS_F5;
		case SDLK_F6:     
			return jcanvas::JKS_F6;
		case SDLK_F7:    
			return jcanvas::JKS_F7;
		case SDLK_F8:   
			return jcanvas::JKS_F8;
		case SDLK_F9:  
			return jcanvas::JKS_F9;
		case SDLK_F10: 
			return jcanvas::JKS_F10;
		case SDLK_F11:
			return jcanvas::JKS_F11;
		case SDLK_F12:
			return jcanvas::JKS_F12;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return jcanvas::JKS_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return jcanvas::JKS_CONTROL;
		case SDLK_LALT:
		case SDLK_RALT:
			return jcanvas::JKS_ALT;
		// case SDLK_ALTGR:
		//	return jcanvas::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jcanvas::JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jcanvas::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
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

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();
    
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, bounds.size.x, bounds.size.y, 32, bounds.size.x*4, 0, 0, 0, 0);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(sg_renderer, surface);
  
  sg_back_buffer->UnlockData();
  
  // SDL_RenderClear(sg_renderer);
  SDL_RenderCopy(sg_renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(sg_renderer);
  
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
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

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_ENTER) {
          // SDL_CaptureMouse(true);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(GetCursor());

          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
        } else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
          // SDL_CaptureMouse(false);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(JCS_DEFAULT);

          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
        } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_OPENED));
        } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
        } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          InternalPaint();
        } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_MOVED));
        } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          InternalPaint();
        
          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_RESIZED));
        } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        }
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if ((event.key.keysym.mod & KMOD_LSHIFT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if ((event.key.keysym.mod & KMOD_RSHIFT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_LGUI) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
        } else if ((event.key.keysym.mod & KMOD_RGUI) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
        } else if ((event.key.keysym.mod & KMOD_MODE) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALTGR);
        } else if ((event.key.keysym.mod & KMOD_CAPS) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CAPS_LOCK);
        // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        }

        type = jcanvas::JKT_UNKNOWN;

        if (event.key.state == SDL_PRESSED) {
          type = jcanvas::JKT_PRESSED;
        } else if (event.key.state == SDL_RELEASED) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        if (event.type == SDL_MOUSEMOTION) {
          // e.motion.x/y
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
          // e.button.button == SDL_BUTTON_LEFT
        } else if (event.type == SDL_MOUSEBUTTONUP) {
        } else if (event.type == SDL_MOUSEWHEEL) {
        }

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event.motion.x;
        sg_mouse_y = event.motion.y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x-1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y-1);

        if (event.type == SDL_MOUSEMOTION) {
          type = jcanvas::JMT_MOVED;
        } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
          if (event.button.button == SDL_BUTTON_LEFT) {
            button = jcanvas::JMB_BUTTON1;
          } else if (event.button.button == SDL_BUTTON_MIDDLE) {
            button = jcanvas::JMB_BUTTON2;
          } else if (event.button.button == SDL_BUTTON_RIGHT) {
            button = jcanvas::JMB_BUTTON3;
          }
          
          if (event.type == SDL_MOUSEBUTTONDOWN) {
            sg_button_state = (jcanvas::jmouseevent_button_t)(sg_button_state | button);
            type = jcanvas::JMT_PRESSED;
          } else if (event.type == SDL_MOUSEBUTTONUP) {
            sg_button_state = (jcanvas::jmouseevent_button_t)(sg_button_state & ~button);
            type = jcanvas::JMT_RELEASED;
          }
        } else if (event.type == SDL_MOUSEWHEEL) {
          type = jcanvas::JMT_ROTATED;
          mouse_z = event.wheel.y;
        }

        if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && sg_button_state != jcanvas::JMB_NONE) {
          SDL_CaptureMouse(SDL_TRUE);
        } else {
          SDL_CaptureMouse(SDL_FALSE);
        }
        
        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(sg_window);

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));

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

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
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

  delete sg_back_buffer;
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

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
	jcanvas::jrect_t<int> t;

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

  if (style == JCS_DEFAULT) {
    type = SDL_SYSTEM_CURSOR_ARROW;
  } else if (style == JCS_CROSSHAIR) {
    type = SDL_SYSTEM_CURSOR_CROSSHAIR;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = SDL_SYSTEM_CURSOR_HAND;
  } else if (style == JCS_MOVE) {
    type = SDL_SYSTEM_CURSOR_SIZEALL;
  } else if (style == JCS_NS) {
    type = SDL_SYSTEM_CURSOR_SIZENS;
  } else if (style == JCS_WE) {
    type = SDL_SYSTEM_CURSOR_SIZEWE;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = SDL_SYSTEM_CURSOR_IBEAM;
  } else if (style == JCS_WAIT) {
    type = SDL_SYSTEM_CURSOR_WAIT;
  }

  SDL_Cursor
    *cursor = SDL_CreateSystemCursor(type);

  SDL_SetCursor(cursor);
  // TODO:: SDL_FreeCursor(cursor);

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
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
	return jcanvas::JWR_NONE;
}

void WindowAdapter::SetIcon(jcanvas::Image *image)
{
  if (image == nullptr) {
    return;
  }

  jcanvas::jpoint_t<int> 
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

jcanvas::Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
