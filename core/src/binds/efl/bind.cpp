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

#include "jmixin/jstring.h"

#include <thread>
#include <mutex>
#include <stdexcept>
#include <atomic>

#include <Ecore_Getopt.h>
#include <Elementary.h>

#include <stdio.h>
#include <errno.h>

namespace jcanvas {

/** \brief */
static Ecore_Evas *ee = nullptr;
/** \brief */
static Evas_Object *sg_window = nullptr;
/** \brief */
static Evas_Object *sg_surface = nullptr;
/** \brief */
static Evas_Coord sg_mouse_x = 0;
/** \brief */
static Evas_Coord sg_mouse_y = 0;
/** \brief */
// static Eina_Bool _mouse_down = false;
/** \brief */
static jrect_t<int> sg_bounds = {0, 0, 0, 0};
/** \brief */
static bool sg_visible = false;
/** \brief */
// static double _mouse_wheel_x = 0.0;
/** \brief */
// static double _mouse_wheel_y = 0.0;
/** \brief */
// static double _mouse_wheel_scale = 1.0;
/** \brief */
// static Ecore_Timer *_mouse_wheel_timer = nullptr;
/** \brief */
static std::shared_ptr<Image> sg_back_buffer = nullptr;
/** \brief */
static jmouseevent_button_t sg_button_state = jmouseevent_button_t::None;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(std::string key)
{
  key = jmixin::String(key).lower_case();

  if (key == "return" or key == "kp_enter") {
    return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
  } else if (key == "backspace") {
    return jkeyevent_symbol_t::Backspace;
  } else if (key == "tab") {
    return jkeyevent_symbol_t::Tab;
    // } else if (key == "") {
    //	return jkeyevent_symbol_t::Cancel;
  } else if (key == "escape") {
    return jkeyevent_symbol_t::Escape;
  } else if (key == "space") {
    return jkeyevent_symbol_t::Space;
  } else if (key == "exclam") {
    return jkeyevent_symbol_t::ExclamationMark;
  } else if (key == "apostrophe") {
    return jkeyevent_symbol_t::Quotation;
  } else if (key == "numbersign") {
    return jkeyevent_symbol_t::Hash;
  } else if (key == "dollar") {
    return jkeyevent_symbol_t::Dollar;
  } else if (key == "percent") {
    return jkeyevent_symbol_t::Percent;
  } else if (key == "ampersand") {   
    return jkeyevent_symbol_t::Ampersand;
  } else if (key == "apostrophe") {
    return jkeyevent_symbol_t::Aposthrophe;
  } else if (key == "parenleft") {
    return jkeyevent_symbol_t::ParenthesisLeft;
  } else if (key == "parenright") {
    return jkeyevent_symbol_t::ParenthesisRight;
  } else if (key == "asterisk") {
    return jkeyevent_symbol_t::Star;
  } else if (key == "plus") {
    return jkeyevent_symbol_t::Plus;
  } else if (key == "comma") {   
    return jkeyevent_symbol_t::Comma;
  } else if (key == "minus") {
    return jkeyevent_symbol_t::Minus;
  } else if (key == "period") {  
    return jkeyevent_symbol_t::Period;
  } else if (key == "slash") {
    return jkeyevent_symbol_t::Slash;
  } else if (key == "0") {     
    return jkeyevent_symbol_t::Number0;
  } else if (key == "1") {
    return jkeyevent_symbol_t::Number1;
  } else if (key == "2") {
    return jkeyevent_symbol_t::Number2;
  } else if (key == "3") {
    return jkeyevent_symbol_t::Number3;
  } else if (key == "4") {
    return jkeyevent_symbol_t::Number4;
  } else if (key == "5") {
    return jkeyevent_symbol_t::Number5;
  } else if (key == "6") {
    return jkeyevent_symbol_t::Number6;
  } else if (key == "7") {
    return jkeyevent_symbol_t::Number7;
  } else if (key == "8") {
    return jkeyevent_symbol_t::Number8;
  } else if (key == "9") {
    return jkeyevent_symbol_t::Number9;
  } else if (key == "colon") {
    return jkeyevent_symbol_t::Colon;
  } else if (key == "semicolon") {
    return jkeyevent_symbol_t::SemiColon;
  } else if (key == "less") {
    return jkeyevent_symbol_t::LessThan;
  } else if (key == "equal") { 
    return jkeyevent_symbol_t::Equals;
  } else if (key == "greater") {
    return jkeyevent_symbol_t::GreaterThan;
  } else if (key == "question") {   
    return jkeyevent_symbol_t::QuestionMark;
  } else if (key == "at") {
    return jkeyevent_symbol_t::At;
  } else if (key == "A") {
    return jkeyevent_symbol_t::A;
  } else if (key == "B") {
    return jkeyevent_symbol_t::B;
  } else if (key == "C") {
    return jkeyevent_symbol_t::C;
  } else if (key == "D") {
    return jkeyevent_symbol_t::D;
  } else if (key == "E") {
    return jkeyevent_symbol_t::E;
  } else if (key == "F") {
    return jkeyevent_symbol_t::F;
  } else if (key == "G") {
    return jkeyevent_symbol_t::G;
  } else if (key == "H") {
    return jkeyevent_symbol_t::H;
  } else if (key == "I") {
    return jkeyevent_symbol_t::I;
  } else if (key == "J") {
    return jkeyevent_symbol_t::J;
  } else if (key == "K") {
    return jkeyevent_symbol_t::K;
  } else if (key == "L") {
    return jkeyevent_symbol_t::L;
  } else if (key == "M") {
    return jkeyevent_symbol_t::M;
  } else if (key == "N") {
    return jkeyevent_symbol_t::N;
  } else if (key == "O") {
    return jkeyevent_symbol_t::O;
  } else if (key == "P") {
    return jkeyevent_symbol_t::P;
  } else if (key == "Q") {
    return jkeyevent_symbol_t::Q;
  } else if (key == "R") {
    return jkeyevent_symbol_t::R;
  } else if (key == "S") {
    return jkeyevent_symbol_t::S;
  } else if (key == "T") {
    return jkeyevent_symbol_t::T;
  } else if (key == "U") {
    return jkeyevent_symbol_t::U;
  } else if (key == "V") {
    return jkeyevent_symbol_t::V;
  } else if (key == "W") {
    return jkeyevent_symbol_t::W;
  } else if (key == "X") {
    return jkeyevent_symbol_t::X;
  } else if (key == "Y") {
    return jkeyevent_symbol_t::Y;
  } else if (key == "Z") {
    return jkeyevent_symbol_t::Z;
  } else if (key == "bracketleft") {
    return jkeyevent_symbol_t::SquareBracketLeft;
  } else if (key == "backslash") {
    return jkeyevent_symbol_t::BackSlash;
  } else if (key == "bracketright") {
    return jkeyevent_symbol_t::SquareBracketRight;
  } else if (key == "asciicircum") {
    return jkeyevent_symbol_t::CircumflexAccent;
  } else if (key == "underscore") {    
    return jkeyevent_symbol_t::Underscore;
  } else if (key == "grave") {
    return jkeyevent_symbol_t::GraveAccent;
  } else if (key == "a") {
    return jkeyevent_symbol_t::a;
  } else if (key == "b") {
    return jkeyevent_symbol_t::b;
  } else if (key == "c") {
    return jkeyevent_symbol_t::c;
  } else if (key == "d") {
    return jkeyevent_symbol_t::d;
  } else if (key == "e") {
    return jkeyevent_symbol_t::e;
  } else if (key == "f") {
    return jkeyevent_symbol_t::f;
  } else if (key == "g") {
    return jkeyevent_symbol_t::g;
  } else if (key == "h") {
    return jkeyevent_symbol_t::h;
  } else if (key == "i") {
    return jkeyevent_symbol_t::i;
  } else if (key == "j") {
    return jkeyevent_symbol_t::j;
  } else if (key == "k") {
    return jkeyevent_symbol_t::k;
  } else if (key == "l") {
    return jkeyevent_symbol_t::l;
  } else if (key == "m") {
    return jkeyevent_symbol_t::m;
  } else if (key == "n") {
    return jkeyevent_symbol_t::n;
  } else if (key == "o") {
    return jkeyevent_symbol_t::o;
  } else if (key == "p") {
    return jkeyevent_symbol_t::p;
  } else if (key == "q") {
    return jkeyevent_symbol_t::q;
  } else if (key == "r") {
    return jkeyevent_symbol_t::r;
  } else if (key == "s") {
    return jkeyevent_symbol_t::s;
  } else if (key == "t") {
    return jkeyevent_symbol_t::t;
  } else if (key == "u") {
    return jkeyevent_symbol_t::u;
  } else if (key == "v") {
    return jkeyevent_symbol_t::v;
  } else if (key == "w") {
    return jkeyevent_symbol_t::w;
  } else if (key == "x") {
    return jkeyevent_symbol_t::x;
  } else if (key == "y") {
    return jkeyevent_symbol_t::y;
  } else if (key == "z") {
    return jkeyevent_symbol_t::z;
  } else if (key == "braceleft") {
    return jkeyevent_symbol_t::CurlyBracketLeft;
  } else if (key == "bar") {  
    return jkeyevent_symbol_t::VerticalBar;
  } else if (key == "braceright") {
    return jkeyevent_symbol_t::CurlyBracketRight;
  } else if (key == "asciitilde") {  
    return jkeyevent_symbol_t::Tilde;
  } else if (key == "delete") {
    return jkeyevent_symbol_t::Delete;
  } else if (key == "left") {
    return jkeyevent_symbol_t::CursorLeft;
  } else if (key == "right") {
    return jkeyevent_symbol_t::CursorRight;
  } else if (key == "up") {  
    return jkeyevent_symbol_t::CursorUp;
  } else if (key == "down") {
    return jkeyevent_symbol_t::CursorDown;
  } else if (key == "insert") {
    return jkeyevent_symbol_t::Insert;
  } else if (key == "home" or key == "kp_home") {
    return jkeyevent_symbol_t::Home;
  } else if (key == "end" or key == "kp_end") {
    return jkeyevent_symbol_t::End;
  } else if (key == "prior" or key == "kp_prior") {
    return jkeyevent_symbol_t::PageUp;
  } else if (key == "next" or key == "kp_next") {
    return jkeyevent_symbol_t::PageDown;
  // } else if (key == "") {   
  //  return jkeyevent_symbol_t::Print;
  } else if (key == "pause") {
    return jkeyevent_symbol_t::Pause;
  //} else if (key == "") {
  //  return jkeyevent_symbol_t::Red;
  //} else if (key == "") {
  //  return jkeyevent_symbol_t::Green;
  //} else if (key == "") {
  //  return jkeyevent_symbol_t::Yellow;
  //} else if (key == "") {
  //  return jkeyevent_symbol_t::Blue;
  } else if (key == "f1") {
    return jkeyevent_symbol_t::F1;
  } else if (key == "f2") {
    return jkeyevent_symbol_t::F2;
  } else if (key == "f3") {
    return jkeyevent_symbol_t::F3;
  } else if (key == "f4") {
    return jkeyevent_symbol_t::F4;
  } else if (key == "f5") {
    return jkeyevent_symbol_t::F5;
  } else if (key == "f6") {     
    return jkeyevent_symbol_t::F6;
  } else if (key == "f7") {    
    return jkeyevent_symbol_t::F7;
  } else if (key == "f8") {   
    return jkeyevent_symbol_t::F8;
  } else if (key == "f9") {  
    return jkeyevent_symbol_t::F9;
  } else if (key == "f10") { 
    return jkeyevent_symbol_t::F10;
  } else if (key == "f11") {
    return jkeyevent_symbol_t::F11;
  } else if (key == "f12") {
    return jkeyevent_symbol_t::F12;
  } else if (key == "shift_l" or key == "shift_r") {
    return jkeyevent_symbol_t::Shift;
  } else if (key == "control_l" or key == "control_r") {
    return jkeyevent_symbol_t::Control;
  } else if (key == "alt_l" or key == "alt_r") {
    return jkeyevent_symbol_t::Alt;
  } else if (key == "altgr") {
    return jkeyevent_symbol_t::AltGr;
  // } else if (key == "") {
  //  return jkeyevent_symbol_t::Meta;
  } else if (key == "win") {
    return jkeyevent_symbol_t::Super;
  // } else if (key == "") {
  //  return jkeyevent_symbol_t::Hyper;
  }

  return jkeyevent_symbol_t::Unknown;
}

void Application::Init(int argc, char **argv)
{
  // CHANGE:: initializing ecore_evas just to get screen size
  if (!ecore_evas_init()) {
    throw std::runtime_error("Unable to init ecore evas");
  }

  ee = ecore_evas_new(NULL, 0, 0, 0, 0, NULL);

  if (!ee) {
    throw std::runtime_error("Unable to create a ecore evas object");
  }

  int w, h;
  
  ecore_evas_screen_geometry_get(ee, nullptr, nullptr, &w, &h);

  _elm_startup_time = ecore_time_unix_get();
  
  elm_init(argc, argv);

  sg_screen.x = w;
  sg_screen.y = h;

  sg_quitting = false;
}

static void InternalPaint(Evas_Object *content)
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
  
  uint32_t *src = (uint32_t *)sg_back_buffer->LockData();

  /*
  Evas_Coord ww, wh;
  uint32_t *dst, *pixel;

  evas_object_geometry_get(content, NULL, NULL, &ww, &wh);
  dst = (unsigned int*)evas_object_image_data_get(sg_surface, EINA_TRUE);
  pixel = dst;

  for (int j=0; j<wh; j++) {
    for (int i=0; i<ww; i++) {
      *pixel++ = *src++;
    }
  }
  */

  evas_object_image_data_set(sg_surface, src);
  evas_object_image_data_update_add(sg_surface, 0, 0, bounds.size.x, bounds.size.y);

  sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

static void render_layout(Evas_Object *content, bool paint)
{
  Evas_Coord wx, wy, ww, wh;

  evas_object_geometry_get(sg_window, &wx, &wy, &ww, &wh);
  evas_object_image_size_set(sg_surface, ww, wh);

  sg_bounds = jrect_t<int>{
    wx, wy, ww, wh
  };

  if (paint == true) {
    InternalPaint(content);
  }
}

static Eina_Bool InternalPaintTick(void *data)
{
  if (sg_quitting == true) {
     return ECORE_CALLBACK_CANCEL;
  }

   Evas_Object *window = (Evas_Object*)data;

   if (sg_repaint.exchange(false) == true) {
     InternalPaint(window);
   }

   Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

   return ECORE_CALLBACK_RENEW;
}

void Application::Loop()
{
  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  ecore_timer_add(0.0, InternalPaintTick, sg_window);

  elm_run();

  sg_quitting = true;
  sg_visible = false;

  sg_jcanvas_window->SetVisible(false);
}

void Application::Quit()
{
  sg_quitting = true;

  elm_exit();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
  
  ecore_evas_free(ee);
}

static void mousedown_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Pressed;

  if (ev->button == 1) {
    button = jmouseevent_button_t::Button1;
  } else if (ev->button == 2) {
    button = jmouseevent_button_t::Button2;
  } else if (ev->button == 3) {
    button = jmouseevent_button_t::Button3;
  }

  mouse_z = 1;

  if (ev->flags == EVAS_BUTTON_DOUBLE_CLICK) {
    mouse_z = 2;
  } else if (ev->flags == EVAS_BUTTON_TRIPLE_CLICK) {
    mouse_z = 3;
  }

  sg_button_state = jenum_t<jmouseevent_button_t>{sg_button_state}.Or(button);

  if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && sg_button_state != jmouseevent_button_t::None) {
    evas_object_pointer_mode_set(sg_window, EVAS_OBJECT_POINTER_MODE_AUTOGRAB); // EVAS_OBJECT_POINTER_MODE_NOGRAB
  }

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mouseup_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Released;

  if (ev->button == 1) {
    button = jmouseevent_button_t::Button1;
  } else if (ev->button == 2) {
    button = jmouseevent_button_t::Button2;
  } else if (ev->button == 3) {
    button = jmouseevent_button_t::Button3;
  }

  mouse_z = 1;

  sg_button_state = jenum_t<jmouseevent_button_t>{sg_button_state}.And(jenum_t<jmouseevent_button_t>{button}.Not());

  if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && sg_button_state != jmouseevent_button_t::None) {
    evas_object_pointer_mode_set(sg_window, EVAS_OBJECT_POINTER_MODE_NOGRAB);
  }

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mousemove_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)einfo;

  int mouse_z = 0;
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Moved;

  sg_mouse_x = ev->cur.canvas.x;
  sg_mouse_y = ev->cur.canvas.y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mousewheel_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Rotated;

  mouse_z = ev->z;

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void keydown_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)einfo;
  const Evas_Modifier *mods = evas_key_modifier_get(evas);
  
  jkeyevent_type_t type = jkeyevent_type_t::Pressed;
  jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

  if (evas_key_modifier_is_set(mods, "Control")) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
  } else if (evas_key_modifier_is_set(mods, "Shift")) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
  } else if (evas_key_modifier_is_set(mods, "Alt")) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
  }

  jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev->key);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void keyup_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)einfo;
  const Evas_Modifier *mods = evas_key_modifier_get(evas);
  
  jkeyevent_type_t type = jkeyevent_type_t::Released;
  jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

  if (evas_key_modifier_is_set(mods, "Control")) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
  } else if (evas_key_modifier_is_set(mods, "Shift")) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
  } else if (evas_key_modifier_is_set(mods, "Alt")) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
  }

  jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev->key);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void window_resize_callback(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
  render_layout(obj, true);
}

static void window_delete_callback(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
  sg_quitting = true;

  elm_exit();
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
  sg_bounds = bounds;
  sg_jcanvas_window = parent;

  Evas_Object *content = nullptr;

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

  sg_window = elm_win_util_standard_add("main", "Main");
  
  if (sg_window == nullptr) {
    throw std::runtime_error("Unable to create window");
  }

  elm_win_focus_highlight_enabled_set(sg_window, EINA_TRUE);
  
  evas_object_resize(sg_window, bounds.size.x * elm_config_scale_get(), bounds.size.y * elm_config_scale_get());

  evas_object_smart_callback_add(sg_window, "delete,request", window_delete_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_RESIZE, window_resize_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_KEY_DOWN, keydown_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_KEY_UP, keyup_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_DOWN, mousedown_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_UP, mouseup_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_MOVE, mousemove_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_WHEEL, mousewheel_callback, NULL);

  content = evas_object_rectangle_add(sg_window);

  elm_win_resize_object_add(sg_window, content);
  evas_object_show(content);

  // INFO:: init surface
  Evas_Coord ww, wh;

  evas_object_geometry_get(sg_window, NULL, NULL, &ww, &wh);
  evas_object_hide(content);

  sg_surface = evas_object_image_add(evas_object_evas_get(content));

  evas_object_image_content_hint_set(sg_surface, EVAS_IMAGE_CONTENT_HINT_DYNAMIC);

  evas_object_image_alpha_set(sg_surface, EINA_FALSE);
  evas_object_image_source_set(sg_surface, content);

  evas_object_image_filled_set(sg_surface, EINA_TRUE);
  evas_object_show(sg_surface);

  elm_win_resize_object_add(sg_window, sg_surface);
  evas_object_image_size_set(sg_surface, ww, wh);

  render_layout(sg_surface, false);

  evas_object_show(sg_window);

  render_layout(sg_window, false);

  sg_visible = true;
}

WindowAdapter::~WindowAdapter()
{
  elm_shutdown();
}

void WindowAdapter::Repaint()
{
  sg_repaint = true;
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
  return false;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
  evas_object_resize(sg_window, bounds.size.x, bounds.size.y);
  evas_object_move(sg_window, bounds.point.x, bounds.point.y);
}

jrect_t<int> WindowAdapter::GetBounds()
{
  return sg_bounds;
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
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p {
    .x = 0,
    .y = 0
  };

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
  if (visible == IsVisible()) {
    return;
  }

  sg_visible = visible;

  if (visible == true) {
    evas_object_show(sg_window);
  } else {
    evas_object_hide(sg_window);
  }
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
}

bool WindowAdapter::IsCursorEnabled()
{
	return true;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
	if (shape == nullptr) {
		return;
	}

	jpoint_t 
    t = shape->GetSize();
	uint32_t 
    data[t.x*t.y];

	shape->GetGraphics()->GetRGBArray(data, {{0, 0}, {t.x, t.y}});

  /*
	SDL_Surface 
    *surface = SDL_CreateRGBSurfaceFrom(data, t.x, t.y, 32, t.x*4, 0, 0, 0, 0);

	if (surface == nullptr) {
		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);
  */
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
	// TODO::
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(std::shared_ptr<Image> image)
{
  if (image == nullptr) {
    return;
  }

  /*
  cairo_surface_t 
    *cairo_surface = cairo_get_target(image->GetGraphics()->GetCairoContext());

  sg_jcanvas_icon = image;

  if (cairo_surface == nullptr) {
    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    return;
  }
  */

  /*
  SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(data, dw, dh, 32, dw*4, 0, 0, 0, 0);

  if (nullptr == icon) {
    return;
  }

  SDL_SetWindowIcon(sg_window, icon);
  SDL_FreeSurface(icon);
  */
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
