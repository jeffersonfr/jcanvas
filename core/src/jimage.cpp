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
#include "jcanvas/core/jimage.h"

namespace jcanvas {

Image::Image(jpixelformat_t pixelformat, jpoint_t<int> size)
{
  _pixelformat = pixelformat;
  _interpolation_method = JIM_NEAREST;
  _size = size;
}

Image::~Image()
{
}

Graphics * Image::GetGraphics()
{
  return nullptr;
}

jpixelformat_t Image::GetPixelFormat()
{
  return _pixelformat;
}

jpoint_t<int> Image::GetSize()
{
  return _size;
}

void Image::SetInterpolationMethod(jinterpolation_method_t method)
{
  _interpolation_method = method;
}

jinterpolation_method_t Image::GetInterpolationMethod()
{
  return _interpolation_method;
}

Image * Image::Flip(jflip_flags_t mode)
{
  return nullptr;
}
    
Image * Image::Shear(jpoint_t<float> size)
{
  return nullptr;
}

Image * Image::Rotate(double radians, bool resize)
{
  return nullptr;
}

Image * Image::Scale(jpoint_t<int> size)
{
  return nullptr;
}

Image * Image::Crop(jrect_t<int> rect)
{
  return nullptr;
}

Image * Image::Blend(double alpha)
{
  return nullptr;
}

Image * Image::Colorize(jcolor_t<float> color)
{
  return nullptr;
}

uint8_t * Image::LockData()
{
  return nullptr;
}

void Image::UnlockData()
{
}

void Image::GetRGBArray(uint32_t *rgb, jrect_t<int> rect)
{
}
    
Image * Image::Clone()
{
  return nullptr;
}

}

