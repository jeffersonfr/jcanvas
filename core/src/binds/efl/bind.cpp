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

#include "jmixin/jstring.h"

#include <thread>
#include <mutex>
#include <stdexcept>

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
static jcanvas::jrect_t<int> sg_bounds = {0, 0, 0, 0};
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
static jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static jcanvas::jmouseevent_button_t sg_button_state = jcanvas::JMB_NONE;
/** \brief */
static bool sg_repaint = false;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(std::string key)
{
  key = jmixin::String(key).lower_case();

  if (key == "return" or key == "kp_enter") {
    return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
  } else if (key == "backspace") {
    return jcanvas::JKS_BACKSPACE;
  } else if (key == "tab") {
    return jcanvas::JKS_TAB;
    // } else if (key == "") {
    //	return jcanvas::JKS_CANCEL;
  } else if (key == "escape") {
    return jcanvas::JKS_ESCAPE;
  } else if (key == "space") {
    return jcanvas::JKS_SPACE;
  } else if (key == "exclam") {
    return jcanvas::JKS_EXCLAMATION_MARK;
  } else if (key == "apostrophe") {
    return jcanvas::JKS_QUOTATION;
  } else if (key == "numbersign") {
    return jcanvas::JKS_NUMBER_SIGN;
  } else if (key == "dollar") {
    return jcanvas::JKS_DOLLAR_SIGN;
  } else if (key == "percent") {
    return jcanvas::JKS_PERCENT_SIGN;
  } else if (key == "ampersand") {   
    return jcanvas::JKS_AMPERSAND;
  } else if (key == "apostrophe") {
    return jcanvas::JKS_APOSTROPHE;
  } else if (key == "parenleft") {
    return jcanvas::JKS_PARENTHESIS_LEFT;
  } else if (key == "parenright") {
    return jcanvas::JKS_PARENTHESIS_RIGHT;
  } else if (key == "asterisk") {
    return jcanvas::JKS_STAR;
  } else if (key == "plus") {
    return jcanvas::JKS_PLUS_SIGN;
  } else if (key == "comma") {   
    return jcanvas::JKS_COMMA;
  } else if (key == "minus") {
    return jcanvas::JKS_MINUS_SIGN;
  } else if (key == "period") {  
    return jcanvas::JKS_PERIOD;
  } else if (key == "slash") {
    return jcanvas::JKS_SLASH;
  } else if (key == "0") {     
    return jcanvas::JKS_0;
  } else if (key == "1") {
    return jcanvas::JKS_1;
  } else if (key == "2") {
    return jcanvas::JKS_2;
  } else if (key == "3") {
    return jcanvas::JKS_3;
  } else if (key == "4") {
    return jcanvas::JKS_4;
  } else if (key == "5") {
    return jcanvas::JKS_5;
  } else if (key == "6") {
    return jcanvas::JKS_6;
  } else if (key == "7") {
    return jcanvas::JKS_7;
  } else if (key == "8") {
    return jcanvas::JKS_8;
  } else if (key == "9") {
    return jcanvas::JKS_9;
  } else if (key == "colon") {
    return jcanvas::JKS_COLON;
  } else if (key == "semicolon") {
    return jcanvas::JKS_SEMICOLON;
  } else if (key == "less") {
    return jcanvas::JKS_LESS_THAN_SIGN;
  } else if (key == "equal") { 
    return jcanvas::JKS_EQUALS_SIGN;
  } else if (key == "greater") {
    return jcanvas::JKS_GREATER_THAN_SIGN;
  } else if (key == "question") {   
    return jcanvas::JKS_QUESTION_MARK;
  } else if (key == "at") {
    return jcanvas::JKS_AT;
  } else if (key == "A") {
    return jcanvas::JKS_A;
  } else if (key == "B") {
    return jcanvas::JKS_B;
  } else if (key == "C") {
    return jcanvas::JKS_C;
  } else if (key == "D") {
    return jcanvas::JKS_D;
  } else if (key == "E") {
    return jcanvas::JKS_E;
  } else if (key == "F") {
    return jcanvas::JKS_F;
  } else if (key == "G") {
    return jcanvas::JKS_G;
  } else if (key == "H") {
    return jcanvas::JKS_H;
  } else if (key == "I") {
    return jcanvas::JKS_I;
  } else if (key == "J") {
    return jcanvas::JKS_J;
  } else if (key == "K") {
    return jcanvas::JKS_K;
  } else if (key == "L") {
    return jcanvas::JKS_L;
  } else if (key == "M") {
    return jcanvas::JKS_M;
  } else if (key == "N") {
    return jcanvas::JKS_N;
  } else if (key == "O") {
    return jcanvas::JKS_O;
  } else if (key == "P") {
    return jcanvas::JKS_P;
  } else if (key == "Q") {
    return jcanvas::JKS_Q;
  } else if (key == "R") {
    return jcanvas::JKS_R;
  } else if (key == "S") {
    return jcanvas::JKS_S;
  } else if (key == "T") {
    return jcanvas::JKS_T;
  } else if (key == "U") {
    return jcanvas::JKS_U;
  } else if (key == "V") {
    return jcanvas::JKS_V;
  } else if (key == "W") {
    return jcanvas::JKS_W;
  } else if (key == "X") {
    return jcanvas::JKS_X;
  } else if (key == "Y") {
    return jcanvas::JKS_Y;
  } else if (key == "Z") {
    return jcanvas::JKS_Z;
  } else if (key == "bracketleft") {
    return jcanvas::JKS_SQUARE_BRACKET_LEFT;
  } else if (key == "backslash") {
    return jcanvas::JKS_BACKSLASH;
  } else if (key == "bracketright") {
    return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
  } else if (key == "asciicircum") {
    return jcanvas::JKS_CIRCUMFLEX_ACCENT;
  } else if (key == "underscore") {    
    return jcanvas::JKS_UNDERSCORE;
  } else if (key == "grave") {
    return jcanvas::JKS_GRAVE_ACCENT;
  } else if (key == "a") {
    return jcanvas::JKS_a;
  } else if (key == "b") {
    return jcanvas::JKS_b;
  } else if (key == "c") {
    return jcanvas::JKS_c;
  } else if (key == "d") {
    return jcanvas::JKS_d;
  } else if (key == "e") {
    return jcanvas::JKS_e;
  } else if (key == "f") {
    return jcanvas::JKS_f;
  } else if (key == "g") {
    return jcanvas::JKS_g;
  } else if (key == "h") {
    return jcanvas::JKS_h;
  } else if (key == "i") {
    return jcanvas::JKS_i;
  } else if (key == "j") {
    return jcanvas::JKS_j;
  } else if (key == "k") {
    return jcanvas::JKS_k;
  } else if (key == "l") {
    return jcanvas::JKS_l;
  } else if (key == "m") {
    return jcanvas::JKS_m;
  } else if (key == "n") {
    return jcanvas::JKS_n;
  } else if (key == "o") {
    return jcanvas::JKS_o;
  } else if (key == "p") {
    return jcanvas::JKS_p;
  } else if (key == "q") {
    return jcanvas::JKS_q;
  } else if (key == "r") {
    return jcanvas::JKS_r;
  } else if (key == "s") {
    return jcanvas::JKS_s;
  } else if (key == "t") {
    return jcanvas::JKS_t;
  } else if (key == "u") {
    return jcanvas::JKS_u;
  } else if (key == "v") {
    return jcanvas::JKS_v;
  } else if (key == "w") {
    return jcanvas::JKS_w;
  } else if (key == "x") {
    return jcanvas::JKS_x;
  } else if (key == "y") {
    return jcanvas::JKS_y;
  } else if (key == "z") {
    return jcanvas::JKS_z;
  } else if (key == "braceleft") {
    return jcanvas::JKS_CURLY_BRACKET_LEFT;
  } else if (key == "bar") {  
    return jcanvas::JKS_VERTICAL_BAR;
  } else if (key == "braceright") {
    return jcanvas::JKS_CURLY_BRACKET_RIGHT;
  } else if (key == "asciitilde") {  
    return jcanvas::JKS_TILDE;
  } else if (key == "delete") {
    return jcanvas::JKS_DELETE;
  } else if (key == "left") {
    return jcanvas::JKS_CURSOR_LEFT;
  } else if (key == "right") {
    return jcanvas::JKS_CURSOR_RIGHT;
  } else if (key == "up") {  
    return jcanvas::JKS_CURSOR_UP;
  } else if (key == "down") {
    return jcanvas::JKS_CURSOR_DOWN;
  } else if (key == "insert") {
    return jcanvas::JKS_INSERT;
  } else if (key == "home" or key == "kp_home") {
    return jcanvas::JKS_HOME;
  } else if (key == "end" or key == "kp_end") {
    return jcanvas::JKS_END;
  } else if (key == "prior" or key == "kp_prior") {
    return jcanvas::JKS_PAGE_UP;
  } else if (key == "next" or key == "kp_next") {
    return jcanvas::JKS_PAGE_DOWN;
  // } else if (key == "") {   
  //  return jcanvas::JKS_PRINT;
  } else if (key == "pause") {
    return jcanvas::JKS_PAUSE;
  //} else if (key == "") {
  //  return jcanvas::JKS_RED;
  //} else if (key == "") {
  //  return jcanvas::JKS_GREEN;
  //} else if (key == "") {
  //  return jcanvas::JKS_YELLOW;
  //} else if (key == "") {
  //  return jcanvas::JKS_BLUE;
  } else if (key == "f1") {
    return jcanvas::JKS_F1;
  } else if (key == "f2") {
    return jcanvas::JKS_F2;
  } else if (key == "f3") {
    return jcanvas::JKS_F3;
  } else if (key == "f4") {
    return jcanvas::JKS_F4;
  } else if (key == "f5") {
    return jcanvas::JKS_F5;
  } else if (key == "f6") {     
    return jcanvas::JKS_F6;
  } else if (key == "f7") {    
    return jcanvas::JKS_F7;
  } else if (key == "f8") {   
    return jcanvas::JKS_F8;
  } else if (key == "f9") {  
    return jcanvas::JKS_F9;
  } else if (key == "f10") { 
    return jcanvas::JKS_F10;
  } else if (key == "f11") {
    return jcanvas::JKS_F11;
  } else if (key == "f12") {
    return jcanvas::JKS_F12;
  } else if (key == "shift_l" or key == "shift_r") {
    return jcanvas::JKS_SHIFT;
  } else if (key == "control_l" or key == "control_r") {
    return jcanvas::JKS_CONTROL;
  } else if (key == "alt_l" or key == "alt_r") {
    return jcanvas::JKS_ALT;
  } else if (key == "altgr") {
    return jcanvas::JKS_ALTGR;
  // } else if (key == "") {
  //  return jcanvas::JKS_META;
  } else if (key == "win") {
    return jcanvas::JKS_SUPER;
  // } else if (key == "") {
  //  return jcanvas::JKS_HYPER;
  }

  return jcanvas::JKS_UNKNOWN;
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

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

static void render_layout(Evas_Object *content, bool paint)
{
  Evas_Coord wx, wy, ww, wh;

  evas_object_geometry_get(sg_window, &wx, &wy, &ww, &wh);
  evas_object_image_size_set(sg_surface, ww, wh);

  sg_bounds = jcanvas::jrect_t<int>{
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

   if (sg_repaint == true) {
     sg_repaint = false;

     InternalPaint(window);
   }

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
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_PRESSED;

  if (ev->button == 1) {
    button = jcanvas::JMB_BUTTON1;
  } else if (ev->button == 2) {
    button = jcanvas::JMB_BUTTON2;
  } else if (ev->button == 3) {
    button = jcanvas::JMB_BUTTON3;
  }

  mouse_z = 1;

  if (ev->flags == EVAS_BUTTON_DOUBLE_CLICK) {
    mouse_z = 2;
  } else if (ev->flags == EVAS_BUTTON_TRIPLE_CLICK) {
    mouse_z = 3;
  }

  sg_button_state = (jcanvas::jmouseevent_button_t)(sg_button_state | button);

  if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && sg_button_state != jcanvas::JMB_NONE) {
    evas_object_pointer_mode_set(sg_window, EVAS_OBJECT_POINTER_MODE_AUTOGRAB); // EVAS_OBJECT_POINTER_MODE_NOGRAB
  }

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mouseup_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_RELEASED;

  if (ev->button == 1) {
    button = jcanvas::JMB_BUTTON1;
  } else if (ev->button == 2) {
    button = jcanvas::JMB_BUTTON2;
  } else if (ev->button == 3) {
    button = jcanvas::JMB_BUTTON3;
  }

  mouse_z = 1;

  sg_button_state = (jcanvas::jmouseevent_button_t)(sg_button_state & ~button);

  if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && sg_button_state != jcanvas::JMB_NONE) {
    evas_object_pointer_mode_set(sg_window, EVAS_OBJECT_POINTER_MODE_NOGRAB);
  }

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mousemove_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)einfo;

  int mouse_z = 0;
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_MOVED;

  sg_mouse_x = ev->cur.canvas.x;
  sg_mouse_y = ev->cur.canvas.y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mousewheel_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_ROTATED;

  mouse_z = ev->z;

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void keydown_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)einfo;
  const Evas_Modifier *mods = evas_key_modifier_get(evas);
  
  jcanvas::jkeyevent_type_t type = jcanvas::JKT_PRESSED;
  jcanvas::jkeyevent_modifiers_t mod;

  mod = (jcanvas::jkeyevent_modifiers_t)(0);

  if (evas_key_modifier_is_set(mods, "Control")) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
  } else if (evas_key_modifier_is_set(mods, "Shift")) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
  } else if (evas_key_modifier_is_set(mods, "Alt")) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
  }

  jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev->key);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void keyup_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)einfo;
  const Evas_Modifier *mods = evas_key_modifier_get(evas);
  
  jcanvas::jkeyevent_type_t type = jcanvas::JKT_RELEASED;
  jcanvas::jkeyevent_modifiers_t mod;

  mod = (jcanvas::jkeyevent_modifiers_t)(0);

  if (evas_key_modifier_is_set(mods, "Control")) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
  } else if (evas_key_modifier_is_set(mods, "Shift")) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
  } else if (evas_key_modifier_is_set(mods, "Alt")) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
  }

  jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev->key);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
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

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
{
	if (sg_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

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

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
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

jcanvas::jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t p {
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

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
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
	return jcanvas::JWR_NONE;
}

void WindowAdapter::SetIcon(jcanvas::Image *image)
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

jcanvas::Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
