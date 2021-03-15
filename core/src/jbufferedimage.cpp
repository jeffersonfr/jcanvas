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
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jhslcolorspace.h"

#include <memory>

#ifdef PNG_IMAGE
#include "providers/include/pngimage.h"
#endif

#ifdef BMP_IMAGE
#include "providers/include/bmpimage.h"
#endif

#ifdef GIF_IMAGE
#include "providers/include/gifimage.h"
#endif

#ifdef ICO_IMAGE
#include "providers/include/icoimage.h"
#endif

#ifdef JPG_IMAGE
#include "providers/include/jpgimage.h"
#endif

#ifdef PCX_IMAGE
#include "providers/include/pcximage.h"
#endif

#ifdef PPM_IMAGE
#include "providers/include/ppmimage.h"
#endif

#ifdef TGA_IMAGE
#include "providers/include/tgaimage.h"
#endif

#ifdef XBM_IMAGE
#include "providers/include/xbmimage.h"
#endif

#ifdef XPM_IMAGE
#include "providers/include/xpmimage.h"
#endif

#ifdef HEIF_IMAGE
#include "providers/include/heifimage.h"
#endif

#ifdef WEBP_IMAGE
#include "providers/include/webpimage.h"
#endif

#ifdef JP2000_IMAGE
#include "providers/include/jp2000image.h"
#endif

#ifdef SVG_IMAGE
#include "providers/include/svgimage.h"
#endif

#ifdef TIFF_IMAGE
#include "providers/include/tiffimage.h"
#endif

#ifdef BPG_IMAGE
#include "providers/include/bpgimage.h"
#endif

#ifdef FLIF_IMAGE
#include "providers/include/flifimage.h"
#endif

#ifdef MJPEG_IMAGE
#include "providers/include/mjpegimage.h"
#endif

#include <cairo.h>

#include <string.h>

namespace jcanvas {

static void NearestNeighborScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
  int x_ratio = (int)((w << 16)/sw) + 1;
  int y_ratio = (int)((h << 16)/sh) + 1;
  int x2, y2;

  for (int i=0; i<sh; i++) {
    y2 = ((i*y_ratio) >> 16);

    uint32_t *t = dst + i*sw;
    uint32_t *p = src + y2*w;
    int rat = 0;

    for (int j=0; j<sw; j++) {
      x2 = (rat >> 16);
      *t++ = p[x2];
      rat += x_ratio;
    }
  }

  /*
  for (int i=0; i<sh; i++) {
    for (int j=0; j<sw; j++) {
      x2 = ((j*x_ratio) >> 16) ;
      y2 = ((i*y_ratio) >> 16) ;
      dst[(i*sw)+j] = src[(y2*w)+x2] ;
    }                
  }
  */
}
    
static void BilinearScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
  int a, b, c, d, x, y, index;
  float x_ratio = ((float)(w-1))/sw;
  float y_ratio = ((float)(h-1))/sh;
  float x_diff, y_diff;
  int blue, red, green, alpha;
  int offset = 0;

  for (int i=0; i<sh; i++) {
    for (int j=0; j<sw; j++) {
      x = (int)(x_ratio * j);
      y = (int)(y_ratio * i);
      x_diff = (x_ratio * j) - x;
      y_diff = (y_ratio * i) - y;
      index = (y*w + x);

      a = src[index + 0*w + 0];
      b = src[index + 0*w + 1];
      c = src[index + 1*w + 0];
      d = src[index + 1*w + 1];

      float m1 = (1-x_diff)*(1-y_diff);
      float m2 = (x_diff)*(1-y_diff);
      float m3 = (y_diff)*(1-x_diff);
      float m4 = (x_diff*y_diff);

      blue = (int)(((a>>0x00) & 0xff)*m1 + ((b>>0x00) & 0xff)*m2 + ((c>>0x00) & 0xff)*m3 + ((d>>0x00) & 0xff)*m4);
      green = (int)(((a>>0x08) & 0xff)*m1 + ((b>>0x08) & 0xff)*m2 + ((c>>0x08) & 0xff)*m3 + ((d>>0x08) & 0xff)*m4);
      red = (int)(((a>>0x10) & 0xff)*m1 + ((b>>0x10) & 0xff)*m2 + ((c>>0x10) & 0xff)*m3 + ((d>>0x10) & 0xff)*m4);
      alpha = (int)(((a>>0x18) & 0xff)*m1 + ((b>>0x18) & 0xff)*m2 + ((c>>0x18) & 0xff)*m3 + ((d>>0x18) & 0xff)*m4);

      dst[offset++] = 
        ((alpha << 0x18) & 0xff000000) | ((red << 0x10) & 0x00ff0000) |
        ((green << 0x08) & 0x0000ff00) | ((blue << 0x00) & 0x000000ff);
    }
  }
}

static float CubicHermite(float A, float B, float C, float D, float t) 
{
  float a = -A/2.0f + (3.0f*B)/2.0f - (3.0f*C)/2.0f + D/2.0f;
  float b = A - (5.0f*B)/2.0f + 2.0f*C - D/2.0f;
  float c = -A/2.0f + C/2.0f;
  float d = B;

  return a*t*t*t + b*t*t + c*t + d;
}

void GetPixelClamped(uint32_t *src, int x, int y, int w, int h, uint8_t temp[4])
{
  if (x < 0) {
    x = 0;
  }

  if (x > (w - 1)) {
    x = w - 1;
  }
  
  if (y < 0) {
    y = 0;
  }

  if (y > (h - 1)) {
    y = h - 1;
  }

  uint32_t p = src[y*w + x];

  temp[0] = (p >> 0x00) & 0xff;
  temp[1] = (p >> 0x08) & 0xff;
  temp[2] = (p >> 0x10) & 0xff;
  temp[3] = (p >> 0x18) & 0xff;
}

static void SampleBicubic(uint32_t *src, int w, int h, float u, float v, uint8_t sample[4]) 
{
  float x = (u*w) - 0.5;
  int xint = (int)x;
  float xfract = x - floor(x);

  float y = (v*h) - 0.5;
  int yint = (int)y;
  float yfract = y - floor(y);

  uint8_t p00[4];
  uint8_t p10[4];
  uint8_t p20[4];
  uint8_t p30[4];

  uint8_t p01[4];
  uint8_t p11[4];
  uint8_t p21[4];
  uint8_t p31[4];

  uint8_t p02[4];
  uint8_t p12[4];
  uint8_t p22[4];
  uint8_t p32[4];

  uint8_t p03[4];
  uint8_t p13[4];
  uint8_t p23[4];
  uint8_t p33[4];

  // 1st row
  GetPixelClamped(src, xint - 1, yint - 1, w, h, p00);
  GetPixelClamped(src, xint + 0, yint - 1, w, h, p10);
  GetPixelClamped(src, xint + 1, yint - 1, w, h, p20);
  GetPixelClamped(src, xint + 2, yint - 1, w, h, p30);

  // 2nd row
  GetPixelClamped(src, xint - 1, yint + 0, w, h, p01);
  GetPixelClamped(src, xint + 0, yint + 0, w, h, p11);
  GetPixelClamped(src, xint + 1, yint + 0, w, h, p21);
  GetPixelClamped(src, xint + 2, yint + 0, w, h, p31);

  // 3rd row
  GetPixelClamped(src, xint - 1, yint + 1, w, h, p02);
  GetPixelClamped(src, xint + 0, yint + 1, w, h, p12);
  GetPixelClamped(src, xint + 1, yint + 1, w, h, p22);
  GetPixelClamped(src, xint + 2, yint + 1, w, h, p32);

  // 4th row
  GetPixelClamped(src, xint - 1, yint + 2, w, h, p03);
  GetPixelClamped(src, xint + 0, yint + 2, w, h, p13);
  GetPixelClamped(src, xint + 1, yint + 2, w, h, p23);
  GetPixelClamped(src, xint + 2, yint + 2, w, h, p33);

  // interpolate bi-cubically!
  for (int i = 0; i < 4; i++) {
    float col0 = CubicHermite(p00[i], p10[i], p20[i], p30[i], xfract);
    float col1 = CubicHermite(p01[i], p11[i], p21[i], p31[i], xfract);
    float col2 = CubicHermite(p02[i], p12[i], p22[i], p32[i], xfract);
    float col3 = CubicHermite(p03[i], p13[i], p23[i], p33[i], xfract);

    float value = CubicHermite(col0, col1, col2, col3, yfract);

    if (value < 0.0f) {
      value = 0.0f;
    }

    if (value > 255.0f) {
      value = 255.0f;
    }

    sample[i] = (uint8_t)value;
  }
}

static void BicubicScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
  uint8_t sample[4];
  int y, x;

  for (y=0; y<sh; y++) {
    float v = (float)y/(float)(sh - 1);

    for (x=0; x<sw; x++) {
      float u = (float)x/(float)(sw - 1);

      SampleBicubic(src, w, h, u, v, sample);

      dst[y*sw + x] = sample[3] << 0x18 | sample[2] << 0x10 | sample[1] << 0x08 | sample[0] << 0x00;
    }
  }
}

static void NearesNeighborRotate(uint32_t *src, int w, int h, uint32_t *dst, int dw, int dh, double radians, bool resize)
{
  double angle = fmod(radians, 2*M_PI);
  int precision = 1024;
  int sinTheta = precision*sin(angle);
  int cosTheta = precision*cos(angle);

  int iw = dw;
  int ih = dh;

  memset(dst, 0, iw*ih*sizeof(uint32_t));

  int sxc = w/2;
  int syc = h/2;
  int dxc = iw/2;
  int dyc = ih/2;
  int xo;
  int yo;
  int t1;
  int t2;

  for (int j=0; j<ih; j++) {
    uint32_t *ptr = dst + j*iw;

    t1 = (j-dyc)*sinTheta;
    t2 = (j-dyc)*cosTheta;

    for (int i=0; i<iw; i++) {
      xo = ((i-dxc)*cosTheta - t1)/precision;
      yo = ((i-dxc)*sinTheta + t2)/precision;

      if (xo >= -sxc && xo <= sxc && yo >= -syc && yo <= syc) {
        *(ptr+i) = *(src + (yo+syc)*w + (xo+sxc));
      }
    }
  }
}

BufferedImage::BufferedImage(jpixelformat_t pixelformat, jpoint_t<int> size):
  Image(pixelformat, size)
{
  if (size.x <= 0 || size.y <= 0) {
    throw std::runtime_error("Image must have width and height greater than 0");
  }

  _pixelformat = pixelformat;
  _size = size;

  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  _cairo_surface = cairo_image_surface_create(format, _size.x, _size.y);

  _graphics = new Graphics(_cairo_surface);
}

BufferedImage::BufferedImage(cairo_surface_t *surface):
  Image(jpixelformat_t::Unknown, {-1, -1})
{
  if (surface == nullptr) {
    throw std::runtime_error("Unable to get target surface from cairo context");
  }

  _cairo_surface = cairo_surface_reference(surface);

  cairo_format_t
    format = cairo_image_surface_get_format(_cairo_surface);

  if (format == CAIRO_FORMAT_ARGB32) {
    _pixelformat = jpixelformat_t::ARGB;
  } else if (format == CAIRO_FORMAT_RGB24) {
    _pixelformat = jpixelformat_t::RGB32;
  } else if (format == CAIRO_FORMAT_RGB16_565) {
    _pixelformat = jpixelformat_t::RGB16;
  } else {
    _pixelformat = jpixelformat_t::Unknown;
  }
  
  _size.x = cairo_image_surface_get_width(_cairo_surface);
  _size.y = cairo_image_surface_get_height(_cairo_surface);

  _graphics = new Graphics(_cairo_surface);
}

BufferedImage::BufferedImage(std::string file):
  Image(jpixelformat_t::Unknown, {-1, -1})
{
  std::ifstream f;

  f.open(file);

  if (!f) {
    throw std::invalid_argument("Cannot request data from a invalid stream");
  }

  ConstructFromStream(f);
}

BufferedImage::BufferedImage(std::istream &stream):
  Image(jpixelformat_t::Unknown, {-1, -1})
{
  ConstructFromStream(stream);
}

void BufferedImage::ConstructFromStream(std::istream &stream)
{
  if (!stream) {
    throw std::invalid_argument("Cannot request data from a invalid stream");
  }

  stream.seekg (0, stream.end);
  int size = stream.tellg();
  stream.seekg (0, stream.beg);

  uint8_t *buffer = new uint8_t[size];

  stream.read((char *)buffer, size);

  if (!stream) {
    throw std::invalid_argument("Unable to get all image content");
  }
  
  cairo_surface_t *surface = create_png_surface_from_data(buffer, size);

  if (surface == nullptr) {
#ifdef JPG_IMAGE
    surface = create_jpg_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef BPG_IMAGE
    surface = create_bpg_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef SVG_IMAGE
    surface = create_svg_surface_from_data(buffer, size, -1, -1);
#endif
  }

  if (surface == nullptr) {
#ifdef TIFF_IMAGE
    surface = create_tif_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef JP2000_IMAGE
    surface = create_jp2000_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef BMP_IMAGE
    surface = create_bmp_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef PPM_IMAGE
    surface = create_ppm_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef HEIF_IMAGE
    surface = create_heif_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef FLIF_IMAGE
    surface = create_flif_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef WEBP_IMAGE
    surface = create_webp_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef GIF_IMAGE
    surface = create_gif_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef ICO_IMAGE
    surface = create_ico_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef PCX_IMAGE
    surface = create_pcx_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef TGA_IMAGE
    surface = create_tga_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef XBM_IMAGE
    surface = create_xbm_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef XPM_IMAGE
    surface = create_xpm_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
#ifdef MJPEG_IMAGE
    surface = create_mjpeg_surface_from_data(buffer, size);
#endif
  }

  if (surface == nullptr) {
    delete [] buffer;

    throw std::runtime_error("Cannot open this image type");
  }

  _cairo_surface = surface;

  _pixelformat = jpixelformat_t::Unknown;

  cairo_format_t format = cairo_image_surface_get_format(_cairo_surface);

  if (format == CAIRO_FORMAT_ARGB32) {
    _pixelformat = jpixelformat_t::ARGB;
  } else if (format == CAIRO_FORMAT_RGB24) {
    _pixelformat = jpixelformat_t::RGB32;
  } else if (format == CAIRO_FORMAT_RGB16_565) {
    _pixelformat = jpixelformat_t::RGB16;
  }

  _size.x = cairo_image_surface_get_width(_cairo_surface);
  _size.y = cairo_image_surface_get_height(_cairo_surface);

  _graphics = new Graphics(_cairo_surface);

  delete [] buffer;
}

BufferedImage::~BufferedImage()
{
  if (_graphics != nullptr) {
    delete _graphics;
    _graphics = nullptr;
  }

  cairo_surface_destroy(_cairo_surface);
}

Graphics * BufferedImage::GetGraphics()
{
  return _graphics;
}

std::shared_ptr<Image> BufferedImage::Flip(jflip_t mode)
{
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, _size.x, _size.y);
  cairo_t 
    *context = cairo_create(surface);

  cairo_matrix_t ms, mt, m;

  if (mode == jflip_t::Horizontal) {
    cairo_matrix_init_scale(&ms, -1.0f, 1.0f);
    cairo_matrix_init_translate(&mt, -_size.x, 0.0f);
  } else {
    cairo_matrix_init_scale(&ms, 1.0f, -1.0f);
    cairo_matrix_init_translate(&mt, 0.0f, -_size.y);
  }

  cairo_matrix_multiply(&m, &mt, &ms);
  cairo_set_matrix(context, &m);

  cairo_surface_flush(_cairo_surface);
  cairo_set_source_surface(context, _cairo_surface, 0, 0);
  cairo_paint(context);

  std::shared_ptr<Image> tmp = std::make_shared<BufferedImage>(surface);

  cairo_destroy(context);
  cairo_surface_destroy(surface);

  return tmp;
}

std::shared_ptr<Image> BufferedImage::Shear(jpoint_t<float> size)
{
  int 
    tx = _size.x*fabs(size.x),
    ty = _size.y*fabs(size.y);

  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, _size.x + tx, _size.y + ty);
  cairo_t 
    *context = cairo_create(surface);

  cairo_matrix_t m;

  cairo_matrix_init(&m,
      1.0f, size.y,
      size.x, 1.0f,
      0.0f, 0.0f
   );

  cairo_transform(context, &m);
  // cairo_set_matrix(context, &m);

  if (size.x < 0.0f) {
    cairo_translate(context, tx, 0);
  }

  if (size.y < 0.0f) {
    cairo_translate(context, 0, ty);
  }

  cairo_surface_flush(_cairo_surface);
  cairo_set_source_surface(context, _cairo_surface, 0, 0);
  cairo_paint(context);

  std::shared_ptr<Image> tmp = std::make_shared<BufferedImage>(surface);

  cairo_destroy(context);

  return tmp;
}

std::shared_ptr<Image> BufferedImage::Rotate(double radians, bool resize)
{
  double angle = fmod(radians, 2*M_PI);

  int iw = _size.x;
  int ih = _size.y;

  if (resize == true) {
    int precision = 10240;
    int sinTheta = (int)(precision*sin(angle));
    int cosTheta = (int)(precision*cos(angle));

    iw = (abs(_size.x*cosTheta) + abs(_size.y*sinTheta))/precision;
    ih = (abs(_size.x*sinTheta) + abs(_size.y*cosTheta))/precision;
    
    cairo_format_t 
      format = CAIRO_FORMAT_INVALID;

    if (_pixelformat == jpixelformat_t::ARGB) {
      format = CAIRO_FORMAT_ARGB32;
    } else if (_pixelformat == jpixelformat_t::RGB32) {
      format = CAIRO_FORMAT_RGB24;
    } else if (_pixelformat == jpixelformat_t::RGB24) {
      format = CAIRO_FORMAT_RGB24;
    } else if (_pixelformat == jpixelformat_t::RGB16) {
      format = CAIRO_FORMAT_RGB16_565;
    }

    cairo_surface_t 
      *surface = cairo_image_surface_create(format, iw, ih);

    std::shared_ptr<Image> 
      image = std::make_shared<BufferedImage>(surface);

    if (GetGraphics()->GetAntialias() == jantialias_mode_t::None) {
      uint32_t *src = new uint32_t[_size.x*_size.y];
      uint32_t *dst = new uint32_t[iw*ih];

      GetRGBArray(src, {0, 0, _size.x, _size.y});

      NearesNeighborRotate(src, _size.x, _size.y, dst, iw, ih, radians, true);

      image->GetGraphics()->SetRGBArray(dst, {0, 0, iw, ih});

      delete [] dst;
      delete [] src;
    } else {
      cairo_t *dst_context = cairo_create(image->GetGraphics()->GetCairoSurface());

      cairo_translate(dst_context, iw/2, ih/2);
      cairo_rotate(dst_context, -radians);
      cairo_translate(dst_context, -iw/2, -ih/2);
      cairo_set_source_surface(dst_context, _cairo_surface, (iw - _size.x)/2, (ih - _size.y)/2);
      cairo_paint(dst_context);

      cairo_destroy(dst_context);
    }

    return image;
  }

  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, iw, ih);

  std::shared_ptr<Image>
    image = std::make_shared<BufferedImage>(surface);
  
  if (GetGraphics()->GetAntialias() == jantialias_mode_t::None) {
    uint32_t *src = new uint32_t[_size.x*_size.y];
    uint32_t *dst = new uint32_t[iw*ih];

    GetRGBArray(src, {0, 0, _size.x, _size.y});

    NearesNeighborRotate(src, _size.x, _size.y, dst, iw, ih, radians, false);

    image->GetGraphics()->SetRGBArray(dst, {0, 0, iw, ih});

    delete [] dst;
    delete [] src;
  } else {
    cairo_t *dst_context = cairo_create(image->GetGraphics()->GetCairoSurface());

    cairo_translate(dst_context, _size.x/2, _size.y/2);
    cairo_rotate(dst_context, -radians);
    cairo_translate(dst_context, -_size.x/2, -_size.y/2);
    cairo_set_source_surface(dst_context, _cairo_surface, 0, 0);
    cairo_paint(dst_context);

    cairo_destroy(dst_context);
  }

  cairo_surface_destroy(surface);

  return image;
}

std::shared_ptr<Image> BufferedImage::Scale(jpoint_t<int> size)
{
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, size.x, size.y);
  cairo_t
    *context = cairo_create(surface);
  
#ifdef SVG_IMAGE
  std::string *data = (std::string *)cairo_surface_get_user_data(_cairo_surface, nullptr);

  if (data != nullptr) {
    cairo_surface_t *svg_surface = create_svg_surface_from_data((uint8_t *)data->c_str(), data->size(), size.x, size.y);
    
    cairo_set_source_surface(context, svg_surface, 0, 0);
    cairo_paint(context);

    std::shared_ptr<Image> tmp = std::make_shared<BufferedImage>(surface);

    cairo_surface_destroy(svg_surface);
    cairo_destroy(context);

    return tmp;
  }
#endif

  std::shared_ptr<Image>
    image = std::make_shared<BufferedImage>(surface);

  if (GetGraphics()->GetAntialias() == jantialias_mode_t::None) {
    jblitting_t method = GetBlittingFlags();

    uint32_t *src = new uint32_t[_size.x*_size.y];
    uint32_t *dst = new uint32_t[size.x*size.y];

    GetRGBArray(src, {0, 0, _size.x, _size.y});

    if (method == jblitting_t::Fast or method == jblitting_t::Nearest) {
      NearestNeighborScale(src, dst, _size.x, _size.y, size.x, size.y); 
    } else if (method == jblitting_t::Bilinear) {
      BilinearScale(src, dst, _size.x, _size.y, size.x, size.y); 
    } else {
      BicubicScale(src, dst, _size.x, _size.y, size.x, size.y); 
    }

    image->GetGraphics()->SetRGBArray(dst, {0, 0, size.x, size.y});

    delete [] dst;
    delete [] src;
  } else {
    cairo_t *context = cairo_create(image->GetGraphics()->GetCairoSurface());

    cairo_surface_flush(_cairo_surface);
    cairo_scale(context, (double)size.x/_size.x, (double)size.y/_size.y);
    cairo_set_source_surface(context, _cairo_surface, 0, 0);
    cairo_paint(context);

    cairo_destroy(context);
  }

  cairo_destroy(context);

  return image;
}

std::shared_ptr<Image> BufferedImage::Crop(jrect_t<int> rect)
{
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, rect.size.x, rect.size.y);
  cairo_t
    *context = cairo_create(surface);

  cairo_surface_flush(_cairo_surface);
  cairo_set_source_surface(context, _cairo_surface, -rect.point.x, -rect.point.y);
  cairo_paint(context);

  std::shared_ptr<Image> tmp = std::make_shared<BufferedImage>(surface);

  cairo_destroy(context);

  return tmp;
}

std::shared_ptr<Image> BufferedImage::Blend(double alpha)
{
  if (alpha < 0.0) {
    alpha = 0.0;
  }

  if (alpha > 1.0) {
    alpha = 1.0;
  }

  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, _size.x, _size.y);
  cairo_t
    *context = cairo_create(surface);

  cairo_surface_flush(_cairo_surface);
  cairo_set_source_surface(context, _cairo_surface, 0, 0);
  cairo_paint_with_alpha(context, alpha);

  std::shared_ptr<Image> tmp = std::make_shared<BufferedImage>(surface);

  cairo_destroy(context);

  return tmp;
}

std::shared_ptr<Image> BufferedImage::Colorize(jcolor_t<float> color)
{
  std::shared_ptr<Image>
    image = Blend(1.0);

  cairo_surface_t 
    *surface = image->GetGraphics()->GetCairoSurface();

  if (surface == nullptr) {
    return nullptr;
  }

  uint8_t *data = cairo_image_surface_get_data(surface);

  if (data == nullptr) {
    return nullptr;
  }

  int stride = cairo_image_surface_get_stride(surface);

  jvector_t<3, float> 
    hsb = color.ToHSB();

  HSLColorSpace 
    hsl(hsb[0], hsb[1], 0.0);
  jpoint_t<int> 
    size = image->GetSize();

  if (image->GetPixelFormat() == jpixelformat_t::ARGB) {
    for (int j=0; j<size.y; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=4) {
        // int a = *(dst + i + 3);
        int r = *(dst + i + 2);
        int g = *(dst + i + 1);
        int b = *(dst + i + 0);

        hsl.GetRGB(&r, &g, &b);

        // *(dst + i + 3) = a;
        *(dst + i + 2) = r;
        *(dst + i + 1) = g;
        *(dst + i + 0) = b;
      }
    }
  } else if (image->GetPixelFormat() == jpixelformat_t::RGB32) {
    for (int j=0; j<size.y; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=4) {
        // int a = *(dst + i + 3);
        int r = *(dst + i + 2);
        int g = *(dst + i + 1);
        int b = *(dst + i + 0);

        hsl.GetRGB(&r, &g, &b);

        // *(dst + i + 3) = a;
        *(dst + i + 2) = r;
        *(dst + i + 1) = g;
        *(dst + i + 0) = b;
      }
    }
  } else if (image->GetPixelFormat() == jpixelformat_t::RGB24) {
    for (int j=0; j<size.y; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=3) {
        int r = *(dst + i + 2);
        int g = *(dst + i + 1);
        int b = *(dst + i + 0);

        hsl.GetRGB(&r, &g, &b);

        *(dst + i + 2) = r;
        *(dst + i + 1) = g;
        *(dst + i + 0) = b;
      }
    }
  } else if (image->GetPixelFormat() == jpixelformat_t::RGB16) {
    for (int j=0; j<size.y; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=2) {
        uint16_t pixel = *((uint16_t *)dst);
        int r = (pixel >> 0x0b) & 0x1f;
        int g = (pixel >> 0x05) & 0x3f;
        int b = (pixel >> 0x00) & 0x1f;

        hsl.GetRGB(&r, &g, &b);

        *(dst + i + 1) = (r << 0x03 | g >> 0x03) & 0xff;
        *(dst + i + 0) = (g << 0x03 | b >> 0x00) & 0xff;
      }
    }
  }
  
  cairo_surface_mark_dirty(surface);

  return image;
}

uint8_t * BufferedImage::LockData()
{
  _mutex.lock();

  return cairo_image_surface_get_data(_cairo_surface);
}

void BufferedImage::UnlockData()
{
  cairo_surface_mark_dirty(_cairo_surface);
    
  _mutex.unlock();
}

void BufferedImage::GetRGBArray(uint32_t *rgb, jrect_t<int> rect)
{
  if (_graphics == nullptr) {
    return;
  }

  _graphics->GetRGBArray(rgb, rect);
}
    
std::shared_ptr<Image> BufferedImage::Clone()
{
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == jpixelformat_t::ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(format, _size.x, _size.y);

  std::shared_ptr<Image>
    clone = std::make_shared<BufferedImage>(surface);
  Graphics 
    *g = clone->GetGraphics();
  jcomposite_t 
    flags = g->GetCompositeFlags();

  cairo_surface_destroy(surface);

  g->SetCompositeFlags(jcomposite_t::Src);

  if (g->DrawImage(shared_from_this(), jpoint_t<int>{0, 0}) == false) {
    return nullptr;
  }

  g->SetCompositeFlags(flags);

  return clone;
}

}

