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
#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/core/jimage.h"

#include <algorithm>

namespace jcanvas {

Button::Button(std::string text, std::shared_ptr<Image> image)
{
  SetLayout<BorderLayout>();
  SetInsets({2, 2, 2, 2});
  SetScrollable(false);
  SetFocusable(true);
  SetPreferredSize({128, 32});
  SetBackground(std::make_shared<SolidBackground>());
  SetBorder(std::make_shared<RectangleBorder>());
  
  Build(text, image);
}

Button::~Button()
{
  RemoveAll();

  delete _text;
  delete _image;
}

void Button::Build(std::string text, std::shared_ptr<Image> image)
{
  RemoveAll();

  if (_text != nullptr) {
    delete _text;
    _text = nullptr;
  }

  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }

  if (text.empty() == false) {
    _text = new Text(text);

    _text->SetBackground(nullptr);
    _text->SetBorder(nullptr);
    _text->SetEditable(false);
    _text->SetFocusable(false);
    _text->SetScrollable(false);
    _text->SetHorizontalAlign(jhorizontal_align_t::Center);
    _text->SetVerticalAlign(jvertical_align_t::Center);
    
    Add(_text, jborderlayout_align_t::Center);
  }

  if (image != nullptr) {
    _image = new FlatImage(image);

    _image->SetBackground(nullptr);
    _image->SetBorder(nullptr);
    _image->SetFocusable(false);
    _image->SetScrollable(false);
    _image->SetAlign(jrect_align_t::Contains);
   
    if (_text == nullptr) {
      Add(_image, jborderlayout_align_t::Center);
    } else {
      Add(_image, jborderlayout_align_t::West);
    }
  }
}

void Button::SetText(std::string text)
{
  Build(text, _image->GetImage());
}

Text * Button::GetTextComponent()
{
  return _text;
}

void Button::SetImage(std::shared_ptr<Image> image)
{
  Build(_text->GetText(), _image->GetImage());
}

FlatImage * Button::GetImageComponent()
{
  return _image;
}

bool Button::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (event->GetSymbol() == jkeyevent_symbol_t::Enter) {
    _is_down = true;

    return true;
  }

  return false;
}

bool Button::KeyReleased(KeyEvent *event)
{
  if (Component::KeyReleased(event) == true) {
    return true;
  }

  if (event->GetSymbol() == jkeyevent_symbol_t::Enter) {
    _is_down = false;

    DispatchActionEvent(new ActionEvent(this));

    return true;
  }

  return false;
}

bool Button::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    _is_down = true;

    return true;
  }

  return false;
}

bool Button::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    _is_down = false;

    return true;
  }

  return false;
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
