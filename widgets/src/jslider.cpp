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
#include "jcanvas/widgets/jslider.h"

namespace jcanvas {

Slider::Slider():
  SliderComponent()
{
  _pressed = false;
  _stone_size = 24;
  _inverted = false;

  SetFocusable(true);
}

Slider::~Slider()
{
}

int Slider::GetStoneSize()
{
  return _stone_size;
}
    
void Slider::SetStoneSize(int size)
{
  _stone_size = size;
}

void Slider::SetInverted(bool b)
{
  if (_inverted == b) {
    return;
  }

  _inverted = b;
}

bool Slider::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEnabled() == false) {
    return false;
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
      SetValue(_value+_maximum_tick);

      catched = true;
    } else if (action == jkeyevent_symbol_t::PageUp) {
      SetValue(_value-_maximum_tick);

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
    
bool Slider::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    elocation = event->GetLocation();
  jrect_t<int>
    bounds = GetBounds();
  int
    dw = bounds.size.x - theme.padding.left - theme.padding.right - _stone_size,
    dh = bounds.size.y - theme.padding.top - theme.padding.bottom - _stone_size;
  bool 
    catched = false;

  if (event->GetButton() == jmouseevent_button_t::Button1) {
    catched = true;

    if (_type == jscroll_orientation_t::Horizontal) {
      if (elocation.y > 0 && elocation.y < (bounds.size.y)) {
        int d = (int)((_value*dw)/(GetMaximum()-GetMinimum()));

        _pressed = false;

        if (elocation.x > theme.padding.left && elocation.x < (theme.padding.left+d)) {
          SetValue(_value-_maximum_tick);
        } else if (elocation.x > (theme.padding.left+d+_stone_size) && elocation.x < (bounds.size.x)) {
          SetValue(_value+_maximum_tick);
        } else if (elocation.x > (theme.padding.left+d) && elocation.x < (theme.padding.left+d+_stone_size)) {
          _pressed = true;
        }
      }
    } else if (_type == jscroll_orientation_t::Vertical) {
      if (elocation.x > 0 && elocation.x < (bounds.size.x)) {
        int d = (int)((_value*dh)/(GetMaximum()-GetMinimum()));

        _pressed = false;

        if (elocation.y > theme.padding.top && elocation.y < (theme.padding.top+d)) {
          SetValue(_value-_maximum_tick);
        } else if (elocation.y > (theme.padding.top+d+_stone_size) && elocation.y < (bounds.size.y)) {
          SetValue(_value+_maximum_tick);
        }
      }
    }
  } 

  return catched;
}

    
bool Slider::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  _pressed = false;

  return false;
}
    
bool Slider::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    elocation = event->GetLocation();
  jrect_t<int>
    bounds = GetBounds();
  int
    dw = bounds.size.x - theme.padding.left - theme.padding.right - _stone_size,
    dh = bounds.size.y - theme.padding.top - theme.padding.bottom - _stone_size;

  if (_pressed == true) {
    int diff = GetMaximum()-GetMinimum();

    if (_type == jscroll_orientation_t::Horizontal) {
      SetValue(diff*(elocation.x - _stone_size/2)/dw);
    } else if (_type == jscroll_orientation_t::Vertical) {
      SetValue(diff*(elocation.y - _stone_size/2)/dh);
    }

    return true;
  }

  return false;
}
    
bool Slider::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  _pressed = false;

  SetValue(GetValue()+_minimum_tick*event->GetClicks());

  return true;
}
    
void Slider::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();
  int
    w = bounds.size.x - theme.padding.left - theme.padding.right,
    h = bounds.size.y - theme.padding.top - theme.padding.bottom;

  if (_type == jscroll_orientation_t::Horizontal) {
    int 
      d = (int)((_value*(w - _stone_size))/(GetMaximum() - GetMinimum()));

    if (d > (w - _stone_size)) {
      d = w - _stone_size;
    }

    if (HasFocus() == true) {
      g->SetColor(theme.fg.focus);
    } else {
      g->SetColor(theme.scroll.color.normal);
    }
    
    g->FillRectangle({theme.padding.left, theme.padding.top+(h-4)/2, w, 4});

    if (_inverted == false) {
      std::vector<jpoint_t<int>> p = {
        {0, 0},
        {_stone_size, 0},
        {_stone_size, (int)(h*0.4)},
        {_stone_size/2, h},
        {0, (int)(h*0.4)}
      };

      g->FillPolygon({(int)d + theme.padding.left, theme.padding.top}, p, 5);
    } else {
      std::vector<jpoint_t<int>> p = {
        {_stone_size/2, 0},
        {_stone_size, (int)(h*0.6)},
        {_stone_size, h},
        {0, h},
        {0, (int)(h*0.6)}
      };

      g->FillPolygon({(int)d + theme.padding.left, theme.padding.top}, p);
    }
  } else if (_type == jscroll_orientation_t::Vertical) {
    int 
      d = (int)((_value*(h-_stone_size))/(GetMaximum()-GetMinimum()));

    if (d > (h - _stone_size)) {
      d = h - _stone_size;
    }

    if (HasFocus() == true) {
      g->SetColor(theme.fg.focus);
    } else {
      g->SetColor(theme.scroll.color.normal);
    }
    
    g->FillRectangle({(w-10)/2 + theme.padding.left, theme.padding.top, 10, h});

    if (_inverted == false) {
      std::vector<jpoint_t<int>> p = {
        {0, 0},
        {(int)(bounds.size.x*0.4), 0},
        {w, _stone_size/2},
        {(int)(bounds.size.x*0.4), _stone_size},
        {0, _stone_size}
      };

      g->FillPolygon({theme.padding.left, (int)d + theme.padding.top}, p);
    } else {
      std::vector<jpoint_t<int>> p = {
        {0, _stone_size/2},
        {(int)(bounds.size.x*0.6), 0},
        {w, 0},
        {w, _stone_size},
        {(int)(bounds.size.x*0.6), _stone_size}
      };

      g->FillPolygon({theme.padding.left, (int)d + theme.padding.top}, p);
    }
  }
}

}
