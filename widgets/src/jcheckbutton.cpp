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
#include "jcanvas/widgets/jcheckbutton.h"

#include <algorithm>

namespace jcanvas {

CheckButton::CheckButton(jcheckbox_type_t type, std::string text):
  Component()
{
  _halign = jhorizontal_align_t::Center;
  _valign = jvertical_align_t::Center;

  _type = type;
  _text = text;
  _checked = false;
  _is_wrap = false;

  jtheme_t
    &theme = GetTheme();

  theme.padding = {8, 0, 0, 0};

  SetFocusable(true);
}

CheckButton::~CheckButton()
{
}

void CheckButton::SetWrap(bool b)
{
  if (_is_wrap == b) {
    return;
  }

  _is_wrap = b;
}

bool CheckButton::IsWrap()
{
  return _is_wrap;
}

void CheckButton::SetText(std::string text)
{
  _text = text;
}

std::string CheckButton::GetText()
{
  return _text;
}

void CheckButton::SetType(jcheckbox_type_t type)
{
  _type = type;
}

bool CheckButton::IsSelected()
{
  return _checked;
}

void CheckButton::SetSelected(bool b)
{
  if (_checked != b) {
    _checked = b;

    DispatchToggleEvent(new ToggleEvent(this, _checked));
  }
}

void CheckButton::SetHorizontalAlign(jhorizontal_align_t align)
{
  if (_halign != align) {
    _halign = align;
  }
}

jhorizontal_align_t CheckButton::GetHorizontalAlign()
{
  return _halign;
}

void CheckButton::SetVerticalAlign(jvertical_align_t align)
{
  if (_valign != align) {
    _valign = align;
  }
}

jvertical_align_t CheckButton::GetVerticalAlign()
{
  return _valign;
}
    
bool CheckButton::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  bool catched = false;

  jkeyevent_symbol_t action = event->GetSymbol();

  if (action == jkeyevent_symbol_t::Enter) {
    if (_type == jcheckbox_type_t::Check) {
      if (_checked == true) {
        SetSelected(false);
      } else {
        SetSelected(true);
      }
    } else {
      SetSelected(true);
    }

    catched = true;
  }

  return catched;
}

bool CheckButton::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  jpoint_t<int>
    size = GetSize();

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    jpoint_t
      elocation = event->GetLocation();
    int
      ms = size.y;

    if (size.y > size.x) {
      ms = size.x;
    }

    if ((elocation.x > 0 && elocation.x < ms) && (elocation.y > 0 && elocation.y < ms)) {
      if (_type == jcheckbox_type_t::Check) {
        if (_checked == true) {
          SetSelected(false);
        } else {
          SetSelected(true);
        }
      } else {
        SetSelected(true);
      }
    }

    return true;
  }

  return false;
}

bool CheckButton::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}

bool CheckButton::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool CheckButton::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  return false;
}

jcheckbox_type_t CheckButton::GetType()
{
  return _type;
}

void CheckButton::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();

  bounds = theme.padding.bounds(jrect_t<int>{{0, 0}, bounds.size});

  int
    major = 16,
    minor = 4,
    cs = (std::min(bounds.size.x, bounds.size.y) - major)/2;

  g->SetColor(theme.fg.select);

  if (_type == jcheckbox_type_t::Check) {
    g->FillRectangle({theme.padding.left, theme.padding.top + (bounds.size.y - cs)/2, major, major});
  } else if (_type == jcheckbox_type_t::Radio) {
    g->FillCircle({theme.padding.left + major/2, bounds.size.y/2}, major/2);
  }

  if (IsSelected() == true) {
    g->SetColor(theme.fg.normal);

    if (_type == jcheckbox_type_t::Check) {
      g->FillRectangle({theme.padding.left + minor, theme.padding.top + (bounds.size.y - cs)/2 + minor, 2*minor, 2*minor});
    } else {
      g->FillCircle({theme.padding.left + major/2, bounds.size.y/2}, minor);
    }
  }

  theme.padding.left = theme.padding.left + major + theme.padding.left;

  if (theme.font.primary != nullptr) {
    g->SetFont(theme.font.primary);

    if (IsEnabled() == true) {
      if (HasFocus() == true) {
        g->SetColor(theme.fg.focus);
      } else {
        g->SetColor(theme.fg.normal);
      }
    } else {
      g->SetColor(theme.fg.disable);
    }

    std::string text = GetText();

    if (_is_wrap == false) {
      text = theme.font.primary->TruncateString(text, "...", bounds.size.x);
    }

    g->DrawString(text, theme.padding.bounds(bounds), _halign, _valign);
  }
}

void CheckButton::RegisterToggleListener(ToggleListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_check_listener_mutex);

  if (std::find(_check_listeners.begin(), _check_listeners.end(), listener) == _check_listeners.end()) {
    _check_listeners.push_back(listener);
  }
}

void CheckButton::RemoveToggleListener(ToggleListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_check_listener_mutex);

  _check_listeners.erase(std::remove(_check_listeners.begin(), _check_listeners.end(), listener), _check_listeners.end());
}

void CheckButton::DispatchToggleEvent(ToggleEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _check_listener_mutex.lock();

  std::vector<ToggleListener *> listeners = _check_listeners;

  _check_listener_mutex.unlock();

  for (std::vector<ToggleListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    ToggleListener *listener = (*i);

    listener->StateChanged(event);
  }

  delete event;
}

const std::vector<ToggleListener *> & CheckButton::GetToggleListeners()
{
  return _check_listeners;
}

}
