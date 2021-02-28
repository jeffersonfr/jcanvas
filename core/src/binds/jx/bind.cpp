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
#include "../include/nativewindow.h"

#include "jcanvas/core/jbufferedimage.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#include "jxclient.h"

namespace jcanvas {

/** \brief */
static jx::Client *sg_window = nullptr;
/** \brief */
static jcanvas::Image *sg_back_buffer = nullptr;
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
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
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
  
      jcanvas::jkeyevent_type_t 
        type {
          jcanvas::JKT_UNKNOWN};
      jcanvas::jkeyevent_modifiers_t 
        mod {
          };

      if (event.mod == jx::KeyModifiers::Shift) {
        mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
      } else if (event.mod == jx::KeyModifiers::Control) {
        mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
      } else if (event.mod == jx::KeyModifiers::Alt) {
        mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
      }

      if (event.down == true) {
        type = jcanvas::JKT_PRESSED;
      } else {
        type = jcanvas::JKT_RELEASED;
      }

      jcanvas::jkeyevent_symbol_t
        symbol = TranslateToNativeKeySymbol(event.id);

      sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
    }

    std::map<jcanvas::jmouseevent_button_t, bool> 
      sg_mouse_button_state;

    virtual void onPointer(const jx::PointerEvent &event) override
    {
      jcanvas::jmouseevent_button_t 
        button {
          jcanvas::JMB_NONE};
      jcanvas::jmouseevent_type_t 
        type {
          jcanvas::JMT_UNKNOWN};
      int 
        mouse_z = 0;

      sg_mouse_x = CLAMP(event.pos.x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(event.pos.y, 0, sg_screen.y - 1);

      if (event.button == jx::PointerButton::Button1) {
        button = jcanvas::JMB_BUTTON1;
      } else if (event.button == jx::PointerButton::Button2) {
        button = jcanvas::JMB_BUTTON2;
      } else if (event.button == jx::PointerButton::Button3) {
        button = jcanvas::JMB_BUTTON3;
      }

      if (event.button == jx::PointerButton::None) {
        type = jcanvas::JMT_MOVED;
      } else {
        if (event.down == true) {
          type = jcanvas::JMT_PRESSED;
        } else {
          type = jcanvas::JMT_RELEASED;
        }

        sg_mouse_button_state[button] = event.down;
      }

      sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
    }

    virtual void onPaint(cairo_t *cr) override
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
        
      g->Reset();
      g->SetCompositeFlags(jcanvas::JCF_SRC);

      sg_jcanvas_window->Paint(g);

      g->Flush();

      cairo_surface_t *cairo_surface = g->GetCairoSurface();

      // cairo_surface_flush(cairo_surface);
      cairo_set_source_surface(cr, cairo_surface, 0, 0);
      cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
      cairo_paint(cr);

      sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
    }

  private:
    static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(jx::EventKey symbol)
    {
      switch (symbol) {
        case jx::EventKey::Enter:
          return jcanvas::JKS_ENTER;
        case jx::EventKey::Backspace:
        	return jcanvas::JKS_BACKSPACE;
        case jx::EventKey::Tab:
          return jcanvas::JKS_TAB;
        case jx::EventKey::Escape:
          return jcanvas::JKS_ESCAPE;
        case jx::EventKey::Space:
          return jcanvas::JKS_SPACE;
        case jx::EventKey::Exclamation:
          return jcanvas::JKS_EXCLAMATION_MARK;
        case jx::EventKey::DoubleQuotes:
          return jcanvas::JKS_QUOTATION;
        case jx::EventKey::Hash:
          return jcanvas::JKS_NUMBER_SIGN;
        case jx::EventKey::Dollar:
          return jcanvas::JKS_DOLLAR_SIGN;
        case jx::EventKey::Percent:
          return jcanvas::JKS_PERCENT_SIGN;
        case jx::EventKey::Ampersand:
          return jcanvas::JKS_AMPERSAND;
        case jx::EventKey::Quote:
          return jcanvas::JKS_APOSTROPHE;
        case jx::EventKey::LeftParentesis:
          return jcanvas::JKS_PARENTHESIS_LEFT;
        case jx::EventKey::RightParentesis:
          return jcanvas::JKS_PARENTHESIS_RIGHT;
        case jx::EventKey::Star:
          return jcanvas::JKS_STAR;
        case jx::EventKey::Plus:
          return jcanvas::JKS_PLUS_SIGN;
        case jx::EventKey::Comma:
          return jcanvas::JKS_COMMA;
        case jx::EventKey::Minus:
          return jcanvas::JKS_MINUS_SIGN;
        case jx::EventKey::Dot:
          return jcanvas::JKS_PERIOD;
        case jx::EventKey::Slash:
          return jcanvas::JKS_SLASH;
        case jx::EventKey::Zero:
          return jcanvas::JKS_0;
        case jx::EventKey::One:
          return jcanvas::JKS_1;
        case jx::EventKey::Two:
          return jcanvas::JKS_2;
        case jx::EventKey::Three:
          return jcanvas::JKS_3;
        case jx::EventKey::Four:
          return jcanvas::JKS_4;
        case jx::EventKey::Five:
          return jcanvas::JKS_5;
        case jx::EventKey::Six:
          return jcanvas::JKS_6;
        case jx::EventKey::Seven:
          return jcanvas::JKS_7;
        case jx::EventKey::Eight:
          return jcanvas::JKS_8;
        case jx::EventKey::Nine:
          return jcanvas::JKS_9;
        case jx::EventKey::Colon:
          return jcanvas::JKS_COLON;
        case jx::EventKey::SemiColon:
          return jcanvas::JKS_SEMICOLON;
        case jx::EventKey::LessThan:
          return jcanvas::JKS_LESS_THAN_SIGN;
        case jx::EventKey::Equals:
          return jcanvas::JKS_EQUALS_SIGN;
        case jx::EventKey::GreaterThan:
          return jcanvas::JKS_GREATER_THAN_SIGN;
        case jx::EventKey::Question:
          return jcanvas::JKS_QUESTION_MARK;
        case jx::EventKey::At:
          return jcanvas::JKS_AT;
        case jx::EventKey::LeftBracket:
          return jcanvas::JKS_SQUARE_BRACKET_LEFT;
        case jx::EventKey::BackSlash:
          return jcanvas::JKS_BACKSLASH;
        case jx::EventKey::RightBracket:
          return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
        case jx::EventKey::Circumflex:
          return jcanvas::JKS_CIRCUMFLEX_ACCENT;
        case jx::EventKey::Underscore:
          return jcanvas::JKS_UNDERSCORE;
        case jx::EventKey::Grave:
          return jcanvas::JKS_GRAVE_ACCENT;
        case jx::EventKey::A:
          return jcanvas::JKS_a;
        case jx::EventKey::B:
          return jcanvas::JKS_b;
        case jx::EventKey::C:
          return jcanvas::JKS_c;
        case jx::EventKey::D:
          return jcanvas::JKS_d;
        case jx::EventKey::E:
          return jcanvas::JKS_e;
        case jx::EventKey::F:
          return jcanvas::JKS_f;
        case jx::EventKey::G:
          return jcanvas::JKS_g;
        case jx::EventKey::H:
          return jcanvas::JKS_h;
        case jx::EventKey::I:
          return jcanvas::JKS_i;
        case jx::EventKey::J:
          return jcanvas::JKS_j;
        case jx::EventKey::K:
          return jcanvas::JKS_k;
        case jx::EventKey::L:
          return jcanvas::JKS_l;
        case jx::EventKey::M:
          return jcanvas::JKS_m;
        case jx::EventKey::N:
          return jcanvas::JKS_n;
        case jx::EventKey::O:
          return jcanvas::JKS_o;
        case jx::EventKey::P:
          return jcanvas::JKS_p;
        case jx::EventKey::Q:
          return jcanvas::JKS_q;
        case jx::EventKey::R:
          return jcanvas::JKS_r;
        case jx::EventKey::S:
          return jcanvas::JKS_s;
        case jx::EventKey::T:
          return jcanvas::JKS_t;
        case jx::EventKey::U:
          return jcanvas::JKS_u;
        case jx::EventKey::V:
          return jcanvas::JKS_v;
        case jx::EventKey::W:
          return jcanvas::JKS_w;
        case jx::EventKey::X:
          return jcanvas::JKS_x;
        case jx::EventKey::Y:
          return jcanvas::JKS_y;
        case jx::EventKey::Z:
          return jcanvas::JKS_z;
        case jx::EventKey::LeftCurlyBracket:
          return jcanvas::JKS_CURLY_BRACKET_LEFT;
        case jx::EventKey::Bar:
          return jcanvas::JKS_VERTICAL_BAR;
        case jx::EventKey::RightCurlyBracket:
          return jcanvas::JKS_CURLY_BRACKET_RIGHT;
        case jx::EventKey::Tilde:
          return jcanvas::JKS_TILDE;
        default: 
          break;
      }

      return jcanvas::JKS_UNKNOWN;
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

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
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

NativeWindow::~NativeWindow()
{
  delete sg_back_buffer;
  sg_back_buffer = nullptr;

  delete sg_window;
  sg_window = nullptr;
}

void NativeWindow::Repaint()
{
  sg_window->requestPaint();
}

void NativeWindow::ToggleFullScreen()
{
}

void NativeWindow::SetTitle(std::string title)
{
  sg_title = title;
}

std::string NativeWindow::GetTitle()
{
  return sg_title;
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
	return 1.0f;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return true;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
  return {
    0, 0, sg_screen
  };
}
		
void NativeWindow::SetVisible(bool visible)
{
  sg_visible = true;
}

bool NativeWindow::IsVisible()
{
	return sg_visible;
}
		
void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return false;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	return p;
}

jcursor_style_t NativeWindow::GetCursor()
{
  jx::Cursor id = sg_window->currentCursor();

  if (id == jx::Cursor::Default) {
    return JCS_DEFAULT;
  } else if (id == jx::Cursor::Crosshair) {
    return JCS_CROSSHAIR;
  } else if (id == jx::Cursor::East) {
    return JCS_EAST;
  } else if (id == jx::Cursor::West) {
    return JCS_WEST;
  } else if (id == jx::Cursor::North) {
    return JCS_NORTH;
  } else if (id == jx::Cursor::South) {
    return JCS_SOUTH;
  } else if (id == jx::Cursor::Hand) {
    return JCS_HAND;
  } else if (id == jx::Cursor::Move) {
    return JCS_MOVE;
  } else if (id == jx::Cursor::NS) {
    return JCS_NS;
  } else if (id == jx::Cursor::WE) {
    return JCS_WE;
  } else if (id == jx::Cursor::NWCorner) {
    return JCS_NW_CORNER;
  } else if (id == jx::Cursor::NECorner) {
    return JCS_NE_CORNER;
  } else if (id == jx::Cursor::SWCorner) {
    return JCS_SW_CORNER;
  } else if (id == jx::Cursor::SECorner) {
    return JCS_SE_CORNER;
  } else if (id == jx::Cursor::Text) {
    return JCS_TEXT;
  } else if (id == jx::Cursor::Wait) {
    return JCS_WAIT;
  }

  return JCS_DEFAULT;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
}

bool NativeWindow::IsCursorEnabled()
{
	return false; // glutGet(GLUT_WINDOW_CURSOR) != jx::Cursor::NONE;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  jx::Cursor type = jx::Cursor::Default;

  if (style == JCS_DEFAULT) {
    type = jx::Cursor::Default;
  } else if (style == JCS_CROSSHAIR) {
    type = jx::Cursor::Crosshair;
  } else if (style == JCS_EAST) {
    type = jx::Cursor::East;
  } else if (style == JCS_WEST) {
    type = jx::Cursor::West;
  } else if (style == JCS_NORTH) {
    type = jx::Cursor::North;
  } else if (style == JCS_SOUTH) {
    type = jx::Cursor::South;
  } else if (style == JCS_HAND) {
    type = jx::Cursor::Hand;
  } else if (style == JCS_MOVE) {
    type = jx::Cursor::Move;
  } else if (style == JCS_NS) {
    type = jx::Cursor::NS;
  } else if (style == JCS_WE) {
    type = jx::Cursor::WE;
  } else if (style == JCS_NW_CORNER) {
    type = jx::Cursor::NWCorner;
  } else if (style == JCS_NE_CORNER) {
    type = jx::Cursor::NECorner;
  } else if (style == JCS_SW_CORNER) {
    type = jx::Cursor::SWCorner;
  } else if (style == JCS_SE_CORNER) {
    type = jx::Cursor::SECorner;
  } else if (style == JCS_TEXT) {
    type = jx::Cursor::Text;
  } else if (style == JCS_WAIT) {
    type = jx::Cursor::Wait;
  }

  sg_window->changeCursor(type);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
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

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jcanvas::JWR_NONE;
}

void NativeWindow::SetIcon(jcanvas::Image *image)
{
  sg_jcanvas_icon = image;
}

jcanvas::Image * NativeWindow::GetIcon()
{
  return sg_jcanvas_icon;
}

}
