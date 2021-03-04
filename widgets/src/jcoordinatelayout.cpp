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
#include "jcanvas/widgets/jcoordinatelayout.h"

namespace jcanvas {

CoordinateLayout::CoordinateLayout(int width, int height, jcoordinatelayout_orientation_t type)
{
  if (width <= 0 || height <= 0) {
    throw std::out_of_range("CoordinateLayout cannot work with limits lower than zero");
  }

  _size.x = width;
  _size.y = height;
  _type = type;
}

CoordinateLayout::~CoordinateLayout()
{
}

void CoordinateLayout::SetSize(jpoint_t<int> size)
{
  _size = size;
}

void CoordinateLayout::SetType(jcoordinatelayout_orientation_t type)
{
  _type = type;
}

jpoint_t<int> CoordinateLayout::GetSize()
{
  return _size;
}

jcoordinatelayout_orientation_t CoordinateLayout::GetType()
{
  return _type;
}

jpoint_t<int> CoordinateLayout::GetMinimumLayoutSize(Container *parent)
{
  jpoint_t<int> t = {0, 0};

  return t;
}

jpoint_t<int> CoordinateLayout::GetMaximumLayoutSize(Container *parent)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> CoordinateLayout::GetPreferredLayoutSize(Container *target) 
{
  if ((void *)target == nullptr) {
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

void CoordinateLayout::DoLayout(Container *target)
{
  if ((void *)target == nullptr) {
    return;
  }

  jpoint_t<int> 
    tsize = target->GetSize();

  if (tsize.x <= 0 || tsize.y <= 0) {
    return;
  }

  double 
    sx = 1.0,
    sy = 1.0;
  int 
    nmembers = target->GetComponentCount();

  if (_size.x < 0) {
    _size.x = tsize.x;
  }

  if (_size.y < 0) {
    _size.y = tsize.y;
  }

  if (jenum_t{_type}.And(jcoordinatelayout_orientation_t::Horizontal)) {
    sx = (double)tsize.x/_size.x;
  }

  if (jenum_t{_type}.And(jcoordinatelayout_orientation_t::Vertical)) {
    sy = (double)tsize.y/_size.y;
  }

  for (int i=0; i<nmembers; i++) {
    Component *c = target->GetComponents()[i];

    jpoint_t point = c->GetLocation();
    jpoint_t<int> size = c->GetPreferredSize();

    // c->SetLocation((int)(point.x*sx), (int)(point.y*sy));
    c->SetBounds((int)(point.x*sx), (int)(point.y*sy), size.x, size.y);
  }

  _size.x = tsize.x;
  _size.y = tsize.y;
}

}

