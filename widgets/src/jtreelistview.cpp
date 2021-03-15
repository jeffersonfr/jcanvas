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
#include "jcanvas/widgets/jtreelistview.h"

namespace jcanvas {

TreeListView::TreeListView():
  Component(),
  ItemComponent()
{
  _item_gap = 4;
  _pressed = false;
  _item_size = 32;
  _selected_index = -1;

  SetFocusable(true);
}

TreeListView::~TreeListView() 
{
}

void TreeListView::AddEmptyItem()
{
  Item *item = new Item();

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

void TreeListView::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

void TreeListView::AddImageItem(std::string text, std::shared_ptr<Image> image)
{
  Item *item = new Item(text, image);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

void TreeListView::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

int TreeListView::GetItemSize()
{
  return _item_size;
}

void TreeListView::SetItemSize(int size)
{
  if (size <= 0) {
    return;
  }

  _item_size = size;
}

void TreeListView::SetCurrentIndex(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    throw std::out_of_range("Index out of bounds exception");
  }

  jpoint_t
    slocation = GetScrollLocation();

  _index = i;

  SetScrollLocation(slocation.x, _index*(GetItemSize()+GetItemGap()));
}

bool TreeListView::IsSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return false;
  }

  if (_selected_index == i) {
    return true;
  }

  return false;
}

void TreeListView::SetSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_selected_index == i) {
    _selected_index = -1;
  } else {
    _selected_index = i;
  }
}

void TreeListView::Select(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  _selected_index = i;
}

void TreeListView::Deselect(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  _selected_index = -1;
}

void TreeListView::Expand(Item *item)
{
  _expanded_items[item] = true;
}

void TreeListView::Collapse(Item *item)
{
  _expanded_items[item] = false;
}

bool TreeListView::IsExpanded(Item *item)
{
  return _expanded_items[item];
}

void TreeListView::ExpandAll(Item *item)
{
  for (std::map<Item *, bool>::iterator i=_expanded_items.begin(); i!=_expanded_items.end(); i++) {
    _expanded_items[i->first] = true;
  }
}

void TreeListView::CollapseAll(Item *item)
{
  for (std::map<Item *, bool>::iterator i=_expanded_items.begin(); i!=_expanded_items.end(); i++) {
    _expanded_items[i->first] = false;
  }
}

int TreeListView::GetItemGap()
{
  return _item_gap;
}

void TreeListView::SetItemGap(int gap)
{
  _item_gap = gap;
}

int TreeListView::GetSelectedIndex()
{
  return _selected_index;
}

bool TreeListView::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEnabled() == false) {
    return false;
  }

  jkeyevent_symbol_t action = event->GetSymbol();

  bool catched = false;

  if (action == JKS_CURSOR_UP) {
    IncrementLines(1);
    
    catched = true;
  } else if (action == JKS_PAGE_UP) {
    // TODO:: IncrementLines((_size.y-2*(bordersize+_vertical_gap))/(_item_size+_item_gap));
    
    catched = true;
  } else if (action == JKS_CURSOR_DOWN) {
    DecrementLines(1);

    catched = true;
  } else if (action == JKS_PAGE_DOWN) {
    // TODO:: DecrementLines((_size.y-2*(bordersize+_vertical_gap))/(_item_size+_item_gap));

    catched = true;
  } else if (action == JKS_HOME) {
    IncrementLines(_items.size());
    
    catched = true;
  } else if (action == JKS_END) {
    DecrementLines(_items.size());
    
    catched = true;
  } else if (action == JKS_ENTER) {
    SetSelected(_index);

    if (_items.size() > 0) { 
      DispatchSelectEvent(new SelectEvent(_items[_index], _index, JSET_ACTION));
    }

    catched = true;
  }

  return catched;
}

bool TreeListView::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return false;
}

bool TreeListView::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}
  
bool TreeListView::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool TreeListView::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  jpoint_t
    slocation = GetScrollLocation();

  SetScrollLocation(slocation.x, slocation.y + _item_size*event->GetClicks());

  return true;
}

void TreeListView::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();
  jpoint_t 
    scroll_location = GetScrollLocation();
  int
    w = size.x - GetHorizontalPadding();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;
  int offset = 4;

  for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
    if ((*i)->GetType() == JIT_IMAGE) {
      offset += GetItemSize() + 8;

      break;
    }
  }

  theme.padding.left = theme.padding.left - scrollx;
  theme.padding.top = theme.padding.top - scrolly;

  for (int i=0; i<(int)_items.size(); i++) {
    int dy = padding.top + (GetItemSize() + GetItemGap())*i;

    if ((dy + GetItemSize()) < 0 || dy > size.y) {
      continue;
    }

    Item *item = _items[i];

    if (item->IsEnabled() == true) {
      g->SetColor(theme.fg.normal);
    } else {
      g->SetColor(theme.fg.disable);
    }

    if (_index != i) {
      if (_selected_index == i) {  
        g->SetColor(theme.fg.select);
      }
    } else {
      g->SetColor(theme.fg.focus);
    }

    g->FillRectangle({theme.padding.left, theme.padding.top+(GetItemSize() + GetItemGap())*i, w, GetItemSize()});

    if (_selected_index == i) {
      g->SetColor(theme.fg.select);
    }

    if (_items[i]->GetType() == JIT_EMPTY) {
    } else if (_items[i]->GetType() == JIT_TEXT) {
    } else if (_items[i]->GetType() == JIT_IMAGE) {
      if (_items[i]->GetImage() != nullptr) {
        g->DrawImage(_items[i]->GetImage(), {GetHorizontalPadding(), theme.padding.top + (GetItemSize() + GetItemGap())*i, GetItemSize(), GetItemSize()});
      }
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

      std::string text = _items[i]->GetValue();

      // if (_wrap == false) {
        text = theme.font.primary->TruncateString(text, "...", w - offset);
      // }

      g->DrawString(text, {theme.padding.left + offset, theme.padding.top + (GetItemSize() + GetItemGap())*i, w - offset, GetItemSize()}, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
    }
  }
}

void TreeListView::IncrementLines(int lines)
{
  if (_items.size() == 0) {
    return;
  }

  jpoint_t<int>
    size = GetSize();
  jpoint_t 
    scroll_location = GetScrollLocation();
  int
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;
  int 
    old_index = _index;

  _index = _index - lines;

  if (_index < 0) {
    if (_loop == false) {
      _index = 0;
    } else {
      _index = (int)(_items.size()-1);
    }
  }

  if ((_item_size+_item_gap)*_index < scrolly) {
    ScrollToVisibleArea({scrollx, (std::max)(0, (_item_size+_item_gap)*_index), size.x, size.y}, this);
  } else if ((scrolly+size.y) < (_item_size+_item_gap)*(int)_index) {
    ScrollToVisibleArea({scrollx, (_item_size+_item_gap)*(_index+1)-size.y+2*_item_gap, size.x, size.y}, this);
  }

  if (_index != old_index) {
    DispatchSelectEvent(new SelectEvent(_items[_index], _index, JSET_UP)); 
  }
}

void TreeListView::DecrementLines(int lines)
{
  if (_items.size() == 0) { 
    return;
  }

  jpoint_t<int>
    size = GetSize();
  int 
    old_index = _index;

  _index = _index + lines;

  if (_index >= (int)_items.size()) {
    if (_loop == false) {
      if (_items.size() > 0) {
        _index = _items.size()-1;
      } else {
        _index = 0;
      }
    } else {
      _index = 0;
    }
  }

  jpoint_t scroll_location = GetScrollLocation();
  int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
      scrolly = (IsScrollableY() == true)?scroll_location.y:0;

  if ((scrolly+size.y) < (_item_size+_item_gap)*(int)(_index+1)) {
    ScrollToVisibleArea({scrollx, (_item_size+_item_gap)*(_index+1)-size.y+2*_item_gap, size.x, size.y}, this);
  } else if ((_item_size+_item_gap)*_index < scrolly) {
    ScrollToVisibleArea({scrollx, (std::max)(0, (_item_size+_item_gap)*_index), size.x, size.y}, this);
  }

  if (_index != old_index) {
    DispatchSelectEvent(new SelectEvent(_items[_index], _index, JSET_DOWN)); 
  }
}

jpoint_t<int> TreeListView::GetScrollDimension()
{
  jpoint_t<int> 
    t = GetSize();

  // t.x = t.x;
  t.y = _items.size()*(GetItemSize() + GetItemGap()) + GetVerticalPadding();

  return  t;
}

}
