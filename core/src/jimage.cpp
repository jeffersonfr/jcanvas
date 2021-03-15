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
  _blitting = jblitting_t::Nearest;
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

void Image::SetBlittingFlags(jblitting_t flags)
{
  _blitting = flags;
}

jblitting_t Image::GetBlittingFlags()
{
  return _blitting;
}

std::shared_ptr<Image> Image::Flip(jflip_t mode)
{
  return nullptr;
}
    
std::shared_ptr<Image> Image::Shear(jpoint_t<float> size)
{
  return nullptr;
}

std::shared_ptr<Image> Image::Rotate(double radians, bool resize)
{
  return nullptr;
}

std::shared_ptr<Image> Image::Scale(jpoint_t<int> size)
{
  return nullptr;
}

std::shared_ptr<Image> Image::Crop(jrect_t<int> rect)
{
  return nullptr;
}

std::shared_ptr<Image> Image::Blend(double alpha)
{
  return nullptr;
}

std::shared_ptr<Image> Image::Colorize(jcolor_t<float> color)
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
    
std::shared_ptr<Image> Image::Clone()
{
  return nullptr;
}

}

