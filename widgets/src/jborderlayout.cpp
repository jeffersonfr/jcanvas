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
#include "jcanvas/widgets/jborderlayout.h"
#include "jcanvas/widgets/jcontainer.h"

#include <algorithm>

namespace jcanvas {

BorderLayout::BorderLayout(int hgap, int vgap):
  Layout()
{
  _hgap = hgap;
  _vgap = vgap;
  
  north = nullptr;
  west = nullptr;
  east = nullptr;
  south = nullptr;
  center = nullptr;
  firstLine = nullptr;
  lastLine = nullptr;
  firstItem = nullptr;
  lastItem = nullptr;
}

BorderLayout::~BorderLayout()
{
}

int BorderLayout::GetHGap() 
{
  return _hgap;
}

void BorderLayout::SetHGap(int hgap) 
{
  _hgap = hgap;
}

int BorderLayout::GetVGap() 
{
  return _vgap;
}

void BorderLayout::SetVgap(int vgap) 
{
  _vgap = vgap;
}

jpoint_t<int> BorderLayout::GetMinimumLayoutSize(std::shared_ptr<Container> parent)
{
  // WARN:: sync parent
  std::shared_ptr<Component> cmp = nullptr;
  jpoint_t<int> t = {0, 0};
  bool ltr = (parent->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight);
      
  if ((cmp = GetChild(jborderlayout_align_t::East, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetMinimumSize();
      t.x += d.x + _hgap;
      t.y = (std::max)(d.y, t.y);
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::West, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetMinimumSize();
      t.x += d.x + _hgap;
      t.y = (std::max)(d.y, t.y);
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::Center, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetMinimumSize();
      t.x += d.x;
      t.y = (std::max)(d.y, t.y);
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::North, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetMinimumSize();
      t.x = (std::max)(d.x, t.x);
      t.y += d.y + _vgap;
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::South, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetMinimumSize();
      t.x = (std::max)(d.x, t.x);
      t.y += d.y + _vgap;
  }

  jinsets_t insets = parent->GetInsets();

  t.x += insets.left + insets.right;
  t.y += insets.top + insets.bottom;

  return t;
}

jpoint_t<int> BorderLayout::GetMaximumLayoutSize(std::shared_ptr<Container> parent)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> BorderLayout::GetPreferredLayoutSize(std::shared_ptr<Container> parent)
{
  // WARN:: sync parent
  std::shared_ptr<Component> cmp = nullptr;
  jpoint_t<int> t = {0, 0};
  bool ltr = (parent->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight);
      
  if ((cmp = GetChild(jborderlayout_align_t::East, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetPreferredSize();
      t.x += d.x + _hgap;
      t.y = (std::max)(d.y, t.y);
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::West, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetPreferredSize();
      t.x += d.x + _hgap;
      t.y = (std::max)(d.y, t.y);
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::Center, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetPreferredSize();
      t.x += d.x;
      t.y = (std::max)(d.y, t.y);
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::North, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetPreferredSize();
      t.x = (std::max)(d.x, t.x);
      t.y += d.y + _vgap;
  }
  
  if ((cmp = GetChild(jborderlayout_align_t::South, ltr)) != nullptr) {
      jpoint_t<int> d = cmp->GetPreferredSize();
      t.x = (std::max)(d.x, t.x);
      t.y += d.y + _vgap;
  }

  jinsets_t insets = parent->GetInsets();

  t.x += insets.left + insets.right;
  t.y += insets.top + insets.bottom;

  return t;
}

void BorderLayout::DoLayout(std::shared_ptr<Container> target)
{
  // WARN:: sync with jframe
  std::shared_ptr<Component>
    c;
  jinsets_t 
    insets = target->GetInsets();
  jpoint_t<int> 
    psize;
  int 
    top = insets.top,
    bottom = target->GetSize().y - insets.bottom,
    left = insets.left,
    right = target->GetSize().x - insets.right;
  bool 
    ltr = (target->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight || target->GetComponentOrientation() == jcomponent_orientation_t::RightToLeft);

  if ((c = GetChild(jborderlayout_align_t::North, ltr)) != nullptr) {
    c->SetSize(right - left, c->GetSize().y);

    psize = c->GetPreferredSize();

    c->SetBounds(left, top, right - left, psize.y);
    top += psize.y + _vgap;
  }

  if ((c = GetChild(jborderlayout_align_t::South, ltr)) != nullptr) {
    c->SetSize(right - left, c->GetSize().y);

    psize = c->GetPreferredSize();

    c->SetBounds(left, bottom - psize.y, right - left, psize.y);
    bottom -= psize.y + _vgap;
  }

  if ((c = GetChild(jborderlayout_align_t::East, ltr)) != nullptr) {
    c->SetSize(c->GetSize().x, bottom - top);

    psize = c->GetPreferredSize();

    c->SetBounds(right - psize.x, top, psize.x, bottom - top);
    right -= psize.x + _hgap;
  }

  if ((c = GetChild(jborderlayout_align_t::West, ltr)) != nullptr) {
    c->SetSize(c->GetSize().x, bottom - top);

    psize = c->GetPreferredSize();

    c->SetBounds(left, top, psize.x, bottom - top);
    left += psize.x + _hgap;
  }

  if ((c = GetChild(jborderlayout_align_t::Center, ltr)) != nullptr) {
    c->SetBounds(left, top, right - left, bottom - top);
  }
}

void BorderLayout::AddLayoutComponent(std::shared_ptr<Component> c, jborderlayout_align_t align) 
{
  // WARN:: sync with jframe
  if (align == jborderlayout_align_t::Center) {
    center = c;
  } else if (align == jborderlayout_align_t::North) {
    north = c;
  } else if (align == jborderlayout_align_t::South) {
    south = c;
  } else if (align == jborderlayout_align_t::East) {
    east = c;
  } else if (align == jborderlayout_align_t::West) {
    west = c;
  } else if (align == jborderlayout_align_t::PageStart) {
    firstLine = c;
  } else if (align == jborderlayout_align_t::PageEnd) {
    lastLine = c;
  } else if (align == jborderlayout_align_t::LineStart) {
    firstItem = c;
  } else if (align == jborderlayout_align_t::LineEnd) {
    lastItem = c;
  }
}

void BorderLayout::RemoveLayoutComponent(std::shared_ptr<Component> c) 
{
  // WARN:: sync with jframe
  if (c == center) {
    center = nullptr;
  } else if (c == north) {
    north = nullptr;
  } else if (c == south) {
    south = nullptr;
  } else if (c == east) {
    east = nullptr;
  } else if (c == west) {
    west = nullptr;
  }

  if (c == firstLine) {
    firstLine = nullptr;
  } else if (c == lastLine) {
    lastLine = nullptr;
  } else if (c == firstItem) {
    firstItem = nullptr;
  } else if (c == lastItem) {
    lastItem = nullptr;
  }
}

void BorderLayout::RemoveLayoutComponents() 
{
  // WARN:: sync with jframe
  center = nullptr;
  north = nullptr;
  south = nullptr;
  east = nullptr;
  west = nullptr;
  firstLine = nullptr;
  lastLine = nullptr;
  firstItem = nullptr;
  lastItem = nullptr;
}

std::shared_ptr<Component> BorderLayout::GetLayoutComponent(jborderlayout_align_t align) 
{
  if (align == jborderlayout_align_t::Center) {
    return center;
  } else if (align == jborderlayout_align_t::North) {
    return north;
  } else if (align == jborderlayout_align_t::South) {
    return south;
  } else if (align == jborderlayout_align_t::West) {
    return west;
  } else if (align == jborderlayout_align_t::East) {
    return east;
  } else if (align == jborderlayout_align_t::PageStart) {
    return firstLine;
  } else if (align == jborderlayout_align_t::PageEnd) {
    return lastLine;
  } else if (align == jborderlayout_align_t::LineStart) {
    return firstItem;
  } else if (align == jborderlayout_align_t::LineEnd) {
    return lastItem;
  }

  return nullptr;
}

std::shared_ptr<Component> BorderLayout::GetLayoutComponent(std::shared_ptr<Container> target, jborderlayout_align_t align) 
{
  std::shared_ptr<Component> result;
  bool ltr = (target->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight || target->GetComponentOrientation() == jcomponent_orientation_t::RightToLeft);

  if (align == jborderlayout_align_t::North) {
    result = (firstLine != nullptr) ? firstLine : north;
  } else if (align == jborderlayout_align_t::South) {
    result = (lastLine != nullptr) ? lastLine : south;
  } else if (align == jborderlayout_align_t::West) {
    result = ltr ? firstItem : lastItem;
    
    if (result == nullptr) {
      result = west;
    }
  } else if (align == jborderlayout_align_t::East) {
    result = ltr ? lastItem : firstItem;
    
    if (result == nullptr) {
      result = east;
    }
  } else if (align == jborderlayout_align_t::Center) {
    result = center;
  }

  return result;
}

jborderlayout_align_t BorderLayout::GetConstraints(std::shared_ptr<Component> c) 
{
  if (c == nullptr){
    return jborderlayout_align_t::Unknown;
  }

  if (c == center) {
    return jborderlayout_align_t::Center;
  } else if (c == north) {
    return jborderlayout_align_t::North;
  } else if (c == south) {
    return jborderlayout_align_t::South;
  } else if (c == west) {
    return jborderlayout_align_t::West;
  } else if (c == east) {
    return jborderlayout_align_t::East;
  } else if (c == firstLine) {
    return jborderlayout_align_t::PageStart;
  } else if (c == lastLine) {
    return jborderlayout_align_t::PageEnd;
  } else if (c == firstItem) {
    return jborderlayout_align_t::LineStart;
  } else if (c == lastItem) {
    return jborderlayout_align_t::LineEnd;
  }

  return jborderlayout_align_t::Unknown;
}

std::shared_ptr<Component> BorderLayout::GetChild(jborderlayout_align_t key, bool ltr) 
{
  std::shared_ptr<Component> result;

  if (key == jborderlayout_align_t::North) {
    result = (firstLine != nullptr) ? firstLine : north;
  } else if (key == jborderlayout_align_t::South) {
    result = (lastLine != nullptr) ? lastLine : south;
  } else if (key == jborderlayout_align_t::West) {
    result = ltr ? firstItem : lastItem;
    if (result == nullptr) {
      result = west;
    }
  } else if (key == jborderlayout_align_t::East) {
    result = ltr ? lastItem : firstItem;
    if (result == nullptr) {
      result = east;
    }
  } else if (key == jborderlayout_align_t::Center) {
    result = center;
  }

  if (result != nullptr && !result->IsVisible()) {
    result = nullptr;
  }

  return result;
}

}

