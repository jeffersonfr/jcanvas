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

#include <nana/gui.hpp>
#include <nana/gui/screen.hpp>
#include <nana/paint/pixel_buffer.hpp>

namespace jcanvas {

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static nana::form *fm = nullptr;
/** \brief */
static nana::drawing *dw = nullptr;
/** \brief */
static std::mutex sg_paint_mutex;
/** \brief */
static std::condition_variable sg_paint_condition;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jcanvas::jrect_t<int> sg_last_bounds = {0, 0, 0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
  switch (symbol) {
    case 13:
      return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
    case 8:
      return jcanvas::JKS_BACKSPACE;
    case 9:
      return jcanvas::JKS_TAB;
    // case ALLEGRO_KEY_CANCEL:
    //  return jcanvas::JKS_CANCEL;
    case 27:
      return jcanvas::JKS_ESCAPE;
    case 32:
      return jcanvas::JKS_SPACE;
    // case 39:
    //  return jcanvas::JKS_APOSTHROFE;
    case 44:   
      return jcanvas::JKS_COMMA;
    case 45:
      return jcanvas::JKS_MINUS_SIGN;
    case 46:  
      return jcanvas::JKS_PERIOD;
    case 47:
      return jcanvas::JKS_SLASH;
    case 48:     
      return jcanvas::JKS_0;
    case 49:
      return jcanvas::JKS_1;
    case 50:
      return jcanvas::JKS_2;
    case 51:
      return jcanvas::JKS_3;
    case 52:
      return jcanvas::JKS_4;
    case 53:
      return jcanvas::JKS_5;
    case 54:
      return jcanvas::JKS_6;
    case 55:
      return jcanvas::JKS_7;
    case 56:
      return jcanvas::JKS_8;
    case 57:
      return jcanvas::JKS_9;
    case 59:
      return jcanvas::JKS_SEMICOLON;
    case 61: 
      return jcanvas::JKS_EQUALS_SIGN;
    case 65:
      return jcanvas::JKS_a;
    case 66:
      return jcanvas::JKS_b;
    case 67:
      return jcanvas::JKS_c;
    case 68:
      return jcanvas::JKS_d;
    case 69:
      return jcanvas::JKS_e;
    case 70:
      return jcanvas::JKS_f;
    case 71:
      return jcanvas::JKS_g;
    case 72:
      return jcanvas::JKS_h;
    case 73:
      return jcanvas::JKS_i;
    case 74:
      return jcanvas::JKS_j;
    case 75:
      return jcanvas::JKS_k;
    case 76:
      return jcanvas::JKS_l;
    case 77:
      return jcanvas::JKS_m;
    case 78:
      return jcanvas::JKS_n;
    case 79:
      return jcanvas::JKS_o;
    case 80:
      return jcanvas::JKS_p;
    case 81:
      return jcanvas::JKS_q;
    case 82:
      return jcanvas::JKS_r;
    case 83:
      return jcanvas::JKS_s;
    case 84:
      return jcanvas::JKS_t;
    case 85:
      return jcanvas::JKS_u;
    case 86:
      return jcanvas::JKS_v;
    case 87:
      return jcanvas::JKS_w;
    case 88:
      return jcanvas::JKS_x;
    case 89:
      return jcanvas::JKS_y;
    case 90:
      return jcanvas::JKS_z;
    case 91:
      return jcanvas::JKS_SQUARE_BRACKET_LEFT;
    case 92:   
      return jcanvas::JKS_BACKSLASH;
    case 93:
      return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
    case 96:
      return jcanvas::JKS_GRAVE_ACCENT;
    case 127:
      return jcanvas::JKS_DELETE;
    case 37:
      return jcanvas::JKS_CURSOR_LEFT;
    case 39:
      return jcanvas::JKS_CURSOR_RIGHT;
    case 38:  
      return jcanvas::JKS_CURSOR_UP;
    case 40:
      return jcanvas::JKS_CURSOR_DOWN;
    // case 16777222:  
    //  return jcanvas::JKS_INSERT;
    // case 16777232:     
    //  return jcanvas::JKS_HOME;
    // case 16777233:
    //  return jcanvas::JKS_END;
    // case 16777238:
    //  return jcanvas::JKS_PAGE_UP;
    // case 16777239:
    //  return jcanvas::JKS_PAGE_DOWN;
    // case ALLEGRO_KEY_PRINTSCREEN:   
    //  return jcanvas::JKS_PRINT;
    // case 16777224:
    //  return jcanvas::JKS_PAUSE;
    // case ALLEGRO_KEY_RED:
    //  return jcanvas::JKS_RED;
    // case ALLEGRO_KEY_GREEN:
    //  return jcanvas::JKS_GREEN;
    // case ALLEGRO_KEY_YELLOW:
    //  return jcanvas::JKS_YELLOW;
    // case ALLEGRO_KEY_BLUE:
    //  return jcanvas::JKS_BLUE;
    case 65470:
      return jcanvas::JKS_F1;
    case 65471:
      return jcanvas::JKS_F2;
    case 65472:
      return jcanvas::JKS_F3;
    case 65473:
      return jcanvas::JKS_F4;
    case 65474:
      return jcanvas::JKS_F5;
    case 65475:
      return jcanvas::JKS_F6;
    case 65476:
      return jcanvas::JKS_F7;
    case 65477:
      return jcanvas::JKS_F8;
    case 65478:
      return jcanvas::JKS_F9;
    case 65479:
      return jcanvas::JKS_F10;
    case 65480:
      return jcanvas::JKS_F11;
    case 65481:
      return jcanvas::JKS_F12;
    case 16:
      return jcanvas::JKS_SHIFT;
    case 17:
      return jcanvas::JKS_CONTROL;
    // case 16777251:
    //  return jcanvas::JKS_ALT;
    // case ALLEGRO_KEY_ALTGR:
    //  return jcanvas::JKS_ALTGR;
    // case ALLEGRO_KEY_LMETA:
    // case ALLEGRO_KEY_RMETA:
    //  return jcanvas::JKS_META;
    // case ALLEGRO_KEY_LSUPER:
    // case ALLEGRO_KEY_RSUPER:
    //  return jcanvas::JKS_SUPER;
    // case ALLEGRO_KEY_HYPER:
    //  return jcanvas::JKS_HYPER;
    default: 
      break;
  }

  return jcanvas::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
  nana::screen s = nana::screen();
  const ::nana::rectangle r = s.get_primary().workarea();

  sg_screen.x = r.x;
  sg_screen.y = r.y;
  
  sg_quitting = false;
}

static void InternalPaint()
{
  while (sg_quitting == false) {
    std::unique_lock<std::mutex> lock(sg_paint_mutex);
    
    dw->update();

    sg_paint_condition.wait_for(lock, std::chrono::milliseconds(1));
  }
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_OPENED));

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  std::thread thread = std::thread(InternalPaint);

  ::nana::exec();

  sg_quitting = true;

  thread.join();
  
  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;
  
  fm->close();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

static void destroy_callback(const nana::arg_destroy &arg)
{
  fm->close();

  sg_quitting = true;

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
}

static void expose_callback(const nana::arg_expose &arg)
{
  dw->update();
}

static void resized_callback(const nana::arg_resized &arg)
{
  dw->update();
          
  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_RESIZED));
}

static void moved_callback(const nana::arg_move &arg)
{
  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_MOVED));
}

static void focus_callback(const nana::arg_mouse &arg)
{
  if (arg.evt_code == nana::event_code::mouse_enter) {
    sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
  } else if (arg.evt_code == nana::event_code::mouse_leave) {
    sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
  }
}

static void mouse_input_callback(const nana::arg_mouse &arg)
{
  static jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;

  int mouse_z = 0;
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;

  sg_mouse_x = arg.pos.x;
  sg_mouse_y = arg.pos.y;

  if (arg.evt_code == nana::event_code::mouse_move) {
    type = jcanvas::JMT_MOVED;
  } else {
    if (arg.button == nana::mouse::left_button) {
      button = jcanvas::JMB_BUTTON1;
    } else if (arg.button == nana::mouse::middle_button) {
      button = jcanvas::JMB_BUTTON2;
    } else if (arg.button == nana::mouse::right_button) {
      button = jcanvas::JMB_BUTTON3;
    }
  
    if (arg.evt_code == nana::event_code::mouse_down or arg.evt_code == nana::event_code::dbl_click) {
      type = jcanvas::JMT_PRESSED;
      buttons = (jcanvas::jmouseevent_button_t)(buttons | button);
    } else if (arg.evt_code == nana::event_code::mouse_up) {
      type = jcanvas::JMT_RELEASED;
      buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
    }

    mouse_z = 1;
  
    if (arg.evt_code == nana::event_code::dbl_click) {
      mouse_z = 2;
    }
  }

  if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
    fm->set_capture(true);
  } else {
    fm->release_capture();
  }

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mouse_wheel_input_callback(const nana::arg_wheel &arg)
{
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_ROTATED;

  int mouse_z = 1;

  if (arg.upwards == false) {
    mouse_z = -1;
  }

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void key_input_callback(const nana::arg_keyboard &arg)
{
  jcanvas::jkeyevent_type_t type;
  jcanvas::jkeyevent_modifiers_t mod;

  mod = (jcanvas::jkeyevent_modifiers_t)(0);

  if (arg.shift == true) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
  } else if (arg.ctrl == true) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
  } else if (arg.alt == true) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
  }

  type = jcanvas::JKT_UNKNOWN;
  
  if (arg.evt_code == nana::event_code::key_press) {
    type = jcanvas::JKT_PRESSED;
  } else if (arg.evt_code == nana::event_code::key_release) {
    type = jcanvas::JKT_RELEASED;
  }

  jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(arg.key);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void paint_callback(const nana::paint::graphics& graph)
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

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  g->Reset();
  g->SetCompositeFlags(jcanvas::JCF_SRC);

  sg_jcanvas_window->Paint(g);

  g->Flush();

  uint8_t *data = sg_back_buffer->LockData();

  nana::paint::pixel_buffer pixbuf{ graph.handle(), nana::rectangle{ graph.size() } };
  // size sz = pixbuf.size();

  for (int i=0; i<bounds.size.y; i++) {
    const uint8_t *src = data + i*bounds.size.x*4;

    pixbuf.fill_row(i, src, bounds.size.x*4, 32);
  }

  // pixbuf.put((unsigned char *)data, bounds.size.x, bounds.size.y, 32, bounds.size.x*4, false);
  pixbuf.paste(graph.handle(), {});

  sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
  
  std::this_thread::yield();
}

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
{
  if (fm != nullptr) {
    throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_mouse_x = 0;
  sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  fm = new nana::form{nana::API::make_center(bounds.size.x, bounds.size.y)};

  fm->move(bounds.point.x, bounds.point.y);

  dw = new nana::drawing(*fm);

  dw->draw(paint_callback);
  
  // TODO:: cursor, resize, expose
  fm->events().destroy(destroy_callback);
  fm->events().expose(expose_callback);
  fm->events().resized(resized_callback);
  fm->events().move(moved_callback);
  fm->events().mouse_enter(focus_callback);
  fm->events().mouse_leave(focus_callback);

  fm->events().dbl_click(mouse_input_callback);
  fm->events().mouse_move(mouse_input_callback);
  fm->events().mouse_down(mouse_input_callback);
  fm->events().mouse_up(mouse_input_callback);
  fm->events().mouse_wheel(mouse_wheel_input_callback);
  
  fm->events().key_press(key_input_callback);
  fm->events().key_release(key_input_callback);

  fm->show();
}

NativeWindow::~NativeWindow()
{
  fm->close();

  delete fm;
  delete dw;
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint()
{
  sg_paint_condition.notify_all();
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_last_bounds = GetBounds();

    fm->size({(nana::size::value_type)sg_screen.x, (nana::size::value_type)sg_screen.y});
    fm->move({0, 0});

    sg_fullscreen = true;
  } else {
    fm->size({(nana::size::value_type)sg_last_bounds.size.x, (nana::size::value_type)sg_last_bounds.size.y});
    fm->move({sg_last_bounds.point.x, sg_last_bounds.point.y});

    sg_fullscreen = false;
  }

}

void NativeWindow::SetTitle(std::string title)
{
  fm->caption(title);
}

std::string NativeWindow::GetTitle()
{
  return fm->caption();
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
  return false;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  fm->size({(nana::size::value_type)width, (nana::size::value_type)height});
  fm->move({x, y});
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
  nana::size s = fm->size();
  nana::point p = fm->pos();

  return {
    p.x,
    p.y,
    (int)s.width,
    (int)s.height
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
  jpoint_t<int> p {
    .x = sg_mouse_x, 
    .y = sg_mouse_y
  };

  return p;
}

void NativeWindow::SetVisible(bool visible)
{
  if (visible == false) {
    fm->hide();
  } else {
    fm->show();
  }
}

bool NativeWindow::IsVisible()
{
  return fm->visible();
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jcanvas_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
}

bool NativeWindow::IsCursorEnabled()
{
  return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  nana::cursor type = nana::cursor::arrow;

  if (style == JCS_DEFAULT) {
  } else if (style == JCS_CROSSHAIR) {
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = nana::cursor::hand;
  } else if (style == JCS_MOVE) {
  } else if (style == JCS_NS) {
    type = nana::cursor::size_ns;
  } else if (style == JCS_WE) {
    type = nana::cursor::size_we;
  } else if (style == JCS_NW_CORNER) {
    type = nana::cursor::size_top_left;
  } else if (style == JCS_NE_CORNER) {
    type = nana::cursor::size_top_right;
  } else if (style == JCS_SW_CORNER) {
    type = nana::cursor::size_bottom_left;
  } else if (style == JCS_SE_CORNER) {
    type = nana::cursor::size_bottom_right;
  } else if (style == JCS_TEXT) {
    type = nana::cursor::iterm;
  } else if (style == JCS_WAIT) {
    type = nana::cursor::wait;
  }

  fm->cursor(type);
  
  sg_jcanvas_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
  // TODO::
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
  return sg_jcanvas_icon;
}

}
