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
#include "jcanvas/widgets/jscrollbar.h"

namespace jcanvas {

ScrollBar::ScrollBar():
  SliderComponent()
{
  _pressed = false;
  _stone_size = 32;
  _label_visible = true;
  
  SetFocusable(true);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;
}

jscroll_orientation_t ScrollBar::GetScrollOrientation()
{
  return _type;
}

void ScrollBar::SetStoneSize(int size)
{
  _stone_size = size;
}

int ScrollBar::GetStoneSize()
{
  return _stone_size;
}

bool ScrollBar::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  jkeyevent_symbol_t action = event->GetSymbol();

  bool catched = false;

  if (_type == jscroll_orientation_t::Horizontal) {
    if (action == jkeyevent_symbol_t::CursorLeft) {
      SetValue(_value-_minimum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::CursorRight) {
      SetValue(_value+_minimum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::PageDown) {
      SetValue(_value-_maximum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::PageUp) {
      SetValue(_value+_maximum_tick);

      catched = true;
    }
  } else if (_type == jscroll_orientation_t::Vertical) {
    if (action == jkeyevent_symbol_t::CursorUp) {
      SetValue(_value-_minimum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::CursorDown) {
      SetValue(_value+_minimum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::PageDown) {
      SetValue(_value-_maximum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::PageUp) {
      SetValue(_value+_maximum_tick);

      catched = true;
    }
  }

  return catched;
}

bool ScrollBar::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t
    elocation = event->GetLocation();
  jrect_t<int>
    bounds = GetBounds();
  int 
    arrow_size,
    dx = theme.padding.left,
    dy = theme.padding.right,
    dw = bounds.size.x - 2*dx - _stone_size,
    dh = bounds.size.y - 2*dy - _stone_size;
  bool 
    catched = false;

  if (_type == jscroll_orientation_t::Horizontal) {
    arrow_size = bounds.size.y/2;
  } else {
    arrow_size = bounds.size.x/2;
  }

  if (event->GetButton() != jmouseevent_button_t::Button1) {
    return false;
  }

  catched = true;

  if (_type == jscroll_orientation_t::Horizontal) {
    if (elocation.y > 0 && elocation.y < (bounds.size.y)) {
      int d = (int)((_value*(dw-2*arrow_size))/(GetMaximum()-GetMinimum()));

      _pressed = false;

      if (elocation.x > (dx) && elocation.x < (arrow_size + dx)) {
        SetValue(_value - _minimum_tick);
      } else if (elocation.x > (bounds.size.x - arrow_size - dx) && elocation.x < (bounds.size.x - dx)) {
        SetValue(_value + _minimum_tick);
      } else if (elocation.x > (arrow_size + dx) && elocation.x < (arrow_size + dx + d)) {
        SetValue(_value - _maximum_tick);
      } else if (elocation.x > (arrow_size + dx + d + _stone_size) && elocation.x < (bounds.size.x - arrow_size)) {
        SetValue(_value + _maximum_tick);
      } else if (elocation.x > (arrow_size + dx + d) && elocation.x < (arrow_size + dx + d + _stone_size)) {
        _pressed = true;
      }
    }
  } else if (_type == jscroll_orientation_t::Vertical) {
    if (elocation.x > 0 && elocation.x < (bounds.size.x)) {
      int d = (int)((_value*(dh-2*arrow_size))/(GetMaximum()-GetMinimum()));

      _pressed = false;

      if (elocation.y > (dy) && elocation.y < (arrow_size + dy)) {
        SetValue(_value - _minimum_tick);
      } else if (elocation.y > (bounds.size.y - arrow_size - dy) && elocation.y < (bounds.size.y - dy)) {
        SetValue(_value + _minimum_tick);
      } else if (elocation.y > (arrow_size + dy) && elocation.y < (arrow_size + dy + d)) {
        SetValue(_value - _maximum_tick);
      } else if (elocation.y > (arrow_size + dy + d + _stone_size) && elocation.y < (bounds.size.y - arrow_size)) {
        SetValue(_value + _maximum_tick);
      } else if (elocation.y > (arrow_size + dy + d) && elocation.y < (arrow_size + dy + d + _stone_size)) {
        _pressed = true;
      }
    }
  }


  return catched;
}

bool ScrollBar::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }
  
  _pressed = false;

  return true;
}

bool ScrollBar::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }
  
  jtheme_t
    theme = GetTheme();
  jpoint_t
    elocation = event->GetLocation();
  jrect_t<int>
    bounds = GetBounds();
  int 
    arrow_size,
    dx = theme.padding.left,
    dy = theme.padding.right,
    dw = bounds.size.x - 2*dx - _stone_size,
    dh = bounds.size.y - 2*dy - _stone_size;

  if (_type == jscroll_orientation_t::Horizontal) {
    arrow_size = bounds.size.y/2;
  } else {
    arrow_size = bounds.size.x/2;
  }

  if (_pressed == true) {
    int 
      diff = GetMaximum()-GetMinimum();

    if (_type == jscroll_orientation_t::Horizontal) {
      SetValue(diff*(elocation.x - _stone_size/2 - arrow_size)/(dw - 2*arrow_size));
    } else if (_type == jscroll_orientation_t::Vertical) {
      SetValue(diff*(elocation.y - _stone_size/2 - arrow_size)/(dh - 2*arrow_size));
    }

    return true;
  }

  return false;
}

bool ScrollBar::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  _pressed = false;

  SetValue(GetValue()+_minimum_tick*event->GetClicks());

  return true;
}

void ScrollBar::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();

  if (_type == jscroll_orientation_t::Horizontal) {
    int
      arrow_size = bounds.size.y/2,
      limit = bounds.size.x - _stone_size - 2*arrow_size;
    double 
      d = (_value*limit)/(GetMaximum() - GetMinimum());

    if (d > limit) {
      d = limit;
    }

    if (HasFocus() == true) {
      g->SetColor(theme.fg.focus);
    } else {
      g->SetColor(theme.scroll.color.normal);
    }

    g->FillRectangle({(int)d + arrow_size + theme.padding.left, theme.padding.top, _stone_size, bounds.size.y});

    g->FillTriangle({theme.padding.left + bounds.size.x, theme.padding.top+arrow_size}, {theme.padding.left + bounds.size.x - arrow_size, theme.padding.top}, {theme.padding.left + bounds.size.x - arrow_size, theme.padding.top+2*arrow_size});
    g->FillTriangle({theme.padding.left, theme.padding.top+arrow_size}, {theme.padding.left+arrow_size, theme.padding.top}, {theme.padding.left+arrow_size, theme.padding.top+2*arrow_size});
  } else if (_type == jscroll_orientation_t::Vertical) {
    int 
      arrow_size = bounds.size.x/2,
      limit = bounds.size.y - _stone_size - 2*arrow_size - theme.padding.top - theme.padding.bottom;
    double 
      d = (_value*limit)/(GetMaximum()-GetMinimum());

    if (d > limit) {
      d = limit;
    }

    if (HasFocus() == true) {
      g->SetColor(theme.fg.focus);
    } else {
      g->SetColor(theme.scroll.color.normal);
    }

    g->FillRectangle({theme.padding.left, (int)d + arrow_size + theme.padding.top + theme.padding.top + theme.padding.bottom, bounds.size.x, _stone_size});
    g->FillTriangle({theme.padding.left, theme.padding.top+arrow_size}, {theme.padding.left + bounds.size.x/2, theme.padding.top}, {theme.padding.left + bounds.size.x, theme.padding.top+arrow_size});
    g->FillTriangle({theme.padding.left, theme.padding.top + bounds.size.y -arrow_size}, {theme.padding.left + bounds.size.x/2, theme.padding.top + bounds.size.y}, {theme.padding.left + bounds.size.x, theme.padding.top + bounds.size.y -arrow_size});
  }
}

}
