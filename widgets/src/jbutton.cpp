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
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/core/jimage.h"

#include <algorithm>

namespace jcanvas {

Button::Button(std::string text):
  Button(text, nullptr)
{
}

Button::Button(std::string text, std::shared_ptr<Image> image):
  Component()
{
  _halign = jhorizontal_align_t::Center;
  _valign = jvertical_align_t::Center;

  _text = text;
  _image = image;

  SetFocusable(true);
}

Button::~Button()
{
}

void Button::UpdatePreferredSize()
{
  jpoint_t<int> 
    t = {
      0, 0
    };

  jtheme_t
    theme = GetTheme();

  if (_image != nullptr) {
    jpoint_t<int>
      size = _image->GetSize();

    t.x = t.x + size.x;
    t.y = t.y + size.y;

    if (theme.font.primary != nullptr and GetText().empty() == false) {
      t.x = t.x + 4;
    }
  }

  if (theme.font.primary != nullptr) {
    jfont_extends_t 
      extends = theme.font.primary->GetStringExtends(GetText());

    t.x = t.x + int(extends.size.x - extends.bearing.x);
    t.y = t.y + int(extends.size.y - extends.bearing.y);
  }

  SetPreferredSize(t + jpoint_t<int>{2*theme.border.size.x, 2*theme.border.size.y});
}

void Button::SetText(std::string text)
{
  if (_text != text) {
    _text = text;

    UpdatePreferredSize();
  }
}

std::string Button::GetText()
{
  return _text;
}

void Button::SetImage(std::shared_ptr<Image> image)
{
  if (_image != image) {
    _image = image;

    UpdatePreferredSize();
  }
}

std::shared_ptr<Image> Button::GetImage()
{
  return _image;
}

void Button::SetHorizontalAlign(jhorizontal_align_t align)
{
  if (_halign != align) {
    _halign = align;
  }
}

jhorizontal_align_t Button::GetHorizontalAlign()
{
  return _halign;
}

void Button::SetVerticalAlign(jvertical_align_t align)
{
  if (_valign != align) {
    _valign = align;
  }
}

jvertical_align_t Button::GetVerticalAlign()
{
  return _valign;
}

bool Button::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  bool catched = false;

  if (event->GetSymbol() == jkeyevent_symbol_t::Enter) {
    DispatchActionEvent(new ActionEvent(this));

    catched = true;
  }

  return catched;
}

bool Button::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    DispatchActionEvent(new ActionEvent(this));

    return true;
  }

  return false;
}

bool Button::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}

bool Button::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool Button::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  return false;
}

void Button::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();

  if (_image != nullptr) {
    jpoint_t<int>
      size = _image->GetSize();

    g->DrawImage(_image, jpoint_t<int>{theme.padding.left, theme.padding.top});

    theme.padding.left = theme.padding.left + size.x + 4;
  }

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

    std::string text = theme.font.primary->TruncateString(GetText(), "...", bounds.size.x);

    g->DrawString(text, theme.padding.bounds(jrect_t<int>{{0, 0}, bounds.size}), _halign, _valign);
  }
}

void Button::RegisterActionListener(ActionListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_action_listener_mutex);

  if (std::find(_action_listeners.begin(), _action_listeners.end(), listener) == _action_listeners.end()) {
    _action_listeners.push_back(listener);
  }
}

void Button::RemoveActionListener(ActionListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_action_listener_mutex);

  _action_listeners.erase(std::remove(_action_listeners.begin(), _action_listeners.end(), listener), _action_listeners.end());
}

void Button::DispatchActionEvent(ActionEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _action_listener_mutex.lock();

  std::vector<ActionListener *> listeners = _action_listeners;

  _action_listener_mutex.unlock();

  for (std::vector<ActionListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    ActionListener *listener = (*i);

    listener->ActionPerformed(event);
  }

  delete event;
}

const std::vector<ActionListener *> & Button::GetActionListeners()
{
  return _action_listeners;
}

}
