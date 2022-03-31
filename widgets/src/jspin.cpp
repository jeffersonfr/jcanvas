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
#include "jcanvas/widgets/jspin.h"
#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jresources.h"

#include <stdexcept>

namespace jcanvas {

Spin::Spin(std::pair<int, int> range)
{
  _current_index = 0;
  _low_range = range.first;
  _high_range = range.second;
  
  if ((_high_range - _low_range) <= 0) {
    throw std::runtime_error("Items must have positive elements");
  }

  _is_range = true;
  _is_loop_enabled = false;
  
  Build(std::to_string(_low_range));
}

Spin::Spin(const std::vector<std::string> &items):
  _items(items)
{
  if (items.empty() == true) {
    throw std::runtime_error("Items must have elements");
  }

  _current_index = 0;
  _low_range = 0;
  _high_range = items.size();
  _is_range = false;
  _is_loop_enabled = false;

  Build(_items[0]);
}

Spin::~Spin()
{
  _previous.RemoveActionListener(this);
  _next.RemoveActionListener(this);
}

void Spin::Build(std::string value)
{
  SetLayout<BorderLayout>();

  _previous.SetImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/left-arrow.png"));
  // _previous.SetFocusable(false);
  _previous.SetPreferredSize(_previous.GetImageComponent()->GetPreferredSize());
  _previous.SetBorder(nullptr);

  _previous.RegisterActionListener(this);

  _text.SetText(value);
  _text.SetFocusable(false);
  _text.SetEditable(false);
  _text.SetBorder(nullptr);
  _text.SetHorizontalAlign(jhorizontal_align_t::Center);
  _text.SetVerticalAlign(jvertical_align_t::Center);

  _next.SetImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/right-arrow.png"));
  // _next.SetFocusable(false);
  _next.SetPreferredSize(_next.GetImageComponent()->GetPreferredSize());
  _next.SetBorder(nullptr);

  _next.RegisterActionListener(this);

  Add(&_previous, jborderlayout_align_t::West);
  Add(&_text, jborderlayout_align_t::Center);
  Add(&_next, jborderlayout_align_t::East);

  SetInsets({2, 2, 2, 2});
  SetScrollable(false);
  SetFocusable(true);
  SetPreferredSize({160, 32});
  SetBackground(std::make_shared<SolidBackground>());
  SetBorder(std::make_shared<RectangleBorder>());
}

void Spin::Previous()
{
  int index = _current_index - 1;

  if (_is_range == false) {
    if (index < 0) {
      index = 0;

      if (_is_loop_enabled == true) {
        index = _items.size() - 1;
      }
    }
  } else {
    if (index < _low_range) {
      index = _low_range;

      if (_is_loop_enabled == true) {
        index = _high_range - 1;
      }
    }
  }
    
  SetCurrentIndex(index);
}

void Spin::Next()
{
  int index = _current_index + 1;

  if (_is_range == false) {
    if (index > (int)(_items.size() - 1)) {
      index = _items.size() - 1;

      if (_is_loop_enabled == true) {
        index = 0;
      }
    }
  } else {
    if (index > (int)(_high_range - 1)) {
      index = _high_range - 1;

      if (_is_loop_enabled == true) {
        index = _low_range;
      }
    }
  }

  SetCurrentIndex(index);
}

void Spin::ActionPerformed(ActionEvent *event)
{
  Button *button = reinterpret_cast<Button *>(event->GetSource());

  if (button == &_previous) {
    Previous();

    DispatchSelectEvent(new SelectEvent(this, _current_index, jselectevent_type_t::Left));
  } else {
    Next();
  
    DispatchSelectEvent(new SelectEvent(this, _current_index, jselectevent_type_t::Right));
  }
}

void Spin::SetLoopEnabled(bool enabled)
{
  _is_loop_enabled = enabled;
}

bool Spin::IsLoopEnabled()
{
  return _is_loop_enabled;
}

std::string Spin::GetValue()
{
  return _text.GetText();
}

void Spin::SetCurrentIndex(int index)
{
  if (index < _low_range) {
    index = _low_range;
  }

  if (index > _high_range) {
    index = _high_range;
  }

  _current_index = index;

  if (_is_range == false) {
    _text.SetText(_items[_current_index]);
  } else {
    _text.SetText(std::to_string(_current_index));
  }
}

int Spin::GetCurrentIndex()
{
  return _current_index;
}

void Spin::RegisterSelectListener(SelectListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_select_listener_mutex);

  if (std::find(_select_listeners.begin(), _select_listeners.end(), listener) == _select_listeners.end()) {
    _select_listeners.push_back(listener);
  }
}

void Spin::RemoveSelectListener(SelectListener *listener)
{
  if (listener == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock1(_remove_select_listener_mutex);
  std::lock_guard<std::mutex> lock2(_select_listener_mutex);

  _select_listeners.erase(std::remove(_select_listeners.begin(), _select_listeners.end(), listener), _select_listeners.end());
}

void Spin::DispatchSelectEvent(SelectEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _select_listener_mutex.lock();

  std::vector<SelectListener *> listeners = _select_listeners;

  _select_listener_mutex.unlock();

  std::lock_guard<std::mutex> lock(_remove_select_listener_mutex);

  for (std::vector<SelectListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    SelectListener *listener = (*i);

    if (event->GetType() == jselectevent_type_t::Action) {
      listener->ItemSelected(event);
    } else {
      listener->ItemChanged(event);
    }
  }

  delete event;
}

const std::vector<SelectListener *> & Spin::GetSelectListeners()
{
  return _select_listeners;
}

}

