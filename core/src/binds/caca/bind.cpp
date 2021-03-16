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

#include <caca.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

namespace jcanvas {

/** \brief */
static std::shared_ptr<Image> sg_back_buffer;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static caca_display_t *dp = nullptr;
/** \brief */
static cucul_canvas_t *cv = nullptr;
/** \brief */
static cucul_dither_t *dither = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 0x1c:
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		case 0x0e:
			return jkeyevent_symbol_t::Backspace;
		case 0x0f:
			return jkeyevent_symbol_t::Tab;
		// case SDLK_CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case 0x01:
			return jkeyevent_symbol_t::Escape;
		case 0x39:
			return jkeyevent_symbol_t::Space;
		case 0x29:
			return jkeyevent_symbol_t::Aposthrophe;
		case 0x33:
			return jkeyevent_symbol_t::Comma;
		case 0x0c:
			return jkeyevent_symbol_t::Minus;
		case 0x34:  
			return jkeyevent_symbol_t::Period;
		case 0x59:
			return jkeyevent_symbol_t::Slash;
		case 0x0b:     
			return jkeyevent_symbol_t::Number0;
		case 0x02:
			return jkeyevent_symbol_t::Number1;
		case 0x03:
			return jkeyevent_symbol_t::Number2;
		case 0x04:
			return jkeyevent_symbol_t::Number3;
		case 0x05:
			return jkeyevent_symbol_t::Number4;
		case 0x06:
			return jkeyevent_symbol_t::Number5;
		case 0x07:
			return jkeyevent_symbol_t::Number6;
		case 0x08:
			return jkeyevent_symbol_t::Number7;
		case 0x09:
			return jkeyevent_symbol_t::Number8;
		case 0x0a:
			return jkeyevent_symbol_t::Number9;
		case 0x35:
			return jkeyevent_symbol_t::SemiColon;
		case 0x0d: 
			return jkeyevent_symbol_t::Equals;
		case 0x1b:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case 0x56:   
			return jkeyevent_symbol_t::BackSlash;
		case 0x2b:
			return jkeyevent_symbol_t::SquareBracketRight;
		case 0x1e:       
			return jkeyevent_symbol_t::a;
		case 0x30:
			return jkeyevent_symbol_t::b;
		case 0x2e:
			return jkeyevent_symbol_t::c;
		case 0x20:
			return jkeyevent_symbol_t::d;
		case 0x12:
			return jkeyevent_symbol_t::e;
		case 0x21:
			return jkeyevent_symbol_t::f;
		case 0x22:
			return jkeyevent_symbol_t::g;
		case 0x23:
			return jkeyevent_symbol_t::h;
		case 0x17:
			return jkeyevent_symbol_t::i;
		case 0x24:
			return jkeyevent_symbol_t::j;
		case 0x25:
			return jkeyevent_symbol_t::k;
		case 0x26:
			return jkeyevent_symbol_t::l;
		case 0x32:
			return jkeyevent_symbol_t::m;
		case 0x31:
			return jkeyevent_symbol_t::n;
		case 0x18:
			return jkeyevent_symbol_t::o;
		case 0x19:
			return jkeyevent_symbol_t::p;
		case 0x10:
			return jkeyevent_symbol_t::q;
		case 0x13:
			return jkeyevent_symbol_t::r;
		case 0x1f:
			return jkeyevent_symbol_t::s;
		case 0x14:
			return jkeyevent_symbol_t::t;
		case 0x16:
			return jkeyevent_symbol_t::u;
		case 0x2f:
			return jkeyevent_symbol_t::v;
		case 0x11:
			return jkeyevent_symbol_t::w;
		case 0x2d:
			return jkeyevent_symbol_t::x;
		case 0x15:
			return jkeyevent_symbol_t::y;
		case 0x2c:
			return jkeyevent_symbol_t::z;
		// case SDLK_BACKQUOTE:
		//	return jkeyevent_symbol_t::GraveAccent;
		case 0x28:  
			return jkeyevent_symbol_t::Tilde;
		case 0x6f:
			return jkeyevent_symbol_t::Delete;
		case 0x69:
			return jkeyevent_symbol_t::CursorLeft;
		case 0x6a:
			return jkeyevent_symbol_t::CursorRight;
		case 0x67:  
			return jkeyevent_symbol_t::CursorUp;
		case 0x6c:
			return jkeyevent_symbol_t::CursorDown;
		case 0x6e:  
			return jkeyevent_symbol_t::Insert;
		case 0x66:     
			return jkeyevent_symbol_t::Home;
		case 0x6b:
			return jkeyevent_symbol_t::End;
		case 0x68:
			return jkeyevent_symbol_t::PageUp;
		case 0x6d:
			return jkeyevent_symbol_t::PageDown;
		case 0x63:   
			return jkeyevent_symbol_t::Print;
		case 0x77:
			return jkeyevent_symbol_t::Pause;
		// case SDLK_RED:
		//	return jkeyevent_symbol_t::Red;
		// case SDLK_GREEN:
		//	return jkeyevent_symbol_t::Green;
		// case SDLK_YELLOW:
		//	return jkeyevent_symbol_t::Yellow;
		// case SDLK_BLUE:
		//	return jkeyevent_symbol_t::Blue;
		case 0x3b:
			return jkeyevent_symbol_t::F1;
		case 0x3c:
			return jkeyevent_symbol_t::F2;
		case 0x3d:
			return jkeyevent_symbol_t::F3;
		case 0x3e:
			return jkeyevent_symbol_t::F4;
		case 0x3f:
			return jkeyevent_symbol_t::F5;
		case 0x40:
			return jkeyevent_symbol_t::F6;
		case 0x41:    
			return jkeyevent_symbol_t::F7;
		case 0x42:
			return jkeyevent_symbol_t::F8;
		case 0x43:  
			return jkeyevent_symbol_t::F9;
		case 0x44: 
			return jkeyevent_symbol_t::F10;
		case 0x57:
			return jkeyevent_symbol_t::F11;
		case 0x58:
			return jkeyevent_symbol_t::F12;
		case 0x2a: // left
		case 0x36: // right
			return jkeyevent_symbol_t::Shift;
		case 0x1d: // left
		case 0x61: // right
			return jkeyevent_symbol_t::Control;
		case 0x38: // left
			return jkeyevent_symbol_t::Alt;
		case 0x64: 
		  return jkeyevent_symbol_t::AltGr;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jkeyevent_symbol_t::Meta;
		case 0x7d:
			return jkeyevent_symbol_t::Super;
		// case SDLK_HYPER:
		//	return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

void Application::Init(int argc, char **argv)
{
  cv = cucul_create_canvas(0, 0);

  if (cv == nullptr) {
    exit(-1);
  }

  dp = caca_create_display_with_driver(cv, nullptr); //"ncurses");

  if (dp == nullptr) {
    cucul_free_canvas(cv);

    exit(-1);
  }

	int width = cucul_get_canvas_width(cv);
	int height = cucul_get_canvas_height(cv);

  dither = cucul_create_dither(
      32, width, height, width*4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);

  if (dither == nullptr) {
    cucul_free_canvas(cv);
    caca_free_display(dp);

    exit(-1);
  }

  cucul_clear_canvas(cv);

	sg_screen.x = width;
	sg_screen.y = height;

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

	int iw = cucul_get_canvas_width(cv);
	int ih = cucul_get_canvas_height(cv);

  std::shared_ptr<Image> scale = sg_back_buffer->Scale({iw, ih});

  uint32_t *data = (uint32_t *)scale->LockData();

  if (dither != nullptr) {
    /* 
     * INFO:: algorithms for dithering
     * none
     * no dithering
     * ordered2
     * 2x2 ordered dithering
     * ordered4
     * 4x4 ordered dithering
     * ordered8
     * 8x8 ordered dithering
     * random
     * random dithering
     * fstein
     * Floyd-Steinberg dithering
     */

    caca_set_dither_algorithm(dither, "none");
    cucul_set_color_ansi(cv, CUCUL_COLOR_DEFAULT, CUCUL_COLOR_BLACK);
    cucul_dither_bitmap(cv, 0, 0, iw, ih, dither, data);
    caca_refresh_display(dp);
  }

  scale->UnlockData();
  
  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct caca_event cev;
  struct input_event ev;
  // bool shift = false;
  
  int 
    fdk = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    while (caca_get_event(dp, CACA_EVENT_ANY, &cev, 0) > 0) {
      caca_event_type mtype = caca_get_event_type(&cev);

      if (mtype == CACA_EVENT_MOUSE_PRESS or mtype == CACA_EVENT_MOUSE_RELEASE or mtype == CACA_EVENT_MOUSE_MOTION) {
        static int 
          buttonMask = 0x00;

        int 
          mbutton = cev.data.mouse.button;
        int 
          x = cev.data.mouse.x,
          y = cev.data.mouse.y;

        if (mtype == CACA_EVENT_MOUSE_PRESS) {
          buttonMask = buttonMask | (1 << (mbutton - 1));
        } else if (mtype == CACA_EVENT_MOUSE_RELEASE) {
          buttonMask = buttonMask & ~(1 << (mbutton - 1));
        }

        jmouseevent_button_t button = jmouseevent_button_t::None;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        if (mtype == CACA_EVENT_MOUSE_PRESS) {
          type = jmouseevent_type_t::Pressed;
        } else if (mtype == CACA_EVENT_MOUSE_RELEASE) {
          type = jmouseevent_type_t::Released;
        } else if (mtype == CACA_EVENT_MOUSE_MOTION) {
          type = jmouseevent_type_t::Moved;

          sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
          sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);
        }

        if (mbutton == 1) {
          button = jmouseevent_button_t::Button1;
        } else if (mbutton == 2) {
          button = jmouseevent_button_t::Button2;
        } else if (mbutton == 3) {
          button = jmouseevent_button_t::Button3;
        }

        int dx = (int)(sg_mouse_x*sg_screen.x)/(float)cucul_get_canvas_width(cv);
        int dy = (int)(sg_mouse_y*sg_screen.y)/(float)cucul_get_canvas_height(cv);

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {dx, dy}, mouse_z));
      } else if (mtype == CACA_EVENT_RESIZE) {
          if (dither != nullptr) {
            cucul_free_dither(dither);
          }

          dither = cucul_create_dither(
              32, cev.data.resize.w, cev.data.resize.h, cev.data.resize.w*4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);

          if (dither == nullptr) {
            sg_quitting = true;
          }

          InternalPaint();
      } else if (mtype == CACA_EVENT_QUIT) {
          sg_quitting = true;
      }
    }

    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

        if (ev.code == 0x2a) { // LSHIFT
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if (ev.code == 0x36) { // RSHIFT
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
        /*
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
        } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
        */
        }

        if (ev.value == 1 or ev.value == 2) {
          type = jkeyevent_type_t::Pressed;
        } else if (ev.value == 0) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      }
    }
  }

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));

  sg_quitting = true;
 
  close(fdk);

  sg_jcanvas_window->SetVisible(false);
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	sg_mouse_x = 0;
	sg_mouse_y = 0;

  sg_screen.x = bounds.size.x;
  sg_screen.y = bounds.size.y;

  sg_jcanvas_window = parent;
}

WindowAdapter::~WindowAdapter()
{
  cucul_clear_canvas(cv);

  if (dither != nullptr) {
    cucul_free_dither(dither);
  }
  
  if (dp != nullptr) {
    caca_free_display(dp);
  }
  
  if (cv != nullptr) {
    cucul_free_canvas(cv);
  }
  
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
  // caca_set_display_title(dp, _title.c_str());
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
  sg_screen.x = bounds.size.x;
  sg_screen.y = bounds.size.y;
}

jrect_t<int> WindowAdapter::GetBounds()
{
  return {
    0,
    0,
    sg_screen.x,
    sg_screen.y
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
  return sg_quitting == false;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;
}

bool WindowAdapter::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
	sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(std::shared_ptr<Image> image)
{
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return nullptr;
}

}
