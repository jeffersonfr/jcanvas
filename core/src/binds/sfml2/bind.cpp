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
	jantialias_t jAntialiasModeNone = jantialias_t::None;
}

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
static std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static sf::RenderWindow *sg_window = nullptr;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
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
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jrect_t<int> sg_previous_bounds;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(sf::Keyboard::Key symbol)
{
	switch (symbol) {
		case sf::Keyboard::Return:
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		case sf::Keyboard::BackSpace:
			return jkeyevent_symbol_t::Backspace;
		case sf::Keyboard::Tab:
			return jkeyevent_symbol_t::Tab;
		// case sf::Keyboard::CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case sf::Keyboard::Escape:
			return jkeyevent_symbol_t::Escape;
		case sf::Keyboard::Space:
			return jkeyevent_symbol_t::Space;
		// case sf::Keyboard::EXCLAIM:
		//	return jkeyevent_symbol_t::ExclamationMark;
		// case sf::Keyboard::QUOTEDBL:
		//	return jkeyevent_symbol_t::Quotation;
		// case sf::Keyboard::HASH:
		//	return jkeyevent_symbol_t::Hash;
		// case sf::Keyboard::DOLLAR:
		//	return jkeyevent_symbol_t::DollarSign;
		// case sf::Keyboard::PERCENT_SIGN:
		//	return jkeyevent_symbol_t::PercentSign;
		// case sf::Keyboard::AMPERSAND:   
		//	return jkeyevent_symbol_t::Ampersand;
		case sf::Keyboard::Quote:
			return jkeyevent_symbol_t::Aposthrophe;
		// case sf::Keyboard::LEFTPAREN:
		//	return jkeyevent_symbol_t::ParenthesisLeft;
		// case sf::Keyboard::RIGHTPAREN:
		//	return jkeyevent_symbol_t::ParenthesisRight;
		case sf::Keyboard::Multiply:
			return jkeyevent_symbol_t::Star;
		case sf::Keyboard::Add:
			return jkeyevent_symbol_t::Plus;
		case sf::Keyboard::Comma:   
			return jkeyevent_symbol_t::Comma;
		case sf::Keyboard::Dash:
		case sf::Keyboard::Subtract:
			return jkeyevent_symbol_t::Minus;
		case sf::Keyboard::Period:  
			return jkeyevent_symbol_t::Period;
		case sf::Keyboard::Divide:
		case sf::Keyboard::Slash:
			return jkeyevent_symbol_t::Slash;
		case sf::Keyboard::Numpad0: 
		case sf::Keyboard::Num0: 
			return jkeyevent_symbol_t::Number0;
		case sf::Keyboard::Numpad1: 
		case sf::Keyboard::Num1:
			return jkeyevent_symbol_t::Number1;
		case sf::Keyboard::Numpad2: 
		case sf::Keyboard::Num2:
			return jkeyevent_symbol_t::Number2;
		case sf::Keyboard::Numpad3: 
		case sf::Keyboard::Num3:
			return jkeyevent_symbol_t::Number3;
		case sf::Keyboard::Numpad4: 
		case sf::Keyboard::Num4:
			return jkeyevent_symbol_t::Number4;
		case sf::Keyboard::Numpad5: 
		case sf::Keyboard::Num5:
			return jkeyevent_symbol_t::Number5;
		case sf::Keyboard::Numpad6: 
		case sf::Keyboard::Num6:
			return jkeyevent_symbol_t::Number6;
		case sf::Keyboard::Numpad7: 
		case sf::Keyboard::Num7:
			return jkeyevent_symbol_t::Number7;
		case sf::Keyboard::Numpad8: 
		case sf::Keyboard::Num8:
			return jkeyevent_symbol_t::Number8;
		case sf::Keyboard::Numpad9: 
		case sf::Keyboard::Num9:
			return jkeyevent_symbol_t::Number9;
		// case sf::Keyboard::COLON:
		//	return jkeyevent_symbol_t::Colon;
		case sf::Keyboard::SemiColon:
			return jkeyevent_symbol_t::SemiColon;
		// case sf::Keyboard::LESS:
		// 	return jkeyevent_symbol_t::LessThan;
		case sf::Keyboard::Equal: 
			return jkeyevent_symbol_t::Equals;
		// case sf::Keyboard::GREATER:
		//	return jkeyevent_symbol_t::GreaterThan;
		// case sf::Keyboard::QUESTION:   
		//	return jkeyevent_symbol_t::QuestionMark;
		// case sf::Keyboard::AT:
		//	return jkeyevent_symbol_t::At;
		case sf::Keyboard::A:
			return jkeyevent_symbol_t::a;
		case sf::Keyboard::B:
			return jkeyevent_symbol_t::b;
		case sf::Keyboard::C:
			return jkeyevent_symbol_t::c;
		case sf::Keyboard::D:
			return jkeyevent_symbol_t::d;
		case sf::Keyboard::E:
			return jkeyevent_symbol_t::e;
		case sf::Keyboard::F:
			return jkeyevent_symbol_t::f;
		case sf::Keyboard::G:
			return jkeyevent_symbol_t::g;
		case sf::Keyboard::H:
			return jkeyevent_symbol_t::h;
		case sf::Keyboard::I:
			return jkeyevent_symbol_t::i;
		case sf::Keyboard::J:
			return jkeyevent_symbol_t::j;
		case sf::Keyboard::K:
			return jkeyevent_symbol_t::k;
		case sf::Keyboard::L:
			return jkeyevent_symbol_t::l;
		case sf::Keyboard::M:
			return jkeyevent_symbol_t::m;
		case sf::Keyboard::N:
			return jkeyevent_symbol_t::n;
		case sf::Keyboard::O:
			return jkeyevent_symbol_t::o;
		case sf::Keyboard::P:
			return jkeyevent_symbol_t::p;
		case sf::Keyboard::Q:
			return jkeyevent_symbol_t::q;
		case sf::Keyboard::R:
			return jkeyevent_symbol_t::r;
		case sf::Keyboard::S:
			return jkeyevent_symbol_t::s;
		case sf::Keyboard::T:
			return jkeyevent_symbol_t::t;
		case sf::Keyboard::U:
			return jkeyevent_symbol_t::u;
		case sf::Keyboard::V:
			return jkeyevent_symbol_t::v;
		case sf::Keyboard::W:
			return jkeyevent_symbol_t::w;
		case sf::Keyboard::X:
			return jkeyevent_symbol_t::x;
		case sf::Keyboard::Y:
			return jkeyevent_symbol_t::y;
		case sf::Keyboard::Z:
			return jkeyevent_symbol_t::z;
		case sf::Keyboard::LBracket:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case sf::Keyboard::BackSlash:   
			return jkeyevent_symbol_t::BackSlash;
		case sf::Keyboard::RBracket:
			return jkeyevent_symbol_t::SquareBracketRight;
		// case sf::Keyboard::CARET:
		//	return jkeyevent_symbol_t::CircumflexAccent;
		// case sf::Keyboard::UNDERSCORE:    
		//	return jkeyevent_symbol_t::Underscore;
		// case sf::Keyboard::BACKQUOTE:
		//	return jkeyevent_symbol_t::GraveAccent;
		// case sf::Keyboard::CURLY_BRACKET_LEFT:
		//	return jkeyevent_symbol_t::CurlyBracketLeft;
		// case sf::Keyboard::VERTICAL_BAR:  
		//	return jkeyevent_symbol_t::VerticalBar;
		// case sf::Keyboard::CURLY_BRACKET_RIGHT:
		//	return jkeyevent_symbol_t::CurlyBracketRight;
		case sf::Keyboard::Tilde:  
			return jkeyevent_symbol_t::Tilde;
		case sf::Keyboard::Delete:
			return jkeyevent_symbol_t::Delete;
		case sf::Keyboard::Left:
			return jkeyevent_symbol_t::CursorLeft;
		case sf::Keyboard::Right:
			return jkeyevent_symbol_t::CursorRight;
		case sf::Keyboard::Up:
			return jkeyevent_symbol_t::CursorUp;
		case sf::Keyboard::Down:
			return jkeyevent_symbol_t::CursorDown;
		case sf::Keyboard::Insert:  
			return jkeyevent_symbol_t::Insert;
		case sf::Keyboard::Home:     
			return jkeyevent_symbol_t::Home;
		case sf::Keyboard::End:
			return jkeyevent_symbol_t::End;
		case sf::Keyboard::PageUp:
			return jkeyevent_symbol_t::PageUp;
		case sf::Keyboard::PageDown:
			return jkeyevent_symbol_t::PageDown;
		// case sf::Keyboard::PRINT:   
		//	return jkeyevent_symbol_t::Print;
		case sf::Keyboard::Pause:
			return jkeyevent_symbol_t::Pause;
		// case sf::Keyboard::RED:
		//	return jkeyevent_symbol_t::Red;
		// case sf::Keyboard::GREEN:
		//	return jkeyevent_symbol_t::Green;
		// case sf::Keyboard::YELLOW:
		//	return jkeyevent_symbol_t::Yellow;
		// case sf::Keyboard::BLUE:
		//	return jkeyevent_symbol_t::Blue;
		case sf::Keyboard::F1:
			return jkeyevent_symbol_t::F1;
		case sf::Keyboard::F2:
			return jkeyevent_symbol_t::F2;
		case sf::Keyboard::F3:
			return jkeyevent_symbol_t::F3;
		case sf::Keyboard::F4:
			return jkeyevent_symbol_t::F4;
		case sf::Keyboard::F5:
			return jkeyevent_symbol_t::F5;
		case sf::Keyboard::F6:     
			return jkeyevent_symbol_t::F6;
		case sf::Keyboard::F7:    
			return jkeyevent_symbol_t::F7;
		case sf::Keyboard::F8:   
			return jkeyevent_symbol_t::F8;
		case sf::Keyboard::F9:  
			return jkeyevent_symbol_t::F9;
		case sf::Keyboard::F10: 
			return jkeyevent_symbol_t::F10;
		case sf::Keyboard::F11:
			return jkeyevent_symbol_t::F11;
		case sf::Keyboard::F12:
			return jkeyevent_symbol_t::F12;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			return jkeyevent_symbol_t::Shift;
		case sf::Keyboard::LControl:
		case sf::Keyboard::RControl:
			return jkeyevent_symbol_t::Control;
		case sf::Keyboard::LAlt:
		case sf::Keyboard::RAlt:
			return jkeyevent_symbol_t::Alt;
		// case sf::Keyboard::ALTGR:
		//	return jkeyevent_symbol_t::AltGr;
		// case sf::Keyboard::LMETA:
		// case sf::Keyboard::RMETA:
		//	return jkeyevent_symbol_t::Meta;
		// case sf::Keyboard::LSUPER:
		// case sf::Keyboard::RSUPER:
		//	return jkeyevent_symbol_t::Super;
		case sf::Keyboard::RSystem:
			return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
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

  uint8_t *data = sg_back_buffer->LockData();

	sf::Texture texture;
	sf::Sprite sprite;

	texture.create(bounds.size.x, bounds.size.y);
	texture.setSmooth(g->GetAntialias() != jAntialiasModeNone);

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

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
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

    Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

		// while (sg_window->waitEvent(event)) {
	  while (sg_window->pollEvent(event)) {
      if (event.type == sf::Event::MouseEntered) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
      } else if (event.type == sf::Event::MouseLeft) {
        // SetCursor(jcursor_style_t::Default);

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
      } else if (event.type == sf::Event::Resized) {
        InternalPaint();

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Resized));
      // } else if (event.type == sf::Event::Hidden) {
      // } else if (event.type == sf::Event::Exposed) {
      // } else if (event.type == sf::Event::Moved) {
      } else if (event.type == sf::Event::TouchBegan) {
      } else if (event.type == sf::Event::TouchMoved) {
      } else if (event.type == sf::Event::TouchEnded) {
      } else if (event.type == sf::Event::LostFocus) {
      } else if (event.type == sf::Event::GainedFocus) {
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Opened));
      } else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jKeyEventModifiersNone;

        bool 
          shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift),
          control = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl),
          alt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

        if (shift == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        }

        if (control == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        }

        if (alt == true) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        }

        if (event.type == sf::Event::KeyPressed) {
          type = jkeyevent_type_t::Pressed;

          // TODO:: grab pointer events
        } else if (event.type == sf::Event::KeyReleased) {
          type = jkeyevent_type_t::Released;

          // TODO:: ungrab pointer events
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.code);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (
          event.type == sf::Event::MouseMoved || 
          event.type == sf::Event::MouseButtonPressed || 
          event.type == sf::Event::MouseButtonReleased ||
          event.type == sf::Event::MouseWheelMoved ||
          event.type == sf::Event::MouseWheelScrolled) {
        static jmouseevent_button_t buttons = jMouseEventButtonNone;

        jmouseevent_button_t button = jMouseEventButtonNone;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        if (event.type == sf::Event::MouseMoved) {
          type = jmouseevent_type_t::Moved;

          sg_mouse_x = event.mouseMove.x;
          sg_mouse_y = event.mouseMove.y;
        } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
          sg_mouse_x = event.mouseButton.x;
          sg_mouse_y = event.mouseButton.y;

          if (event.mouseButton.button == sf::Mouse::Left) {
            button = jMouseEventButtonButton1;
          } else if (event.mouseButton.button == sf::Mouse::Middle) {
            button = jMouseEventButtonButton2;
          } else if (event.mouseButton.button == sf::Mouse::Right) {
            button = jMouseEventButtonButton3;
          }
          
          if (event.type == sf::Event::MouseButtonPressed) {
            type = jmouseevent_type_t::Pressed;
            buttons = jenum_t<jmouseevent_button_t>{buttons}.Or(button);
          } else if (event.type == sf::Event::MouseButtonReleased) {
            type = jmouseevent_type_t::Released;
            buttons = jenum_t<jmouseevent_button_t>{buttons}.And(jenum_t<jmouseevent_button_t>{button}.Not());
          }
        } else if (event.type == sf::Event::MouseWheelMoved || event.type == sf::Event::MouseWheelScrolled) {
          type = jmouseevent_type_t::Rotated;

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

        if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jMouseEventButtonNone) {
          sg_window->setMouseCursorGrabbed(true);
        } else {
          sg_window->setMouseCursorGrabbed(false);
        }

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (event.type == sf::Event::Closed) {
        sg_window->close();

        sg_quitting = true;
        
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
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

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	if (sg_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

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
  
  sg_back_buffer = nullptr;
  sg_jcanvas_window = nullptr;
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

jrect_t<int> WindowAdapter::GetBounds()
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

  if (style == jcursor_style_t::Default) {
    type = sf::Cursor::Arrow;
  } else if (style == jcursor_style_t::Crosshair) {
    type = sf::Cursor::Cross;
  } else if (style == jcursor_style_t::East) {
  } else if (style == jcursor_style_t::West) {
  } else if (style == jcursor_style_t::North) {
  } else if (style == jcursor_style_t::South) {
  } else if (style == jcursor_style_t::Hand) {
    type = sf::Cursor::Hand;
  } else if (style == jcursor_style_t::Move) {
    type = sf::Cursor::Hand;
  } else if (style == jcursor_style_t::Vertical) {
    type = sf::Cursor::SizeVertical;
  } else if (style == jcursor_style_t::Horizontal) {
    type = sf::Cursor::SizeHorizontal;
  } else if (style == jcursor_style_t::NorthWest) {
  } else if (style == jcursor_style_t::NorthEast) {
  } else if (style == jcursor_style_t::SouthWest) {
  } else if (style == jcursor_style_t::SouthEast) {
  } else if (style == jcursor_style_t::Text) {
    type = sf::Cursor::Text;
  } else if (style == jcursor_style_t::Wait) {
    type = sf::Cursor::Wait;
  }

  sf::Cursor cursor;

  if (cursor.loadFromSystem(type) == true) {
    sg_window->setMouseCursor(cursor);
  }
  */
	
  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

  jpoint_t<int> 
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
	return jWindowRotationNone;
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
