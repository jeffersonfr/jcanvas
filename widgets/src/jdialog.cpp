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
#include "jcanvas/widgets/jdialog.h"
#include "jcanvas/widgets/jframe.h"

namespace jcanvas {

Dialog::Dialog(Container *parent, jrect_t<int> bounds):
  Dialog("", parent, bounds)
{
}

Dialog::Dialog(std::string title, Container *parent, jrect_t<int> bounds):
  Container(bounds)
{
  if (parent == nullptr) {
    throw std::invalid_argument("Parent must have a valid pointer");
  }

  if (dynamic_cast<Frame *>(parent) == nullptr) {
    throw std::invalid_argument("Unable to convert parent to Frame");
  }

  jinsets_t<int> 
    insets;
  
  insets.left = 8;
  insets.top = 8;
  insets.right = 8;
  insets.bottom = 8;

  SetInsets(insets);
  SetTitle(title);
  SetBackgroundVisible(true);
  SetParent(parent);

  _focus_owner = nullptr;
  _is_modal = false;
}

Dialog::~Dialog()
{
  Close();
}

void Dialog::SetTitle(std::string title)
{
  jtheme_t
    theme = GetTheme();
  jinsets_t<int>
    insets;
  
  _title = title;

  insets.left = 8;
  insets.top = 8;
  insets.right = 8;
  insets.bottom = 8;

  if (_title.empty() == false) {
    if (theme.font.primary != nullptr) {
      insets.top = theme.font.primary->GetSize() + 16;
    }
  }
   
  SetInsets(insets);
}

std::string Dialog::GetTitle()
{
  return _title;
}

void Dialog::Paint(Graphics *g)
{
  Container::Paint(g);

  if (_title.empty() == false) {
    jtheme_t
      theme = GetTheme();

    if (theme.font.primary != nullptr) {
      jpoint_t<int>
        size = GetSize();

      g->SetFont(theme.font.primary);
      g->SetColor(theme.fg.normal);
      g->DrawString(_title, {0, 8, size.x, theme.font.primary->GetSize()}, jhorizontal_align_t::Center, jvertical_align_t::Center);
    }
  }
}

bool Dialog::IsModal()
{
  return _is_modal;
}

void Dialog::Exec(bool modal)
{
  GetParent()->InternalAddDialog(this);

  if (modal == true) {
    std::unique_lock<std::mutex> lock(_modal_mutex);

    _modal_condition.wait(lock);
  }
}

void Dialog::Close()
{
  GetParent()->InternalRemoveDialog(this);

  _modal_condition.notify_one();
}

Component * Dialog::GetFocusOwner()
{
  return _focus_owner;
}

void Dialog::RequestComponentFocus(Component *c)
{
  if (_focus_owner != nullptr && _focus_owner != c) {
    _focus_owner->ReleaseFocus();
  }

  _focus_owner = c;

  Repaint();

  c->DispatchFocusEvent(new FocusEvent(c, jfocusevent_type_t::Gain));
}

void Dialog::ReleaseComponentFocus(Component *c)
{
  if (_focus_owner == nullptr or _focus_owner != c) {
    return;
  }

  _focus_owner = nullptr;

  Repaint();

  c->DispatchFocusEvent(new FocusEvent(c, jfocusevent_type_t::Lost));
}

Container * Dialog::GetFocusCycleRootAncestor()
{
  return this;
}

bool Dialog::KeyPressed(KeyEvent *event)
{
  Container::KeyPressed(event);

  if (event->GetSymbol() == jkeyevent_symbol_t::Escape) {
    Close();
  }

  return true;
}

bool Dialog::KeyReleased(KeyEvent *event)
{
  Container::KeyReleased(event);

  return true;
}

bool Dialog::KeyTyped(KeyEvent *event)
{
  Container::KeyTyped(event);

  return true;
}

bool Dialog::MousePressed(MouseEvent *event)
{
  Container::MousePressed(event);

  return true;
}

bool Dialog::MouseReleased(MouseEvent *event)
{
  Container::MouseReleased(event);

  return true;
}

bool Dialog::MouseMoved(MouseEvent *event)
{
  Container::MouseMoved(event);

  return true;
}

bool Dialog::MouseWheel(MouseEvent *event)
{
  Container::MouseWheel(event);

  return true;
}

}
