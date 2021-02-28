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

#include <directfb.h>

namespace jcanvas {

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
static std::map<jcursor_style_t, struct cursor_params_t> sg_jcanvas_cursors;
/** \brief */
static IDirectFB *sg_directfb = nullptr;
/** \brief */
static IDirectFBDisplayLayer *sg_layer = nullptr;
/** \brief */
static IDirectFBWindow *sg_window = nullptr;
/** \brief */
static IDirectFBSurface *sg_surface = nullptr;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
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
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(DFBInputDeviceKeySymbol symbol)
{
	switch (symbol) {
		case DIKS_NULL:
			return jcanvas::JKS_UNKNOWN;
		case DIKS_ENTER:
			return jcanvas::JKS_ENTER;
		case DIKS_BACKSPACE:
			return jcanvas::JKS_BACKSPACE;
		case DIKS_TAB:
			return jcanvas::JKS_TAB;
		//case DIKS_RETURN:
		//	return jcanvas::JKS_RETURN;
		case DIKS_CANCEL:
			return jcanvas::JKS_CANCEL;
		case DIKS_ESCAPE:
			return jcanvas::JKS_ESCAPE;
		case DIKS_SPACE:
			return jcanvas::JKS_SPACE;
		case DIKS_EXCLAMATION_MARK:
			return jcanvas::JKS_EXCLAMATION_MARK;
		case DIKS_QUOTATION:
			return jcanvas::JKS_QUOTATION;
		case DIKS_NUMBER_SIGN:
			return jcanvas::JKS_NUMBER_SIGN;
		case DIKS_DOLLAR_SIGN:
			return jcanvas::JKS_DOLLAR_SIGN;
		case DIKS_PERCENT_SIGN:
			return jcanvas::JKS_PERCENT_SIGN;
		case DIKS_AMPERSAND:   
			return jcanvas::JKS_AMPERSAND;
		case DIKS_APOSTROPHE:
			return jcanvas::JKS_APOSTROPHE;
		case DIKS_PARENTHESIS_LEFT:
			return jcanvas::JKS_PARENTHESIS_LEFT;
		case DIKS_PARENTHESIS_RIGHT:
			return jcanvas::JKS_PARENTHESIS_RIGHT;
		case DIKS_ASTERISK:
			return jcanvas::JKS_STAR;
		case DIKS_PLUS_SIGN:
			return jcanvas::JKS_PLUS_SIGN;
		case DIKS_COMMA:   
			return jcanvas::JKS_COMMA;
		case DIKS_MINUS_SIGN:
			return jcanvas::JKS_MINUS_SIGN;
		case DIKS_PERIOD:  
			return jcanvas::JKS_PERIOD;
		case DIKS_SLASH:
			return jcanvas::JKS_SLASH;
		case DIKS_0:     
			return jcanvas::JKS_0;
		case DIKS_1:
			return jcanvas::JKS_1;
		case DIKS_2:
			return jcanvas::JKS_2;
		case DIKS_3:
			return jcanvas::JKS_3;
		case DIKS_4:
			return jcanvas::JKS_4;
		case DIKS_5:
			return jcanvas::JKS_5;
		case DIKS_6:
			return jcanvas::JKS_6;
		case DIKS_7:
			return jcanvas::JKS_7;
		case DIKS_8:
			return jcanvas::JKS_8;
		case DIKS_9:
			return jcanvas::JKS_9;
		case DIKS_COLON:
			return jcanvas::JKS_COLON;
		case DIKS_SEMICOLON:
			return jcanvas::JKS_SEMICOLON;
		case DIKS_LESS_THAN_SIGN:
			return jcanvas::JKS_LESS_THAN_SIGN;
		case DIKS_EQUALS_SIGN: 
			return jcanvas::JKS_EQUALS_SIGN;
		case DIKS_GREATER_THAN_SIGN:
			return jcanvas::JKS_GREATER_THAN_SIGN;
		case DIKS_QUESTION_MARK:   
			return jcanvas::JKS_QUESTION_MARK;
		case DIKS_AT:      
			return jcanvas::JKS_AT;
		case DIKS_SQUARE_BRACKET_LEFT:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case DIKS_BACKSLASH:   
			return jcanvas::JKS_BACKSLASH;
		case DIKS_SQUARE_BRACKET_RIGHT:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case DIKS_CIRCUMFLEX_ACCENT:
			return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		case DIKS_UNDERSCORE:    
			return jcanvas::JKS_UNDERSCORE;
		case DIKS_GRAVE_ACCENT:
			return jcanvas::JKS_GRAVE_ACCENT;
		case DIKS_SMALL_A:       
			return jcanvas::JKS_a;
		case DIKS_SMALL_B:
			return jcanvas::JKS_b;
		case DIKS_SMALL_C:
			return jcanvas::JKS_c;
		case DIKS_SMALL_D:
			return jcanvas::JKS_d;
		case DIKS_SMALL_E:
			return jcanvas::JKS_e;
		case DIKS_SMALL_F:
			return jcanvas::JKS_f;
		case DIKS_SMALL_G:
			return jcanvas::JKS_g;
		case DIKS_SMALL_H:
			return jcanvas::JKS_h;
		case DIKS_SMALL_I:
			return jcanvas::JKS_i;
		case DIKS_SMALL_J:
			return jcanvas::JKS_j;
		case DIKS_SMALL_K:
			return jcanvas::JKS_k;
		case DIKS_SMALL_L:
			return jcanvas::JKS_l;
		case DIKS_SMALL_M:
			return jcanvas::JKS_m;
		case DIKS_SMALL_N:
			return jcanvas::JKS_n;
		case DIKS_SMALL_O:
			return jcanvas::JKS_o;
		case DIKS_SMALL_P:
			return jcanvas::JKS_p;
		case DIKS_SMALL_Q:
			return jcanvas::JKS_q;
		case DIKS_SMALL_R:
			return jcanvas::JKS_r;
		case DIKS_SMALL_S:
			return jcanvas::JKS_s;
		case DIKS_SMALL_T:
			return jcanvas::JKS_t;
		case DIKS_SMALL_U:
			return jcanvas::JKS_u;
		case DIKS_SMALL_V:
			return jcanvas::JKS_v;
		case DIKS_SMALL_W:
			return jcanvas::JKS_w;
		case DIKS_SMALL_X:
			return jcanvas::JKS_x;
		case DIKS_SMALL_Y:
			return jcanvas::JKS_y;
		case DIKS_SMALL_Z:
			return jcanvas::JKS_z;
		case DIKS_CURLY_BRACKET_LEFT:
			return jcanvas::JKS_CURLY_BRACKET_LEFT;
		case DIKS_VERTICAL_BAR:  
			return jcanvas::JKS_VERTICAL_BAR;
		case DIKS_CURLY_BRACKET_RIGHT:
			return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case DIKS_TILDE:  
			return jcanvas::JKS_TILDE;
		case DIKS_DELETE:
			return jcanvas::JKS_DELETE;
		case DIKS_CURSOR_LEFT:
			return jcanvas::JKS_CURSOR_LEFT;
		case DIKS_CURSOR_RIGHT:
			return jcanvas::JKS_CURSOR_RIGHT;
		case DIKS_CURSOR_UP:  
			return jcanvas::JKS_CURSOR_UP;
		case DIKS_CURSOR_DOWN:
			return jcanvas::JKS_CURSOR_DOWN;
		case DIKS_INSERT:  
			return jcanvas::JKS_INSERT;
		case DIKS_HOME:     
			return jcanvas::JKS_HOME;
		case DIKS_END:
			return jcanvas::JKS_END;
		case DIKS_PAGE_UP:
			return jcanvas::JKS_PAGE_UP;
		case DIKS_PAGE_DOWN:
			return jcanvas::JKS_PAGE_DOWN;
		case DIKS_PRINT:   
			return jcanvas::JKS_PRINT;
		case DIKS_PAUSE:
			return jcanvas::JKS_PAUSE;
		case DIKS_RED:
			return jcanvas::JKS_RED;
		case DIKS_GREEN:
			return jcanvas::JKS_GREEN;
		case DIKS_YELLOW:
			return jcanvas::JKS_YELLOW;
		case DIKS_BLUE:
			return jcanvas::JKS_BLUE;
		case DIKS_F1:
			return jcanvas::JKS_F1;
		case DIKS_F2:
			return jcanvas::JKS_F2;
		case DIKS_F3:
			return jcanvas::JKS_F3;
		case DIKS_F4:
			return jcanvas::JKS_F4;
		case DIKS_F5:
			return jcanvas::JKS_F5;
		case DIKS_F6:     
			return jcanvas::JKS_F6;
		case DIKS_F7:    
			return jcanvas::JKS_F7;
		case DIKS_F8:   
			return jcanvas::JKS_F8;
		case DIKS_F9:  
			return jcanvas::JKS_F9;
		case DIKS_F10: 
			return jcanvas::JKS_F10;
		case DIKS_F11:
			return jcanvas::JKS_F11;
		case DIKS_F12:
			return jcanvas::JKS_F12;
		case DIKS_SHIFT:
			return jcanvas::JKS_SHIFT;
		case DIKS_CONTROL:
			return jcanvas::JKS_CONTROL;
		case DIKS_ALT:
			return jcanvas::JKS_ALT;
		case DIKS_ALTGR:
			return jcanvas::JKS_ALTGR;
		case DIKS_META:
			return jcanvas::JKS_META;
		case DIKS_SUPER:
			return jcanvas::JKS_SUPER;
		case DIKS_HYPER:
			return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
	if ((IDirectFB **)sg_directfb != nullptr) {
		return;
	}

	if (DirectFBInit(nullptr, 0) != DFB_OK) {
		throw std::runtime_error("Problem to init directfb");
	}

	// Create the super interface
	if (DirectFBCreate((IDirectFB **)&sg_directfb) != DFB_OK) {
		throw std::runtime_error("Problem to create directfb reference");
	}

	DFBDisplayLayerConfig config;

	// Get the primary display layer
	if (sg_directfb->GetDisplayLayer(sg_directfb, (DFBDisplayLayerID)DLID_PRIMARY, &sg_layer) != DFB_OK) {
		throw std::runtime_error("Problem to get display layer");
	}
	
	sg_layer->SetCooperativeLevel(sg_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// sg_layer->SetCooperativeLevel(sg_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	sg_layer->GetConfiguration(sg_layer, &config);

	sg_screen.x = config.width;
	sg_screen.y = config.height;
	
	sg_directfb->GetDeviceDescription(sg_directfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE | DLCONF_OPTIONS);
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
	  config.options = DLOP_FLICKER_FILTERING;

		sg_layer->SetConfiguration(sg_layer, &config);
	}

#define CURSOR_INIT(type, ix, iy, hotx, hoty) \
	t.cursor = new jcanvas::BufferedImage(JPF_ARGB, {w, h}); \
	t.hot_x = hotx; \
	t.hot_y = hoty; \
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jcanvas::jpoint_t<int>{0, 0}); \
	sg_jcanvas_cursors[type] = t; \

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
	if (sg_jcanvas_window == nullptr || sg_surface == nullptr || sg_jcanvas_window->IsVisible() == false) {
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

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

	DFBRectangle rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = bounds.size.x;
	rect.h = bounds.size.y;

	sg_surface->Write(sg_surface, &rect, data, bounds.size.x*4);
		
  if (g->IsVerticalSyncEnabled() == false) {
  	sg_surface->Flip(sg_surface, nullptr, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
  } else {
  	sg_surface->Flip(sg_surface, nullptr, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
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

  IDirectFBEventBuffer *event_buffer;
  DFBWindowEvent event;
  
  sg_window->CreateEventBuffer(sg_window, &event_buffer);

	if (event_buffer == nullptr) {
		return;
	}

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    event_buffer->WaitForEventWithTimeout(event_buffer, 0, 1);

    while (event_buffer->GetEvent(event_buffer, DFB_EVENT(&event)) == DFB_OK) {
      event_buffer->Reset(event_buffer);

      if (event.type == DWET_ENTER) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
      } else if (event.type == DWET_LEAVE) {
        // SetCursor(JCS_DEFAULT);

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
      } else if (event.type == DWET_CLOSE || event.type == DWET_DESTROYED) {
        sg_quitting = true;
      } else if (event.type == DWET_KEYDOWN || event.type == DWET_KEYUP) {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if ((event.flags & DIEF_MODIFIERS) != 0) {
          if ((event.modifiers & DIMM_SHIFT) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
          } else if ((event.modifiers & DIMM_CONTROL) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
          } else if ((event.modifiers & DIMM_ALT) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
          } else if ((event.modifiers & DIMM_ALTGR) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALTGR);
          } else if ((event.modifiers & DIMM_META) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_META);
          } else if ((event.modifiers & DIMM_SUPER) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
          } else if ((event.modifiers & DIMM_HYPER) != 0) {
            mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_HYPER);
          }
        }

        type = (jcanvas::jkeyevent_type_t)(0);

        if (event.type == DWET_KEYDOWN) {
          type = jcanvas::JKT_PRESSED;
        } else if (event.type == DWET_KEYUP) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key_symbol);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP || event.type == DWET_WHEEL || event.type == DWET_MOTION) {
        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event.cx;
        sg_mouse_y = event.cy;

        if (event.type == DWET_MOTION) {
          type = jcanvas::JMT_MOVED;
        } else if (event.type == DWET_WHEEL) {
          type = jcanvas::JMT_ROTATED;
          mouse_z = event.step;
        } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP) {
          if (event.type == DWET_BUTTONDOWN) {
            type = jcanvas::JMT_PRESSED;
          } else if (event.type == DWET_BUTTONUP) {
            type = jcanvas::JMT_RELEASED;
          }

          if (event.button == DIBI_LEFT) {
            button = jcanvas::JMB_BUTTON1;
          } else if (event.button == DIBI_RIGHT) {
            button = jcanvas::JMB_BUTTON2;
          } else if (event.button == DIBI_MIDDLE) {
            button = jcanvas::JMB_BUTTON3;
          }
        }

        if ((event.buttons & DIBM_LEFT) != 0) {
          buttons = (jcanvas::jmouseevent_button_t)(button | jcanvas::JMB_BUTTON1);
        }

        if ((event.buttons & DIBM_RIGHT) != 0) {
          buttons = (jcanvas::jmouseevent_button_t)(button | jcanvas::JMB_BUTTON2);
        }

        if ((event.buttons & DIBI_MIDDLE) != 0) {
          buttons = (jcanvas::jmouseevent_button_t)(button | jcanvas::JMB_BUTTON3);
        }

        if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
          sg_window->GrabPointer(sg_window);
        } else {
          sg_window->UngrabPointer(sg_window);
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      }
    }
  }

  sg_quitting = true;
  
	if (event_buffer != nullptr) {
		event_buffer->Release(event_buffer);
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

	DFBWindowDescription desc;

	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING | DWDESC_SURFACE_CAPS);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);
	desc.pixelformat = DSPF_ARGB;
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_DOUBLE);
	desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
	desc.stacking = DWSC_UPPER;
	desc.posx   = bounds.point.x;
	desc.posy   = bounds.point.y;
	desc.width  = bounds.size.x;
	desc.height = bounds.size.y;

	IDirectFBDisplayLayer *layer = nullptr;
	
	if (sg_directfb->GetDisplayLayer(sg_directfb, (DFBDisplayLayerID)(DLID_PRIMARY), &layer) != DFB_OK) {
		throw std::runtime_error("Problem to get the device layer");
	} 

	if (layer->CreateWindow(layer, &desc, &sg_window) != DFB_OK) {
		throw std::runtime_error("Cannot create a window");
	}

	if (sg_window->GetSurface(sg_window, &sg_surface) != DFB_OK) {
		sg_window->Release(sg_window);

		throw std::runtime_error("Cannot get a window's surface");
	}

    sg_window->GrabKeyboard(sg_window);
    sg_window->GrabPointer(sg_window);

	// Add ghost option (behave like an overlay)
	// sg_window->SetOptions(sg_window, (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
	// Move window to upper stacking class
	// sg_window->SetStackingClass(sg_window, DWSC_UPPER);
	// sg_window->RequestFocus(sg_window);
	// Make it the top most window
	// sg_window->RaiseToTop(sg_window);
	sg_window->SetOpacity(sg_window, 0xff);
	// sg_surface->SetRenderOptions(sg_surface, DSRO_ALL);
	// sg_window->DisableEvents(sg_window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));
	
	sg_surface->SetDrawingFlags(sg_surface, (DFBSurfaceDrawingFlags)(DSDRAW_NOFX)); // BLEND));
	sg_surface->SetBlittingFlags(sg_surface, (DFBSurfaceBlittingFlags)(DSBLIT_NOFX)); // BLEND_ALPHACHANNEL));
	sg_surface->SetPorterDuff(sg_surface, (DFBSurfacePorterDuffRule)(DSPD_NONE));

	sg_surface->Clear(sg_surface, 0x00, 0x00, 0x00, 0x00);
#if ((DIRECTFB_MAJOR_VERSION * 1000000) + (DIRECTFB_MINOR_VERSION * 1000) + DIRECTFB_MICRO_VERSION) >= 1007000
	sg_surface->Flip(sg_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_FLUSH));
#else
	sg_surface->Flip(sg_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
#endif
	sg_surface->Clear(sg_surface, 0x00, 0x00, 0x00, 0x00);
  
  SetCursor(sg_jcanvas_cursors[JCS_DEFAULT].cursor, sg_jcanvas_cursors[JCS_DEFAULT].hot_x, sg_jcanvas_cursors[JCS_DEFAULT].hot_y);
}

WindowAdapter::~WindowAdapter()
{
  delete sg_jcanvas_icon;
  sg_jcanvas_icon = nullptr;
  
  sg_window->UngrabKeyboard(sg_window);
  sg_window->UngrabPointer(sg_window);

	if (sg_surface != NULL) {
		sg_surface->Release(sg_surface);
	  sg_surface = NULL;
	}

	if (sg_window != NULL) {
		sg_window->SetOpacity(sg_window, 0x00);
		sg_window->Close(sg_window);
		// CHANGE:: freeze if resize before the first 'release' in tests/restore.cpp
		// sg_window->Destroy(sg_window);
		// sg_window->Release(sg_window);
	  sg_window = NULL;
	}

	if (sg_layer != nullptr) {
		sg_layer->Release(sg_layer);
		sg_layer = nullptr;
	}

	if (sg_directfb != nullptr) {
		sg_directfb->Release(sg_directfb);
		sg_directfb = nullptr;
	}
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  // TODO::
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
	sg_window->SetOpacity(sg_window, (int)(opacity * 255.0f));
}

float WindowAdapter::GetOpacity()
{
  uint8_t o;

  sg_window->GetOpacity(sg_window, &o);

	return (o * 100.0f)/255.0f;
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
  if (sg_window != NULL) {
    sg_window->SetBounds(sg_window, bounds.point.x, bounds.point.y, bounds.size.x, bounds.size.y);
    sg_window->ResizeSurface(sg_window, bounds.size.x, bounds.size.y);
  }
}

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
	jcanvas::jrect_t<int> t;

	sg_window->GetPosition(sg_window, &t.point.x, &t.point.y);
	sg_window->GetSize(sg_window, &t.size.x, &t.size.y);

	return t;
}
		
void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return true;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
	sg_layer->WarpCursor(sg_layer, x, y);
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	sg_layer->GetCursorPosition(sg_layer, &p.x, &p.y);

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
	if (visible == true) {
	  // SetOpacity(_opacity);
	} else {
	  SetOpacity(0x00);
  }
}

bool WindowAdapter::IsVisible()
{
  uint8_t o;

  sg_window->GetOpacity(sg_window, &o);

  if (o == 0x00) {
    return false;
  }

  return true;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;

	sg_layer->EnableCursor(sg_layer, sg_cursor_enabled);
}

bool WindowAdapter::IsCursorEnabled()
{
	return true;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
	sg_jcanvas_cursor = style;

	SetCursor(sg_jcanvas_cursors[style].cursor, sg_jcanvas_cursors[style].hot_x, sg_jcanvas_cursors[style].hot_y);
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	IDirectFBSurface *surface = nullptr;
	DFBSurfaceDescription desc;
  jcanvas::jpoint_t<int> size = shape->GetSize();

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.pixelformat = DSPF_ARGB;
	desc.width = size.x;
	desc.height = size.y;

	if (sg_directfb->CreateSurface(sg_directfb, &desc, &surface) != DFB_OK) {
		throw std::runtime_error("Cannot allocate memory to the image surface");
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	shape->GetGraphics()->GetRGBArray((uint32_t *)ptr, {0, 0, desc.width, desc.height});

	surface->Unlock(surface);

	sg_layer->SetCursorShape(sg_layer, surface, hotx, hoty);

	surface->Release(surface);
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
#if ((DIRECTFB_MAJOR_VERSION * 1000000) + (DIRECTFB_MINOR_VERSION * 1000) + DIRECTFB_MICRO_VERSION) >= 1007000
	if (sg_window != NULL) {
		sg_window->SetRotation(sg_window, rotation);
	}
#endif
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
