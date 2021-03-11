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
#include "jcanvas/widgets/jprogressbar.h"

namespace jcanvas {

ProgressBar::ProgressBar(jscroll_orientation_t type):
  Component()
{
  _type = type;
  _value = 0;
  _fixe_delta = 10;
  _delta = _fixe_delta;
  _stone_size = 32;
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;

  Repaint();
}

jscroll_orientation_t ProgressBar::GetScrollOrientation()
{
  return _type;
}

int ProgressBar::GetStoneSize()
{
  return _stone_size;
}

void ProgressBar::SetStoneSize(int size)
{
  _stone_size = size;

  Repaint();
}
    
double ProgressBar::GetValue()
{
  return _value;
}

void ProgressBar::SetValue(double i)
{
  _value = (int)i;

  if (_value < 0.0) {
    _value = 0;
  }

  if (_value > 100) {
    _value = 100;
  }

  Repaint();
}

void ProgressBar::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();
  std::string 
    text;

  if (_type == jscroll_orientation_t::Horizontal) {
    double 
      d = (_value*bounds.size.x)/100.0;
    char 
      t[255];

    if (d > bounds.size.x) {
      d = bounds.size.x;
    }

    g->SetColor(theme.scroll.color.normal);
    g->FillRectangle({theme.padding.left, theme.padding.top, (int)d, bounds.size.y});

    snprintf(t, 255-1, "%d %%", _value);

    text = (char *)t;
  } else if (_type == jscroll_orientation_t::Vertical) {
    double 
      d = (_value*bounds.size.y)/100.0;
    char 
      t[255];

    if (d > bounds.size.y) {
      d = bounds.size.y;
    }

    g->SetColor(theme.scroll.color.normal);
    g->FillRectangle({theme.padding.left, theme.padding.top, bounds.size.x, (int)d});

    snprintf(t, 255-1, "%d %%", _value);

    text = (char *)t;
  }

  if (IsEnabled() == true) {
    if (HasFocus() == true) {
      g->SetColor(theme.fg.focus);
    } else {
      g->SetColor(theme.fg.normal);
    }
  } else {
    g->SetColor(theme.fg.disable);
  }

  int length = theme.font.primary->GetStringWidth(theme.font.primary->TruncateString(text, "...", bounds.size.x));

  g->DrawString(text, {theme.padding.left + (bounds.size.x - length)/2, theme.padding.top, bounds.size.x, bounds.size.y});
}

}
