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

#include <rfb/rfb.h>
#include <X11/keysym.h>

#define SCREEN_BPP 4

namespace jcanvas {

/** \brief */
static std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static rfbScreenInfoPtr sg_server;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
/** \brief */
static uint32_t last_mouse_state = 0x00;
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
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(rfbKeySym symbol)
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

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
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
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(_window);

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
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
  static jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

  if (sg_jcanvas_window == nullptr) {
    return;
  }

  jkeyevent_type_t type = jkeyevent_type_t::Unknown;


#define UPDATE_MODIFIERS(flag) \
    if (down == true) { \
      mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(flag); \
    } else { \
      mod = jenum_t<jkeyevent_modifiers_t>{mod}.And(jenum_t<jkeyevent_modifiers_t>{flag}.Not()); \
    } \

  if (k == XK_Shift_L) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Shift);
  } else if (k == XK_Shift_R) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Shift);
  } else if (k == XK_Control_L) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Control);
  } else if (k == XK_Control_R) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Control);
  } else if (k == XK_Alt_L) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Alt);
  } else if (k == XK_Alt_R) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Alt);
  } else if (k == XK_Caps_Lock) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::CapsLock);
  } else if (k == XK_Meta_L) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Meta);
  } else if (k == XK_Meta_R) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Meta);
  } else if (k == XK_Super_L) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Super);
  } else if (k == XK_Super_R) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Super);
  } else if (k == XK_Hyper_L) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Hyper);
  } else if (k == XK_Hyper_R) {
    UPDATE_MODIFIERS(jkeyevent_modifiers_t::Hyper);
  }

  if (down) {
    type = jkeyevent_type_t::Pressed;
  } else {
    type = jkeyevent_type_t::Released;
  }

  jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(k);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void ProcessMouseEvents(int buttonMask, int x, int y, rfbClientPtr cl)
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  int mouse_z = 0;
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Unknown;

  type = jmouseevent_type_t::Pressed;

  if (sg_mouse_x != x || sg_mouse_y != y) {
    type = jmouseevent_type_t::Moved;
  }

  sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
  sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

  if ((buttonMask & 0x01) == 0 && (last_mouse_state & 0x01)) {
    type = jmouseevent_type_t::Released;
  } else if ((buttonMask & 0x02) == 0 && (last_mouse_state & 0x02)) {
    type = jmouseevent_type_t::Released;
  } else if ((buttonMask & 0x04) == 0 && (last_mouse_state & 0x04)) {
    type = jmouseevent_type_t::Released;
  } 

  if ((buttonMask & 0x01) && (last_mouse_state & 0x01) == 0) {
    button = jmouseevent_button_t::Button1;
  } else if ((buttonMask & 0x02) && (last_mouse_state & 0x02) == 0) {
    button = jmouseevent_button_t::Button2;
  } else if ((buttonMask & 0x04) && (last_mouse_state & 0x04) == 0) {
    button = jmouseevent_button_t::Button3;
  }

  last_mouse_state = buttonMask;

  if ((buttonMask & 0x08) || (buttonMask & 0x10)) {
    type = jmouseevent_type_t::Rotated;
    mouse_z = 1;
  }

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	if (sg_server != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

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
  sg_jcanvas_icon = nullptr;

  rfbShutdownServer(sg_server, true);
  
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

jrect_t<int> WindowAdapter::GetBounds()
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
  */

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
	return jwindow_rotation_t::None;
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
