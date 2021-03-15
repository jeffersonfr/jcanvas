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
#include "jcanvas/widgets/jflatimage.h"

namespace jcanvas {

FlatImage::FlatImage(std::shared_ptr<Image> image):
  Component()
{
  _align = jrect_align_t::Center;
  _image = image;
}

FlatImage::~FlatImage()
{
}

void FlatImage::SetImage(std::shared_ptr<Image> image)
{
  _image = image;
}

std::shared_ptr<Image> FlatImage::GetImage()
{
  return _image;
}

void FlatImage::SetAlign(jrect_align_t align)
{
  _align = align;
}

jrect_align_t FlatImage::GetAlign()
{
  return _align;
}

void FlatImage::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();

  if (_image != nullptr) {
    jrect_t<int> bounds = theme.padding.bounds(jrect_t<int>{{0, 0}, GetBounds().size});

    g->ClipRect(bounds);

    g->DrawImage(_image, bounds.align(_align, jrect_t<int>{{0, 0}, _image->GetSize()}));
  }
}

}
