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
#include "jcanvas/widgets/jlistbox.h"
#include "jcanvas/widgets/jcolumnlayout.h"
#include "jcanvas/widgets/jrectangleborder.h"

namespace jcanvas {

ListBox::ListBox()
{
  _selection_type = jlistbox_selection_t::Single;

  SetLayout<ColumnLayout>();
  GetLayout<ColumnLayout>()->SetCrossAxisAlign(jcrossaxis_align_t::Stretch);

  SetFocusable(true);
  SetInsets({2, 2, 2, 2});
  SetBorder<RectangleBorder>();
}

ListBox::~ListBox() 
{
}

void ListBox::ActionPerformed(ActionEvent *event)
{
  Button *item = reinterpret_cast<Button *>(event->GetSource());

  SelectItem(item);
}

void ListBox::SelectItem(Button *item)
{
  if (item == nullptr) {
    return;
  }

  if (_selection_type == jlistbox_selection_t::Single) {
    UnselectAll();

    jtheme_t &theme = item->GetTheme();

    theme.bg.normal = GetTheme().bg.select;

    item->SetPressed(true);
  } else if (_selection_type == jlistbox_selection_t::Multiple) {
    if (item->IsPressed() == true) {
      UnselectItem(item);
    } else {
      jtheme_t &theme = item->GetTheme();

      theme.bg.normal = GetTheme().bg.select;

      item->SetPressed(true);
    }
  }
}

void ListBox::SelectItemByIndex(int index)
{
  SelectItem(GetItemByIndex(index));
}

void ListBox::UnselectItem(Button *item)
{
  if (item == nullptr) {
    return;
  }

  jtheme_t &theme = item->GetTheme();

  theme.bg.normal = GetTheme().bg.normal;

  item->SetPressed(false);
}

void ListBox::UnselectItemByIndex(int index)
{
  UnselectItem(GetItemByIndex(index));
}

void ListBox::UnselectAll()
{
  for (auto item : GetComponents()) {
    UnselectItem(dynamic_cast<Button *>(item));
  }
}

void ListBox::RemoveItem(Button *item)
{
  item->RemoveActionListener(this);

  Container::Remove(item);

  delete item;
}

void ListBox::RemoveItemByIndex(int index)
{
  RemoveItem(GetItemByIndex(index));
}

void ListBox::SetSelectionType(jlistbox_selection_t type)
{
  _selection_type = type;

  for (auto cmp : GetComponents()) {
    if (_selection_type == jlistbox_selection_t::None) {
      cmp->SetFocusable(false);
    } else {
      cmp->SetFocusable(true);
    }
  }
}

jlistbox_selection_t ListBox::GetSelectionType()
{
  return _selection_type;
}

void ListBox::SelectIndexes(std::vector<int> indexes)
{
  for (auto index : indexes) {
    GetItemByIndex(index)->SetPressed(true);
  }
}

std::vector<int> ListBox::GetSelectedIndexes()
{
  std::vector<int> indexes;
  int i = 0;

  for (auto cmp : GetComponents()) {
    Button *item = dynamic_cast<Button *>(cmp);

    if (item->IsPressed() == true) {
      indexes.push_back(i);
    }

    i = i + 1;
  }

  return indexes;
}

std::vector<Button *> ListBox::GetSelectedItems()
{
  std::vector<Button *> items;

  for (auto cmp : GetComponents()) {
    Button *item = dynamic_cast<Button *>(cmp);

    if (item->IsPressed() == true) {
      items.push_back(item);
    }
  }

  return items;
}

Button * ListBox::GetItemByIndex(int index)
{
  return dynamic_cast<Button *>(GetComponents()[index]);
}

void ListBox::RegisterSelectListener(SelectListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_select_listener_mutex);

  if (std::find(_select_listeners.begin(), _select_listeners.end(), listener) == _select_listeners.end()) {
    _select_listeners.push_back(listener);
  }
}

void ListBox::RemoveSelectListener(SelectListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_select_listener_mutex);

  _select_listeners.erase(std::remove(_select_listeners.begin(), _select_listeners.end(), listener), _select_listeners.end());
}

void ListBox::DispatchSelectEvent(SelectEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _select_listener_mutex.lock();

  std::vector<SelectListener *> listeners = _select_listeners;

  _select_listener_mutex.unlock();

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

const std::vector<SelectListener *> & ListBox::GetSelectListeners()
{
  return _select_listeners;
}

}
