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
#include "jcanvas/core/jindexedimage.h"
#include "jcanvas/core/jhslcolorspace.h"
#include "jcanvas/core/jenum.h"

#include <string.h>
#include <math.h>

namespace jcanvas {

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint8_t *data, jpoint_t<int> size):
  Image(jpixelformat_t::Unknown, size)
{
  _palette = new uint32_t[palette_size];
  _palette_size = palette_size;
  
  memcpy(_palette, palette, palette_size*sizeof(uint32_t));
  
  int 
    length = size.x*size.y;

  _data = new uint8_t[length];
  
  memcpy(_data, data, length*sizeof(uint8_t));
}

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint32_t *argb, jpoint_t<int> size):
  Image(jpixelformat_t::Unknown, size)
{
  _palette = new uint32_t[palette_size];
  _palette_size = palette_size;
  
  memcpy(_palette, palette, palette_size*sizeof(uint32_t));
  
  int 
    length = size.x*size.y;

  _data = new uint8_t[length];

  for (int i=0; i<length; i++) {
    _data[i] = 0;

    for (int j=0; j<_palette_size; j++) {
      if (argb[i] == _palette[j]) {
        _data[i] = j;

        break;
      }
    }
  }
}

IndexedImage::~IndexedImage()
{
  if ((void *)_palette != nullptr) {
    delete [] _palette;
    _palette = nullptr;
  }

  if ((void *)_data != nullptr) {
    delete [] _data;
    _data = nullptr;
  }
}

std::shared_ptr<IndexedImage> IndexedImage::Pack(std::shared_ptr<Image> image)
{
  std::shared_ptr<IndexedImage> packed;

  if (image != nullptr) {
    if (image->GetGraphics() != nullptr) {
      jpoint_t<int> 
        size = image->GetSize();
      uint32_t 
        rgb[size.x*size.y];

      image->GetRGBArray(rgb, {0, 0, size.x, size.y});

      packed = Pack(rgb, size);
    }
  }

  return packed;
}

std::shared_ptr<IndexedImage> IndexedImage::Pack(uint32_t *rgb, jpoint_t<int> size)
{
  if ((void *)rgb == nullptr) {
    return nullptr;
  }

  uint32_t 
    palette[256];
  int 
    length = size.x*size.y,
    palette_location = 0;

  for (int i=0; i<length; i++) {
    uint32_t
      current = rgb[i];
    bool
      flag = false;

    for (int j=0; j<palette_location; j++) {
      if (palette[j] == current) {
        flag = true;

        break;
      }
    }
    
    if (flag == false) {
      palette[palette_location++] = current;

      if (palette_location >= 256) {
        throw std::runtime_error("IndexedImage cannot support palettes with more than 256 colors");
      }
    }
  }

  return std::make_shared<IndexedImage>(palette, palette_location, rgb, size);
}

std::shared_ptr<Image> IndexedImage::Flip(jflip_t t)
{
  jpoint_t<int> 
    size = GetSize();
  uint8_t 
    *data = new uint8_t[size.x*size.y];
  
  if (jenum_t<jflip_t>{t}.And(jflip_t::Horizontal)) {
    for (int j=0; j<size.y; j++) {
      for (int i=0; i<size.x; i++) {
        int 
          index = j*size.x,
          pixel = data[index+i];

        data[index+i] = _data[index+size.x-i-1];
        _data[index+size.x-i-1] = pixel;
      }
    }
  }

  if (jenum_t<jflip_t>{t}.And(jflip_t::Vertical)) {
    int 
      offset = (size.y-1)*size.x;

    for (int i=0; i<size.x; i++) {
      for (int j=0; j<size.y; j++) {
        int 
          index = j*size.x + i,
          pixel = data[index];

        data[index] = _data[offset-index];
        _data[offset-index] = pixel;
      }
    }
  }

  std::shared_ptr<Image> image = std::make_shared<IndexedImage>(_palette, _palette_size, data, size);

  delete [] data;

  return image;
}

std::shared_ptr<Image> IndexedImage::Rotate(float radians, bool resize)
{
  jpoint_t<int> 
    isize = GetSize();
  float 
    angle = fmod(radians, 2*M_PI);
  int 
    precision = 1024,
    iw = isize.x,
    ih = isize.y,
    sinTheta = (int)(precision*sin(angle)),
    cosTheta = (int)(precision*cos(angle));

  if (resize == true) {
    iw = (abs(isize.x*cosTheta)+abs(isize.y*sinTheta))/precision;
    ih = (abs(isize.x*sinTheta)+abs(isize.y*cosTheta))/precision;
  }

  uint8_t 
    *data = new uint8_t[iw*ih];
  int 
    sxc = isize.x/2,
    syc = isize.y/2,
    dxc = iw/2,
    dyc = ih/2,
    xo,
    yo,
    t1,
    t2;

  for (int j=0; j<ih; j++) {
    t1 = (j - dyc)*sinTheta;
    t2 = (j - dyc)*cosTheta;

    for (int i=0; i<iw; i++) {
      xo = ((i - dxc)*cosTheta - t1)/precision;
      yo = ((i - dxc)*sinTheta + t2)/precision;

      if (xo >= -sxc && xo <= sxc && yo >= -syc && yo <= syc) {
        data[j*iw + i] = _data[(yo + syc)*isize.x + (xo + sxc)];
      } else {
        data[j*iw+i] = 0;
      }
    }
  }

  std::shared_ptr<Image> image = std::make_shared<IndexedImage>(_palette, _palette_size, data, jpoint_t<int>{iw, ih});

  delete[] data;

  return image;
}

std::shared_ptr<Image> IndexedImage::Scale(jpoint_t<int> size)
{
  if (size.x <= 0 || size.y <= 0) {
    return nullptr;
  }

  jpoint_t<int> 
    isize = GetSize();
  float 
    xRatio = isize.x/(float)size.x,
    yRatio = isize.y/(float)size.y;
  uint8_t 
    *data = new uint8_t[size.x*size.y];

  for(int y=0; y<size.y; y++) {
    float 
      src = ((int)(y * yRatio)) * isize.x;
    int 
      dst = y * size.x;

    for (int x=0; x<size.x; x++) {
      data[dst + x] = _data[(int)src];
    
      src = src + xRatio;
    }
  }

  std::shared_ptr<Image> image = std::make_shared<IndexedImage>(_palette, _palette_size, data, size);

  delete [] data;

  return image;
}

std::shared_ptr<Image> IndexedImage::Crop(jrect_t<int> rect)
{
  if (rect.size.x <= 0 || rect.size.y <= 0) {
    return nullptr;
  }

  int 
    length = rect.size.x*rect.size.y;
  uint8_t 
    *data = new uint8_t[length];

  for (int i=0; i<length; i++) {
    data[i] = _data[rect.point.x + i%rect.size.x + ((rect.point.y + i/rect.size.x) * _size.x)];
  }

  std::shared_ptr<Image> image = std::make_shared<IndexedImage>(_palette, _palette_size, data, rect.size);

  delete [] data;

  return image;
}

std::shared_ptr<Image> IndexedImage::Blend(float alpha)
{
  return nullptr;
}

std::shared_ptr<Image> IndexedImage::Colorize(jcolor_t<float> color)
{
  jpoint_t<int> 
    size = GetSize();
  uint32_t 
    palette[_palette_size];
  jvector_t<3, float>
    hsb = color.ToHSB();

  HSLColorSpace 
    hsl(hsb[0], hsb[1], 0.0);

  for (int i=0; i<_palette_size; i++) {
    jcolor_t<float>
      color(_palette[i]);
    int 
      r = color[2],
      g = color[1],
      b = color[0];

    hsl.GetRGB(&r, &g, &b);

    palette[i] = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
  }

  return std::shared_ptr<IndexedImage>(new IndexedImage(palette, _palette_size, _data, size));
}

uint8_t * IndexedImage::LockData()
{
  return (uint8_t *)_data;
}

void IndexedImage::UnlockData()
{
}

void IndexedImage::GetRGBArray(uint32_t *rgb, jrect_t<int> rect)
{
  jpoint_t<int> 
    size = GetSize();

  if ((rect.point.x + rect.size.x) > size.x || (rect.point.y + rect.size.y) > size.y) {
    throw std::range_error("The limits are out of bounds");
  }

  if (rgb == nullptr) {
    throw std::invalid_argument("Destination buffer must be valid");
  }

  for (int j=0; j<rect.size.y; j++) {
    int 
      data = (rect.point.y + j)*size.x + rect.point.x,
      line = j*rect.size.x;

    for (int i=0; i<rect.size.x; i++) {
      rgb[line + i] = _palette[_data[data + i]];
    }
  }
}
    
void IndexedImage::GetPalette(uint32_t **palette, int *size)
{
  if (palette != nullptr) {
    *palette = _palette;
  }

  if (size != nullptr) {
    *size = _palette_size;
  }
}

void IndexedImage::SetPalette(uint32_t *palette, int palette_size)
{
  _palette = new uint32_t[palette_size];

  _palette_size = palette_size;
  
  memcpy(_palette, palette, palette_size*sizeof(uint32_t));
}

std::shared_ptr<Image> IndexedImage::Clone()
{
  return std::make_shared<IndexedImage>(_palette, _palette_size, _data, _size);
}

}

