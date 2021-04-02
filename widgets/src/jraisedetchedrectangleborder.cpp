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
#include "jcanvas/widgets/jraisedetchedrectangleborder.h"
#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

RaisedEtchedRectangleBorder::RaisedEtchedRectangleBorder(std::size_t size, jcolor_t<float> color):
  RectangleBorder(size, color)
{
}

RaisedEtchedRectangleBorder::~RaisedEtchedRectangleBorder()
{
}

void RaisedEtchedRectangleBorder::Paint(Component *cmp, Graphics *g)
{
  if (cmp == nullptr) {
    return;
  }

  const jpen_t oldPen = g->GetPen();
  jpen_t pen = g->GetPen();
  jpoint_t<int> size = cmp->GetSize();
  jcolor_t<float> color = GetColor();
  std::size_t width = GetSize();
  int 
    step = 0x20,
    dr = color[2],
    dg = color[1],
    db = color[0];

  g->SetCompositeFlags(jcomposite_flags_t::SrcOver);
  
  pen.size = -GetSize();

  g->SetPen(pen);

  g->SetColor({dr + step, dg + step, db + step});
  g->DrawRectangle({0, 0, size});
    
  pen.size = -GetSize()/2;

  g->SetPen(pen);

  g->SetColor({dr - step, dg - step, db - step});
  g->DrawRectangle({0, 0, size - jpoint_t<std::size_t>{width, width}/2});

  g->SetCompositeFlags(jcomposite_flags_t::Src);
  g->SetPen(oldPen);
}

}
