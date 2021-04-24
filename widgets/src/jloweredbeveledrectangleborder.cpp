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
#include "jcanvas/widgets/jloweredbeveledrectangleborder.h"
#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

LoweredBeveledRectangleBorder::LoweredBeveledRectangleBorder(std::size_t size, jcolor_t<float> color):
  RectangleBorder(size, color)
{
}

LoweredBeveledRectangleBorder::~LoweredBeveledRectangleBorder()
{
}

void LoweredBeveledRectangleBorder::Paint(Component *cmp, Graphics *g)
{
  if (cmp == nullptr) {
    return;
  }

  const jpen_t oldPen = g->GetPen();
  jpen_t pen = g->GetPen();

  pen.size = 1;

  g->SetPen(pen);

  g->SetCompositeFlags(jcomposite_flags_t::SrcOver);

  jpoint_t<int> size = cmp->GetSize();
  jcolor_t<float> color = GetColor();
  std::size_t width = GetSize();
  int 
    step = 0x20,
    dr = color[2],
    dg = color[1],
    db = color[0];
    // da = color[3];

  for (int i=0; i<(int)width && i<size.x && i<size.y; i++) {
    g->SetColor({dr - step, dg - step, db - step});
    g->DrawLine({i, i}, {size.x - i, i}); //cima
    g->SetColor({dr + step, dg + step, db + step});
    g->DrawLine({i, size.y - i}, {size.x - i, size.y - i}); //baixo
  }

  for (int i=0; i<(int)width && i<size.x && i<size.y; i++) {
    g->SetColor({dr - step, dg - step, db - step});
    g->DrawLine({i, i}, {i, size.y - i}); //esquerda
    g->SetColor({dr + step, dg + step, db + step});
    g->DrawLine({size.x - i, i}, {size.x - i, size.y - i}); //direita
  }

  g->SetCompositeFlags(jcomposite_flags_t::Src);
  g->SetPen(oldPen);
}

}
