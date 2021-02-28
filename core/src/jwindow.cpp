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
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindowadapter.h"

#include <algorithm>

#define SUBTITLE_SIZE    32

namespace jcanvas {

Window::Window(jpoint_t<int> size, jpoint_t<int> point)
{
  _fps = 30;

  _event_manager = new EventManager(this);
  _instance = new WindowAdapter(this, jrect_t<int>{point, size});

  SetTitle("Main");
  SetUndecorated(false);
}

Window::~Window()
{
  try {
    _exec_thread.join();
  } catch (std::system_error &e) {
  }
  
  if (_event_manager != nullptr) {
    delete _event_manager;
    _event_manager = nullptr;
  }

  delete _instance;
  _instance = nullptr;
}

EventManager * Window::GetEventManager()
{
  return _event_manager;
}
    
void Window::ShowApp()
{
  Repaint();
}

void Window::Exec()
{
  _exec_thread = std::thread(&Window::ShowApp, this);
}

void Window::SetResizable(bool resizable)
{
  _instance->SetResizable(resizable);
}

bool Window::IsResizable()
{
  return _instance->IsResizable();
}

void Window::ToggleFullScreen()
{
  _instance->ToggleFullScreen();
}

jcursor_style_t Window::GetCursor()
{
  return _instance->GetCursor();
}

void Window::SetBounds(jrect_t<int> bounds)
{
  _instance->SetBounds(bounds);
}

jpoint_t<int> Window::GetPosition()
{
  return GetBounds().point;
}

void Window::SetPosition(jpoint_t<int> pos)
{
  SetBounds({pos, GetSize()});
}

jpoint_t<int> Window::GetSize()
{
  return GetBounds().size;
}

void Window::SetSize(jpoint_t<int> size)
{
  SetBounds({GetPosition(), size});
}

jrect_t<int> Window::GetBounds()
{
  return _instance->GetBounds();
}

void Window::SetTitle(std::string title)
{
  _instance->SetTitle(title);
}

std::string Window::GetTitle()
{
  return _instance->GetTitle();
}

void Window::SetOpacity(float opacity)
{
  _instance->SetOpacity(opacity);
}

float Window::GetOpacity()
{
  return _instance->GetOpacity();
}

void Window::SetUndecorated(bool undecorate)
{
  _instance->SetUndecorated(undecorate);
}

bool Window::IsUndecorated()
{
  return _instance->IsUndecorated();
}

void Window::SetIcon(Image *image)
{
  _instance->SetIcon(image);
}

Image * Window::GetIcon()
{
  return _instance->GetIcon();
}

void Window::Repaint()
{
  if (IsVisible() == false) {
    return;
  }

  _instance->Repaint();
}

void Window::PaintBackground(Graphics *g)
{
}

void Window::PaintGlassPane(Graphics *g)
{
}

void Window::SetFramesPerSecond(int fps)
{
  _fps = fps;
}

int Window::GetFramesPerSecond()
{
  return _fps;
}

void Window::Paint(Graphics *g)
{
	g->Clear();
}

void Window::SetVisible(bool visible)
{
  _instance->SetVisible(visible);
}

bool Window::IsVisible()
{
  return _instance->IsVisible();
}
    
jwindow_rotation_t Window::GetRotation()
{
  return _instance->GetRotation();
}

void Window::SetRotation(jwindow_rotation_t t)
{
  _instance->SetRotation(t);
}

bool Window::KeyPressed(KeyEvent *event)
{
  return false;
}

bool Window::KeyReleased(KeyEvent *event)
{
  return false;
}

bool Window::KeyTyped(KeyEvent *event)
{
  return false;
}

bool Window::MousePressed(MouseEvent *event)
{
  return false;
}

bool Window::MouseReleased(MouseEvent *event)
{
  return false;
}

bool Window::MouseMoved(MouseEvent *event)
{
  return false;
}

bool Window::MouseWheel(MouseEvent *event)
{
  return false;
}

void Window::SetCursorLocation(int x, int y)
{
  _instance->SetCursorLocation(x, y);
}

jpoint_t<int> Window::GetCursorLocation()
{
  return _instance->GetCursorLocation();
}

void Window::SetCursorEnabled(bool enable)
{
  return _instance->SetCursorEnabled(enable);
}

bool Window::IsCursorEnabled()
{
  return _instance->IsCursorEnabled();
}

void Window::SetCursor(jcursor_style_t t)
{
  _instance->SetCursor(t);
}

void Window::SetCursor(Image *shape, int hotx, int hoty)
{
  _instance->SetCursor(shape, hotx, hoty);
}

void Window::RegisterKeyListener(KeyListener *listener) 
{
   std::lock_guard<std::mutex> guard(_key_listener_mutex);

  std::vector<KeyListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

  if (i == _key_listeners.end()) {
    _key_listeners.push_back(listener);
  }
}

void Window::RemoveKeyListener(KeyListener *listener) 
{
   std::lock_guard<std::mutex> guard(_key_listener_mutex);

  for (std::vector<KeyListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
    KeyListener *l = (*i);

    if (dynamic_cast<KeyListener *>(l) == listener) {
      _key_listeners.erase(i);

      break;
    }
  }
}

const std::vector<KeyListener *> & Window::GetKeyListeners()
{
  return _key_listeners;
}

void Window::RegisterMouseListener(MouseListener *listener) 
{
   std::lock_guard<std::mutex> guard(_mouse_listener_mutex);

  std::vector<MouseListener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

  if (i == _mouse_listeners.end()) {
    _mouse_listeners.push_back(listener);
  }
}

void Window::RemoveMouseListener(MouseListener *listener) 
{
   std::lock_guard<std::mutex> guard(_mouse_listener_mutex);

  for (std::vector<MouseListener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
    MouseListener *l = (*i);

    if (dynamic_cast<MouseListener *>(l) == listener) {
      _mouse_listeners.erase(i);

      break;
    }
  }
}

const std::vector<MouseListener *> & Window::GetMouseListeners()
{
  return _mouse_listeners;
}

void Window::RegisterWindowListener(WindowListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_window_listener_mutex);

  if (std::find(_window_listeners.begin(), _window_listeners.end(), listener) == _window_listeners.end()) {
    _window_listeners.push_back(listener);
  }
}

void Window::RemoveWindowListener(WindowListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_window_listener_mutex);

  _window_listeners.erase(std::remove(_window_listeners.begin(), _window_listeners.end(), listener), _window_listeners.end());
}

void Window::DispatchWindowEvent(WindowEvent *event)
{
  if (event == nullptr) {
    return;
  }

  std::vector<WindowListener *> listeners;
  
  _window_listener_mutex.lock();

  listeners = _window_listeners;

  _window_listener_mutex.unlock();

  for (std::vector<WindowListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    WindowListener *listener = (*i);

    if (event->GetType() == JWET_CLOSING) {
      listener->WindowClosing(event);
    } else if (event->GetType() == JWET_CLOSED) {
      listener->WindowClosed(event);
    } else if (event->GetType() == JWET_OPENED) {
      listener->WindowOpened(event);
    } else if (event->GetType() == JWET_RESIZED) {
      listener->WindowResized(event);
    } else if (event->GetType() == JWET_MOVED) {
      listener->WindowMoved(event);
    } else if (event->GetType() == JWET_PAINTED) {
      listener->WindowPainted(event);
    } else if (event->GetType() == JWET_ENTERED) {
      listener->WindowEntered(event);
    } else if (event->GetType() == JWET_LEAVED) {
      listener->WindowLeaved(event);
    }
  }

  delete event;
}

const std::vector<WindowListener *> & Window::GetWindowListeners()
{
  return _window_listeners;
}

}

