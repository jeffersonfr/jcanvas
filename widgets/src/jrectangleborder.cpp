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
#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

RectangleBorder::RectangleBorder(std::size_t size, jcolor_t<float> color)
{
  _size = size;
  _color = color;
}

RectangleBorder::~RectangleBorder()
{
}

void RectangleBorder::SetSize(std::size_t size)
{
  _size = size;
}

std::size_t RectangleBorder::GetSize()
{
  return _size;
}

void RectangleBorder::SetColor(jcolor_t<float> color)
{
  _color = color;
}

jcolor_t<float> RectangleBorder::GetColor()
{
  return _color;
}

void RectangleBorder::Paint(Component *cmp, Graphics *g)
{
  if (cmp == nullptr) {
    return;
  }

  const jpen_t oldPen = g->GetPen();

  jpen_t pen = oldPen;

  pen.size = GetSize();

  g->SetPen(pen);

  g->SetCompositeFlags(jcomposite_flags_t::SrcOver);
  g->SetColor(GetColor());
  g->DrawRectangle({0, 0, cmp->GetSize()});
  g->SetCompositeFlags(jcomposite_flags_t::Src);
  
  g->SetPen(oldPen);
}

}
