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
#include "include/pngimage.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <jpeglib.h>

namespace jcanvas {

typedef struct {
	uint32_t last;
	uint8_t *data;
} cairocks_read_png_closure_t;

static cairo_status_t cairocks_read_png(void *closure, uint8_t *data, uint32_t length) 
{
	cairocks_read_png_closure_t *c = (cairocks_read_png_closure_t *)(closure);

	memcpy(data, c->data + c->last, length);

	c->last = c->last + length;

	return CAIRO_STATUS_SUCCESS;
}

cairo_surface_t * create_png_surface_from_data(uint8_t *data, int size) 
{
	cairocks_read_png_closure_t closure;

	closure.last = 0;
	closure.data = data;

	cairo_surface_t *surface;
	
	surface = cairo_image_surface_create_from_png_stream(cairocks_read_png, &closure);

	if (surface == nullptr) {
		return nullptr;
	}

	cairo_format_t format = cairo_image_surface_get_format(surface);
	int sw = cairo_image_surface_get_width(surface);
	int sh = cairo_image_surface_get_height(surface);

	if (format == CAIRO_FORMAT_INVALID || sw <= 0 || sh <= 0) {
		cairo_surface_destroy(surface);

		return nullptr;
	}

	return surface;
}

}

