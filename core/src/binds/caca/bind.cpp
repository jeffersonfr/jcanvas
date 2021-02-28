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

#include <caca.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

namespace jcanvas {

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
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
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 0x1c:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case 0x0e:
			return jcanvas::JKS_BACKSPACE;
		case 0x0f:
			return jcanvas::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case 0x01:
			return jcanvas::JKS_ESCAPE;
		case 0x39:
			return jcanvas::JKS_SPACE;
		case 0x29:
			return jcanvas::JKS_APOSTROPHE;
		case 0x33:
			return jcanvas::JKS_COMMA;
		case 0x0c:
			return jcanvas::JKS_MINUS_SIGN;
		case 0x34:  
			return jcanvas::JKS_PERIOD;
		case 0x59:
			return jcanvas::JKS_SLASH;
		case 0x0b:     
			return jcanvas::JKS_0;
		case 0x02:
			return jcanvas::JKS_1;
		case 0x03:
			return jcanvas::JKS_2;
		case 0x04:
			return jcanvas::JKS_3;
		case 0x05:
			return jcanvas::JKS_4;
		case 0x06:
			return jcanvas::JKS_5;
		case 0x07:
			return jcanvas::JKS_6;
		case 0x08:
			return jcanvas::JKS_7;
		case 0x09:
			return jcanvas::JKS_8;
		case 0x0a:
			return jcanvas::JKS_9;
		case 0x35:
			return jcanvas::JKS_SEMICOLON;
		case 0x0d: 
			return jcanvas::JKS_EQUALS_SIGN;
		case 0x1b:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case 0x56:   
			return jcanvas::JKS_BACKSLASH;
		case 0x2b:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case 0x1e:       
			return jcanvas::JKS_a;
		case 0x30:
			return jcanvas::JKS_b;
		case 0x2e:
			return jcanvas::JKS_c;
		case 0x20:
			return jcanvas::JKS_d;
		case 0x12:
			return jcanvas::JKS_e;
		case 0x21:
			return jcanvas::JKS_f;
		case 0x22:
			return jcanvas::JKS_g;
		case 0x23:
			return jcanvas::JKS_h;
		case 0x17:
			return jcanvas::JKS_i;
		case 0x24:
			return jcanvas::JKS_j;
		case 0x25:
			return jcanvas::JKS_k;
		case 0x26:
			return jcanvas::JKS_l;
		case 0x32:
			return jcanvas::JKS_m;
		case 0x31:
			return jcanvas::JKS_n;
		case 0x18:
			return jcanvas::JKS_o;
		case 0x19:
			return jcanvas::JKS_p;
		case 0x10:
			return jcanvas::JKS_q;
		case 0x13:
			return jcanvas::JKS_r;
		case 0x1f:
			return jcanvas::JKS_s;
		case 0x14:
			return jcanvas::JKS_t;
		case 0x16:
			return jcanvas::JKS_u;
		case 0x2f:
			return jcanvas::JKS_v;
		case 0x11:
			return jcanvas::JKS_w;
		case 0x2d:
			return jcanvas::JKS_x;
		case 0x15:
			return jcanvas::JKS_y;
		case 0x2c:
			return jcanvas::JKS_z;
		// case SDLK_BACKQUOTE:
		//	return jcanvas::JKS_GRAVE_ACCENT;
		case 0x28:  
			return jcanvas::JKS_TILDE;
		case 0x6f:
			return jcanvas::JKS_DELETE;
		case 0x69:
			return jcanvas::JKS_CURSOR_LEFT;
		case 0x6a:
			return jcanvas::JKS_CURSOR_RIGHT;
		case 0x67:  
			return jcanvas::JKS_CURSOR_UP;
		case 0x6c:
			return jcanvas::JKS_CURSOR_DOWN;
		case 0x6e:  
			return jcanvas::JKS_INSERT;
		case 0x66:     
			return jcanvas::JKS_HOME;
		case 0x6b:
			return jcanvas::JKS_END;
		case 0x68:
			return jcanvas::JKS_PAGE_UP;
		case 0x6d:
			return jcanvas::JKS_PAGE_DOWN;
		case 0x63:   
			return jcanvas::JKS_PRINT;
		case 0x77:
			return jcanvas::JKS_PAUSE;
		// case SDLK_RED:
		//	return jcanvas::JKS_RED;
		// case SDLK_GREEN:
		//	return jcanvas::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jcanvas::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jcanvas::JKS_BLUE;
		case 0x3b:
			return jcanvas::JKS_F1;
		case 0x3c:
			return jcanvas::JKS_F2;
		case 0x3d:
			return jcanvas::JKS_F3;
		case 0x3e:
			return jcanvas::JKS_F4;
		case 0x3f:
			return jcanvas::JKS_F5;
		case 0x40:
			return jcanvas::JKS_F6;
		case 0x41:    
			return jcanvas::JKS_F7;
		case 0x42:
			return jcanvas::JKS_F8;
		case 0x43:  
			return jcanvas::JKS_F9;
		case 0x44: 
			return jcanvas::JKS_F10;
		case 0x57:
			return jcanvas::JKS_F11;
		case 0x58:
			return jcanvas::JKS_F12;
		case 0x2a: // left
		case 0x36: // right
			return jcanvas::JKS_SHIFT;
		case 0x1d: // left
		case 0x61: // right
			return jcanvas::JKS_CONTROL;
		case 0x38: // left
			return jcanvas::JKS_ALT;
		case 0x64: 
		  return jcanvas::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jcanvas::JKS_META;
		case 0x7d:
			return jcanvas::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
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

	int iw = cucul_get_canvas_width(cv);
	int ih = cucul_get_canvas_height(cv);

  jcanvas::Image *scale = sg_back_buffer->Scale({iw, ih});

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
  
  delete scale;

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
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

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        if (mtype == CACA_EVENT_MOUSE_PRESS) {
          type = jcanvas::JMT_PRESSED;
        } else if (mtype == CACA_EVENT_MOUSE_RELEASE) {
          type = jcanvas::JMT_RELEASED;
        } else if (mtype == CACA_EVENT_MOUSE_MOTION) {
          type = jcanvas::JMT_MOVED;

          sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
          sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);
        }

        if (mbutton == 1) {
          button = jcanvas::JMB_BUTTON1;
        } else if (mbutton == 2) {
          button = jcanvas::JMB_BUTTON2;
        } else if (mbutton == 3) {
          button = jcanvas::JMB_BUTTON3;
        }

        int dx = (int)(sg_mouse_x*sg_screen.x)/(float)cucul_get_canvas_width(cv);
        int dy = (int)(sg_mouse_y*sg_screen.y)/(float)cucul_get_canvas_height(cv);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {dx, dy}, mouse_z));
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
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if (ev.code == 0x2a) { // LSHIFT
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if (ev.code == 0x36) { // RSHIFT
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        /*
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
        } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        } else if ((event.key.keysym.mod & ) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        */
        }

        type = jcanvas::JKT_UNKNOWN;

        if (ev.value == 1 or ev.value == 2) {
          type = jcanvas::JKT_PRESSED;
        } else if (ev.value == 0) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      }
    }
  }

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));

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

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
{
	sg_mouse_x = 0;
	sg_mouse_y = 0;

  sg_screen.x = bounds.size.x;
  sg_screen.y = bounds.size.y;

  sg_jcanvas_window = parent;
}

NativeWindow::~NativeWindow()
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
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint()
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
}

void NativeWindow::SetTitle(std::string title)
{
  // caca_set_display_title(dp, _title.c_str());
}

std::string NativeWindow::GetTitle()
{
  return std::string();
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
	return 1.0;
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
  sg_screen.x = width;
  sg_screen.y = height;
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
  return {
    0,
    0,
    sg_screen.x,
    sg_screen.y
  };
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

void NativeWindow::SetVisible(bool visible)
{
  
}

bool NativeWindow::IsVisible()
{
  return sg_quitting == false;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jcanvas_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
	sg_jcanvas_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
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
}

jcanvas::Image * NativeWindow::GetIcon()
{
  return nullptr;
}

}
