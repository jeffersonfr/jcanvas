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

#include <rfb/rfb.h>
#include <X11/keysym.h>

#define SCREEN_BPP 4

namespace jcanvas {

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static rfbScreenInfoPtr sg_server;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static uint32_t last_mouse_state = 0x00;
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
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(rfbKeySym symbol)
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
	sg_screen.x = -1;
	sg_screen.y = -1;

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

	int size = bounds.size.x*bounds.size.y;
	uint8_t *src = sg_back_buffer->LockData();
	uint8_t *dst = (uint8_t *)sg_server->frameBuffer;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[0];
		dst[1] = src[1];
		dst[0] = src[2];

		src = src + 4;
		dst = dst + SCREEN_BPP;
	}

  rfbMarkRectAsModified(sg_server, 0, 0, bounds.size.x, bounds.size.y);

	sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  while (sg_quitting == false and rfbIsActive(sg_server)) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    rfbProcessEvents(sg_server, 1000000/100);

    std::this_thread::yield();
  }

  /*
	SDL_Event event;
  
  sg_quitting = false;
  
	while (sg_quitting == false) {

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
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(_window);

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
      }
    }
  }

  sg_quitting = true;
  
  sg_jcanvas_window->SetVisible(false);
  */
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

static void ProcessKeyEvents(rfbBool down, rfbKeySym k, rfbClientPtr cl)
{
  static jcanvas::jkeyevent_modifiers_t mod = jcanvas::JKM_NONE;

  if (sg_jcanvas_window == nullptr) {
    return;
  }

  jcanvas::jkeyevent_type_t type;

#define UPDATE_MODIFIERS(flag) \
    if (down == true) { \
      mod = (jcanvas::jkeyevent_modifiers_t)(mod | flag); \
    } else { \
      mod = (jcanvas::jkeyevent_modifiers_t)(mod & ~flag); \
    } \

  if (k == XK_Shift_L) {
    UPDATE_MODIFIERS(jcanvas::JKM_SHIFT);
  } else if (k == XK_Shift_R) {
    UPDATE_MODIFIERS(jcanvas::JKM_SHIFT);
  } else if (k == XK_Control_L) {
    UPDATE_MODIFIERS(jcanvas::JKM_CONTROL);
  } else if (k == XK_Control_R) {
    UPDATE_MODIFIERS(jcanvas::JKM_CONTROL);
  } else if (k == XK_Alt_L) {
    UPDATE_MODIFIERS(jcanvas::JKM_ALT);
  } else if (k == XK_Alt_R) {
    UPDATE_MODIFIERS(jcanvas::JKM_ALT);
  } else if (k == XK_Caps_Lock) {
    UPDATE_MODIFIERS(jcanvas::JKM_CAPS_LOCK);
  } else if (k == XK_Meta_L) {
    UPDATE_MODIFIERS(jcanvas::JKM_META);
  } else if (k == XK_Meta_R) {
    UPDATE_MODIFIERS(jcanvas::JKM_META);
  } else if (k == XK_Super_L) {
    UPDATE_MODIFIERS(jcanvas::JKM_SUPER);
  } else if (k == XK_Super_R) {
    UPDATE_MODIFIERS(jcanvas::JKM_SUPER);
  } else if (k == XK_Hyper_L) {
    UPDATE_MODIFIERS(jcanvas::JKM_HYPER);
  } else if (k == XK_Hyper_R) {
    UPDATE_MODIFIERS(jcanvas::JKM_HYPER);
  }

  type = jcanvas::JKT_UNKNOWN;

  if (down) {
    type = jcanvas::JKT_PRESSED;
  } else {
    type = jcanvas::JKT_RELEASED;
  }

  jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(k);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void ProcessMouseEvents(int buttonMask, int x, int y, rfbClientPtr cl)
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  int mouse_z = 0;
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;

  type = jcanvas::JMT_PRESSED;

  if (sg_mouse_x != x || sg_mouse_y != y) {
    type = jcanvas::JMT_MOVED;
  }

  sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
  sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

  if ((buttonMask & 0x01) == 0 && (last_mouse_state & 0x01)) {
    type = jcanvas::JMT_RELEASED;
  } else if ((buttonMask & 0x02) == 0 && (last_mouse_state & 0x02)) {
    type = jcanvas::JMT_RELEASED;
  } else if ((buttonMask & 0x04) == 0 && (last_mouse_state & 0x04)) {
    type = jcanvas::JMT_RELEASED;
  } 

  if ((buttonMask & 0x01) && (last_mouse_state & 0x01) == 0) {
    button = jcanvas::JMB_BUTTON1;
  } else if ((buttonMask & 0x02) && (last_mouse_state & 0x02) == 0) {
    button = jcanvas::JMB_BUTTON2;
  } else if ((buttonMask & 0x04) && (last_mouse_state & 0x04) == 0) {
    button = jcanvas::JMB_BUTTON3;
  }

  last_mouse_state = buttonMask;

  if ((buttonMask & 0x08) || (buttonMask & 0x10)) {
    type = jcanvas::JMT_ROTATED;
    mouse_z = 1;
  }

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
{
	if (sg_server != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_server = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

	sg_screen.x = bounds.size.x;
	sg_screen.y = bounds.size.y;

  sg_server = rfbGetScreen(0, nullptr, bounds.size.x, bounds.size.y, 8, 3, SCREEN_BPP);

  if (!sg_server) {
		throw std::runtime_error("Cannot create a vnc sg_server");
  }

  sg_server->desktopName = "jcanvas-sg_server";
  sg_server->frameBuffer= (char *)malloc(bounds.size.x*bounds.size.y*SCREEN_BPP);
  sg_server->alwaysShared = true;
  sg_server->colourMap.is16 = false;
  sg_server->serverFormat.trueColour = false;
  sg_server->kbdAddEvent = ProcessKeyEvents;
  sg_server->ptrAddEvent = ProcessMouseEvents;
  sg_server->screenData = this;

  rfbInitServer(sg_server);           
}

WindowAdapter::~WindowAdapter()
{
  delete sg_jcanvas_icon;
  sg_jcanvas_icon = nullptr;

  rfbShutdownServer(sg_server, true);
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
}

void WindowAdapter::SetTitle(std::string title)
{
}

std::string WindowAdapter::GetTitle()
{
	return std::string();
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
	return 1.0;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
}

bool WindowAdapter::IsUndecorated()
{
  return true;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
}

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
	return {
    .point = {
      .x = 0,
      .y = 0
    },
    .size = {
      .x = sg_screen.x,
      .y = sg_screen.y
    }
  };
}
		
void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return false;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
}

bool WindowAdapter::IsVisible()
{
  return true;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  /*
   * TODO:: add all clients in a list and iterate over all
  if (enabled == false) {
    rfbHideCursor(cl);
  } else {
    rfbShowCursor(cl);
  }
  */
}

bool WindowAdapter::IsCursorEnabled()
{
	return true;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  /*
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
  */

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

	SDL_Surface *surface = nullptr;
	uint32_t rmask = 0x000000ff;
	uint32_t gmask = 0x0000ff00;
	uint32_t bmask = 0x00ff0000;
	uint32_t amask = 0xff000000;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#endif

	surface = SDL_CreateRGBSurfaceFrom(data, t.x, t.y, 32, t.x*4, rmask, gmask, bmask, amask);

	if (surface == nullptr) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
  */
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
