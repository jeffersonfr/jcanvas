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

#include <nana/gui.hpp>
#include <nana/gui/screen.hpp>
#include <nana/paint/pixel_buffer.hpp>

namespace jcanvas {

/** \brief */
static std::shared_ptr<Image> sg_back_buffer = nullptr;
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
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jrect_t<int> sg_last_bounds = {0, 0, 0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static std::shared_ptr<Image> sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
  switch (symbol) {
    case 13:
      return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
    case 8:
      return jkeyevent_symbol_t::Backspace;
    case 9:
      return jkeyevent_symbol_t::Tab;
    // case ALLEGRO_KEY_CANCEL:
    //  return jkeyevent_symbol_t::Cancel;
    case 27:
      return jkeyevent_symbol_t::Escape;
    case 32:
      return jkeyevent_symbol_t::Space;
    // case 39:
    //  return jkeyevent_symbol_t::Aposthrophe;
    case 44:   
      return jkeyevent_symbol_t::Comma;
    case 45:
      return jkeyevent_symbol_t::Minus;
    case 46:  
      return jkeyevent_symbol_t::Period;
    case 47:
      return jkeyevent_symbol_t::Slash;
    case 48:     
      return jkeyevent_symbol_t::Number0;
    case 49:
      return jkeyevent_symbol_t::Number1;
    case 50:
      return jkeyevent_symbol_t::Number2;
    case 51:
      return jkeyevent_symbol_t::Number3;
    case 52:
      return jkeyevent_symbol_t::Number4;
    case 53:
      return jkeyevent_symbol_t::Number5;
    case 54:
      return jkeyevent_symbol_t::Number6;
    case 55:
      return jkeyevent_symbol_t::Number7;
    case 56:
      return jkeyevent_symbol_t::Number8;
    case 57:
      return jkeyevent_symbol_t::Number9;
    case 59:
      return jkeyevent_symbol_t::SemiColon;
    case 61: 
      return jkeyevent_symbol_t::Equals;
    case 65:
      return jkeyevent_symbol_t::a;
    case 66:
      return jkeyevent_symbol_t::b;
    case 67:
      return jkeyevent_symbol_t::c;
    case 68:
      return jkeyevent_symbol_t::d;
    case 69:
      return jkeyevent_symbol_t::e;
    case 70:
      return jkeyevent_symbol_t::f;
    case 71:
      return jkeyevent_symbol_t::g;
    case 72:
      return jkeyevent_symbol_t::h;
    case 73:
      return jkeyevent_symbol_t::i;
    case 74:
      return jkeyevent_symbol_t::j;
    case 75:
      return jkeyevent_symbol_t::k;
    case 76:
      return jkeyevent_symbol_t::l;
    case 77:
      return jkeyevent_symbol_t::m;
    case 78:
      return jkeyevent_symbol_t::n;
    case 79:
      return jkeyevent_symbol_t::o;
    case 80:
      return jkeyevent_symbol_t::p;
    case 81:
      return jkeyevent_symbol_t::q;
    case 82:
      return jkeyevent_symbol_t::r;
    case 83:
      return jkeyevent_symbol_t::s;
    case 84:
      return jkeyevent_symbol_t::t;
    case 85:
      return jkeyevent_symbol_t::u;
    case 86:
      return jkeyevent_symbol_t::v;
    case 87:
      return jkeyevent_symbol_t::w;
    case 88:
      return jkeyevent_symbol_t::x;
    case 89:
      return jkeyevent_symbol_t::y;
    case 90:
      return jkeyevent_symbol_t::z;
    case 91:
      return jkeyevent_symbol_t::SquareBracketLeft;
    case 92:   
      return jkeyevent_symbol_t::BackSlash;
    case 93:
      return jkeyevent_symbol_t::SquareBracketRight;
    case 96:
      return jkeyevent_symbol_t::GraveAccent;
    case 127:
      return jkeyevent_symbol_t::Delete;
    case 37:
      return jkeyevent_symbol_t::CursorLeft;
    case 39:
      return jkeyevent_symbol_t::CursorRight;
    case 38:  
      return jkeyevent_symbol_t::CursorUp;
    case 40:
      return jkeyevent_symbol_t::CursorDown;
    // case 16777222:  
    //  return jkeyevent_symbol_t::Insert;
    // case 16777232:     
    //  return jkeyevent_symbol_t::Home;
    // case 16777233:
    //  return jkeyevent_symbol_t::End;
    // case 16777238:
    //  return jkeyevent_symbol_t::PageUp;
    // case 16777239:
    //  return jkeyevent_symbol_t::PageDown;
    // case ALLEGRO_KEY_PRINTSCREEN:   
    //  return jkeyevent_symbol_t::Print;
    // case 16777224:
    //  return jkeyevent_symbol_t::Pause;
    // case ALLEGRO_KEY_RED:
    //  return jkeyevent_symbol_t::Red;
    // case ALLEGRO_KEY_GREEN:
    //  return jkeyevent_symbol_t::Green;
    // case ALLEGRO_KEY_YELLOW:
    //  return jkeyevent_symbol_t::Yellow;
    // case ALLEGRO_KEY_BLUE:
    //  return jkeyevent_symbol_t::Blue;
    case 65470:
      return jkeyevent_symbol_t::F1;
    case 65471:
      return jkeyevent_symbol_t::F2;
    case 65472:
      return jkeyevent_symbol_t::F3;
    case 65473:
      return jkeyevent_symbol_t::F4;
    case 65474:
      return jkeyevent_symbol_t::F5;
    case 65475:
      return jkeyevent_symbol_t::F6;
    case 65476:
      return jkeyevent_symbol_t::F7;
    case 65477:
      return jkeyevent_symbol_t::F8;
    case 65478:
      return jkeyevent_symbol_t::F9;
    case 65479:
      return jkeyevent_symbol_t::F10;
    case 65480:
      return jkeyevent_symbol_t::F11;
    case 65481:
      return jkeyevent_symbol_t::F12;
    case 16:
      return jkeyevent_symbol_t::Shift;
    case 17:
      return jkeyevent_symbol_t::Control;
    // case 16777251:
    //  return jkeyevent_symbol_t::Alr;
    // case ALLEGRO_KEY_ALTGR:
    //  return jkeyevent_symbol_t::AltGr;
    // case ALLEGRO_KEY_LMETA:
    // case ALLEGRO_KEY_RMETA:
    //  return jkeyevent_symbol_t::Meta;
    // case ALLEGRO_KEY_LSUPER:
    // case ALLEGRO_KEY_RSUPER:
    //  return jkeyevent_symbol_t::Super;
    // case ALLEGRO_KEY_HYPER:
    //  return jkeyevent_symbol_t::Hyper;
    default: 
      break;
  }

  return jkeyevent_symbol_t::Unknown;
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

    Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());
  }
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Opened));

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

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
}

static void expose_callback(const nana::arg_expose &arg)
{
  dw->update();
}

static void resized_callback(const nana::arg_resized &arg)
{
  dw->update();
          
  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Resized));
}

static void moved_callback(const nana::arg_move &arg)
{
  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Moved));
}

static void focus_callback(const nana::arg_mouse &arg)
{
  if (arg.evt_code == nana::event_code::mouse_enter) {
    sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
  } else if (arg.evt_code == nana::event_code::mouse_leave) {
    sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
  }
}

static void mouse_input_callback(const nana::arg_mouse &arg)
{
  static jmouseevent_button_t buttons = jmouseevent_button_t::None;

  int mouse_z = 0;
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Unknown;

  sg_mouse_x = arg.pos.x;
  sg_mouse_y = arg.pos.y;

  if (arg.evt_code == nana::event_code::mouse_move) {
    type = jmouseevent_type_t::Moved;
  } else {
    if (arg.button == nana::mouse::left_button) {
      button = jmouseevent_button_t::Button1;
    } else if (arg.button == nana::mouse::middle_button) {
      button = jmouseevent_button_t::Button2;
    } else if (arg.button == nana::mouse::right_button) {
      button = jmouseevent_button_t::Button3;
    }
  
    if (arg.evt_code == nana::event_code::mouse_down or arg.evt_code == nana::event_code::dbl_click) {
      type = jmouseevent_type_t::Pressed;
      buttons = jenum_t<jmouseevent_button_t>{buttons}.Or(button);
    } else if (arg.evt_code == nana::event_code::mouse_up) {
      type = jmouseevent_type_t::Released;
      buttons = jenum_t<jmouseevent_button_t>{buttons}.And(jenum_t<jmouseevent_button_t>{button}.Not());
    }

    mouse_z = 1;
  
    if (arg.evt_code == nana::event_code::dbl_click) {
      mouse_z = 2;
    }
  }

  if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jmouseevent_button_t::None) {
    fm->set_capture(true);
  } else {
    fm->release_capture();
  }

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mouse_wheel_input_callback(const nana::arg_wheel &arg)
{
  jmouseevent_button_t button = jmouseevent_button_t::None;
  jmouseevent_type_t type = jmouseevent_type_t::Rotated;

  int mouse_z = 1;

  if (arg.upwards == false) {
    mouse_z = -1;
  }

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void key_input_callback(const nana::arg_keyboard &arg)
{
  jkeyevent_type_t type = jkeyevent_type_t::Unknown;
  jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

  if (arg.shift == true) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
  } else if (arg.ctrl == true) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
  } else if (arg.alt == true) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
  }

  if (arg.evt_code == nana::event_code::key_press) {
    type = jkeyevent_type_t::Pressed;
  } else if (arg.evt_code == nana::event_code::key_release) {
    type = jkeyevent_type_t::Released;
  }

  jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(arg.key);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void paint_callback(const nana::paint::graphics& graph)
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

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
  if (fm != nullptr) {
    throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = std::make_shared<BufferedImage>(_DATA_PREFIX"/images/small-gnu.png");

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

WindowAdapter::~WindowAdapter()
{
  fm->close();

  delete fm;
  delete dw;
  
  sg_back_buffer = nullptr;
  sg_jcanvas_window = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_paint_condition.notify_all();
}

void WindowAdapter::ToggleFullScreen()
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

void WindowAdapter::SetTitle(std::string title)
{
  fm->caption(title);
}

std::string WindowAdapter::GetTitle()
{
  return fm->caption();
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
  fm->size({(nana::size::value_type)bounds.size.x, (nana::size::value_type)bounds.size.y});
  fm->move({bounds.point.x, bounds.point.y});
}

jrect_t<int> WindowAdapter::GetBounds()
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
  jpoint_t<int> p {
    .x = sg_mouse_x, 
    .y = sg_mouse_y
  };

  return p;
}

void WindowAdapter::SetVisible(bool visible)
{
  if (visible == false) {
    fm->hide();
  } else {
    fm->show();
  }
}

bool WindowAdapter::IsVisible()
{
  return fm->visible();
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
  nana::cursor type = nana::cursor::arrow;

  if (style == jcursor_style_t::Default) {
  } else if (style == jcursor_style_t::Crosshair) {
  } else if (style == jcursor_style_t::East) {
  } else if (style == jcursor_style_t::West) {
  } else if (style == jcursor_style_t::North) {
  } else if (style == jcursor_style_t::South) {
  } else if (style == jcursor_style_t::Hand) {
    type = nana::cursor::hand;
  } else if (style == jcursor_style_t::Move) {
  } else if (style == jcursor_style_t::Vertical) {
    type = nana::cursor::size_ns;
  } else if (style == jcursor_style_t::Horizontal) {
    type = nana::cursor::size_we;
  } else if (style == jcursor_style_t::NorthWest) {
    type = nana::cursor::size_top_left;
  } else if (style == jcursor_style_t::NorthEast) {
    type = nana::cursor::size_top_right;
  } else if (style == jcursor_style_t::SouthWest) {
    type = nana::cursor::size_bottom_left;
  } else if (style == jcursor_style_t::SouthEast) {
    type = nana::cursor::size_bottom_right;
  } else if (style == jcursor_style_t::Text) {
    type = nana::cursor::iterm;
  } else if (style == jcursor_style_t::Wait) {
    type = nana::cursor::wait;
  }

  fm->cursor(type);
  
  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
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
}

std::shared_ptr<Image> WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
