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
#include "include/bmpimage.h"
#include "include/bitmap.h"

#include "../include/jcanvas/core/jgraphics.h"

#include <string>
#include <istream>

namespace jcanvas {

cairo_surface_t * create_bmp_surface_from_stream(std::istream &stream) 
{
	CBitmap bitmap;

	if (bitmap.Load(stream) == false) {
		return nullptr;
	}

	int sw = (int)bitmap.GetWidth();
	int sh = (int)bitmap.GetHeight();
	unsigned int sz = sw*sh;

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
		return nullptr;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == nullptr) {
		return nullptr;
	}

	if (bitmap.GetBits((void *)data, sz, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, true) == false) {
		return nullptr;
	}

	// INFO:: remove alpha channel from bmp ?
	uint32_t k, *ptr = (uint32_t *)data;

	for (int i=0; i<sw; i++) {
		for (int j=0; j<sh/2; j++) {
			k = ptr[(j*sw+i)];
			
			ptr[(j*sw+i)] = ptr[((sh-j-1)*sw+i)];
			ptr[((sh-j-1)*sw+i)] = k;
		}
	}

  /*
	for (int i=0; i<(int)sz; i++) {
		int alpha = data[i*4+3];

		data[i*4+2] = ALPHA_PREMULTIPLY(data[i*4+2], alpha);
		data[i*4+1] = ALPHA_PREMULTIPLY(data[i*4+1], alpha);
		data[i*4+0] = ALPHA_PREMULTIPLY(data[i*4+0], alpha);
	}
  */

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_bmp_surface_from_data(uint8_t *data, int size)
{
  std::string str(reinterpret_cast<char *>(data), size);
  std::istringstream is(str);
	
  cairo_surface_t *surface = nullptr;

	surface = create_bmp_surface_from_stream(is);

	return surface;
}

}

