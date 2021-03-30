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

Button::Button(std::shared_ptr<Image> image):
  Button("", image)
{
}

Button::Button(std::string text, std::shared_ptr<Image> image)
{
  OnClick(
    [](Button *thiz, bool down) {
      thiz->SetPressed(down);
    });

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
  Text *textComponent = _text;
  FlatImage *imageComponent = _image;

  _text = nullptr;

  if (text.empty() == false) {
    _text = new Text(text);

    _text->SetBackground((textComponent != nullptr)?textComponent->GetBackground():nullptr);
    _text->SetBorder((textComponent != nullptr)?textComponent->GetBorder():nullptr);
    _text->SetEditable((textComponent != nullptr)?textComponent->IsEditable():false);
    _text->SetFocusable((textComponent != nullptr)?textComponent->IsFocusable():false);
    _text->SetScrollable((textComponent != nullptr)?textComponent->IsScrollable():false);
    _text->SetHorizontalAlign((textComponent != nullptr)?textComponent->GetHorizontalAlign():jhorizontal_align_t::Center);
    _text->SetVerticalAlign((textComponent != nullptr)?textComponent->GetVerticalAlign():jvertical_align_t::Center);

    Add(_text, jborderlayout_align_t::Center);
  }

  _image = nullptr;

  if (image != nullptr) {
    _image = new FlatImage(image);

    _image->SetBackground((imageComponent != nullptr)?imageComponent->GetBackground():nullptr);
    _image->SetBorder((imageComponent != nullptr)?imageComponent->GetBorder():nullptr);
    _image->SetFocusable((imageComponent != nullptr)?imageComponent->IsFocusable():false);
    _image->SetScrollable((imageComponent != nullptr)?imageComponent->IsScrollable():false);
    _image->SetAlign((imageComponent != nullptr)?imageComponent->GetAlign():jrect_align_t::Contains);

    if (_text == nullptr) {
      Add(_image, jborderlayout_align_t::Center);
    } else {
      Add(_image, jborderlayout_align_t::West);
    }
  }
  
  Remove(textComponent);
  Remove(imageComponent);

  delete textComponent;
  delete imageComponent;
}

std::function<void(Button *, bool)> Button::OnClick(std::function<void(Button *, bool)> callback)
{
  auto previous = _onclick;

  _onclick = callback;

  return previous;
}

void Button::Click()
{
  if (_onclick != nullptr) {
    _onclick(this, true);
  }

  if (_onclick != nullptr) {
    _onclick(this, false);
  }
}

void Button::SetPressed(bool param)
{
  _is_pressed = param;
}

bool Button::IsPressed()
{
  return _is_pressed;
}

void Button::SetText(std::string text)
{
  std::shared_ptr<Image> image;

  if (GetImageComponent() != nullptr) {
    image = GetImageComponent()->GetImage();
  }

  Build(text, image);
}

Text * Button::GetTextComponent()
{
  return _text;
}

void Button::SetImage(std::shared_ptr<Image> image)
{
  std::string text;

  if (GetTextComponent() != nullptr) {
    text = GetTextComponent()->GetText();
  }

  Build(text, image);
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
    if (_onclick != nullptr) {
      _onclick(this, true);
    }

    DispatchActionEvent(new ActionEvent(this));

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
    if (_onclick != nullptr) {
      _onclick(this, false);
    }

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
    if (_onclick != nullptr) {
      _onclick(this, true);
    }

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

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    if (_onclick != nullptr) {
      _onclick(this, false);
    }

    DispatchActionEvent(new ActionEvent(this));

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

  std::lock_guard<std::mutex> lock1(_remove_action_listener_mutex);
  std::lock_guard<std::mutex> lock2(_action_listener_mutex);

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

  std::lock_guard<std::mutex> guard(_remove_action_listener_mutex);

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
