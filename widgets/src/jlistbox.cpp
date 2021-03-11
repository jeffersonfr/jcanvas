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

namespace jcanvas {

ListBox::ListBox():
  Component(),
  ItemComponent()
{
  _pressed = false;
  _selected_index = -1;
  _mode = jlistbox_selection_t::None;

  SetFocusable(true);
}

ListBox::~ListBox() 
{
}

void ListBox::UpdatePreferredSize()
{
  jtheme_t
    theme = GetTheme();
  jrect_t<int> 
    bounds = GetBounds();

  // TODO:: list all items to see the largest width [+ image.x + gap] 

  bounds.size.x = bounds.size.x + theme.padding.left + theme.padding.right + 2*theme.border.size.x;
  bounds.size.y = _items.size()*(GetItemSize() + GetItemGap()) - GetItemGap() + theme.padding.top + theme.padding.bottom + 2*theme.border.size.y;

  SetPreferredSize(bounds.size);
}

void ListBox::SetSelectionType(jlistbox_selection_t type)
{
  if (_mode == type) {
    return;
  }

  _mode = type;
  _selected_index = -1;

  for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
    (*i)->SetSelected(false);
  }
}

jlistbox_selection_t ListBox::GetSelectionType()
{
  return _mode;
}

void ListBox::AddEmptyItem()
{
  Item *item = new Item();

  item->SetHorizontalAlign(jhorizontal_align_t::Left);
    
  AddInternalItem(item);
  AddItem(item);

  UpdatePreferredSize();
}

void ListBox::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  item->SetHorizontalAlign(jhorizontal_align_t::Left);
    
  AddInternalItem(item);
  AddItem(item);
  
  UpdatePreferredSize();
}

void ListBox::AddImageItem(std::string text, Image *image)
{
  Item *item = new Item(text, image);

  item->SetHorizontalAlign(jhorizontal_align_t::Left);
    
  AddInternalItem(item);
  AddItem(item);

  UpdatePreferredSize();
}

void ListBox::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  item->SetHorizontalAlign(jhorizontal_align_t::Left);
    
  AddInternalItem(item);
  AddItem(item);

  UpdatePreferredSize();
}

void ListBox::SetCurrentIndex(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    throw std::out_of_range("Index out of bounds exception");
  }

  _index = i;

  jpoint_t<int>
    slocation = GetScrollLocation();

  SetScrollLocation(slocation.x, _index*(GetItemSize() + GetItemGap()));
}

bool ListBox::IsSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return false;
  }

  if (_mode == jlistbox_selection_t::Single) {
    if (_selected_index == i) {
      return true;
    }
  } else if (_mode == jlistbox_selection_t::Multiple) {
    return _items[i]->IsSelected();
  }

  return false;
}

void ListBox::SetSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_mode == jlistbox_selection_t::Single) {
    if (_selected_index == i) {
      _selected_index = -1;
    } else {
      _selected_index = i;
    }

    Repaint();
  } else if (_mode == jlistbox_selection_t::Multiple) {
    if (item->IsSelected()) {
      item->SetSelected(false);
    } else {
      item->SetSelected(true);
    }

    Repaint();
  }
}

void ListBox::Select(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_mode == jlistbox_selection_t::Single) {
    _selected_index = i;

    Repaint();
  } else if (_mode == jlistbox_selection_t::Multiple) {
    item->SetSelected(true);

    Repaint();
  }
}

void ListBox::Deselect(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_mode == jlistbox_selection_t::Single) {
    _selected_index = -1;

    Repaint();
  } else if (_mode == jlistbox_selection_t::Multiple) {
    item->SetSelected(false);

    Repaint();
  }
}

int ListBox::GetSelectedIndex()
{
  return _selected_index;
}

bool ListBox::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEnabled() == false) {
    return false;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();
  jkeyevent_symbol_t 
    action = event->GetSymbol();
  bool 
    catched = false;

  if (action == jkeyevent_symbol_t::CursorUp) {
    IncrementLines(1);
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::PageUp) {
    IncrementLines((size.y - theme.padding.top - theme.padding.bottom)/(GetItemSize() + GetItemGap()));
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::CursorDown) {
    DecrementLines(1);

    catched = true;
  } else if (action == jkeyevent_symbol_t::PageDown) {
    DecrementLines((size.y - theme.padding.top - theme.padding.bottom)/(GetItemSize() + GetItemGap()));

    catched = true;
  } else if (action == jkeyevent_symbol_t::Home) {
    IncrementLines(_items.size());
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::End) {
    DecrementLines(_items.size());
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::Enter) {
    SetSelected(_index);

    if (_items.size() > 0) { 
      DispatchSelectEvent(new SelectEvent(_items[_index], _index, jselectevent_type_t::Action));
    }

    catched = true;
  }

  return catched;
}

bool ListBox::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return true;
}

bool ListBox::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}
  
bool ListBox::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool ListBox::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  jpoint_t<int>
    slocation = GetScrollLocation();

  SetScrollLocation(slocation.x, slocation.y + GetItemSize()*event->GetClicks());

  Repaint();

  return true;
}

void ListBox::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t<int> 
    scroll_location = GetScrollLocation();
  jrect_t<int>
    bounds = GetBounds();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;
  int 
    offset = theme.padding.left;

  for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
    if ((*i)->GetType() == jitem_type_t::Image) {
      offset += GetItemSize() + theme.padding.left;

      break;
    }
  }

  theme.padding.left = theme.padding.left - scrollx;
  theme.padding.top = theme.padding.top - scrolly;

  for (int i=0; i<(int)_items.size(); i++) {
    int dy = theme.padding.top + (GetItemSize() + GetItemGap())*i;

    if ((dy + GetItemSize()) < 0 || dy > bounds.size.y) {
      continue;
    }

    Item *item = _items[i];

    if (item->IsEnabled() == true) {
      g->SetColor(theme.bg.normal);
    } else {
      g->SetColor(theme.bg.disable);
    }

    if (_index != i) {
      if (_mode == jlistbox_selection_t::Single) {  
        if (_selected_index == i) {  
          g->SetColor(theme.bg.select);
        }
      } else if (_mode == jlistbox_selection_t::Multiple) {  
        if (item->IsSelected() == true) {  
          g->SetColor(theme.bg.select);
        }
      }
    } else {
      g->SetColor(theme.bg.focus);
    }

    g->FillRectangle({theme.padding.left, theme.padding.top + (GetItemSize() + GetItemGap())*i, bounds.size.x, GetItemSize()});

    if (theme.font.primary != nullptr) {
      g->SetFont(theme.font.primary);

      if (item->IsEnabled() == true) {
        g->SetColor(theme.fg.normal);
      } else {
        g->SetColor(theme.fg.disable);
      }

      if (_index != i) {
        if (_mode == jlistbox_selection_t::Single) {  
          if (_selected_index == i) {  
            g->SetColor(theme.fg.select);
          }
        } else if (_mode == jlistbox_selection_t::Multiple) {  
          if (item->IsSelected() == true) {  
            g->SetColor(theme.fg.select);
          }
        }
      } else {
        g->SetColor(theme.fg.focus);
      }

      std::string text = _items[i]->GetValue();

      // if (_wrap == false) {
        text = theme.font.primary->TruncateString(text, "...", bounds.size.x - offset);
      // }

      g->DrawString(text, {theme.padding.left + offset, theme.padding.top + (GetItemSize() + GetItemGap())*i, bounds.size.x - offset, GetItemSize()}, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
    }
    
    if (_items[i]->GetType() == jitem_type_t::Empty) {
    } else if (_items[i]->GetType() == jitem_type_t::Text) {
    } else if (_items[i]->GetType() == jitem_type_t::Image) {
      if (_items[i]->GetImage() != nullptr) {
        g->DrawImage(_items[i]->GetImage(), {theme.padding.left, theme.padding.top + (GetItemSize() + GetItemGap())*i, GetItemSize(), GetItemSize()});
      }
    }

  }
}

void ListBox::IncrementLines(int lines)
{
  if (_items.size() == 0) {
    return;
  }

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

  jpoint_t<int> 
    scroll_location = GetScrollLocation();
  jpoint_t<int>
    size = GetSize();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;

  if ((GetItemSize() + GetItemGap())*_index < scrolly) {
    ScrollToVisibleArea({scrollx, (std::max)(0, (GetItemSize() + GetItemGap())*_index), size.x, size.y}, this);
  } else if ((scrolly+size.y) < (GetItemSize() + GetItemGap())*(int)_index) {
    ScrollToVisibleArea({scrollx, (GetItemSize() + GetItemGap())*(_index + 1) - size.y + 2*GetItemGap(), size.x, size.y}, this);
  }

  if (_index != old_index) {
    Repaint();

    DispatchSelectEvent(new SelectEvent(_items[_index], _index, jselectevent_type_t::Up)); 
  }
}

void ListBox::DecrementLines(int lines)
{
  if (_items.size() == 0) { 
    return;
  }

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

  jpoint_t<int> 
    scroll_location = GetScrollLocation();
  jpoint_t<int>
    size = GetSize();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;

  if ((scrolly + size.y) < (GetItemSize() + GetItemGap())*(int)(_index + 1)) {
    ScrollToVisibleArea({scrollx, (GetItemSize() + GetItemGap())*(_index + 1)-size.y+2*GetItemGap(), size.x, size.y}, this);
  } else if ((GetItemSize() + GetItemGap())*_index < scrolly) {
    ScrollToVisibleArea({scrollx, (std::max)(0, (GetItemSize() + GetItemGap())*_index), size.x, size.y}, this);
  }

  if (_index != old_index) {
    Repaint();

    DispatchSelectEvent(new SelectEvent(_items[_index], _index, jselectevent_type_t::Down)); 
  }
}

jpoint_t<int> ListBox::GetScrollDimension()
{
  jtheme_t
    theme = GetTheme();
  jrect_t<int> 
    bounds = GetBounds();
  jtheme_border_t
    border = theme.border;

  bounds.size.y = _items.size()*(GetItemSize() + GetItemGap()) + theme.padding.top + theme.padding.bottom;

  return bounds.size;
}

}
