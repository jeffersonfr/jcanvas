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

Window::Window(jpoint_t<int> size, jpoint_t<int> point):
  _event_manager(this), _window_adapter(new WindowAdapter(this, jrect_t<int>{point, size}))
{
  _fps = 30;

  SetTitle("Main");
  SetUndecorated(false);
}

Window::~Window()
{
  try {
    _exec_thread.join();
  } catch (std::system_error &e) {
  }

  delete _window_adapter;
  _window_adapter = nullptr;
}

EventManager & Window::GetEventManager()
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
  _window_adapter->SetResizable(resizable);
}

bool Window::IsResizable()
{
  return _window_adapter->IsResizable();
}

void Window::ToggleFullScreen()
{
  _window_adapter->ToggleFullScreen();
}

jcursor_style_t Window::GetCursor()
{
  return _window_adapter->GetCursor();
}

void Window::SetBounds(jrect_t<int> bounds)
{
  _window_adapter->SetBounds(bounds);
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
  return _window_adapter->GetBounds();
}

void Window::SetTitle(std::string title)
{
  _window_adapter->SetTitle(title);
}

std::string Window::GetTitle()
{
  return _window_adapter->GetTitle();
}

void Window::SetOpacity(float opacity)
{
  _window_adapter->SetOpacity(opacity);
}

float Window::GetOpacity()
{
  return _window_adapter->GetOpacity();
}

void Window::SetUndecorated(bool undecorate)
{
  _window_adapter->SetUndecorated(undecorate);
}

bool Window::IsUndecorated()
{
  return _window_adapter->IsUndecorated();
}

void Window::Repaint()
{
  if (IsVisible() == false) {
    return;
  }

  _window_adapter->Repaint();
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
  _window_adapter->SetVisible(visible);
}

bool Window::IsVisible()
{
  return _window_adapter->IsVisible();
}
    
jwindow_rotation_t Window::GetRotation()
{
  return _window_adapter->GetRotation();
}

void Window::SetRotation(jwindow_rotation_t t)
{
  _window_adapter->SetRotation(t);
}

void Window::SetCursorLocation(int x, int y)
{
  _window_adapter->SetCursorLocation(x, y);
}

jpoint_t<int> Window::GetCursorLocation()
{
  return _window_adapter->GetCursorLocation();
}

void Window::SetCursorEnabled(bool enable)
{
  return _window_adapter->SetCursorEnabled(enable);
}

bool Window::IsCursorEnabled()
{
  return _window_adapter->IsCursorEnabled();
}

void Window::SetCursor(jcursor_style_t t)
{
  _window_adapter->SetCursor(t);
}

void Window::SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty)
{
  _window_adapter->SetCursor(shape, hotx, hoty);
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

    if (event->GetType() == jwindowevent_type_t::Closing) {
      listener->WindowClosing(event);
    } else if (event->GetType() == jwindowevent_type_t::Closed) {
      listener->WindowClosed(event);
    } else if (event->GetType() == jwindowevent_type_t::Opened) {
      listener->WindowOpened(event);
    } else if (event->GetType() == jwindowevent_type_t::Resized) {
      listener->WindowResized(event);
    } else if (event->GetType() == jwindowevent_type_t::Moved) {
      listener->WindowMoved(event);
    } else if (event->GetType() == jwindowevent_type_t::Painted) {
      listener->WindowPainted(event);
    } else if (event->GetType() == jwindowevent_type_t::Entered) {
      listener->WindowEntered(event);
    } else if (event->GetType() == jwindowevent_type_t::Leaved) {
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

