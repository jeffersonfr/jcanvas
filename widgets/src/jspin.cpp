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
#include "jcanvas/widgets/jspin.h"

namespace jcanvas {

Spin::Spin():
  Component(),
  ItemComponent()
{
  _type = jscroll_orientation_t::Horizontal;
  // _type = jscroll_orientation_t::Vertical;
  
  SetFocusable(true);
}

Spin::~Spin()
{
}

void Spin::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;

  Repaint();
}

jscroll_orientation_t Spin::GetScrollOrientation()
{
  return _type;
}

void Spin::NextItem()
{
  _index++;

  if (_index >= (int)_items.size()) {
    _index = _items.size()-1;

    if (_loop == true) {
      _index = 0;
    }
  }

  Repaint();

  DispatchSelectEvent(new SelectEvent(this, _index, jselectevent_type_t::Right));
}

void Spin::PreviousItem()
{
  _index--;

  if (_index < 0) {
    _index = 0;

    if (_loop == true) {
      _index = _items.size() - 1;
    }
  }

  Repaint();

  DispatchSelectEvent(new SelectEvent(this, _index, jselectevent_type_t::Left));
}

bool Spin::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (_items.size() == 0) {
    return true;
  }

  jkeyevent_symbol_t action = event->GetSymbol();

  bool catched = false;

  if (action == jkeyevent_symbol_t::CursorLeft) {
    if (_type == jscroll_orientation_t::Horizontal) {
      PreviousItem();

      catched = true;
    }
  } else if (action == jkeyevent_symbol_t::CursorRight) {
    if (_type == jscroll_orientation_t::Horizontal) {
      NextItem();

      catched = true;
    }
  } else if (action == jkeyevent_symbol_t::CursorUp) {
    if (_type == jscroll_orientation_t::Vertical) {
      PreviousItem();

      catched = true;
    }
  } else if (action == jkeyevent_symbol_t::CursorDown) {
    if (_type == jscroll_orientation_t::Vertical) {
      NextItem();

      catched = true;
    }
  } else if (action == jkeyevent_symbol_t::Enter) {
    DispatchSelectEvent(new SelectEvent(this, _index, jselectevent_type_t::Action));

    catched = true;
  }

  return catched;
}

bool Spin::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  if (_items.size() == 0) {
    return true;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    elocation = event->GetLocation();
  jrect_t<int>
    bounds = GetBounds();
  jinsets_t<int>
    padding = GetPadding();
  int
    h = bounds.size.y - padding.top - padding.bottom;
  bool 
    catched = false;

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    int 
      arrow_size;

    catched = true;

    if (_type == jscroll_orientation_t::Horizontal) {
      arrow_size = h/2;
    } else {
      arrow_size = (h - 8)/2;
    }

    RequestFocus();

    if (_type == jscroll_orientation_t::Horizontal) {
      if (elocation.y > padding.top && elocation.y < (padding.top + bounds.size.y)) {
        if (elocation.x > (bounds.size.x - arrow_size - padding.left) && elocation.x < (bounds.size.x - padding.left)) {
          NextItem();
        } else if (elocation.x > padding.left && elocation.x < (padding.left + arrow_size)) {
          PreviousItem();
        }
      }
    } else if (_type == jscroll_orientation_t::Vertical) {
      if (elocation.x > (bounds.size.x - 2*arrow_size - padding.left) && elocation.x < (bounds.size.x - padding.left)) {
        if (elocation.y > padding.top && elocation.y < (h/2)) {
          PreviousItem();
        } else if (elocation.y > (padding.top + h/2) && elocation.y < (padding.top + h)) {
          NextItem();
        }
      }
    }
  }

  return catched;
}


bool Spin::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }
  
  return false;
}

bool Spin::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }
  
  return false;
}

bool Spin::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  return false;
}

void Spin::AddEmptyItem()
{
  Item *item = new Item();

  if (_type == jscroll_orientation_t::Horizontal) {
    item->SetHorizontalAlign(jhorizontal_align_t::Center);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  if (_type == jscroll_orientation_t::Horizontal) {
    item->SetHorizontalAlign(jhorizontal_align_t::Center);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::AddImageItem(std::string text, Image *image)
{
  Item *item = new Item(text, image);

  if (_type == jscroll_orientation_t::Horizontal) {
    item->SetHorizontalAlign(jhorizontal_align_t::Center);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  if (_type == jscroll_orientation_t::Horizontal) {
    item->SetHorizontalAlign(jhorizontal_align_t::Center);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();
  jinsets_t<int>
    padding = GetPadding();
  int
    w = bounds.size.x - padding.left - padding.right,
    h = bounds.size.y - padding.top - padding.bottom;
  int
    arrow_size = 8;

  if (_type == jscroll_orientation_t::Horizontal) {
    int offset = (bounds.size.y - 2*arrow_size)/2;

    if (_loop == true || (_index < ((int)_items.size()-1))) {
      if (HasFocus() == true) {
        g->SetColor(theme.fg.focus);
      } else {
        g->SetColor(theme.fg.normal);
      }

      g->FillTriangle({w, padding.top + arrow_size + offset}, {padding.left + w - arrow_size, padding.top + offset}, {padding.left + w - arrow_size, padding.top + 2*arrow_size + offset});
    }

    if (_loop == true || (_index > 0 && _items.size() > 0)) {
      if (HasFocus() == true) {
        g->SetColor(theme.fg.focus);
      } else {
        g->SetColor(theme.fg.normal);
      }

      g->FillTriangle({padding.left, padding.top + arrow_size + offset}, {padding.left + arrow_size, padding.top + offset}, {padding.left + arrow_size, padding.top + 2*arrow_size + offset});
    }

    if (_items.size() > 0) {
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

        std::string text = _items[_index]->GetValue();

        // if (_wrap == false) {
          text = theme.font.primary->TruncateString(text, "...", w);
        // }

        g->DrawString(text, {padding.left, padding.top, w, h}, _items[_index]->GetHorizontalAlign(), _items[_index]->GetVerticalAlign());
      }
    }
  } else if (_type == jscroll_orientation_t::Vertical) {
    int offset = (bounds.size.y/2 - arrow_size)/2;

    if (_loop == true || (_index < ((int)_items.size()-1))) {
      if (HasFocus() == true) {
        g->SetColor(theme.fg.focus);
      } else {
        g->SetColor(theme.fg.normal);
      }
    }

    g->FillTriangle({bounds.size.x - 2*arrow_size - padding.left, padding.top + arrow_size + offset}, {bounds.size.x - padding.left, padding.top + arrow_size + offset}, {bounds.size.x - arrow_size - padding.left, padding.top + offset});

    if (_loop == true || (_index > 0 && _items.size() > 0)) {
      if (HasFocus() == true) {
        g->SetColor(theme.fg.focus);
      } else {
        g->SetColor(theme.fg.normal);
      }
    }

    g->FillTriangle({bounds.size.x - 2*arrow_size - padding.left, padding.top + arrow_size + bounds.size.y/2}, {bounds.size.x - padding.left, padding.top + arrow_size + bounds.size.y/2}, {bounds.size.x - arrow_size - padding.left, padding.top + bounds.size.y - offset});


    if (_items.size() > 0) {
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

        std::string text = _items[_index]->GetValue();

        // if (_wrap == false) {
          text = theme.font.primary->TruncateString(text, "...", w);
        // }

        g->DrawString(text, {padding.left, padding.top, w, h}, _items[_index]->GetHorizontalAlign(), _items[_index]->GetVerticalAlign());
      }
    }
  }
}

}

