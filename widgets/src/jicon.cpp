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
#include "jcanvas/widgets/jicon.h"

namespace jcanvas {

Icon::Icon(Image *image):
  Component()
{
  _align = jalign_t::Center;
  _image = image;
}

Icon::~Icon()
{
}

void Icon::SetImage(Image *image)
{
  _image = image;

  Repaint();
}

Image * Icon::GetImage()
{
  return _image;
}

void Icon::SetAlign(jalign_t align)
{
  _align = align;

  Repaint();
}

jalign_t Icon::GetAlign()
{
  return _align;
}

void Icon::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();
  jinsets_t<int>
    padding = GetPadding();

  bounds = padding.bounds(jrect_t<int>{{0, 0}, bounds.size}).align(_align, jrect_t<int>{{0, 0}, _image->GetSize()});

  if (_image != nullptr) {
    g->DrawImage(_image, bounds);
  }
}

}
