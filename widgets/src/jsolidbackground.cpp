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
#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/widgets/jroundedrectangleborder.h"
#include "jcanvas/widgets/jbeveledrectangleborder.h"
#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

SolidBackground::SolidBackground()
{
}

SolidBackground::~SolidBackground()
{
}

void SolidBackground::Paint(Component *cmp, Graphics *g)
{
  if (cmp == nullptr) {
    return;
  }

  jpoint_t<int> size = cmp->GetSize();
  jtheme_t theme = cmp->GetTheme();

  g->SetCompositeFlags(jcomposite_flags_t::SrcOver);

  if (static_cast<uint32_t>(theme.bg.normal) & 0xff000000) {
    g->SetColor(theme.bg.normal);
    g->FillRectangle({0, 0, size});

    std::shared_ptr<Border> border = cmp->GetBorder();

    if (std::dynamic_pointer_cast<RectangleBorder>(border) != nullptr) {
      g->FillRectangle({0, 0, size});
    } else if (std::dynamic_pointer_cast<RoundedRectangleBorder>(border) != nullptr) {
      g->FillRoundRectangle({0, 0, size});
    } else if (std::dynamic_pointer_cast<BeveledRectangleBorder>(border) != nullptr) {
      g->FillBevelRectangle({0, 0, size});
    }
  }

  g->SetCompositeFlags(jcomposite_flags_t::Src);
}

}
