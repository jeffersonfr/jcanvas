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
#include "jcanvas/core/jkeyevent.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
// #include <SFML/Cursor.hpp>
#include <X11/Xlib.h>

namespace jcanvas {

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static sf::RenderWindow *sg_window = nullptr;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::jrect_t<int> sg_previous_bounds;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(sf::Keyboard::Key symbol)
{
	switch (symbol) {
		case sf::Keyboard::Return:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case sf::Keyboard::BackSpace:
			return jcanvas::JKS_BACKSPACE;
		case sf::Keyboard::Tab:
			return jcanvas::JKS_TAB;
		// case sf::Keyboard::CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case sf::Keyboard::Escape:
			return jcanvas::JKS_ESCAPE;
		case sf::Keyboard::Space:
			return jcanvas::JKS_SPACE;
		// case sf::Keyboard::EXCLAIM:
		//	return jcanvas::JKS_EXCLAMATION_MARK;
		// case sf::Keyboard::QUOTEDBL:
		//	return jcanvas::JKS_QUOTATION;
		// case sf::Keyboard::HASH:
		//	return jcanvas::JKS_NUMBER_SIGN;
		// case sf::Keyboard::DOLLAR:
		//	return jcanvas::JKS_DOLLAR_SIGN;
		// case sf::Keyboard::PERCENT_SIGN:
		//	return jcanvas::JKS_PERCENT_SIGN;
		// case sf::Keyboard::AMPERSAND:   
		//	return jcanvas::JKS_AMPERSAND;
		case sf::Keyboard::Quote:
			return jcanvas::JKS_APOSTROPHE;
		// case sf::Keyboard::LEFTPAREN:
		//	return jcanvas::JKS_PARENTHESIS_LEFT;
		// case sf::Keyboard::RIGHTPAREN:
		//	return jcanvas::JKS_PARENTHESIS_RIGHT;
		case sf::Keyboard::Multiply:
			return jcanvas::JKS_STAR;
		case sf::Keyboard::Add:
			return jcanvas::JKS_PLUS_SIGN;
		case sf::Keyboard::Comma:   
			return jcanvas::JKS_COMMA;
		case sf::Keyboard::Dash:
		case sf::Keyboard::Subtract:
			return jcanvas::JKS_MINUS_SIGN;
		case sf::Keyboard::Period:  
			return jcanvas::JKS_PERIOD;
		case sf::Keyboard::Divide:
		case sf::Keyboard::Slash:
			return jcanvas::JKS_SLASH;
		case sf::Keyboard::Numpad0: 
		case sf::Keyboard::Num0: 
			return jcanvas::JKS_0;
		case sf::Keyboard::Numpad1: 
		case sf::Keyboard::Num1:
			return jcanvas::JKS_1;
		case sf::Keyboard::Numpad2: 
		case sf::Keyboard::Num2:
			return jcanvas::JKS_2;
		case sf::Keyboard::Numpad3: 
		case sf::Keyboard::Num3:
			return jcanvas::JKS_3;
		case sf::Keyboard::Numpad4: 
		case sf::Keyboard::Num4:
			return jcanvas::JKS_4;
		case sf::Keyboard::Numpad5: 
		case sf::Keyboard::Num5:
			return jcanvas::JKS_5;
		case sf::Keyboard::Numpad6: 
		case sf::Keyboard::Num6:
			return jcanvas::JKS_6;
		case sf::Keyboard::Numpad7: 
		case sf::Keyboard::Num7:
			return jcanvas::JKS_7;
		case sf::Keyboard::Numpad8: 
		case sf::Keyboard::Num8:
			return jcanvas::JKS_8;
		case sf::Keyboard::Numpad9: 
		case sf::Keyboard::Num9:
			return jcanvas::JKS_9;
		// case sf::Keyboard::COLON:
		//	return jcanvas::JKS_COLON;
		case sf::Keyboard::SemiColon:
			return jcanvas::JKS_SEMICOLON;
		// case sf::Keyboard::LESS:
		// 	return jcanvas::JKS_LESS_THAN_SIGN;
		case sf::Keyboard::Equal: 
			return jcanvas::JKS_EQUALS_SIGN;
		// case sf::Keyboard::GREATER:
		//	return jcanvas::JKS_GREATER_THAN_SIGN;
		// case sf::Keyboard::QUESTION:   
		//	return jcanvas::JKS_QUESTION_MARK;
		// case sf::Keyboard::AT:
		//	return jcanvas::JKS_AT;
		case sf::Keyboard::A:
			return jcanvas::JKS_a;
		case sf::Keyboard::B:
			return jcanvas::JKS_b;
		case sf::Keyboard::C:
			return jcanvas::JKS_c;
		case sf::Keyboard::D:
			return jcanvas::JKS_d;
		case sf::Keyboard::E:
			return jcanvas::JKS_e;
		case sf::Keyboard::F:
			return jcanvas::JKS_f;
		case sf::Keyboard::G:
			return jcanvas::JKS_g;
		case sf::Keyboard::H:
			return jcanvas::JKS_h;
		case sf::Keyboard::I:
			return jcanvas::JKS_i;
		case sf::Keyboard::J:
			return jcanvas::JKS_j;
		case sf::Keyboard::K:
			return jcanvas::JKS_k;
		case sf::Keyboard::L:
			return jcanvas::JKS_l;
		case sf::Keyboard::M:
			return jcanvas::JKS_m;
		case sf::Keyboard::N:
			return jcanvas::JKS_n;
		case sf::Keyboard::O:
			return jcanvas::JKS_o;
		case sf::Keyboard::P:
			return jcanvas::JKS_p;
		case sf::Keyboard::Q:
			return jcanvas::JKS_q;
		case sf::Keyboard::R:
			return jcanvas::JKS_r;
		case sf::Keyboard::S:
			return jcanvas::JKS_s;
		case sf::Keyboard::T:
			return jcanvas::JKS_t;
		case sf::Keyboard::U:
			return jcanvas::JKS_u;
		case sf::Keyboard::V:
			return jcanvas::JKS_v;
		case sf::Keyboard::W:
			return jcanvas::JKS_w;
		case sf::Keyboard::X:
			return jcanvas::JKS_x;
		case sf::Keyboard::Y:
			return jcanvas::JKS_y;
		case sf::Keyboard::Z:
			return jcanvas::JKS_z;
		case sf::Keyboard::LBracket:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case sf::Keyboard::BackSlash:   
			return jcanvas::JKS_BACKSLASH;
		case sf::Keyboard::RBracket:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		// case sf::Keyboard::CARET:
		//	return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		// case sf::Keyboard::UNDERSCORE:    
		//	return jcanvas::JKS_UNDERSCORE;
		// case sf::Keyboard::BACKQUOTE:
		//	return jcanvas::JKS_GRAVE_ACCENT;
		// case sf::Keyboard::CURLY_BRACKET_LEFT:
		//	return jcanvas::JKS_CURLY_BRACKET_LEFT;
		// case sf::Keyboard::VERTICAL_BAR:  
		//	return jcanvas::JKS_VERTICAL_BAR;
		// case sf::Keyboard::CURLY_BRACKET_RIGHT:
		//	return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case sf::Keyboard::Tilde:  
			return jcanvas::JKS_TILDE;
		case sf::Keyboard::Delete:
			return jcanvas::JKS_DELETE;
		case sf::Keyboard::Left:
			return jcanvas::JKS_CURSOR_LEFT;
		case sf::Keyboard::Right:
			return jcanvas::JKS_CURSOR_RIGHT;
		case sf::Keyboard::Up:
			return jcanvas::JKS_CURSOR_UP;
		case sf::Keyboard::Down:
			return jcanvas::JKS_CURSOR_DOWN;
		case sf::Keyboard::Insert:  
			return jcanvas::JKS_INSERT;
		case sf::Keyboard::Home:     
			return jcanvas::JKS_HOME;
		case sf::Keyboard::End:
			return jcanvas::JKS_END;
		case sf::Keyboard::PageUp:
			return jcanvas::JKS_PAGE_UP;
		case sf::Keyboard::PageDown:
			return jcanvas::JKS_PAGE_DOWN;
		// case sf::Keyboard::PRINT:   
		//	return jcanvas::JKS_PRINT;
		case sf::Keyboard::Pause:
			return jcanvas::JKS_PAUSE;
		// case sf::Keyboard::RED:
		//	return jcanvas::JKS_RED;
		// case sf::Keyboard::GREEN:
		//	return jcanvas::JKS_GREEN;
		// case sf::Keyboard::YELLOW:
		//	return jcanvas::JKS_YELLOW;
		// case sf::Keyboard::BLUE:
		//	return jcanvas::JKS_BLUE;
		case sf::Keyboard::F1:
			return jcanvas::JKS_F1;
		case sf::Keyboard::F2:
			return jcanvas::JKS_F2;
		case sf::Keyboard::F3:
			return jcanvas::JKS_F3;
		case sf::Keyboard::F4:
			return jcanvas::JKS_F4;
		case sf::Keyboard::F5:
			return jcanvas::JKS_F5;
		case sf::Keyboard::F6:     
			return jcanvas::JKS_F6;
		case sf::Keyboard::F7:    
			return jcanvas::JKS_F7;
		case sf::Keyboard::F8:   
			return jcanvas::JKS_F8;
		case sf::Keyboard::F9:  
			return jcanvas::JKS_F9;
		case sf::Keyboard::F10: 
			return jcanvas::JKS_F10;
		case sf::Keyboard::F11:
			return jcanvas::JKS_F11;
		case sf::Keyboard::F12:
			return jcanvas::JKS_F12;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			return jcanvas::JKS_SHIFT;
		case sf::Keyboard::LControl:
		case sf::Keyboard::RControl:
			return jcanvas::JKS_CONTROL;
		case sf::Keyboard::LAlt:
		case sf::Keyboard::RAlt:
			return jcanvas::JKS_ALT;
		// case sf::Keyboard::ALTGR:
		//	return jcanvas::JKS_ALTGR;
		// case sf::Keyboard::LMETA:
		// case sf::Keyboard::RMETA:
		//	return jcanvas::JKS_META;
		// case sf::Keyboard::LSUPER:
		// case sf::Keyboard::RSUPER:
		//	return jcanvas::JKS_SUPER;
		case sf::Keyboard::RSystem:
			return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
	XInitThreads();

	sf::VideoMode display = sf::VideoMode::getDesktopMode();

	sg_screen.x = display.width;
	sg_screen.y = display.height;
  
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

	sf::Texture texture;
	sf::Sprite sprite;

	texture.create(bounds.size.x, bounds.size.y);
	texture.setSmooth(g->GetAntialias() != JAM_NONE);

	sprite.setTexture(texture, false);
  
  sf::Vector2f targetSize(bounds.size.x, bounds.size.y);

	int size = bounds.size.x*bounds.size.y;
	uint8_t *src = data;

	for (int i=0; i<size; i++) {
    uint8_t p = src[2];

		// src[3] = src[3];
		src[2] = src[0];
		// src[1] = src[1];
		src[0] = p;

		src = src + 4;
	}

	texture.update(data);

  sg_window->setVerticalSyncEnabled(g->IsVerticalSyncEnabled());

	sg_window->clear();
	sg_window->draw(sprite);
	sg_window->display();
	
  sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	sf::Event event;
  
	while (sg_quitting == false && sg_window->isOpen() == true) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

		// while (window->waitEvent(event)) {
		while (sg_window->pollEvent(event)) {
      if (event.type == sf::Event::MouseEntered) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
      } else if (event.type == sf::Event::MouseLeft) {
        // SetCursor(JCS_DEFAULT);

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
      } else if (event.type == sf::Event::Resized) {
        InternalPaint();

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_RESIZED));
      // } else if (event.type == sf::Event::Hidden) {
      // } else if (event.type == sf::Event::Exposed) {
      // } else if (event.type == sf::Event::Moved) {
      } else if (event.type == sf::Event::TouchBegan) {
      } else if (event.type == sf::Event::TouchMoved) {
      } else if (event.type == sf::Event::TouchEnded) {
      } else if (event.type == sf::Event::LostFocus) {
      } else if (event.type == sf::Event::GainedFocus) {
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_OPENED));
      } else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        bool 
          shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift),
          control = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl),
          alt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

        if (shift == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        }

        if (control == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        }

        if (alt == true) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        }

        type = (jcanvas::jkeyevent_type_t)(0);

        if (event.type == sf::Event::KeyPressed) {
          type = jcanvas::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (event.type == sf::Event::KeyReleased) {
          type = jcanvas::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.code);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (
          event.type == sf::Event::MouseMoved || 
          event.type == sf::Event::MouseButtonPressed || 
          event.type == sf::Event::MouseButtonReleased ||
          event.type == sf::Event::MouseWheelMoved ||
          event.type == sf::Event::MouseWheelScrolled) {
        static jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        if (event.type == sf::Event::MouseMoved) {
          type = jcanvas::JMT_MOVED;

          sg_mouse_x = event.mouseMove.x;
          sg_mouse_y = event.mouseMove.y;
        } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
          sg_mouse_x = event.mouseButton.x;
          sg_mouse_y = event.mouseButton.y;

          if (event.mouseButton.button == sf::Mouse::Left) {
            button = jcanvas::JMB_BUTTON1;
          } else if (event.mouseButton.button == sf::Mouse::Middle) {
            button = jcanvas::JMB_BUTTON2;
          } else if (event.mouseButton.button == sf::Mouse::Right) {
            button = jcanvas::JMB_BUTTON3;
          }
          
          if (event.type == sf::Event::MouseButtonPressed) {
            type = jcanvas::JMT_PRESSED;
            buttons = (jcanvas::jmouseevent_button_t)(buttons | button);
          } else if (event.type == sf::Event::MouseButtonReleased) {
            type = jcanvas::JMT_RELEASED;
            buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
          }
        } else if (event.type == sf::Event::MouseWheelMoved || event.type == sf::Event::MouseWheelScrolled) {
          type = jcanvas::JMT_ROTATED;

          if (event.type == sf::Event::MouseWheelMoved) {
            sg_mouse_x = event.mouseWheel.x;
            sg_mouse_y = event.mouseWheel.y;
            mouse_z = event.mouseWheel.delta;
          } else if (event.type == sf::Event::MouseWheelScrolled) {
            sg_mouse_x = event.mouseWheelScroll.x;
            sg_mouse_y = event.mouseWheelScroll.y;
            mouse_z = event.mouseWheelScroll.delta;
          }
        }

        if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
          sg_window->setMouseCursorGrabbed(true);
        } else {
          sg_window->setMouseCursorGrabbed(false);
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (event.type == sf::Event::Closed) {
        sg_window->close();

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
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
	if (sg_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

	int flags = (int)(sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

  // TODO:: set location too
	sg_window = new sf::RenderWindow(sf::VideoMode(bounds.size.x, bounds.size.y), sg_title.c_str(), flags);

	if (sg_window == nullptr) {
		throw std::runtime_error("Cannot create a window");
	}

	sg_window->setPosition(sf::Vector2i(bounds.point.x, bounds.point.y));
	sg_window->requestFocus();
	sg_window->setVerticalSyncEnabled(true);
	sg_window->setFramerateLimit(12000); // CHANGE:: is correct ?
	sg_window->setActive(false);
}

WindowAdapter::~WindowAdapter()
{
  sg_window->close();

  delete sg_window;
  sg_window = nullptr;
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  uint32_t
    flags = (int)(sf::Style::Close);

  if (IsUndecorated() == false) {
    flags = (uint32_t)(flags | sf::Style::Titlebar);
  }

  if (IsResizable() == true) {
    flags = (uint32_t)(flags | sf::Style::Resize);
  }

  sf::RenderWindow 
    *window = nullptr;
  sf::RenderWindow 
    *old = sg_window;

  if (sg_fullscreen == false) {
    std::vector<sf::VideoMode> 
      modes = sf::VideoMode::getFullscreenModes();

    if (modes.size() == 0) {
      return;
    }

    sg_previous_bounds = GetBounds();

    window = new sf::RenderWindow(modes[0], GetTitle().c_str(), flags);

    sg_fullscreen = true;
  } else {
    window = new sf::RenderWindow(sf::VideoMode(sg_previous_bounds.size.x, sg_previous_bounds.size.y), GetTitle().c_str(), flags);

    window->setPosition(sf::Vector2i(sg_previous_bounds.point.x, sg_previous_bounds.point.y));

    sg_fullscreen = false;
  }

  if (sg_window == nullptr) {
    return;
  }

  window->requestFocus();
  window->setVerticalSyncEnabled(true);
  window->setFramerateLimit(120);
  window->setActive(false);

  sg_window = window;

  delete old;
  old = nullptr;
}

void WindowAdapter::SetTitle(std::string title)
{
  sg_title = title;

	sg_window->setTitle(sg_title.c_str());
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
}

bool WindowAdapter::IsUndecorated()
{
  return sg_undecorated;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
	sg_window->setPosition(sf::Vector2i(bounds.point.x, bounds.point.y));
	sg_window->setSize(sf::Vector2u(bounds.size.x, bounds.size.y));
  sg_window->setView(sf::View(sf::FloatRect(0, 0, bounds.size.x, bounds.size.y)));
}

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
  sf::Vector2i 
    location = sg_window->getPosition();
  sf::Vector2u 
    size = sg_window->getSize();

	return {
    location.x, 
    location.y, 
    (int)size.x, 
    (int)size.y
  };
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

	sf::Mouse::setPosition(sf::Vector2i(x, y));
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	sf::Vector2i pos = sf::Mouse::getPosition();

	p.x = pos.x;
	p.y = pos.y;

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;

  sg_window->setVisible(visible);
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

	sg_window->setMouseCursorVisible(sg_cursor_enabled);
}

bool WindowAdapter::IsCursorEnabled()
{
  return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  /*
  sf::Cursor::Type type = sf::Cursor::Arrow;;

  if (style == JCS_DEFAULT) {
    type = sf::Cursor::Arrow;
  } else if (style == JCS_CROSSHAIR) {
    type = sf::Cursor::Cross;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = sf::Cursor::Hand;
  } else if (style == JCS_MOVE) {
    type = sf::Cursor::Hand;
  } else if (style == JCS_NS) {
    type = sf::Cursor::SizeVertical;
  } else if (style == JCS_WE) {
    type = sf::Cursor::SizeHorizontal;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = sf::Cursor::Text;
  } else if (style == JCS_WAIT) {
    type = sf::Cursor::Wait;
  }

  sf::Cursor cursor;

  if (cursor.loadFromSystem(type) == true) {
    sg_window->setMouseCursor(cursor);
  }
  */
	
  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

  jcanvas::jpoint_t<int> 
    size = shape->GetSize();
	uint32_t 
    data[size.x*size.y];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, size.x, size.y});

	if (data == nullptr) {
		return;
	}

  sf::Cursor cursor;

  if (cursor.loadFromPixels(data, sf::Vector2u(size.x, size.y), sf::Vector2u(hotx, hoty)) == true) {
    sg_window->setMouseCursor(cursor);
  }

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
