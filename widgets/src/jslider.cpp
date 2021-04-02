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
#include "jcanvas/widgets/jslider.h"
#include "jcanvas/core/jbufferedimage.h"

namespace jcanvas {

Slider::Slider()
{
  _stone_image = std::make_shared<BufferedImage>(JCANVAS_RESOURCES_DIR "/images/marker.png");

  _is_pressed = false;

  SetRange({0.0f, 100.0f});
  SetTicks({1.0f, 10.0f});
  SetValue(0.0f);
  SetVertical(false);
  SetMetricVisible(true);
  SetFocusable(true);
}

Slider::~Slider()
{
  _stone_image = nullptr;
}

void Slider::SetRange(jrange_t range)
{
  _range = range;
}

jrange_t Slider::GetRange()
{
  return _range;
}

void Slider::SetTicks(jrange_t ticks)
{
  _ticks = ticks;
}

jrange_t Slider::GetTicks()
{
  return _ticks;
}

void Slider::SetStoneImage(std::shared_ptr<Image> image)
{
  _stone_image = image;;
}

std::shared_ptr<Image> Slider::GetStoneImage()
{
  return _stone_image;
}

void Slider::SetVertical(bool vertical)
{
  _is_vertical = vertical;
}

bool Slider::IsVertical()
{
  return _is_vertical;
}

void Slider::SetMetricVisible(bool visible)
{
  _is_metric_visible = visible;
}

bool Slider::IsMetricVisible()
{
  return _is_metric_visible;
}

void Slider::SetValue(float value)
{
  if (value < _range.min) {
    value = _range.min;
  }

  if (value > _range.max) {
    value = _range.max;
  }

  _value = value;
}

float Slider::GetValue()
{
  return _value;
}

void Slider::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();
  jpoint_t<int>
    isize = _stone_image->GetSize();

  float
    percent = (_value - _range.min)/static_cast<float>(_range.max - _range.min)/1.0f;

  if (IsVertical() == false) {
    int 
      max = size.x - theme.padding.left - theme.padding.right - isize.x,
      pos = max*percent;

    g->SetCompositeFlags(jcomposite_flags_t::SrcOver);
    g->DrawImage(_stone_image, jpoint_t<int>{pos + theme.padding.left, (size.y - isize.y)/2});
    g->SetCompositeFlags(jcomposite_flags_t::Src);

    if (IsMetricVisible() == true) {
      int offset = theme.padding.left + isize.x/2;

      g->SetColor(theme.fg.normal);

      for (int i=0; i<=10; i++) {
        g->FillCircle({offset, size.y/2}, 3);

        offset = offset + max/10;
      }
    }
  } else {
    int
      max = size.x - theme.padding.left - theme.padding.right - isize.x,
      pos = max*percent;

    g->SetCompositeFlags(jcomposite_flags_t::SrcOver);
    g->DrawImage(_stone_image, jpoint_t<int>{(size.x - isize.x)/2, pos + theme.padding.top});
    g->SetCompositeFlags(jcomposite_flags_t::Src);
    
    if (IsMetricVisible() == true) {
      int offset = theme.padding.top + isize.y/2;

      g->SetColor(theme.fg.normal);

      for (int i=0; i<=10; i++) {
        g->FillCircle({size.x/2, offset}, 3);

        offset = offset + max/10;
      }
    }
  }
}

bool Slider::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  jkeyevent_symbol_t action = event->GetSymbol();

  if (IsVertical() == false) {
    if (action == jkeyevent_symbol_t::CursorLeft) {
      SetValue(_value - _ticks.min);

      return true;
    } else if (action == jkeyevent_symbol_t::CursorRight) {
      SetValue(_value + _ticks.min);

      return true;
    } else if (action == jkeyevent_symbol_t::PageDown) {
      SetValue(_value - _ticks.max);

      return true;
    } else if (action == jkeyevent_symbol_t::PageUp) {
      SetValue(_value + _ticks.max);

      return true;
    }
  } else {
    if (action == jkeyevent_symbol_t::CursorUp) {
      SetValue(_value - _ticks.min);

      return true;
    } else if (action == jkeyevent_symbol_t::CursorDown) {
      SetValue(_value + _ticks.min);

      return true;
    } else if (action == jkeyevent_symbol_t::PageDown) {
      SetValue(_value + _ticks.max);

      return true;
    } else if (action == jkeyevent_symbol_t::PageUp) {
      SetValue(_value - _ticks.max);

      return true;
    }
  }

  return false;
}

bool Slider::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  if (event->GetButton() != jmouseevent_button_t::Button1) {
    return false;
  } 

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    location = event->GetLocation();
  jpoint_t<int>
    size = GetSize();
  jpoint_t<int>
    isize = _stone_image->GetSize();

  _is_pressed = false;

  float
    percent = (_value - _range.min)/static_cast<float>(_range.max - _range.min)/1.0f;

  if (IsVertical() == false) {
    int 
      max = size.x - theme.padding.left - theme.padding.right - isize.x,
      pos = max*percent;
      
    if (location.x < pos) {
      SetValue(_value - _ticks.max);
    } else if (location.x > (pos + isize.x)) {
      SetValue(_value + _ticks.max);
    } else {
      _is_pressed = true;
    }

    return true;
  } else {
    int
      max = size.y - theme.padding.top - theme.padding.bottom - isize.y,
      pos = max*percent;

    if (location.y < pos) {
      SetValue(_value - _ticks.max);
    } else if (location.y > (pos + isize.y)) {
      SetValue(_value + _ticks.max);
    } else {
      _is_pressed = true;
    }

    return true;
  }

  return false;
}

bool Slider::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  _is_pressed = false;

  return false;
}

bool Slider::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    location = event->GetLocation();
  jpoint_t<int>
    size = GetSize();
  jpoint_t<int>
    isize = _stone_image->GetSize();

  if (_is_pressed == false) {
    return false;
  }

  jrange_t range = GetRange();

  if (IsVertical() == false) {
    int max = size.x - theme.padding.left - theme.padding.right - isize.x;

    SetValue((range.max - range.min)*(location.x - isize.x/2)/max);

    return true;
  } else {
    int max = size.y - theme.padding.top - theme.padding.bottom - isize.y;

    SetValue((range.max - range.min)*(location.y - isize.y/2)/max);
    
    return true;
  }

  return false;
}

bool Slider::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  _is_pressed = false;

  SetValue(GetValue() - _ticks.min*event->GetClicks());

  return true;
}

void Slider::RegisterAdjustmentListener(AdjustmentListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_adjustment_listener_mutex);

  if (std::find(_adjustment_listeners.begin(), _adjustment_listeners.end(), listener) == _adjustment_listeners.end()) {
    _adjustment_listeners.push_back(listener);
  }
}

void Slider::RemoveAdjustmentListener(AdjustmentListener *listener)
{
  if (listener == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock1(_remove_adjustment_listener_mutex);
  std::lock_guard<std::mutex> lock2(_adjustment_listener_mutex);

  _adjustment_listeners.erase(std::remove(_adjustment_listeners.begin(), _adjustment_listeners.end(), listener), _adjustment_listeners.end());
}

void Slider::DispatchAdjustmentEvent(AdjustmentEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _adjustment_listener_mutex.lock();

  std::vector<AdjustmentListener *> listeners = _adjustment_listeners;

  _adjustment_listener_mutex.unlock();

  std::lock_guard<std::mutex> lock(_remove_adjustment_listener_mutex);

  for (std::vector<AdjustmentListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    AdjustmentListener *listener = (*i);

    listener->AdjustmentValueChanged(event);
  }

  delete event;
}

const std::vector<AdjustmentListener *> & Slider::GetAdjustmentListeners()
{
  return _adjustment_listeners;
}

}
