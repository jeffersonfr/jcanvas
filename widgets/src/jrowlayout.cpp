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
#include "jcanvas/widgets/jrowlayout.h"

namespace jcanvas {

RowLayout::RowLayout(jmainaxis_align_t main, jcrossaxis_align_t cross):
  ColumnLayout(main, cross)
{
}

RowLayout::~RowLayout()
{
}

void RowLayout::DoLayout(Container *target)
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
    maxy = size.y - insets.top - insets.bottom;

  for (auto cmp : target->GetComponents()) {
    jpoint_t cpoint = cmp->GetLocation();
    jpoint_t<int> csize = cmp->GetPreferredSize();
    
    if (csize.y > maxy) {
      csize.y = maxy;
      cpoint.y = 0;
    }

    if (_cross_align == jcrossaxis_align_t::Start) {
      // do nothing;
    } else if (_cross_align == jcrossaxis_align_t::End) {
      if (csize.y < maxy) {
        cpoint.y = maxy - csize.y;
      }
    } else if (_cross_align == jcrossaxis_align_t::Stretch) {
      csize.y = maxy;
    }

    cmp->SetBounds({pos.x, cpoint.y + pos.y, csize.x, csize.y});

    pos.x = pos.x + csize.x + _gap;
  }

  pos.x = pos.x - _gap;

  if (_main_align == jmainaxis_align_t::Start) {
    // do nothing;
  } else if (_main_align == jmainaxis_align_t::Center) {
    int offset = (size.x - pos.x)/2;

    if (offset > 0) {
      for (auto cmp : target->GetComponents()) {
        cmp->Move(offset, 0);
      }
    }
  } else if (_main_align == jmainaxis_align_t::End) {
    int offset = size.x - pos.x;

    if (offset > 0) {
      for (auto cmp : target->GetComponents()) {
        cmp->Move(offset, 0);
      }
    }
  } else if (_main_align == jmainaxis_align_t::SpaceAround) {
    int count = target->GetComponents().size();

    if (count > 1) {
      float offset = (size.x - pos.x)/(static_cast<float>(count));

      count = 0;

      for (auto cmp : target->GetComponents()) {
        cmp->Move(offset/2.0f + offset*count++, 0);
      }
    }
  } else if (_main_align == jmainaxis_align_t::SpaceBetween) {
    int count = target->GetComponents().size();

    if (count > 1) {
      float offset = (size.x - pos.x)/(static_cast<float>(count) - 1);

      count = 0;

      for (auto cmp : target->GetComponents()) {
        cmp->Move(offset*count++, 0);
      }
    }
  } else if (_main_align == jmainaxis_align_t::SpaceEvenly) {
    int count = target->GetComponents().size();

    if (count > 1) {
      float offset = (size.x - pos.x)/(static_cast<float>(count) + 1);

      count = 1;

      for (auto cmp : target->GetComponents()) {
        cmp->Move(offset*count++, 0);
      }
    }
  }
}

}

