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
#include "jcanvas/widgets/jslidercomponent.h"

#include <algorithm>

namespace jcanvas {

SliderComponent::SliderComponent(jrect_t<int> bounds):
     Component(bounds)
{
  _index = 0;
  _value = 0;
  _minimum = 0;
  _maximum = 100;
  _minimum_tick = 1;
  _maximum_tick = 10;
  _type = jscroll_orientation_t::Horizontal;
  // _type = JSO_VERTICAL;
}

SliderComponent::~SliderComponent()
{
}

void SliderComponent::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;

  Repaint();
}

jscroll_orientation_t SliderComponent::GetScrollOrientation()
{
  return _type;
}

int SliderComponent::GetValue()
{
  return _value;
}

void SliderComponent::SetValue(int i)
{
  if (_value == i) {
    return;
  }

  if (i < _minimum) {
    i = _minimum;
  }

  if (i > _maximum) {
    i = _maximum;
  }
    
  int diff = i-_value;

  _value = i;

  if (diff > _minimum_tick) {
    DispatchAdjustmentEvent(new AdjustmentEvent(this, jadjustmentevent_type_t::BlockUp, _value));
  } else if (diff < -_minimum_tick) {
    DispatchAdjustmentEvent(new AdjustmentEvent(this, jadjustmentevent_type_t::BlockDown, _value));
  } else if (diff > 0 && diff <= _minimum_tick) {
    DispatchAdjustmentEvent(new AdjustmentEvent(this, jadjustmentevent_type_t::UnitUp, _value));
  } else if (diff < 0 && diff >= -_minimum_tick) {
    DispatchAdjustmentEvent(new AdjustmentEvent(this, jadjustmentevent_type_t::UnitDown, _value));
  }

  Repaint();
}


void SliderComponent::SetRange(int minimum, int maximum)
{
  _minimum = minimum;
  _maximum = maximum;
}

int SliderComponent::GetMinimum()
{
  return _minimum;
}

int SliderComponent::GetMaximum()
{
  return _maximum;
}

void SliderComponent::SetMinimum(int i)
{
  _minimum = i;
}

void SliderComponent::SetMaximum(int i)
{
  _maximum = i;
}

int SliderComponent::GetMinorTickSpacing()
{
  return _minimum_tick;
}

int SliderComponent::GetMajorTickSpacing()
{
  return _maximum_tick;
}

void SliderComponent::SetMinorTickSpacing(int i)
{
  _minimum_tick = i;
}

void SliderComponent::SetMajorTickSpacing(int i)
{
  _maximum_tick = i;
}

void SliderComponent::RegisterAdjustmentListener(AdjustmentListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_adjustment_listener_mutex);

  if (std::find(_adjustment_listeners.begin(), _adjustment_listeners.end(), listener) == _adjustment_listeners.end()) {
    _adjustment_listeners.push_back(listener);
  }
}

void SliderComponent::RemoveAdjustmentListener(AdjustmentListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_adjustment_listener_mutex);

  _adjustment_listeners.erase(std::remove(_adjustment_listeners.begin(), _adjustment_listeners.end(), listener), _adjustment_listeners.end());
}

void SliderComponent::DispatchAdjustmentEvent(AdjustmentEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _adjustment_listener_mutex.lock();

  std::vector<AdjustmentListener *> listeners = _adjustment_listeners;

  _adjustment_listener_mutex.unlock();

  for (std::vector<AdjustmentListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    AdjustmentListener *listener = (*i);

    listener->AdjustmentValueChanged(event);
  }

  delete event;
}

const std::vector<AdjustmentListener *> & SliderComponent::GetAdjustmentListeners()
{
  return _adjustment_listeners;
}

}
