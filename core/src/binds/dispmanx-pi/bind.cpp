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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <termio.h>
#include <fcntl.h>

#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>

extern "C" {
#include "bcm_host.h"
}

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

#define SW 1280
#define SH 720

namespace jcanvas {

struct image_t {
    VC_IMAGE_TYPE_T type;
    int32_t width;
    int32_t height;
    int32_t pitch;
    int32_t alignedHeight;
    uint16_t bitsPerPixel;
    uint32_t size;
    void *buffer;
};

struct layer_t {
    DISPMANX_DISPLAY_HANDLE_T display;
    DISPMANX_UPDATE_HANDLE_T update;
    DISPMANX_RESOURCE_HANDLE_T resource;
    DISPMANX_ELEMENT_HANDLE_T element;
    DISPMANX_MODEINFO_T info;
    image_t image;
    int32_t layer;
};

struct cursor_params_t {
  std::shared_ptr<Image> cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sgsg_jcanvas_cursors;
/** \brief */
static layer_t sg_layer;
/** \brief */
static int sg_mouse_x;
/** \brief */
static int sg_mouse_y;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static struct cursor_params_t sg_cursor_params;

bool image_init(image_t *image, VC_IMAGE_TYPE_T type, int32_t width, int32_t height)
{
    image->bitsPerPixel = 32;
    image->type = type;
    image->width = width;
    image->height = height;
    image->pitch = (ALIGN_TO_16(width) * image->bitsPerPixel) / 8;
    image->alignedHeight = ALIGN_TO_16(height);
    image->size = image->pitch * image->alignedHeight;

    image->buffer = calloc(1, 1);

    if (image->buffer == nullptr) {
	    return false;
    }

    return true;
}

void image_release(image_t *image)
{
    if (image->buffer) {
        free(image->buffer);
    
	image->buffer = nullptr;
    }

    image->type = VC_IMAGE_MIN;
    image->width = 0;
    image->height = 0;
    image->pitch = 0;
    image->alignedHeight = 0;
    image->bitsPerPixel = 0;
    image->size = 0;
}

bool layer_init(layer_t *bg)
{
    bcm_host_init();

    bg->display = vc_dispmanx_display_open(0);

    if (bg->display == 0) {
	    return false;
    }

    if (vc_dispmanx_display_get_info(bg->display, &bg->info) != 0) {
	    return false;
    }

    bg->update = vc_dispmanx_update_start(0);

    if (bg->update == 0) {
	    return false;
    }

    bg->layer = 0;

    VC_DISPMANX_ALPHA_T alpha = { 
	    DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 0x00, 0x00 
    };
    VC_RECT_T src_rect;
    VC_RECT_T dst_rect;
    uint32_t vc_image_ptr;

    if (image_init(&bg->image, VC_IMAGE_XRGB8888, SW, SH) == false) { // RGBA32
	    return false;
    }

    bg->resource = vc_dispmanx_resource_create(bg->image.type, bg->image.width, bg->image.height, &vc_image_ptr);

    if (bg->resource == 0) {
	    return false;
    }

    vc_dispmanx_rect_set(&src_rect, 0, 0, SW << 16, SH << 16);
    vc_dispmanx_rect_set(&dst_rect, 0, 0, bg->info.width, bg->info.height);

    bg->element = vc_dispmanx_element_add(
		    bg->update, bg->display, 2000, &dst_rect, bg->resource, &src_rect, DISPMANX_PROTECTION_NONE, &alpha, nullptr, DISPMANX_NO_ROTATE);

    if (bg->element == 0) {
	    return false;
    }

    if (vc_dispmanx_update_submit(bg->update, nullptr, nullptr) != 0) {
    // if (vc_dispmanx_update_submit_sync(bg->update) != 0) {
	    return false;
    }

    return true;
}

void layer_release(layer_t *bg)
{
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

    image_release(&bg->image);

    vc_dispmanx_element_remove(update, bg->element);
    vc_dispmanx_update_submit_sync(update);
    vc_dispmanx_resource_delete(bg->resource);
    vc_dispmanx_display_close(bg->display);
}

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
  if (layer_init(&sg_layer) == false) {
		throw std::runtime_error("Problem to init dispmanx");
  }

	sg_screen.x = sg_layer.info.width;
	sg_screen.y = sg_layer.info.height;

#define CURSOR_INIT(type, ix, iy, hotx, hoty)\
	t.cursor = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{w, h});\
	t.hot_x = hotx;\
	t.hot_y = hoty;\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jpoint_t<int>{0, 0});	\
	sgsg_jcanvas_cursors[type] = t;\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	std::shared_ptr<Image> cursors = std::make_shared<BufferedImage>(JCANVAS_RESOURCES_DIR "/images/cursors.png");

	CURSOR_INIT(jcursor_style_t::Default, 0, 0, 8, 8);
	CURSOR_INIT(jcursor_style_t::Crosshair, 4, 3, 15, 15);
	CURSOR_INIT(jcursor_style_t::East, 4, 4, 22, 15);
	CURSOR_INIT(jcursor_style_t::West, 5, 4, 9, 15);
	CURSOR_INIT(jcursor_style_t::North, 6, 4, 15, 8);
	CURSOR_INIT(jcursor_style_t::South, 7, 4, 15, 22);
	CURSOR_INIT(jcursor_style_t::Hand, 1, 0, 15, 15);
	CURSOR_INIT(jcursor_style_t::Move, 8, 4, 15, 15);
	CURSOR_INIT(jcursor_style_t::Vertical, 2, 4, 15, 15);
	CURSOR_INIT(jcursor_style_t::Horizontal, 3, 4, 15, 15);
	CURSOR_INIT(jcursor_style_t::NorthWest, 8, 1, 10, 10);
	CURSOR_INIT(jcursor_style_t::NorthEast, 9, 1, 20, 10);
	CURSOR_INIT(jcursor_style_t::SouthWest, 6, 1, 10, 20);
	CURSOR_INIT(jcursor_style_t::SouthEast, 7, 1, 20, 20);
	CURSOR_INIT(jcursor_style_t::Text, 7, 0, 15, 15);
	CURSOR_INIT(jcursor_style_t::Wait, 8, 0, 15, 15);
	
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
    
  if (sg_cursor_enabled == true) {
    // g->DrawImage(sg_cursor_params.cursor, jpoint_t<int>{sg_mouse_x, sg_mouse_y});
  }

  g->Flush();

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  // TODO:: nao alocar isso:: layer.image.buffer;
  VC_RECT_T 
    dst_rect;

  vc_dispmanx_rect_set(&dst_rect, 0, 0, SW, SH);

  sg_layer.update = vc_dispmanx_update_start(0);

  if (sg_layer.update != 0) {

    if (vc_dispmanx_resource_write_data(
          sg_layer.resource, sg_layer.image.type, sg_layer.image.pitch, data, &dst_rect) == 0) {
      if (vc_dispmanx_element_change_source(sg_layer.update, sg_layer.element, sg_layer.resource) == 0) {
        // if (g->IsVerticalSyncEnabled() == true) {
          vc_dispmanx_update_submit_sync(sg_layer.update);
        // }
      }
    }
  }

  sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct input_event ev;
  uint32_t lastsg_mouse_state = 0x00;
  int mouse_x = 0;
  int mouse_y = 0;
  
  int 
    fdk = open("/dev/input/by-path/platform-3f980000.usb-usb-0:1.4:1.0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  int 
    fdm = open("/dev/input/mice", O_RDONLY);

  if(fdm == -1) {   
    printf("Cannot open the mouse device\n");
  }   

  fcntl(fdm, F_SETFL, O_NONBLOCK);

	while (sg_quitting == false) {
    if (mouse_x != sg_mouse_x or mouse_y != sg_mouse_y) {
      mouse_x = sg_mouse_x;
      mouse_y = sg_mouse_y;

      if (sg_cursor_enabled == true) {
        sg_repaint.store(true);
      }
    }

    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
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

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
     
      x = (x < 0)?0:(x > SW)?SW:x;
      y = (y < 0)?0:(y > SH)?SH:y;

      jmouseevent_button_t button = jmouseevent_button_t::None;
      jmouseevent_type_t type = jmouseevent_type_t::Unknown;
      int mouse_z = 0;

      type = jmouseevent_type_t::Pressed;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jmouseevent_type_t::Moved;
      }

      sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

      if ((buttonMask & 0x01) == 0 && (lastsg_mouse_state & 0x01)) {
        type = jmouseevent_type_t::Released;
      } else if ((buttonMask & 0x02) == 0 && (lastsg_mouse_state & 0x02)) {
        type = jmouseevent_type_t::Released;
      } else if ((buttonMask & 0x04) == 0 && (lastsg_mouse_state & 0x04)) {
        type = jmouseevent_type_t::Released;
      } 

      if ((buttonMask & 0x01) != (lastsg_mouse_state & 0x01)) {
        button = jmouseevent_button_t::Button1;
      } else if ((buttonMask & 0x02) != (lastsg_mouse_state & 0x02)) {
        button = jmouseevent_button_t::Button3;
      } else if ((buttonMask & 0x04) != (lastsg_mouse_state & 0x04)) {
        button = jmouseevent_button_t::Button2;
      }

      lastsg_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
    }
  }

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));

  sg_quitting = true;
  
  close(fdk);
  close(fdm);

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
	if (sg_jcanvas_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }
  
  sg_jcanvas_window = parent;

  SetCursor(sgsg_jcanvas_cursors[jcursor_style_t::Default].cursor, sgsg_jcanvas_cursors[jcursor_style_t::Default].hot_x, sgsg_jcanvas_cursors[jcursor_style_t::Default].hot_y);
}

WindowAdapter::~WindowAdapter()
{
  layer_release(&sg_layer);
  
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
    0,
    0,
    SW,
    SH
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
  sg_mouse_x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = sg_mouse_x;
	p.y = sg_mouse_y;

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
}

bool WindowAdapter::IsVisible()
{
  // TODO:: definir a saida para nao travar os apps
  return true;
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
	
  SetCursor(sgsg_jcanvas_cursors[style].cursor, sgsg_jcanvas_cursors[style].hot_x, sgsg_jcanvas_cursors[style].hot_y);
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

  if (sg_cursor_params.cursor != nullptr) {
    sg_cursor_params.cursor = nullptr;
  }

  sg_cursor_params.cursor = shape->Clone();

  sg_cursor_params.hot_x = hotx;
  sg_cursor_params.hot_y = hoty;
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
