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
#include "jcanvas/widgets/jimagebackground.h"
#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

ImageBackground::ImageBackground(std::shared_ptr<Image> image)
{
  _image = image;
}

ImageBackground::~ImageBackground()
{
}

void ImageBackground::SetImage(std::shared_ptr<Image> image)
{
  _image = image;
}

std::shared_ptr<Image> ImageBackground::GetImage()
{
  return _image;
}

void ImageBackground::SetAlign(jrect_align_t align)
{
  _align = align;
}

jrect_align_t ImageBackground::GetAlign()
{
  return _align;
}

void ImageBackground::Paint(Component *cmp, Graphics *g)
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

    if (theme.border.type == jtheme_border_t::style::Round) {
      g->FillRoundRectangle({0, 0, size});
    } else if (theme.border.type == jtheme_border_t::style::Bevel) {
      g->FillBevelRectangle({0, 0, size});
    } else {
      g->FillRectangle({0, 0, size});
    }
  }

  if (_image != nullptr) {
    g->DrawImage(_image, jrect_t<int>{0, 0, size}.align(_align, jrect_t<int>{{0, 0}, _image->GetSize()}));
  }
    
  g->SetCompositeFlags(jcomposite_flags_t::Src);
}

}
