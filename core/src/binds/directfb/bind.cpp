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
#include "jcanvas/core/jresources.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#include <directfb.h>

namespace jcanvas {

struct cursor_params_t {
  std::shared_ptr<Image> cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
static std::shared_ptr<Image> sg_back_buffer;
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
static std::shared_ptr<Image> sg_jcanvas_icon;
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
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

jkeyevent_symbol_t TranslateToNativeKeySymbol(DFBInputDeviceKeySymbol symbol)
{
	switch (symbol) {
		case DIKS_NULL:
			return jkeyevent_symbol_t::Unknown;
		case DIKS_ENTER:
			return jkeyevent_symbol_t::Enter;
		case DIKS_BACKSPACE:
			return jkeyevent_symbol_t::Backspace;
		case DIKS_TAB:
			return jkeyevent_symbol_t::Tab;
		//case DIKS_RETURN:
		//	return jkeyevent_symbol_t::Return;
		case DIKS_CANCEL:
			return jkeyevent_symbol_t::Cancel;
		case DIKS_ESCAPE:
			return jkeyevent_symbol_t::Escape;
		case DIKS_SPACE:
			return jkeyevent_symbol_t::Space;
		case DIKS_EXCLAMATION_MARK:
			return jkeyevent_symbol_t::ExclamationMark;
		case DIKS_QUOTATION:
			return jkeyevent_symbol_t::Quotation;
		case DIKS_NUMBER_SIGN:
			return jkeyevent_symbol_t::Hash;
		case DIKS_DOLLAR_SIGN:
			return jkeyevent_symbol_t::Dollar;
		case DIKS_PERCENT_SIGN:
			return jkeyevent_symbol_t::Percent;
		case DIKS_AMPERSAND:   
			return jkeyevent_symbol_t::Ampersand;
		case DIKS_APOSTROPHE:
			return jkeyevent_symbol_t::Aposthrophe;
		case DIKS_PARENTHESIS_LEFT:
			return jkeyevent_symbol_t::ParenthesisLeft;
		case DIKS_PARENTHESIS_RIGHT:
			return jkeyevent_symbol_t::ParenthesisRight;
		case DIKS_ASTERISK:
			return jkeyevent_symbol_t::Star;
		case DIKS_PLUS_SIGN:
			return jkeyevent_symbol_t::Plus;
		case DIKS_COMMA:   
			return jkeyevent_symbol_t::Comma;
		case DIKS_MINUS_SIGN:
			return jkeyevent_symbol_t::Minus;
		case DIKS_PERIOD:  
			return jkeyevent_symbol_t::Period;
		case DIKS_SLASH:
			return jkeyevent_symbol_t::Slash;
		case DIKS_0:     
			return jkeyevent_symbol_t::Number0;
		case DIKS_1:
			return jkeyevent_symbol_t::Number1;
		case DIKS_2:
			return jkeyevent_symbol_t::Number2;
		case DIKS_3:
			return jkeyevent_symbol_t::Number3;
		case DIKS_4:
			return jkeyevent_symbol_t::Number4;
		case DIKS_5:
			return jkeyevent_symbol_t::Number5;
		case DIKS_6:
			return jkeyevent_symbol_t::Number6;
		case DIKS_7:
			return jkeyevent_symbol_t::Number7;
		case DIKS_8:
			return jkeyevent_symbol_t::Number8;
		case DIKS_9:
			return jkeyevent_symbol_t::Number9;
		case DIKS_COLON:
			return jkeyevent_symbol_t::Colon;
		case DIKS_SEMICOLON:
			return jkeyevent_symbol_t::SemiColon;
		case DIKS_LESS_THAN_SIGN:
			return jkeyevent_symbol_t::LessThan;
		case DIKS_EQUALS_SIGN: 
			return jkeyevent_symbol_t::Equals;
		case DIKS_GREATER_THAN_SIGN:
			return jkeyevent_symbol_t::GreaterThan;
		case DIKS_QUESTION_MARK:   
			return jkeyevent_symbol_t::QuestionMark;
		case DIKS_AT:      
			return jkeyevent_symbol_t::At;
		case DIKS_SQUARE_BRACKET_LEFT:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case DIKS_BACKSLASH:   
			return jkeyevent_symbol_t::BackSlash;
		case DIKS_SQUARE_BRACKET_RIGHT:
			return jkeyevent_symbol_t::SquareBracketRight;
		case DIKS_CIRCUMFLEX_ACCENT:
			return jkeyevent_symbol_t::CircumflexAccent;
		case DIKS_UNDERSCORE:    
			return jkeyevent_symbol_t::Underscore;
		case DIKS_GRAVE_ACCENT:
			return jkeyevent_symbol_t::GraveAccent;
		case DIKS_SMALL_A:       
			return jkeyevent_symbol_t::a;
		case DIKS_SMALL_B:
			return jkeyevent_symbol_t::b;
		case DIKS_SMALL_C:
			return jkeyevent_symbol_t::c;
		case DIKS_SMALL_D:
			return jkeyevent_symbol_t::d;
		case DIKS_SMALL_E:
			return jkeyevent_symbol_t::e;
		case DIKS_SMALL_F:
			return jkeyevent_symbol_t::f;
		case DIKS_SMALL_G:
			return jkeyevent_symbol_t::g;
		case DIKS_SMALL_H:
			return jkeyevent_symbol_t::h;
		case DIKS_SMALL_I:
			return jkeyevent_symbol_t::i;
		case DIKS_SMALL_J:
			return jkeyevent_symbol_t::j;
		case DIKS_SMALL_K:
			return jkeyevent_symbol_t::k;
		case DIKS_SMALL_L:
			return jkeyevent_symbol_t::l;
		case DIKS_SMALL_M:
			return jkeyevent_symbol_t::m;
		case DIKS_SMALL_N:
			return jkeyevent_symbol_t::n;
		case DIKS_SMALL_O:
			return jkeyevent_symbol_t::o;
		case DIKS_SMALL_P:
			return jkeyevent_symbol_t::p;
		case DIKS_SMALL_Q:
			return jkeyevent_symbol_t::q;
		case DIKS_SMALL_R:
			return jkeyevent_symbol_t::r;
		case DIKS_SMALL_S:
			return jkeyevent_symbol_t::s;
		case DIKS_SMALL_T:
			return jkeyevent_symbol_t::t;
		case DIKS_SMALL_U:
			return jkeyevent_symbol_t::u;
		case DIKS_SMALL_V:
			return jkeyevent_symbol_t::v;
		case DIKS_SMALL_W:
			return jkeyevent_symbol_t::w;
		case DIKS_SMALL_X:
			return jkeyevent_symbol_t::x;
		case DIKS_SMALL_Y:
			return jkeyevent_symbol_t::y;
		case DIKS_SMALL_Z:
			return jkeyevent_symbol_t::z;
		case DIKS_CURLY_BRACKET_LEFT:
			return jkeyevent_symbol_t::CurlyBracketLeft;
		case DIKS_VERTICAL_BAR:  
			return jkeyevent_symbol_t::VerticalBar;
		case DIKS_CURLY_BRACKET_RIGHT:
			return jkeyevent_symbol_t::CurlyBracketRight;
		case DIKS_TILDE:  
			return jkeyevent_symbol_t::Tilde;
		case DIKS_DELETE:
			return jkeyevent_symbol_t::Delete;
		case DIKS_CURSOR_LEFT:
			return jkeyevent_symbol_t::CursorLeft;
		case DIKS_CURSOR_RIGHT:
			return jkeyevent_symbol_t::CursorRight;
		case DIKS_CURSOR_UP:  
			return jkeyevent_symbol_t::CursorUp;
		case DIKS_CURSOR_DOWN:
			return jkeyevent_symbol_t::CursorDown;
		case DIKS_INSERT:  
			return jkeyevent_symbol_t::Insert;
		case DIKS_HOME:     
			return jkeyevent_symbol_t::Home;
		case DIKS_END:
			return jkeyevent_symbol_t::End;
		case DIKS_PAGE_UP:
			return jkeyevent_symbol_t::PageUp;
		case DIKS_PAGE_DOWN:
			return jkeyevent_symbol_t::PageDown;
		case DIKS_PRINT:   
			return jkeyevent_symbol_t::Print;
		case DIKS_PAUSE:
			return jkeyevent_symbol_t::Pause;
		case DIKS_RED:
			return jkeyevent_symbol_t::Red;
		case DIKS_GREEN:
			return jkeyevent_symbol_t::Green;
		case DIKS_YELLOW:
			return jkeyevent_symbol_t::Yellow;
		case DIKS_BLUE:
			return jkeyevent_symbol_t::Blue;
		case DIKS_F1:
			return jkeyevent_symbol_t::F1;
		case DIKS_F2:
			return jkeyevent_symbol_t::F2;
		case DIKS_F3:
			return jkeyevent_symbol_t::F3;
		case DIKS_F4:
			return jkeyevent_symbol_t::F4;
		case DIKS_F5:
			return jkeyevent_symbol_t::F5;
		case DIKS_F6:     
			return jkeyevent_symbol_t::F6;
		case DIKS_F7:    
			return jkeyevent_symbol_t::F7;
		case DIKS_F8:   
			return jkeyevent_symbol_t::F8;
		case DIKS_F9:  
			return jkeyevent_symbol_t::F9;
		case DIKS_F10: 
			return jkeyevent_symbol_t::F10;
		case DIKS_F11:
			return jkeyevent_symbol_t::F11;
		case DIKS_F12:
			return jkeyevent_symbol_t::F12;
		case DIKS_SHIFT:
			return jkeyevent_symbol_t::Shift;
		case DIKS_CONTROL:
			return jkeyevent_symbol_t::Control;
		case DIKS_ALT:
			return jkeyevent_symbol_t::Alt;
		case DIKS_ALTGR:
			return jkeyevent_symbol_t::AltGr;
		case DIKS_META:
			return jkeyevent_symbol_t::Meta;
		case DIKS_SUPER:
			return jkeyevent_symbol_t::Super;
		case DIKS_HYPER:
			return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
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
	t.cursor = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{w, h}); \
	t.hot_x = hotx; \
	t.hot_y = hoty; \
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jpoint_t<int>{0, 0}); \
	sg_jcanvas_cursors[type] = t; \

	struct cursor_params_t t;
	int w = 30,
			h = 30;

  std::shared_ptr<Image> cursors = std::make_shared<BufferedImage>(Resources::Path() + "/images/cursors.png");

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
	if (sg_jcanvas_window == nullptr || sg_surface == nullptr || sg_jcanvas_window->IsVisible() == false) {
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

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
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

    Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

    event_buffer->WaitForEventWithTimeout(event_buffer, 0, 0);

    while (event_buffer->GetEvent(event_buffer, DFB_EVENT(&event)) == DFB_OK) {
      event_buffer->Reset(event_buffer);

      if (event.type == DWET_ENTER) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
      } else if (event.type == DWET_LEAVE) {
        // SetCursor(jcursor_style_t::Default);

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
      } else if (event.type == DWET_CLOSE || event.type == DWET_DESTROYED) {
        sg_quitting = true;
      } else if (event.type == DWET_KEYDOWN || event.type == DWET_KEYUP) {
        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

        if ((event.flags & DIEF_MODIFIERS) != 0) {
          if ((event.modifiers & DIMM_SHIFT) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
          } else if ((event.modifiers & DIMM_CONTROL) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
          } else if ((event.modifiers & DIMM_ALT) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
          } else if ((event.modifiers & DIMM_ALTGR) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
          } else if ((event.modifiers & DIMM_META) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
          } else if ((event.modifiers & DIMM_SUPER) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
          } else if ((event.modifiers & DIMM_HYPER) != 0) {
            mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
          }
        }

        if (event.type == DWET_KEYDOWN) {
          type = jkeyevent_type_t::Pressed;
        } else if (event.type == DWET_KEYUP) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key_symbol);

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP || event.type == DWET_WHEEL || event.type == DWET_MOTION) {
        jmouseevent_button_t button = jmouseevent_button_t::None;
        jmouseevent_button_t buttons = jmouseevent_button_t::None;
        jmouseevent_type_t type = jmouseevent_type_t::Unknown;
        int mouse_z = 0;

        sg_mouse_x = event.cx;
        sg_mouse_y = event.cy;

        if (event.type == DWET_MOTION) {
          type = jmouseevent_type_t::Moved;
        } else if (event.type == DWET_WHEEL) {
          type = jmouseevent_type_t::Rotated;
          mouse_z = event.step;
        } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP) {
          if (event.type == DWET_BUTTONDOWN) {
            type = jmouseevent_type_t::Pressed;
          } else if (event.type == DWET_BUTTONUP) {
            type = jmouseevent_type_t::Released;
          }

          if (event.button == DIBI_LEFT) {
            button = jmouseevent_button_t::Button1;
          } else if (event.button == DIBI_RIGHT) {
            button = jmouseevent_button_t::Button2;
          } else if (event.button == DIBI_MIDDLE) {
            button = jmouseevent_button_t::Button3;
          }
        }

        if ((event.buttons & DIBM_LEFT) != 0) {
          buttons = jenum_t<jmouseevent_button_t>{button}.Or(jmouseevent_button_t::Button1);
        }

        if ((event.buttons & DIBM_RIGHT) != 0) {
          buttons = jenum_t<jmouseevent_button_t>{button}.Or(jmouseevent_button_t::Button2);
        }

        if ((event.buttons & DIBI_MIDDLE) != 0) {
          buttons = jenum_t<jmouseevent_button_t>{button}.Or(jmouseevent_button_t::Button3);
        }

        if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jmouseevent_button_t::None) {
          sg_window->GrabPointer(sg_window);
        } else {
          sg_window->UngrabPointer(sg_window);
        }

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      }
    }
  }

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
  
  SetCursor(sg_jcanvas_cursors[jcursor_style_t::Default].cursor, sg_jcanvas_cursors[jcursor_style_t::Default].hot_x, sg_jcanvas_cursors[jcursor_style_t::Default].hot_y);
}

WindowAdapter::~WindowAdapter()
{
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

jrect_t<int> WindowAdapter::GetBounds()
{
	jrect_t<int> t;

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

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

	IDirectFBSurface *surface = nullptr;
	DFBSurfaceDescription desc;
  jpoint_t<int> size = shape->GetSize();

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
