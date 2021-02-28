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
  jcanvas::Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
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
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;
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

    bg->resource = vc_dispmanx_resource_create(bg->image.type, bg->image.x, bg->image.y, &vc_image_ptr);

    if (bg->resource == 0) {
	    return false;
    }

    vc_dispmanx_rect_set(&src_rect, 0, 0, SW << 16, SH << 16);
    vc_dispmanx_rect_set(&dst_rect, 0, 0, bg->info.x, bg->info.y);

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
  if (layer_init(&sg_layer) == false) {
		throw std::runtime_error("Problem to init dispmanx");
  }

	sg_screen.x = sg_layer.info.x;
	sg_screen.y = sg_layer.info.y;

#define CURSOR_INIT(type, ix, iy, hotx, hoty)\
	t.cursor = new jcanvas::BufferedImage(JPF_ARGB, {w, h});\
	t.hot_x = hotx;\
	t.hot_y = hoty;\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jcanvas::jpoint_t<int>{0, 0});	\
	sgsg_jcanvas_cursors[type] = t;\

  /*
	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = new jcanvas::BufferedImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
	delete cursors;
  */
  
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
    
  if (sg_cursor_enabled == true) {
    // g->DrawImage(sg_cursor_params.cursor, jcanvas::jpoint_t<int>{sg_mouse_x, sg_mouse_y});
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

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
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

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
     
      x = (x < 0)?0:(x > SW)?SW:x;
      y = (y < 0)?0:(y > SH)?SH:y;

      jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
      jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
      int mouse_z = 0;

      type = jcanvas::JMT_PRESSED;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jcanvas::JMT_MOVED;
      }

      sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

      if ((buttonMask & 0x01) == 0 && (lastsg_mouse_state & 0x01)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (lastsg_mouse_state & 0x02)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (lastsg_mouse_state & 0x04)) {
        type = jcanvas::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (lastsg_mouse_state & 0x01)) {
        button = jcanvas::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (lastsg_mouse_state & 0x02)) {
        button = jcanvas::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (lastsg_mouse_state & 0x04)) {
        button = jcanvas::JMB_BUTTON2;
      }

      lastsg_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
    }
  }

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));

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

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
{
	if (sg_jcanvas_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }
  
  sg_jcanvas_window = parent;

  SetCursor(sgsg_jcanvas_cursors[JCS_DEFAULT].cursor, sgsg_jcanvas_cursors[JCS_DEFAULT].hot_x, sgsg_jcanvas_cursors[JCS_DEFAULT].hot_y);
}

NativeWindow::~NativeWindow()
{
  layer_release(&sg_layer);
  
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
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
  return {
    0,
    0,
    SW,
    SH
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
  sg_mouse_x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = sg_mouse_x;
	p.y = sg_mouse_y;

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
}

bool NativeWindow::IsVisible()
{
  // TODO:: definir a saida para nao travar os apps
  return true;
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
	
  SetCursor(sgsg_jcanvas_cursors[style].cursor, sgsg_jcanvas_cursors[style].hot_x, sgsg_jcanvas_cursors[style].hot_y);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

  if (sg_cursor_params.cursor != nullptr) {
    delete sg_cursor_params.cursor;
    sg_cursor_params.cursor = nullptr;
  }

  sg_cursor_params.cursor = dynamic_cast<jcanvas::Image *>(shape->Clone());

  sg_cursor_params.hot_x = hotx;
  sg_cursor_params.hot_y = hoty;
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
