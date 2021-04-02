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
#include "jcanvas/widgets/jbeveledrectangleborder.h"
#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

BeveledRectangleBorder::BeveledRectangleBorder(std::size_t size, jcolor_t<float> color):
  RectangleBorder(size, color)
{
}

BeveledRectangleBorder::~BeveledRectangleBorder()
{
}

void BeveledRectangleBorder::SetCornerSize(jpoint_t<int> size)
{
  _corner_size = size;
}

jpoint_t<int> BeveledRectangleBorder::GetCornerSize()
{
  return _corner_size;
}

void BeveledRectangleBorder::SetCorners(jrect_corner_t corners)
{
  _corners = corners;
}

jrect_corner_t BeveledRectangleBorder::GetCorners()
{
  return _corners;
}

void BeveledRectangleBorder::Paint(Component *cmp, Graphics *g)
{
  if (cmp == nullptr) {
    return;
  }

  const jpen_t oldPen = g->GetPen();
  jpen_t pen = g->GetPen();

  pen.size = -GetSize();

  g->SetPen(pen);

  g->SetCompositeFlags(jcomposite_flags_t::SrcOver);
  g->SetColor(GetColor());
  g->DrawBevelRectangle({0, 0, cmp->GetSize()}, _corner_size.x, _corner_size.y, _corners);
  g->SetCompositeFlags(jcomposite_flags_t::Src);
  
  g->SetPen(oldPen);
}

}
