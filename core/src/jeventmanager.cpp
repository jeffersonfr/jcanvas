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
#include "jcanvas/core/jeventmanager.h"
#include "jcanvas/core/jwindow.h"

namespace jcanvas {

EventManager::EventManager(Window *window)
{
  _click_delay = 200; // milliseconds
  _alive = true;
  _window = window;
  _autograb = true;

  _thread = std::thread(&EventManager::ProcessEvents, this);
}

EventManager::~EventManager()
{
  std::unique_lock<std::mutex> lock(_mutex);

  _alive = false;

  _condition.notify_one();

  lock.unlock();

  _thread.join();
}

void EventManager::SetClickDelay(size_t ms)
{
  _click_delay = ms;
}

size_t EventManager::GetClickDelay()
{
  return _click_delay;
}

void EventManager::SetAutoGrab(bool grab)
{
  _autograb = grab;
}

bool EventManager::IsAutoGrab()
{
  return _autograb;
}

void EventManager::PostEvent(EventObject *event)
{
  std::unique_lock<std::mutex> lock(_mutex);

  _events.push_back(event);

  _condition.notify_one();
}

bool EventManager::IsKeyDown(jkeyevent_symbol_t key)
{
  return _key_button_map[key];
}

bool EventManager::IsButtonDown(jmouseevent_button_t button)
{
  return _mouse_button_map[button];
}

const std::vector<EventObject *> & EventManager::GetEvents()
{
  return _events;
}

void EventManager::ProcessEvents()
{
  do {
    std::unique_lock<std::mutex> lock(_mutex);

    while (_events.size() == 0 && _alive == true) {
      _condition.wait(lock);
    }

    lock.unlock();

    if (_alive == false) {
      break;
    }

    EventObject *unknown = nullptr;

    _mutex.lock();

    unknown = _events.front();

    _events.erase(_events.begin());

    _mutex.unlock();
  
    if (dynamic_cast<KeyEvent *>(unknown) != nullptr) {
      KeyListener *listener = dynamic_cast<KeyListener *>(_window);

      if (listener != nullptr) {
        KeyEvent *event = dynamic_cast<KeyEvent *>(unknown);

        if (event->GetType() == jkeyevent_type_t::Pressed) {
          _key_button_map[event->GetSymbol()] = true;

          listener->KeyPressed(event);
        } else if (event->GetType() == jkeyevent_type_t::Released) {
          _key_button_map[event->GetSymbol()] = false;

          listener->KeyReleased(event);
        } else if (event->GetType() == jkeyevent_type_t::Typed) {
          listener->KeyTyped(event);
        }
      }
    } else if (dynamic_cast<MouseEvent *>(unknown) != nullptr) {
      MouseListener *listener = dynamic_cast<MouseListener *>(_window);

      if (listener != nullptr) {
        MouseEvent *event = dynamic_cast<MouseEvent *>(unknown);

        jmouseevent_button_t buttons = jmouseevent_button_t::None;
        int clicks = 1;

        if (event->GetType() == jmouseevent_type_t::Pressed) {
          static std::map<jmouseevent_button_t, std::chrono::time_point<std::chrono::steady_clock>> button_timestamp;

          _mouse_button_map[event->GetButton()] = true;

          if ((event->GetTimestamp() - button_timestamp[event->GetButton()]) < std::chrono::milliseconds(200)) {
            clicks = clicks + 1;
          }

          button_timestamp[event->GetButton()] = event->GetTimestamp();
        } else if (event->GetType() == jmouseevent_type_t::Released) {
          _mouse_button_map[event->GetButton()] = false;
        }
        
        if (_mouse_button_map[jmouseevent_button_t::Button1] == true) {
          buttons = jenum_t{buttons}.Or(jmouseevent_button_t::Button1);
        }

        if (_mouse_button_map[jmouseevent_button_t::Button2] == true) {
          buttons = jenum_t{buttons}.Or(jmouseevent_button_t::Button2);
        }

        if (_mouse_button_map[jmouseevent_button_t::Button3] == true) {
          buttons = jenum_t{buttons}.Or(jmouseevent_button_t::Button3);
        }

        MouseEvent local(event->GetSource(), event->GetType(), event->GetButton(), buttons, event->GetLocation(), clicks);

        if (event->GetType() == jmouseevent_type_t::Pressed) {
          listener->MousePressed(&local);
        } else if (event->GetType() == jmouseevent_type_t::Released) {
          listener->MouseReleased(&local);
        } else if (event->GetType() == jmouseevent_type_t::Moved) {
          listener->MouseMoved(&local);
        } else if (event->GetType() == jmouseevent_type_t::Rotated) {
          listener->MouseWheel(&local);
        }
      }
    }
  
    delete unknown;
  } while (_alive == true);
}

}

