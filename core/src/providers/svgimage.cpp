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
#include "include/svgimage.h"

#include <string>

#include <librsvg-2.0/librsvg/rsvg.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace jcanvas {

void svg_data_destroy(void *data)
{
  std::string *ptr = (std::string *)data;

  delete ptr;
}

cairo_surface_t * create_svg_surface_from_data(uint8_t *data, int size, int width, int height) 
{
  if (memcmp(data, "<?xml", 5) != 0) {
    return nullptr;
  }

  // rsvg_init();

  RsvgHandle 
    *svg = rsvg_handle_new_from_data(data, size, nullptr);
  gdouble
    dsw,
    dsh;

  rsvg_handle_get_intrinsic_size_in_pixels(svg, &dsw, &dsh);

  if (width > 0 and height > 0) {
    dsw = width;
    dsh = height;
  }

  int
    sw = static_cast<int>(dsw),
    sh = static_cast<int>(dsh);

  cairo_surface_t 
    *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

  if (surface == nullptr) {
    return nullptr;
  }

  cairo_t *cr = cairo_create(surface);

  RsvgRectangle 
    rect {0, 0, dsw, dsh};

  rsvg_handle_render_document(svg, cr, &rect, nullptr); 

  cairo_surface_mark_dirty(surface);
  cairo_destroy(cr);

  cairo_surface_set_user_data(surface, nullptr, new std::string((char *)data, size), svg_data_destroy);

  return surface;
}

}

