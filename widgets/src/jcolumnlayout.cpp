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
#include "jcanvas/widgets/jcolumnlayout.h"

namespace jcanvas {

ColumnLayout::ColumnLayout(jmainaxis_align_t main, jcrossaxis_align_t cross)
{
  _main_align = main;
  _cross_align = cross;
}

ColumnLayout::~ColumnLayout()
{
}

void ColumnLayout::SetGap(int gap)
{
  _gap = gap;
}

int ColumnLayout::GetGap()
{
  return _gap;
}

void ColumnLayout::SetMainAxisAlign(jmainaxis_align_t align)
{
  _main_align = align;
}

jmainaxis_align_t ColumnLayout::GetMainAxisAlign()
{
  return _main_align;
}

void ColumnLayout::SetCrossAxisAlign(jcrossaxis_align_t align)
{
  _cross_align = align;
}

jcrossaxis_align_t ColumnLayout::GetCrossAxisAlign()
{
  return _cross_align;
}

jpoint_t<int> ColumnLayout::GetMinimumLayoutSize(Container *parent)
{
  jpoint_t<int> t = {0, 0};

  return t;
}

jpoint_t<int> ColumnLayout::GetMaximumLayoutSize(Container *parent)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> ColumnLayout::GetPreferredLayoutSize(Container *target) 
{
  if (target == nullptr) {
    jpoint_t<int> t = {0, 0};

    return t;
  }

  jinsets_t insets = target->GetInsets();
  jpoint_t<int> t = {0, 0};

  int members = target->GetComponentCount();

  for (int i=0; i<members; i++) {
    Component *cmp = target->GetComponents()[i];

    jpoint_t point = cmp->GetLocation();
    jpoint_t<int> size = cmp->GetSize();

    t.x = std::max(t.x, point.x+size.x+(insets.left+insets.right));
    t.y = std::max(t.y, point.y+size.y+(insets.top+insets.bottom));
  }

  return t;
}

void ColumnLayout::DoLayout(Container *target)
{
  if (target == nullptr) {
    return;
  }

  jpoint_t<int> 
    size = target->GetSize();

  if (size.x <= 0 || size.y <= 0) {
    return;
  }

  jinsets_t 
    insets = target->GetInsets();
  jpoint_t<int> 
    pos {insets.left, insets.top};
  int
    maxx = size.x - insets.left - insets.right;

  if (target->IsScrollableY() == true) {
    jtheme_t theme = target->GetTheme();

    maxx = maxx - theme.scroll.size.x - theme.scroll.padding.x;
  }

  for (auto cmp : target->GetComponents()) {
    jpoint_t cpoint = cmp->GetLocation();
    jpoint_t<int> csize = cmp->GetPreferredSize();
    
    cpoint.x = 0;

    if (csize.x > maxx) {
      csize.x = maxx;
    }

    if (_cross_align == jcrossaxis_align_t::Start) {
    } else if (_cross_align == jcrossaxis_align_t::End) {
      if (csize.x < maxx) {
        cpoint.x = maxx - csize.x;
      }
    } else if (_cross_align == jcrossaxis_align_t::Stretch) {
      csize.x = maxx;
    }

    cmp->SetBounds({cpoint.x + pos.x, pos.y, csize.x, csize.y});

    pos.y = pos.y + csize.y + _gap;
  }

  pos.y = pos.y - _gap;

  if (_main_align == jmainaxis_align_t::Start) {
    // do nothing;
  } else if (_main_align == jmainaxis_align_t::Center) {
    int offset = (size.y - pos.y)/2;

    if (offset > 0) {
      for (auto cmp : target->GetComponents()) {
        cmp->Move(0, offset);
      }
    }
  } else if (_main_align == jmainaxis_align_t::End) {
    int offset = size.y - pos.y;

    if (offset > 0) {
      for (auto cmp : target->GetComponents()) {
        cmp->Move(0, offset);
      }
    }
  } else if (_main_align == jmainaxis_align_t::SpaceAround) {
    int count = target->GetComponents().size();

    if (count > 1) {
      float offset = (size.y - pos.y)/(static_cast<float>(count));

      count = 0;

      for (auto cmp : target->GetComponents()) {
        cmp->Move(0, offset/2.0f + offset*count++);
      }
    }
  } else if (_main_align == jmainaxis_align_t::SpaceBetween) {
    int count = target->GetComponents().size();

    if (count > 1) {
      float offset = (size.y - pos.y)/(static_cast<float>(count) - 1);

      count = 0;

      for (auto cmp : target->GetComponents()) {
        cmp->Move(0, offset*count++);
      }
    }
  } else if (_main_align == jmainaxis_align_t::SpaceEvenly) {
    int count = target->GetComponents().size();

    if (count > 1) {
      float offset = (size.y - pos.y)/(static_cast<float>(count) + 1);

      count = 1;

      for (auto cmp : target->GetComponents()) {
        cmp->Move(0, offset*count++);
      }
    }
  }
}

}

