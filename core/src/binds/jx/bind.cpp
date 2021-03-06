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

#include "jxclient.h"

namespace jcanvas {

/** \brief */
static jx::Client *sg_window = nullptr;
/** \brief */
static Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

class App : public jx::Client {

  public:
    App():
      jx::Client()
    {
    }

    virtual ~App()
    {
    }

    virtual void onConfig(jx::Vec2<int> size) override
    {
      sg_screen.x = size.x;
      sg_screen.y = size.y;

      requestFocus();
      requestPaint();
    }

    virtual void onKey(const jx::KeyEvent &event) override
    {
      std::cout << "Client:input: '" << (int)event.id << "' was " << ((event.down == true)?"pressed":"released") << std::endl;
  
      jkeyevent_type_t type = jkeyevent_type_t::Unknown;
      jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

      if (event.mod == jx::KeyModifiers::Shift) {
        mod = (jkeyevent_modifiers_t)(mod | jkeyevent_modifiers_t::Shift);
      } else if (event.mod == jx::KeyModifiers::Control) {
        mod = (jkeyevent_modifiers_t)(mod | jkeyevent_modifiers_t::Control);
      } else if (event.mod == jx::KeyModifiers::Alt) {
        mod = (jkeyevent_modifiers_t)(mod | jkeyevent_modifiers_t::Alt);
      }

      if (event.down == true) {
        type = jkeyevent_type_t::Pressed;
      } else {
        type = jkeyevent_type_t::Released;
      }

      jkeyevent_symbol_t
        symbol = TranslateToNativeKeySymbol(event.id);

      sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
    }

    std::map<jmouseevent_button_t, bool> 
      sg_mouse_button_state;

    virtual void onPointer(const jx::PointerEvent &event) override
    {
      jmouseevent_button_t 
        button {
          JMB_NONE};
      jmouseevent_type_t 
        type {
          JMT_UNKNOWN};
      int 
        mouse_z = 0;

      sg_mouse_x = CLAMP(event.pos.x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(event.pos.y, 0, sg_screen.y - 1);

      if (event.button == jx::PointerButton::Button1) {
        button = JMB_BUTTON1;
      } else if (event.button == jx::PointerButton::Button2) {
        button = JMB_BUTTON2;
      } else if (event.button == jx::PointerButton::Button3) {
        button = JMB_BUTTON3;
      }

      if (event.button == jx::PointerButton::None) {
        type = JMT_MOVED;
      } else {
        if (event.down == true) {
          type = JMT_PRESSED;
        } else {
          type = JMT_RELEASED;
        }

        sg_mouse_button_state[button] = event.down;
      }

      sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
    }

    virtual void onPaint(cairo_t *cr) override
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
          delete sg_back_buffer;
          sg_back_buffer = nullptr;
        }
      }

      if (sg_back_buffer == nullptr) {
        sg_back_buffer = new BufferedImage(jpixelformat_t::RGB32, bounds.size);
      }

      Graphics 
        *g = sg_back_buffer->GetGraphics();

      g->Reset();
      g->SetCompositeFlags(jcomposite_t::Src);

      sg_jcanvas_window->Paint(g);

      g->Flush();

      Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());
        
      cairo_surface_t *cairo_surface = g->GetCairoSurface();

      // cairo_surface_flush(cairo_surface);
      cairo_set_source_surface(cr, cairo_surface, 0, 0);
      cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
      cairo_paint(cr);

      sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
    }

  private:
    static jkeyevent_symbol_t TranslateToNativeKeySymbol(jx::EventKey symbol)
    {
      switch (symbol) {
        case jx::EventKey::Enter:
          return jkeyevent_symbol_t::Enter;
        case jx::EventKey::Backspace:
        	return jkeyevent_symbol_t::Backspace;
        case jx::EventKey::Tab:
          return jkeyevent_symbol_t::Tab;
        case jx::EventKey::Escape:
          return jkeyevent_symbol_t::Escape;
        case jx::EventKey::Space:
          return jkeyevent_symbol_t::Space;
        case jx::EventKey::Exclamation:
          return jkeyevent_symbol_t::ExclamationMark;
        case jx::EventKey::DoubleQuotes:
          return jkeyevent_symbol_t::Quotation;
        case jx::EventKey::Hash:
          return jkeyevent_symbol_t::Hash;
        case jx::EventKey::Dollar:
          return jkeyevent_symbol_t::Dollar;
        case jx::EventKey::Percent:
          return jkeyevent_symbol_t::Percent;
        case jx::EventKey::Ampersand:
          return jkeyevent_symbol_t::Ampersand;
        case jx::EventKey::Quote:
          return jkeyevent_symbol_t::Aposthrophe;
        case jx::EventKey::LeftParentesis:
          return jkeyevent_symbol_t::ParenthesisLeft;
        case jx::EventKey::RightParentesis:
          return jkeyevent_symbol_t::ParenthesisRight;
        case jx::EventKey::Star:
          return jkeyevent_symbol_t::Star;
        case jx::EventKey::Plus:
          return jkeyevent_symbol_t::Plus;
        case jx::EventKey::Comma:
          return jkeyevent_symbol_t::Comma;
        case jx::EventKey::Minus:
          return jkeyevent_symbol_t::Minus;
        case jx::EventKey::Dot:
          return jkeyevent_symbol_t::Period;
        case jx::EventKey::Slash:
          return jkeyevent_symbol_t::Slash;
        case jx::EventKey::Zero:
          return jkeyevent_symbol_t::Number0;
        case jx::EventKey::One:
          return jkeyevent_symbol_t::Number1;
        case jx::EventKey::Two:
          return jkeyevent_symbol_t::Number2;
        case jx::EventKey::Three:
          return jkeyevent_symbol_t::Number3;
        case jx::EventKey::Four:
          return jkeyevent_symbol_t::Number4;
        case jx::EventKey::Five:
          return jkeyevent_symbol_t::Number5;
        case jx::EventKey::Six:
          return jkeyevent_symbol_t::Number6;
        case jx::EventKey::Seven:
          return jkeyevent_symbol_t::Number7;
        case jx::EventKey::Eight:
          return jkeyevent_symbol_t::Number8;
        case jx::EventKey::Nine:
          return jkeyevent_symbol_t::Number9;
        case jx::EventKey::Colon:
          return jkeyevent_symbol_t::Colon;
        case jx::EventKey::SemiColon:
          return jkeyevent_symbol_t::SemiColon;
        case jx::EventKey::LessThan:
          return jkeyevent_symbol_t::LessThan;
        case jx::EventKey::Equals:
          return jkeyevent_symbol_t::Equals;
        case jx::EventKey::GreaterThan:
          return jkeyevent_symbol_t::GreaterThan;
        case jx::EventKey::Question:
          return jkeyevent_symbol_t::QuestionMark;
        case jx::EventKey::At:
          return jkeyevent_symbol_t::At;
        case jx::EventKey::LeftBracket:
          return jkeyevent_symbol_t::SquareBracketLeft;
        case jx::EventKey::BackSlash:
          return jkeyevent_symbol_t::BackSlash;
        case jx::EventKey::RightBracket:
          return jkeyevent_symbol_t::SquareBracketRight;
        case jx::EventKey::Circumflex:
          return jkeyevent_symbol_t::CircumflexAccent;
        case jx::EventKey::Underscore:
          return jkeyevent_symbol_t::Underscore;
        case jx::EventKey::Grave:
          return jkeyevent_symbol_t::GraveAccent;
        case jx::EventKey::A:
          return jkeyevent_symbol_t::a;
        case jx::EventKey::B:
          return jkeyevent_symbol_t::b;
        case jx::EventKey::C:
          return jkeyevent_symbol_t::c;
        case jx::EventKey::D:
          return jkeyevent_symbol_t::d;
        case jx::EventKey::E:
          return jkeyevent_symbol_t::e;
        case jx::EventKey::F:
          return jkeyevent_symbol_t::f;
        case jx::EventKey::G:
          return jkeyevent_symbol_t::g;
        case jx::EventKey::H:
          return jkeyevent_symbol_t::h;
        case jx::EventKey::I:
          return jkeyevent_symbol_t::i;
        case jx::EventKey::J:
          return jkeyevent_symbol_t::j;
        case jx::EventKey::K:
          return jkeyevent_symbol_t::k;
        case jx::EventKey::L:
          return jkeyevent_symbol_t::l;
        case jx::EventKey::M:
          return jkeyevent_symbol_t::m;
        case jx::EventKey::N:
          return jkeyevent_symbol_t::n;
        case jx::EventKey::O:
          return jkeyevent_symbol_t::o;
        case jx::EventKey::P:
          return jkeyevent_symbol_t::p;
        case jx::EventKey::Q:
          return jkeyevent_symbol_t::q;
        case jx::EventKey::R:
          return jkeyevent_symbol_t::r;
        case jx::EventKey::S:
          return jkeyevent_symbol_t::s;
        case jx::EventKey::T:
          return jkeyevent_symbol_t::t;
        case jx::EventKey::U:
          return jkeyevent_symbol_t::u;
        case jx::EventKey::V:
          return jkeyevent_symbol_t::v;
        case jx::EventKey::W:
          return jkeyevent_symbol_t::w;
        case jx::EventKey::X:
          return jkeyevent_symbol_t::x;
        case jx::EventKey::Y:
          return jkeyevent_symbol_t::y;
        case jx::EventKey::Z:
          return jkeyevent_symbol_t::z;
        case jx::EventKey::LeftCurlyBracket:
          return jkeyevent_symbol_t::CurlyBracketLeft;
        case jx::EventKey::Bar:
          return jkeyevent_symbol_t::VerticalBar;
        case jx::EventKey::RightCurlyBracket:
          return jkeyevent_symbol_t::CurlyBracketRight;
        case jx::EventKey::Tilde:
          return jkeyevent_symbol_t::Tilde;
        default: 
          break;
      }

      return jkeyevent_symbol_t::Unknown;
    }

};

void Application::Init(int argc, char **argv)
{
	sg_screen.x = 1280;
	sg_screen.y = 720;
  
  sg_quitting = false;
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  sg_window->loop();

  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  // sg_window->Release();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	if (sg_window > 0) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");
	
  sg_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  sg_window = new App();

  sg_window->connect();

  if (sg_window == 0) {
		throw std::runtime_error("Cannot create a window");
  }
}

WindowAdapter::~WindowAdapter()
{
  delete sg_back_buffer;
  sg_back_buffer = nullptr;

  delete sg_window;
  sg_window = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_window->requestPaint();
}

void WindowAdapter::ToggleFullScreen()
{
}

void WindowAdapter::SetTitle(std::string title)
{
  sg_title = title;
}

std::string WindowAdapter::GetTitle()
{
  return sg_title;
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
	return 1.0f;
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
    0, 0, sg_screen
  };
}
		
void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = true;
}

bool WindowAdapter::IsVisible()
{
	return sg_visible;
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

jcursor_style_t WindowAdapter::GetCursor()
{
  jx::Cursor id = sg_window->currentCursor();

  if (id == jx::Cursor::Default) {
    return jcursor_style_t::Default;
  } else if (id == jx::Cursor::Crosshair) {
    return jcursor_style_t::Crosshair;
  } else if (id == jx::Cursor::East) {
    return jcursor_style_t::East;
  } else if (id == jx::Cursor::West) {
    return jcursor_style_t::West;
  } else if (id == jx::Cursor::North) {
    return jcursor_style_t::North;
  } else if (id == jx::Cursor::South) {
    return jcursor_style_t::South;
  } else if (id == jx::Cursor::Hand) {
    return jcursor_style_t::Hand;
  } else if (id == jx::Cursor::Move) {
    return jcursor_style_t::Move;
  } else if (id == jx::Cursor::NS) {
    return jcursor_style_t::Vertical;
  } else if (id == jx::Cursor::WE) {
    return jcursor_style_t::Horizontal;
  } else if (id == jx::Cursor::NWCorner) {
    return jcursor_style_t::NorthWest;
  } else if (id == jx::Cursor::NECorner) {
    return jcursor_style_t::NorthEast;
  } else if (id == jx::Cursor::SWCorner) {
    return jcursor_style_t::SouthWest;
  } else if (id == jx::Cursor::SECorner) {
    return jcursor_style_t::SouthEast;
  } else if (id == jx::Cursor::Text) {
    return jcursor_style_t::Text;
  } else if (id == jx::Cursor::Wait) {
    return jcursor_style_t::Wait;
  }

  return jcursor_style_t::Default;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
}

bool WindowAdapter::IsCursorEnabled()
{
	return false; // glutGet(GLUT_WINDOW_CURSOR) != jx::Cursor::NONE;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  jx::Cursor type = jx::Cursor::Default;

  if (style == jcursor_style_t::Default) {
    type = jx::Cursor::Default;
  } else if (style == jcursor_style_t::Crosshair) {
    type = jx::Cursor::Crosshair;
  } else if (style == jcursor_style_t::East) {
    type = jx::Cursor::East;
  } else if (style == jcursor_style_t::West) {
    type = jx::Cursor::West;
  } else if (style == jcursor_style_t::North) {
    type = jx::Cursor::North;
  } else if (style == jcursor_style_t::South) {
    type = jx::Cursor::South;
  } else if (style == jcursor_style_t::Hand) {
    type = jx::Cursor::Hand;
  } else if (style == jcursor_style_t::Move) {
    type = jx::Cursor::Move;
  } else if (style == jcursor_style_t::Vertical) {
    type = jx::Cursor::NS;
  } else if (style == jcursor_style_t::Horizontal) {
    type = jx::Cursor::WE;
  } else if (style == jcursor_style_t::NorthWest) {
    type = jx::Cursor::NWCorner;
  } else if (style == jcursor_style_t::NorthEast) {
    type = jx::Cursor::NECorner;
  } else if (style == jcursor_style_t::SouthWest) {
    type = jx::Cursor::SWCorner;
  } else if (style == jcursor_style_t::SouthEast) {
    type = jx::Cursor::SECorner;
  } else if (style == jcursor_style_t::Text) {
    type = jx::Cursor::Text;
  } else if (style == jcursor_style_t::Wait) {
    type = jx::Cursor::Wait;
  }

  sg_window->changeCursor(type);
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
