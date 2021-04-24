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
#include "jcanvas/core/jgraphics.h"
#include "jcanvas/core/jfont.h"
#include "jcanvas/core/jimage.h"
#include "jcanvas/core/jenum.h"

#include "jmixin/jstring.h"

#define M_2PI  (2*M_PI)
  
#define IMAGE_LIMIT 32768

namespace jcanvas {

Graphics::Graphics(cairo_surface_t *surface)
{
  if (surface == nullptr) {
    throw std::invalid_argument("Invalid pointer to cairo surface");
  }

  _cairo_surface = cairo_surface_reference(surface);

  _translate.x = 0;
  _translate.y = 0;

  _font = nullptr;
  
  _is_vertical_sync_enabled = false;

  _pen.join = jline_join_t::Miter;
  _pen.style = jline_style_t::Butt;
  _pen.size = 1;

  cairo_format_t
    format = cairo_image_surface_get_format(surface);

  if (format == CAIRO_FORMAT_ARGB32) {
    _pixelformat = jpixelformat_t::ARGB;
  } else if (format == CAIRO_FORMAT_RGB24) {
    _pixelformat = jpixelformat_t::RGB32;
  } else if (format == CAIRO_FORMAT_RGB16_565) {
    _pixelformat = jpixelformat_t::RGB16;
  } else {
    _pixelformat = jpixelformat_t::Unknown;
  }
  
  _clip = {
    0, 
    0, 
    cairo_image_surface_get_width(surface), 
    cairo_image_surface_get_height(surface)
  };

  _internal_clip = _clip;

  _cairo_context = cairo_create(_cairo_surface);

  SetAntialias(jantialias_t::Normal);
  SetPen(_pen);
  SetColor(0x00000000);
  ResetGradientStop();
  SetCompositeFlags(jcomposite_flags_t::SrcOver);
  SetBlittingFlags(jblitting_flags_t::Bilinear);
}

Graphics::~Graphics()
{
  cairo_destroy(_cairo_context);
  cairo_surface_destroy(_cairo_surface);
}

cairo_surface_t * Graphics::GetCairoSurface()
{
  return _cairo_surface;
}

std::string Graphics::Dump()
{
  char path[] = "/tmp/jcanvas-XXXXXX";

  int fd = mkstemp(path);

  if (fd < 0) {
    throw std::runtime_error("Unable to dump surface");
  }

  cairo_surface_write_to_png(_cairo_surface, path);
  
  return path;
}

jrect_t<int> Graphics::ClipRect(jrect_t<int> rect)
{
  struct jpoint_t<int> t = Translate();

  jrect_t<int> 
    clip = _internal_clip.Intersection(jrect_t<int>{rect.point.x + t.x, rect.point.y + t.y, rect.size.x, rect.size.y});
  
  SetClip({clip.point.x - t.x, clip.point.y - t.y, clip.size.x, clip.size.y});

  return {clip.point.x, clip.point.y, clip.size.x, clip.size.y};
}

void Graphics::SetClip(jrect_t<int> rect)
{
  struct jpoint_t<int> t = Translate();

  int sw = cairo_image_surface_get_width(_cairo_surface);
  int sh = cairo_image_surface_get_height(_cairo_surface);

  jrect_t<int> 
    region = {0, 0, sw, sh},
    clip = region.Intersection(jrect_t<int>{rect.point.x + t.x, rect.point.y + t.y, rect.size.x, rect.size.y});
  
  _clip.point = clip.point - jpoint_t<int>{t.x, t.y};
  _clip.size = clip.size;
  
  _internal_clip = {clip.point.x, clip.point.y, clip.size.x, clip.size.y};

  cairo_reset_clip(_cairo_context);
  cairo_rectangle(_cairo_context, clip.point.x, clip.point.y, clip.size.x, clip.size.y);
  cairo_clip(_cairo_context);
}

jrect_t<int> Graphics::GetClip()
{
  return _clip;
}

void Graphics::ReleaseClip()
{
  struct jpoint_t<int> t = Translate();

  _clip = {
    0,
    0,
    cairo_image_surface_get_width(_cairo_surface),
    cairo_image_surface_get_height(_cairo_surface)
  };

  _internal_clip = {
    t.x,
    t.y,
    _clip.size.x - t.x,
    _clip.size.y - t.y
  };
  
  cairo_reset_clip(_cairo_context);
}

void Graphics::SetCompositeFlags(jcomposite_flags_t t)
{
  cairo_operator_t o = CAIRO_OPERATOR_CLEAR;

  _composite = t;

  if (_composite == jcomposite_flags_t::Src) {
    o = CAIRO_OPERATOR_SOURCE;
  } else if (_composite == jcomposite_flags_t::SrcOver) {
    o = CAIRO_OPERATOR_OVER;
  } else if (_composite == jcomposite_flags_t::SrcIn) {
    o = CAIRO_OPERATOR_IN;
  } else if (_composite == jcomposite_flags_t::SrcOut) {
    o = CAIRO_OPERATOR_OUT;
  } else if (_composite == jcomposite_flags_t::SrcAtop) {
    o = CAIRO_OPERATOR_ATOP;
  } else if (_composite == jcomposite_flags_t::Dst) {
    o = CAIRO_OPERATOR_DEST;
  } else if (_composite == jcomposite_flags_t::DstOver) {
    o = CAIRO_OPERATOR_DEST_OVER;
  } else if (_composite == jcomposite_flags_t::DstIn) {
    o = CAIRO_OPERATOR_DEST_IN;
  } else if (_composite == jcomposite_flags_t::DstOut) {
    o = CAIRO_OPERATOR_DEST_OUT;
  } else if (_composite == jcomposite_flags_t::DstAtop) {
    o = CAIRO_OPERATOR_DEST_ATOP;
  } else if (_composite == jcomposite_flags_t::Xor) {
    o = CAIRO_OPERATOR_XOR;
  } else if (_composite == jcomposite_flags_t::Add) {
    o = CAIRO_OPERATOR_ADD;
  } else if (_composite == jcomposite_flags_t::Saturate) {
    o = CAIRO_OPERATOR_SATURATE;
  } else if (_composite == jcomposite_flags_t::Multiply) {
    o = CAIRO_OPERATOR_MULTIPLY;
  } else if (_composite == jcomposite_flags_t::Screen) {
    o = CAIRO_OPERATOR_SCREEN;
  } else if (_composite == jcomposite_flags_t::Overlay) {
    o = CAIRO_OPERATOR_OVERLAY;
  } else if (_composite == jcomposite_flags_t::Darken) {
    o = CAIRO_OPERATOR_DARKEN;
  } else if (_composite == jcomposite_flags_t::Lighten) {
    o = CAIRO_OPERATOR_LIGHTEN;
  } else if (_composite == jcomposite_flags_t::Difference) {
    o = CAIRO_OPERATOR_DIFFERENCE;
  } else if (_composite == jcomposite_flags_t::Exclusion) {
    o = CAIRO_OPERATOR_EXCLUSION;
  } else if (_composite == jcomposite_flags_t::Dodge) {
    o = CAIRO_OPERATOR_COLOR_DODGE;
  } else if (_composite == jcomposite_flags_t::Burn) {
    o = CAIRO_OPERATOR_COLOR_BURN;
  } else if (_composite == jcomposite_flags_t::Hard) {
    o = CAIRO_OPERATOR_HARD_LIGHT;
  } else if (_composite == jcomposite_flags_t::Light) {
    o = CAIRO_OPERATOR_SOFT_LIGHT;
  }

  cairo_set_operator(_cairo_context, o);
}

jcomposite_flags_t Graphics::GetCompositeFlags()
{
  return _composite;
}

jblitting_flags_t Graphics::GetBlittingFlags()
{
  return _blitting;
}

void Graphics::SetBlittingFlags(jblitting_flags_t t)
{
  cairo_filter_t o = CAIRO_FILTER_FAST;

  _blitting = t;

  if (_blitting == jblitting_flags_t::Fast) {
    o = CAIRO_FILTER_FAST;
  } else  if (_blitting == jblitting_flags_t::Good) {
    o = CAIRO_FILTER_GOOD;
  } else  if (_blitting == jblitting_flags_t::Best) {
    o = CAIRO_FILTER_BEST;
  } else  if (_blitting == jblitting_flags_t::Nearest) {
    o = CAIRO_FILTER_NEAREST;
  } else  if (_blitting == jblitting_flags_t::Bilinear) {
    o = CAIRO_FILTER_BILINEAR;
  } else  if (_blitting == jblitting_flags_t::Gaussian) {
    o = CAIRO_FILTER_GAUSSIAN;
  }

  cairo_pattern_set_filter(cairo_get_source(_cairo_context), o);
}

void Graphics::Clear()
{
  cairo_save(_cairo_context);
  cairo_set_operator(_cairo_context, CAIRO_OPERATOR_CLEAR);
  cairo_paint(_cairo_context);
  cairo_restore(_cairo_context);

  /*
  int sw = cairo_image_surface_get_width(_cairo_surface);
  int sh = cairo_image_surface_get_height(_cairo_surface);

  cairo_save(_cairo_context);
  cairo_set_source_rgba(_cairo_context, 0, 0, 0, 0);
  cairo_set_operator(_cairo_context, CAIRO_OPERATOR_SOURCE);
  
  cairo_rectangle(_cairo_context, 0, 0, sw, sh);
  cairo_fill(_cairo_context);
  
  cairo_restore(_cairo_context);
  */
}

void Graphics::Clear(jrect_t<int> rect)
{
  struct jpoint_t<int> t = Translate();

  cairo_save(_cairo_context);
  cairo_set_operator(_cairo_context, CAIRO_OPERATOR_CLEAR);
  cairo_rectangle(_cairo_context, t.x + rect.point.x, t.y + rect.point.y, rect.size.x, rect.size.y);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);

  /*
  struct jpoint_t<int> t = Translate();

  cairo_save(_cairo_context);
  cairo_set_source_rgba(_cairo_context, 0, 0, 0, 0);
  cairo_set_operator(_cairo_context, CAIRO_OPERATOR_SOURCE);
  
  cairo_rectangle(_cairo_context, t.x + rect.point.x, t.y + rect.point.y, rect.size.x, rect.size.y);
  cairo_fill(_cairo_context);
  
  cairo_restore(_cairo_context);
  */
}

jcolor_t<float> & Graphics::GetColor()
{
  return _color;
} 

void Graphics::SetColor(const jcolor_t<float> &color)
{
  _color = color;

  cairo_set_source_rgba(_cairo_context, _color.red, _color.green, _color.blue, _color.alpha);
} 

bool Graphics::HasFont()
{
  return (_font != nullptr);
}

void Graphics::SetFont(std::shared_ptr<Font> font)
{
  _font = font;
  
  if (_font != nullptr) {
    _font->ApplyContext(_cairo_context);
  } else {
    cairo_set_font_face(_cairo_context, nullptr);
  }
}

std::shared_ptr<Font> Graphics::GetFont()
{
  return _font;
}

void Graphics::SetAntialias(jantialias_t mode)
{
  cairo_antialias_t t = CAIRO_ANTIALIAS_NONE;

  if (mode == jantialias_t::Fast) {
    t = CAIRO_ANTIALIAS_FAST;
  } else if (mode == jantialias_t::Normal) {
    t = CAIRO_ANTIALIAS_DEFAULT; // DEFAULT, SUBPIXEL
  } else if (mode == jantialias_t::Good) {
    t = CAIRO_ANTIALIAS_GOOD; // GOOD, BEST
  }
    
  cairo_set_antialias(_cairo_context, t);
}

jantialias_t Graphics::GetAntialias()
{
  cairo_antialias_t antialias = cairo_get_antialias(_cairo_context);

  if (antialias == CAIRO_ANTIALIAS_DEFAULT) {
    return jantialias_t::Normal;
  } else if (antialias == CAIRO_ANTIALIAS_SUBPIXEL) {
    return jantialias_t::Normal;
  } else if (antialias == CAIRO_ANTIALIAS_FAST) {
    return jantialias_t::Fast;
  } else if (antialias == CAIRO_ANTIALIAS_GOOD) {
    return jantialias_t::Good;
  } else if (antialias == CAIRO_ANTIALIAS_BEST) {
    return jantialias_t::Good;
  }

  return jantialias_t::None;
}

void Graphics::SetPen(jpen_t t)
{
  _pen = t;

  if (_pen.join == jline_join_t::Bevel) {
    cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_BEVEL);
  } else if (_pen.join == jline_join_t::Round) {
    cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_ROUND);
  } else if (_pen.join == jline_join_t::Miter) {
    cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_MITER);
  }
  
  if (_pen.style == jline_style_t::Round) {
    cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_ROUND);
  } else if (_pen.style == jline_style_t::Butt) {
    cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_BUTT);
  } else if (_pen.style == jline_style_t::Square) {
    cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_SQUARE);
  }
  
  std::vector<double> dashes;

  dashes.reserve(_pen.dashes.size());

  std::copy(dashes.begin(), dashes.end(), _pen.dashes.begin());

  cairo_set_dash(_cairo_context, dashes.data(), dashes.size(), 0.0);
}

jpen_t Graphics::GetPen()
{
  return _pen;
}

void Graphics::DrawLine(jpoint_t<int> p0, jpoint_t<int> p1)
{
  if (_pen.size <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  cairo_save(_cairo_context);
  cairo_move_to(_cairo_context, p0.x + t.x, p0.y + t.y);
  cairo_line_to(_cairo_context, p1.x + t.x, p1.y + t.y);
  cairo_set_line_width(_cairo_context, _pen.size);
  cairo_stroke(_cairo_context);
  cairo_restore(_cairo_context);
}

float EvaluateBezier0(float *data, int ndata, float t) 
{
  if (t < 0.0) {
    return(data[0]);
  }

  if (t >= (float)ndata) {
    return data[ndata-1];
  }

  float result, blend, mu, muk, munk;
  int n, k, kn, nn, nkn;

  mu = t/(float)ndata;

  n = ndata-1;
  result = 0.0;
  muk = 1;
  munk = pow(1-mu,(float)n);

  for (k=0; k<=n; k++) {
    nn = n;
    kn = k;
    nkn = n - k;
    blend = muk * munk;
    muk *= mu;
    munk /= (1-mu);

    while (nn >= 1) {
      blend *= nn;
      nn--;

      if (kn > 1) {
        blend /= (float)kn;
        kn--;
      }

      if (nkn > 1) {
        blend /= (float)nkn;
        nkn--;
      }
    }

    result += data[k] * blend;
  }

  return result;
}

void Graphics::DrawBezierCurve(std::vector<jpoint_t<int>> points, int interpolation)
{
  if (_pen.size <= 0 or points.size() < 3 or interpolation < 2) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  float 
    *x, 
    *y, 
    stepsize;
  int 
    x2, 
    y2;

  stepsize = (float)1.0/(float)interpolation;

  x = new float[points.size() + 1];
  y = new float[points.size() + 1];

  for (int i=0; i<(int)points.size(); i++) {
    x[i] = (float)(t.x + points[i].x);
    y[i] = (float)(t.y + points[i].y);
  }

  x[points.size()] = (float)(t.x + points[0].x);
  y[points.size()] = (float)(t.y + points[0].y);

  float step = 0.0;
  
  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  for (int i=0; i<=(int)(points.size()*interpolation); i++) {
    step = step + stepsize;

    x2 = EvaluateBezier0(x, points.size(), step);
    y2 = EvaluateBezier0(y, points.size(), step);
  
    cairo_line_to(_cairo_context, x2, y2);
  }
    
  delete [] x;
  delete [] y;

  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, _pen.size);
}

void Graphics::FillRectangle(jrect_t<int> rect)
{
  struct jpoint_t<int> t = Translate();

  cairo_save(_cairo_context);
  cairo_rectangle(_cairo_context, rect.point.x + t.x, rect.point.y + t.y, rect.size.x, rect.size.y);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawRectangle(jrect_t<int> rect)
{
  if (rect.size.x <=0 || rect.size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();
  jpen_t pen = GetPen();

  int offset = pen.size/2 + pen.size%2;
  int x = rect.point.x + t.x + offset;
  int y = rect.point.y + t.y + offset;
  int w = rect.size.x - 2*offset;
  int h = rect.size.y - 2*offset;

  cairo_save(_cairo_context);
  cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, abs(pen.size));
  cairo_stroke(_cairo_context);
}

void Graphics::FillBevelRectangle(jrect_t<int> rect, int dx, int dy, jrect_corner_t corners)
{
  if (rect.size.x <=0 || rect.size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  int x = rect.point.x + t.x;
  int y = rect.point.y + t.y;
  int w = rect.size.x;
  int h = rect.size.y;

  if (dx > rect.size.x/2) {
    dx = rect.size.x/2;
  }

  if (dy > rect.size.y/2) {
    dy = rect.size.y/2;
  }

  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopRight)) {
    cairo_line_to(_cairo_context, x + w - dx, y);
    cairo_line_to(_cairo_context, x + w, y + dy);
  } else {
    cairo_line_to(_cairo_context, x + w, y);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomRight)) {
    cairo_line_to(_cairo_context, x + w, y + h - dy);
    cairo_line_to(_cairo_context, x + w - dx, y + h);
  } else {
    cairo_line_to(_cairo_context, x + w, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomLeft)) {
    cairo_line_to(_cairo_context, x + dx, y + h);
    cairo_line_to(_cairo_context, x, y + h - dy);
  } else {
    cairo_line_to(_cairo_context, x, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopLeft)) {
    cairo_line_to(_cairo_context, x, y + dy);
    cairo_line_to(_cairo_context, x + dx, y);
  } else {
    cairo_line_to(_cairo_context, x, y);
  }

  cairo_close_path(_cairo_context);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawBevelRectangle(jrect_t<int> rect, int dx, int dy, jrect_corner_t corners)
{
  if (rect.size.x <=0 || rect.size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();
  jpen_t pen = GetPen();

  int offset = pen.size/2 + pen.size%2;
  int x = rect.point.x + t.x + offset;
  int y = rect.point.y + t.y + offset;
  int w = rect.size.x - 2*offset;
  int h = rect.size.y - 2*offset;

  if (dx > rect.size.x/2) {
    dx = rect.size.x/2;
  }

  if (dy > rect.size.y/2) {
    dy = rect.size.y/2;
  }

  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopRight)) {
    cairo_line_to(_cairo_context, x + w - dx, y);
    cairo_line_to(_cairo_context, x + w, y + dy);
  } else {
    cairo_line_to(_cairo_context, x + w, y);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomRight)) {
    cairo_line_to(_cairo_context, x + w, y + h - dy);
    cairo_line_to(_cairo_context, x + w - dx, y + h);
  } else {
    cairo_line_to(_cairo_context, x + w, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomLeft)) {
    cairo_line_to(_cairo_context, x + dx, y + h);
    cairo_line_to(_cairo_context, x, y + h - dy);
  } else {
    cairo_line_to(_cairo_context, x, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopLeft)) {
    cairo_line_to(_cairo_context, x, y + dy);
    cairo_line_to(_cairo_context, x + dx, y);
  } else {
    cairo_line_to(_cairo_context, x, y);
  }

  cairo_close_path(_cairo_context);
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, abs(pen.size));
  cairo_stroke(_cairo_context);
}

void Graphics::FillRoundRectangle(jrect_t<int> rect, int dx, int dy, jrect_corner_t corners)
{
  if (rect.size.x <=0 || rect.size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  int x = rect.point.x + t.x;
  int y = rect.point.y + t.y;
  int w = rect.size.x;
  int h = rect.size.y;

  if (dx > rect.size.x/2) {
    dx = rect.size.x/2;
  }

  if (dy > rect.size.y/2) {
    dy = rect.size.y/2;
  }

  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopRight)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + w - dx, y + dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, -M_PI_2, 0.0);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x + w, y);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomRight)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + w - dx, y + h - dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_PI_2);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x + w, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomLeft)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + dx, y + h - dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI_2, M_PI);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopLeft)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + dx, y + dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI, M_PI+M_PI_2);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x, y);
  }

  cairo_close_path(_cairo_context);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawRoundRectangle(jrect_t<int> rect, int dx, int dy, jrect_corner_t corners)
{
  if (dx < 1 or dy < 1) {
    throw std::invalid_argument("dx and dy must be greater than 1");
  }

  if (rect.size.x <=0 || rect.size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();
  jpen_t pen = GetPen();

  int offset = pen.size/2 + pen.size%2;
  int x = rect.point.x + t.x + offset;
  int y = rect.point.y + t.y + offset;
  int w = rect.size.x - 2*offset;
  int h = rect.size.y - 2*offset;

  if (dx > rect.size.x/2) {
    dx = rect.size.x/2;
  }

  if (dy > rect.size.y/2) {
    dy = rect.size.y/2;
  }

  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopRight)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + w - dx, y + dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, -M_PI_2, 0.0);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x + w, y);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomRight)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + w - dx, y + h - dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_PI_2);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x + w, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::BottomLeft)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + dx, y + h - dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI_2, M_PI);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x, y + h);
  }

  if (jenum_t<jrect_corner_t>{corners}.And(jrect_corner_t::TopLeft)) {
    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, x + dx, y + dy);
    cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI, M_PI+M_PI_2);
    cairo_restore(_cairo_context);
  } else {
    cairo_line_to(_cairo_context, x, y);
  }

  cairo_close_path(_cairo_context);
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, abs(pen.size));
  cairo_stroke(_cairo_context);
}

void Graphics::FillCircle(jpoint_t<int> point, int rp)
{
  FillArc(point, {rp, rp}, 0.0, M_2PI);
}

void Graphics::DrawCircle(jpoint_t<int> point, int rp)
{
  DrawArc(point, {rp, rp}, 0.0, 2*M_PI);
}

void Graphics::FillEllipse(jpoint_t<int> point, jpoint_t<int> size)
{
  FillArc(point, size, 0.0, M_2PI);
}

void Graphics::DrawEllipse(jpoint_t<int> point, jpoint_t<int> size)
{
  DrawArc(point, size, 0.0, 2*M_PI);
}

void Graphics::FillChord(jpoint_t<int> point, jpoint_t<int> size, float arc0, float arc1)
{
  struct jpoint_t<int> t = Translate();

  arc0 = M_2PI - arc0;
  arc1 = M_2PI - arc1;

  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, point.x + t.x, point.y + t.y);
  cairo_scale(_cairo_context, size.x, size.y);
  cairo_arc(_cairo_context, 0.0, 0.0, 1.0, arc1, arc0);
  cairo_close_path(_cairo_context);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawChord(jpoint_t<int> point, jpoint_t<int> size, float arc0, float arc1)
{
  struct jpoint_t<int> t = Translate();
  jpen_t pen = GetPen();

  int offset = pen.size/2 + pen.size%2;
  int xc = point.x + t.x;
  int yc = point.y + t.y;
  int rx = size.x - offset;
  int ry = size.y - offset;

  arc0 = M_2PI - arc0;
  arc1 = M_2PI - arc1;

  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, xc, yc);
  cairo_scale(_cairo_context, rx, ry);
  cairo_arc(_cairo_context, 0.0, 0.0, 1.0, arc1, arc0);
  cairo_close_path(_cairo_context);
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, pen.size);
  cairo_stroke(_cairo_context);
}

void Graphics::FillArc(jpoint_t<int> point, jpoint_t<int> size, float arc0, float arc1)
{
  if (size.x <= 0 or size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  arc0 = M_2PI - arc0;
  arc1 = M_2PI - arc1;

  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, point.x + t.x, point.y + t.y);
  cairo_scale(_cairo_context, size.x, size.y);
  cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
  cairo_line_to(_cairo_context, 0, 0);
  cairo_close_path(_cairo_context);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawArc(jpoint_t<int> point, jpoint_t<int> size, float arc0, float arc1)
{
  if (size.x <= 0 or size.y <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();
  jpen_t pen = GetPen();

  int offset = pen.size/2 + pen.size%2;
  int xc = point.x + t.x;
  int yc = point.y + t.y;
  int rx = size.x - offset;
  int ry = size.y - offset;

  arc0 = M_2PI - arc0;
  arc1 = M_2PI - arc1;

  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, xc, yc);
  cairo_scale(_cairo_context, rx, ry);
  cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, abs(pen.size));
  cairo_stroke(_cairo_context);
}

void Graphics::FillPie(jpoint_t<int> point, jpoint_t<int> size, float arc0, float arc1)
{
  FillArc(point, size, arc0, arc1);
}

void Graphics::DrawPie(jpoint_t<int> point, jpoint_t<int> size, float arc0, float arc1)
{
  struct jpoint_t<int> t = Translate();
  jpen_t pen = GetPen();

  int offset = pen.size/2 + pen.size%2;
  int xc = point.x + t.x;
  int yc = point.y + t.y;
  int rx = size.x - offset;
  int ry = size.y - offset;

  arc0 = M_2PI - arc0;
  arc1 = M_2PI - arc1;

  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, xc, yc);
  cairo_scale(_cairo_context, rx, ry);
  cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
  cairo_line_to(_cairo_context, 0, 0);
  cairo_close_path(_cairo_context);
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, abs(pen.size));
  cairo_stroke(_cairo_context);
}
    
void Graphics::FillTriangle(jpoint_t<int> p0, jpoint_t<int> p1, jpoint_t<int> p2)
{
  FillPolygon({0, 0}, {p0, p1, p2}, true);
}

void Graphics::DrawTriangle(jpoint_t<int> p0, jpoint_t<int> p1, jpoint_t<int> p2)
{
  DrawPolygon({0, 0}, {p0, p1, p2}, true);
}

void Graphics::DrawPolygon(jpoint_t<int> point, std::vector<jpoint_t<int>> points, bool closed)
{
  if (points.size() < 1 or _pen.size <= 0) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  for (int i=0; i<(int)points.size(); i++) {
    points[i].x = points[i].x + point.x + t.x;
    points[i].y = points[i].y + point.y + t.y;
  }
  
  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);
  cairo_move_to(_cairo_context, points[0].x, points[0].y);
  
  for (int i=1; i<(int)points.size(); i++) {
    cairo_line_to(_cairo_context, points[i].x, points[i].y);
  }

  if (closed == true) {
    cairo_close_path(_cairo_context);
  }
  
  cairo_restore(_cairo_context);
  cairo_set_line_width(_cairo_context, _pen.size);
  cairo_stroke(_cairo_context);
}

void Graphics::FillPolygon(jpoint_t<int> point, std::vector<jpoint_t<int>> points, bool even_odd)
{
  if (points.size() < 1) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  for (int i=0; i<(int)points.size(); i++) {
    points[i].x = points[i].x + point.x + t.x;
    points[i].y = points[i].y + point.y + t.y;
  }
  
  cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);
  cairo_move_to(_cairo_context, points[0].x, points[0].y);
  
  for (int i=1; i <(int)points.size(); i++) {
    cairo_line_to(_cairo_context, points[i].x, points[i].y);
  }

  cairo_close_path(_cairo_context);  
  cairo_restore(_cairo_context);

  if (even_odd == true) {
    cairo_set_fill_rule(_cairo_context, CAIRO_FILL_RULE_EVEN_ODD);
  }

  cairo_fill(_cairo_context);
  
  if (even_odd == true) {
    cairo_set_fill_rule(_cairo_context, CAIRO_FILL_RULE_WINDING);
  }
}

void Graphics::SetGradientStop(float stop, const jcolor_t<float> &color)
{
  jgradient_t t;

  t.color = color;
  t.stop = stop;

  _gradient_stops.push_back(t);
}

void Graphics::ResetGradientStop()
{
  _gradient_stops.clear();
}

void Graphics::FillRadialGradient(jpoint_t<int> point, jpoint_t<int> size, jpoint_t<int> offset, int r0p)
{
  struct jpoint_t<int> t = Translate();

  int xc = point.x + t.x;
  int yc = point.y + t.y;
  int rx = size.x;
  int ry = size.y;
  int x0 = xc + offset.x;
  int y0 = yc + offset.y;
  int r0 = r0p;

  cairo_pattern_t *pattern = cairo_pattern_create_radial(xc, yc, std::max(rx, ry), x0, y0, r0);

  for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
    jgradient_t 
      gradient = (*i);
    jcolor_t<float> 
      &color = gradient.color;

    cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, color.red, color.green, color.blue, color.alpha);
  }

  cairo_set_source(_cairo_context, pattern);
  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, xc, yc);
  cairo_scale(_cairo_context, rx, ry);
  cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_2PI);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
  cairo_pattern_destroy(pattern);
}

void Graphics::FillLinearGradient(jrect_t<int> rect, jpoint_t<int> p0, jpoint_t<int> p1)
{
  struct jpoint_t<int> t = Translate();

  cairo_pattern_t *pattern = cairo_pattern_create_linear(p0.x, p0.y, p1.x, p1.y);
  
  for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
    jgradient_t 
      gradient = (*i);
    jcolor_t<float> 
      &color = gradient.color;

    cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, color.red, color.green, color.blue, color.alpha);
  }
  
  cairo_save(_cairo_context);
  cairo_translate(_cairo_context, rect.point.x + t.x, rect.point.y + t.y);
  cairo_rectangle(_cairo_context, 0, 0, rect.size.x, rect.size.y);
  cairo_set_source(_cairo_context, pattern);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
  cairo_pattern_destroy(pattern);
  
  SetCompositeFlags(_composite);
  SetBlittingFlags(_blitting);
}

void Graphics::DrawString(std::string text, jpoint_t<int> point)
{
  if (_font == nullptr) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  std::shared_ptr<Font> font = GetFont();
  std::string utf8 = text;
  cairo_glyph_t *glyphs = nullptr;
  int glyphs_len = 0;
  cairo_status_t status;

  if (_font->GetEncoding() == jfont_encoding_t::Latin1) {
    utf8 = jmixin::latin1_to_utf8(text);
  }

  status = cairo_scaled_font_text_to_glyphs(
      font->GetScaledFont(), point.x + t.x, point.y + t.y + _font->GetAscender(), utf8.c_str(), utf8.size(), &glyphs, &glyphs_len, nullptr, nullptr, nullptr);

  if (status == CAIRO_STATUS_SUCCESS) {
    cairo_show_glyphs(_cairo_context, glyphs, glyphs_len);
    cairo_glyph_free(glyphs);
  }

  cairo_stroke(_cairo_context);
}

void Graphics::DrawGlyph(int symbol, jpoint_t<int> point)
{
  if (_font == nullptr) {
    return;
  }

  struct jpoint_t<int> t = Translate();

  cairo_glyph_t glyph;

  glyph.x = point.x + t.x;
  glyph.y = point.y + t.y + _font->GetAscender();
  glyph.index = symbol;

  cairo_show_glyphs(_cairo_context, &glyph, 1);
}

void Graphics::Translate(jpoint_t<int> point)
{
  _translate.x += point.x;
  _translate.y += point.y;
}

jpoint_t<int> Graphics::Translate()
{
  return _translate;
}

void Graphics::DrawString(std::string text, jrect_t<int> rect, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
  if (_font == nullptr or _font->GetSize() <= 0 or rect.size.x <= 0 or rect.size.y <= 0) {
    return;
  }

  struct line_t {
    std::string text;
    int dx;
    int dy;
    float sx;
  };

  std::vector<std::string> 
    lines;
  std::vector<struct line_t>
    words;
  int 
    height = _font->GetSize();

  _font->GetStringBreak(&lines, text, rect.size);

  if (halign == jhorizontal_align_t::Left) {
    for (int i=0; i<(int)lines.size(); i++) {
      words.push_back({
          .text = lines[i],
          .dx = 0,
          .dy = i*height,
          .sx = 0.0f
      });
    }
  } else if (halign == jhorizontal_align_t::Center) {
    for (int i=0; i<(int)lines.size(); i++) {
      words.push_back({
          .text = lines[i],
          .dx = (rect.size.x - _font->GetStringWidth(lines[i]))/2,
          .dy = i*height,
          .sx = 0.0f
      });
    }
  } else if (halign == jhorizontal_align_t::Right) {
    for (int i=0; i<(int)lines.size(); i++) {
      words.push_back({
          .text = lines[i],
          .dx = rect.size.x - _font->GetStringWidth(lines[i]),
          .dy = i*height,
          .sx = 0.0f
      });
    }
  } else if (halign == jhorizontal_align_t::Justify) {
    for (int i=0; i<(int)lines.size(); i++) {
      auto tokens = jmixin::String(lines[i]).split(std::string(" "));

      if (tokens.size() <= 1) {
        words.push_back({
            .text = lines[i],
            .dx = 0,
            .dy = i*height,
            .sx = 0.0f
        });
      } else {
        words.push_back({
            .text = lines[i],
            .dx = 0,
            .dy = i*height,
            .sx = (rect.size.x - _font->GetStringWidth(jmixin::join(tokens, "")))/(float)(tokens.size() - 1)
        });
      }
    }
  }

  if (rect.size.y > (int)(lines.size()*height)) {
    if (valign == jvertical_align_t::Top) {
      // for (auto &line : words) {
      // }
    } else if (valign == jvertical_align_t::Center) {
      int d = (rect.size.y - lines.size()*height)/2;
      
      for (auto &line : words) {
        line.dy = line.dy + d;
      }
    } else if (valign == jvertical_align_t::Bottom) {
      int d = rect.size.y - lines.size()*height;
      
      for (auto &line : words) {
        line.dy = line.dy + d;
      }
    } else if (valign == jvertical_align_t::Justify) {
      if (lines.size() > 1) {
        float d = (rect.size.y - lines.size()*height)/(float)(lines.size() - 1);
        int k = 0;

        for (auto &line : words) {
          line.dy = line.dy + d*k++;
        }
      }
    }
  }

  jrect_t<int> clip = GetClip();

  if (clipped == true) {
    ClipRect(rect);
  }
  
  for (auto &line : words) {
    if (line.sx == 0.0f) {
      DrawString(line.text, jpoint_t<int>{rect.point.x + line.dx, rect.point.y + line.dy});
    } else {
      float d = 0.0f;

      jmixin::String(line.text)
        .split(std::string(" "))
        .for_each(
            [&](auto word) {
              DrawString(word, jpoint_t<int>{rect.point.x + line.dx + (int)d, rect.point.y + line.dy});

              d = d + _font->GetStringWidth(word) + line.sx;
            });
    }
  }

  if (clipped == true) {
    SetClip(clip);
  }
}

uint32_t Graphics::GetRGB(jpoint_t<int> point, uint32_t pixel)
{
  struct jpoint_t<int> t = Translate();

  int x = point.x + t.x;
  int y = point.y + t.y;
  int sw = cairo_image_surface_get_width(_cairo_surface);
  int sh = cairo_image_surface_get_height(_cairo_surface);
  
  if ((x < 0 || x > sw) || (y < 0 || y > sh)) {
    throw std::range_error("Index out of bounds");
  }

  uint8_t *data = cairo_image_surface_get_data(_cairo_surface);

  if (data == nullptr) {
    return pixel;
  }

  int stride = cairo_image_surface_get_stride(_cairo_surface);
  
  return *((uint32_t *)(data + y * stride) + x);
}

void Graphics::GetRGBArray(uint32_t *rgb, jrect_t<int> rect)
{
  if (rgb == nullptr) {
    throw std::invalid_argument("Pixel array is null");
  }

  struct jpoint_t<int> t = Translate();

  int x = rect.point.x + t.x;
  int y = rect.point.y + t.y;
  int sw = cairo_image_surface_get_width(_cairo_surface);
  int sh = cairo_image_surface_get_height(_cairo_surface);
  
  if ((x < 0 || (x + rect.size.x) > sw) || (y < 0 || (y + rect.size.y) > sh)) {
    throw std::range_error("Index out of bounds");
  }

  uint8_t *data = cairo_image_surface_get_data(_cairo_surface);

  if (data == nullptr) {
    return;
  }

  uint32_t *ptr = rgb;

  if (ptr == nullptr) {
    throw std::invalid_argument("Destination buffer must be valid");
  }

  int stride = cairo_image_surface_get_stride(_cairo_surface);

  if (_pixelformat == jpixelformat_t::ARGB) {
    for (int j=0; j<rect.size.y; j++) {
      uint8_t *src = (uint8_t *)(data + (y + j) * stride + x * 4);
      uint8_t *dst = (uint8_t *)(ptr + j * rect.size.x);
      int si = 0;
      int di = 0;

      for (int i=0; i<rect.size.x; i++) {
        int alpha = *(src + si + 3);
        *(dst + di + 3) = alpha;
        *(dst + di + 2) = ALPHA_DEMULTIPLY(*(src + si + 2), alpha);
        *(dst + di + 1) = ALPHA_DEMULTIPLY(*(src + si + 1), alpha);
        *(dst + di + 0) = ALPHA_DEMULTIPLY(*(src + si + 0), alpha);

        si = si + 4;
        di = di + 4;
      }
    }
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    for (int j=0; j<rect.size.y; j++) {
      uint8_t *src = (uint8_t *)(data + (y + j) * stride + x * 4);
      uint8_t *dst = (uint8_t *)(ptr + j * rect.size.x);
      int si = 0;
      int di = 0;

      for (int i=0; i<rect.size.x; i++) {
        *(dst + di + 3) = *(src + si + 3);
        *(dst + di + 2) = *(src + si + 2);
        *(dst + di + 1) = *(src + si + 1);
        *(dst + di + 0) = *(src + si + 0);

        si = si + 4;
        di = di + 4;
      }
    }
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    for (int j=0; j<rect.size.y; j++) {
      uint8_t *src = (uint8_t *)(data + (y + j) * stride + x * 3);
      uint8_t *dst = (uint8_t *)(ptr + j * rect.size.x);
      int si = 0;
      int di = 0;

      for (int i=0; i<rect.size.x; i++) {
        uint8_t 
          p0 = *(src + si + 0),
          p1 = *(src + si + 1);

        *(dst + di + 3) = 0xff;
        *(dst + di + 2) = p0 & 0xf8;
        *(dst + di + 1) = (p0 & 0x07) << 0x05 | (p1 & 0x0d) >> 0x03;
        *(dst + di + 0) = p1 & 0xf8;

        si = si + 2; 
        di = di + 4;
      }
    }
  } else {
    throw std::runtime_error("Invalid pixel format");
  }
}

void Graphics::SetRawRGB(uint32_t rgb, jpoint_t<int> point) 
{
  struct jpoint_t<int> t = Translate();

  point = point + t;

  int sw = cairo_image_surface_get_width(_cairo_surface);
  int sh = cairo_image_surface_get_height(_cairo_surface);
  
  if (point.x < 0 or point.y < 0 or point.x > sw or point.y > sh) {
    return;
  }

  uint8_t *data = cairo_image_surface_get_data(_cairo_surface);
  int stride = cairo_image_surface_get_stride(_cairo_surface);

  *(uint32_t *)(data + point.y * stride + point.x * stride / sw) = rgb;
  
  cairo_surface_mark_dirty(_cairo_surface);
}

void Graphics::SetRGB(uint32_t rgb, jpoint_t<int> point) 
{
  try {
    SetRGBArray(&rgb, {point, {1, 1}});
  } catch (...) {
  }
}

void Graphics::SetRGBArray(const uint32_t *rgb, jrect_t<int> rect) 
{
  if (rgb == nullptr) {
    throw std::invalid_argument("Pixel array is null");
  }

  struct jpoint_t<int> t = Translate();

  int x = rect.point.x + t.x;
  int y = rect.point.y + t.y;
  int sw = cairo_image_surface_get_width(_cairo_surface);
  int sh = cairo_image_surface_get_height(_cairo_surface);
  
  if ((x < 0 || (x + rect.size.x) > sw) || (y < 0 || (y + rect.size.y) > sh)) {
    // throw std::range_error("Index out of bounds");
  }

  uint8_t *data = cairo_image_surface_get_data(_cairo_surface);
  int stride = cairo_image_surface_get_stride(_cairo_surface);

  if (data == nullptr) {
    return;
  }

  int step = stride / sw;

  if (_pixelformat == jpixelformat_t::ARGB) {
    for (int j=0; j<rect.size.y; j++) {
      if ((y + j) < 0 or (y + j) >= sh) {
        continue;
      }

			if (x < 0) {
				rect.size.x = rect.size.x + x;
			}

			if (x + rect.size.x > sw) {
				rect.size.x = rect.size.x - x;
			}

      uint8_t *src = (uint8_t *)(rgb + j * rect.size.x);
      uint8_t *dst = (uint8_t *)(data + (y + j) * stride + x*step);
      int si = 0;
      int di = 0;

      for (int i=0; i<rect.size.x; i++) {
        int a = *(src + si + 3);
        int r = *(src + si + 2);
        int g = *(src + si + 1);
        int b = *(src + si + 0);
        int alpha = *(dst + di + 3);
        int pa = alpha;
        int pr = ALPHA_DEMULTIPLY(*(dst + di + 2), pa);
        int pg = ALPHA_DEMULTIPLY(*(dst + di + 1), pa);
        int pb = ALPHA_DEMULTIPLY(*(dst + di + 0), pa);

        if (_composite == jcomposite_flags_t::Clear) {
          pr = 0x00;
          pg = 0x00;
          pb = 0x00;
          pa = 0x00;
        } else if (_composite == jcomposite_flags_t::Src) {
          pr = r;
          pg = g;
          pb = b;
          pa = a;
        } else if (_composite == jcomposite_flags_t::SrcOver) {
          float fr = r/255.0;
          float fg = g/255.0;
          float fb = b/255.0;
          float fa = a/255.0;
          float fpr = pr/255.0;
          float fpg = pg/255.0;
          float fpb = pb/255.0;
          float fpa = pa/255.0;
          float ar = fa + fpa*(1.0 - fa);

          pr = (int)(((fa*fr + fpa*fpr*(1.0 - fa))/ar)*255);
          pg = (int)(((fa*fg + fpa*fpg*(1.0 - fa))/ar)*255);
          pb = (int)(((fa*fb + fpa*fpb*(1.0 - fa))/ar)*255);
          pa = (int)(ar * 255);
        } else if (_composite == jcomposite_flags_t::SrcIn) {
          pr = r;
          pg = g;
          pb = b;
          pa = (a*pa) >> 0x08;
        } else if (_composite == jcomposite_flags_t::SrcOut) {
          pr = r;
          pg = g;
          pb = b;
          pa = (a*(0xff-pa)) >> 0x08;
        } else if (_composite == jcomposite_flags_t::SrcAtop) {
          pr = (int)(a*r + pr*(0xff-a)) >> 0x08;
          pg = (int)(a*g + pg*(0xff-a)) >> 0x08;
          pb = (int)(a*b + pb*(0xff-a)) >> 0x08;
          // pa = pa;
        } else if (_composite == jcomposite_flags_t::Dst) {
          pr = pr;
          pg = pg;
          pb = pb;
          pa = pa;
        } else if (_composite == jcomposite_flags_t::DstOver) {
          float fr = r/255.0;
          float fg = g/255.0;
          float fb = b/255.0;
          float fa = a/255.0;
          float fpr = pr/255.0;
          float fpg = pg/255.0;
          float fpb = pb/255.0;
          float fpa = pa/255.0;
          float ar = fpa + fa*(1.0 - fpa);

          pr = (int)(((fpa*fpr + fa*fr*(1.0 - fpa))/ar)*255);
          pg = (int)(((fpa*fpg + fa*fg*(1.0 - fpa))/ar)*255);
          pb = (int)(((fpa*fpb + fa*fb*(1.0 - fpa))/ar)*255);
          pa = (int)(ar * 255);
        } else if (_composite == jcomposite_flags_t::DstIn) {
          // pr = pr;
          // pg = pg;
          // pb = pb;
          pa = (a*pa) >> 0x08;
        } else if (_composite == jcomposite_flags_t::DstOut) {
          pr = pr;
          pg = pg;
          pb = pb;
          pa = (pa*(0xff-a)) >> 0x08;
        } else if (_composite == jcomposite_flags_t::DstAtop) {
          pr = (int)(pr*pa + r*(0xff-pa)) >> 0x08;
          pg = (int)(pg*pa + g*(0xff-pa)) >> 0x08;
          pb = (int)(pb*pa + b*(0xff-pa)) >> 0x08;
          pa = a;
        } else if (_composite == jcomposite_flags_t::Xor) {
          float fr = r/255.0;
          float fg = g/255.0;
          float fb = b/255.0;
          float fa = a/255.0;
          float fpr = pr/255.0;
          float fpg = pg/255.0;
          float fpb = pb/255.0;
          float fpa = pa/255.0;
          float ar = fa + fpa - 2*fa*fpa;

          pr = (int)(((fa*fr*(1.0 - fpa) + fpa*fpr*(1.0 - fa))/ar) * 255);
          pg = (int)(((fa*fg*(1.0 - fpa) + fpa*fpg*(1.0 - fa))/ar) * 255);
          pb = (int)(((fa*fb*(1.0 - fpa) + fpa*fpb*(1.0 - fa))/ar) * 255);
          pa = (int)(ar * 255);
        } else if (_composite == jcomposite_flags_t::Add) {
          float fr = r/255.0;
          float fg = g/255.0;
          float fb = b/255.0;
          float fa = a/255.0;
          float fpr = pr/255.0;
          float fpg = pg/255.0;
          float fpb = pb/255.0;
          float fpa = pa/255.0;
          float ar = fa + fpa;

          if (ar > 1.0) {
            ar = 1.0;
          }

          pr = (int)(((fa*fr + fpa*fpr)/ar) * 255);
          pg = (int)(((fa*fg + fpa*fpg)/ar) * 255);
          pb = (int)(((fa*fb + fpa*fpb)/ar) * 255);
          pa = (int)(ar * 255);
        } else if (_composite == jcomposite_flags_t::Saturate) {
          float fr = r/255.0;
          float fg = g/255.0;
          float fb = b/255.0;
          float fa = a/255.0;
          float fpr = pr/255.0;
          float fpg = pg/255.0;
          float fpb = pb/255.0;
          float fpa = pa/255.0;
          float ar = fa + fpa;
          float ma = 1.0 - fpa;

          if (ar > 1.0) {
            ar = 1.0;
          }

          if (ma > fa) {
            ma = fa;
          }

          pr = (int)(((fr*ma + fpa*fpr)/ar) * 255);
          pg = (int)(((fg*ma + fpa*fpg)/ar) * 255);
          pb = (int)(((fb*ma + fpa*fpb)/ar) * 255);
          pa = (int)(ar * 255);
        } else {
          float fr = r/255.0;
          float fg = g/255.0;
          float fb = b/255.0;
          float fa = a/255.0;
          float fpr = pr/255.0;
          float fpg = pg/255.0;
          float fpb = pb/255.0;
          float fpa = pa/255.0;
          float ar = fa + fpa*(1.0 - fa);
          float c1 = 1.0/ar;
          float c2 = (1.0 - fpa)*fa;
          float c3 = (1.0 - fa)*fpa;
          float c4 = fa*fpa;
          float fxr = 0.0;
          float fxg = 0.0;
          float fxb = 0.0;

          if (_composite == jcomposite_flags_t::Multiply) {
            fxr = (fr*fpr);
            fxg = (fg*fpg);
            fxb = (fb*fpb);
          } else if (_composite == jcomposite_flags_t::Screen) {
            fxr = (fr+fpr - (fr*fpr));
            fxg = (fg+fpg - (fg*fpg));
            fxb = (fb+fpb - (fb*fpb));
          } else if (_composite == jcomposite_flags_t::Overlay) {
            fxr = ((fpr <= 0.5)?(2.0*fr*fpr):(1.0 - 2.0*(1.0 - fr)*(1.0 - fpr)));
            fxg = ((fpg <= 0.5)?(2.0*fg*fpg):(1.0 - 2.0*(1.0 - fg)*(1.0 - fpg)));
            fxb = ((fpb <= 0.5)?(2.0*fb*fpb):(1.0 - 2.0*(1.0 - fb)*(1.0 - fpb)));
          } else if (_composite == jcomposite_flags_t::Darken) {
            fxr = ((fr < fpr)?fr:fpr);
            fxg = ((fg < fpg)?fg:fpg);
            fxb = ((fb < fpb)?fb:fpb);
          } else if (_composite == jcomposite_flags_t::Lighten) {
            fxr = ((fr > fpr)?fr:fpr);
            fxg = ((fg > fpg)?fg:fpg);
            fxb = ((fb > fpb)?fb:fpb);
          } else if (_composite == jcomposite_flags_t::Difference) {
            fxr = (abs(fr-fpr));
            fxg = (abs(fg-fpg));
            fxb = (abs(fb-fpb));
          } else if (_composite == jcomposite_flags_t::Exclusion) {
            fxr = (fr+fpr - 2*(fr*fpr));
            fxg = (fg+fpg - 2*(fg*fpg));
            fxb = (fb+fpb - 2*(fb*fpb));
          } else if (_composite == jcomposite_flags_t::Dodge) {
            float cr = fpr/(1.0 - fr);
            float cg = fpg/(1.0 - fg);
            float cb = fpb/(1.0 - fb);

            if (cr > 1.0) {
              cr = 1.0;
            }

            if (cg > 1.0) {
              cg = 1.0;
            }

            if (cb > 1.0) {
              cb = 1.0;
            }

            fxr = (fr < 1.0)?cr:1.0;
            fxg = (fg < 1.0)?cg:1.0;
            fxb = (fb < 1.0)?cb:1.0;
          } else if (_composite == jcomposite_flags_t::Burn) {
            float cr = (1.0 - fpr)/fr;
            float cg = (1.0 - fpg)/fg;
            float cb = (1.0 - fpb)/fb;

            if (cr > 1.0) {
              cr = 1.0;
            }

            if (cg > 1.0) {
              cg = 1.0;
            }

            if (cb > 1.0) {
              cb = 1.0;
            }

            fxr = (fr > 0.0)?(1.0-cr):0.0;
            fxg = (fg > 0.0)?(1.0-cg):0.0;
            fxb = (fb > 0.0)?(1.0-cb):0.0;
          } else if (_composite == jcomposite_flags_t::Hard) {
            fxr = (fr <= 0.5)?(2*fr*fpr):(1.0 - 2*(1.0 - fr)*(1.0 - fpr));
            fxg = (fg <= 0.5)?(2*fg*fpg):(1.0 - 2*(1.0 - fg)*(1.0 - fpg));
            fxb = (fb <= 0.5)?(2*fb*fpb):(1.0 - 2*(1.0 - fb)*(1.0 - fpb));
          } else if (_composite == jcomposite_flags_t::Light) {
            float gr = (fpr <= 0.25)?(((16*fpr - 12)*fpr + 4)*fpr):(sqrt(fpr));
            float gg = (fpg <= 0.25)?(((16*fpg - 12)*fpg + 4)*fpg):(sqrt(fpg));
            float gb = (fpb <= 0.25)?(((16*fpb - 12)*fpb + 4)*fpb):(sqrt(fpb));

            fxr = (fr <= 0.5)?(fpr - (1.0 - 2*fr)*fpr*(1.0 - fpr)):(fpr + (2*fr - 1.0)*(gr - fpr));
            fxg = (fg <= 0.5)?(fpg - (1.0 - 2*fg)*fpg*(1.0 - fpg)):(fpg + (2*fg - 1.0)*(gg - fpg));
            fxb = (fb <= 0.5)?(fpb - (1.0 - 2*fb)*fpb*(1.0 - fpb)):(fpb + (2*fb - 1.0)*(gb - fpb));
          }
            
          pr = (int)((c1 * (c2*fr + c3*fpr + c4*fxr)) * 255);
          pg = (int)((c1 * (c2*fg + c3*fpg + c4*fxg)) * 255);
          pb = (int)((c1 * (c2*fb + c3*fpb + c4*fxb)) * 255);
          pa = (int)(ar * 255);
        }

        pr = (pr > 0xff)?0xff:pr;
        pg = (pg > 0xff)?0xff:pg;
        pb = (pb > 0xff)?0xff:pb;
        pa = (pa > 0xff)?0xff:pa;

        *(dst + di + 3) = pa;
        *(dst + di + 2) = ALPHA_PREMULTIPLY(pr, pa);
        *(dst + di + 1) = ALPHA_PREMULTIPLY(pg, pa);
        *(dst + di + 0) = ALPHA_PREMULTIPLY(pb, pa);

        si = si + 4;
        di = di + 4;
      }
    }
  } else if (_pixelformat == jpixelformat_t::RGB32) {
    for (int j=0; j<rect.size.y; j++) {
      if ((y + j) < 0 or (y + j) >= sh) {
        continue;
      }

			if (x < 0) {
				rect.size.x = rect.size.x + x;
			}

			if (x + rect.size.x > sw) {
				rect.size.x = rect.size.x - x;
			}

			if (rect.size.x > 0) {
      	uint32_t *src = (uint32_t *)(rgb + j * rect.size.x);
      	uint32_t *dst = (uint32_t *)(data + (y + j) * stride + x * step);

			  memcpy(dst, src, rect.size.x*4);
			}
    }
  } else if (_pixelformat == jpixelformat_t::RGB16) {
    for (int j=0; j<rect.size.y; j++) {
      if ((y + j) < 0 or (y + j) >= sh) {
        continue;
      }

			if (x < 0) {
				rect.size.x = rect.size.x + x;
			}

			if (x + rect.size.x > sw) {
				rect.size.x = rect.size.x - x;
			}

			if (rect.size.x > 0) {
				uint8_t *src = (uint8_t *)(rgb + j * rect.size.x);
				uint8_t *dst = (uint8_t *)(data + (y + j) * stride + x * step);
				int si = 0;
				int di = 0;

				for (int i=0; i<rect.size.x; i++) {
					int r = *(src + si + 2) & 0xf8;
					int g = *(src + si + 1) = 0xfc;
					int b = *(src + si + 0) & 0xf8;

					*(dst + di + 1) = (r << 0x03 | g >> 0x05);
					*(dst + di + 0) = (g << 0x03 | b >> 0x03);

					si = si + 4;
					di = di + 2;
				}
			}
    }
  } else {
    throw std::runtime_error("Invalid pixel format");
  }
  
  cairo_surface_mark_dirty(_cairo_surface);
}

bool Graphics::DrawImage(std::shared_ptr<Image> img, jpoint_t<int> point)
{
  if (img == nullptr) {
    return false;
  }

  Graphics *g = img->GetGraphics();

  if (g != nullptr) {
    jpoint_t<int> isize = img->GetSize();
    jpoint_t<int> t = Translate();

    cairo_surface_t *cairo_surface = g->GetCairoSurface();

    cairo_save(_cairo_context);
    cairo_translate(_cairo_context, point.x + t.x, point.y + t.y);
    cairo_set_source_surface(_cairo_context, cairo_surface, 0, 0);
  
    SetBlittingFlags(_blitting);
  	
    cairo_rectangle(_cairo_context, 0, 0, isize.x, isize.y);
  	cairo_fill(_cairo_context);
    cairo_restore(_cairo_context);
  } else {
    jpoint_t<int> size = img->GetSize();
    uint32_t *rgb = new uint32_t[size.x*size.y];

    img->GetRGBArray(rgb, {0, 0, size.x, size.y});
  
    SetRGBArray(rgb, {point, size});

    delete [] rgb;
  }

  return true;
}

bool Graphics::DrawImage(std::shared_ptr<Image> img, jrect_t<int> dst)
{
  if (img == nullptr or dst.size.x <= 0 or dst.size.x > IMAGE_LIMIT or dst.size.y <= 0 or dst.size.y > IMAGE_LIMIT) {
    return false;
  }

  Graphics *g = img->GetGraphics();

  if (g != nullptr) {
    jpoint_t<int> isize = img->GetSize();
    jpoint_t<int> t = Translate();

    float dx = dst.size.x/(float)isize.x;
    float dy = dst.size.y/(float)isize.y;

    cairo_save(_cairo_context);
    cairo_surface_t *cairo_surface = g->GetCairoSurface();
		cairo_translate(_cairo_context, dst.point.x + t.x, dst.point.y + t.y);
    cairo_scale(_cairo_context, dx, dy);
    cairo_set_source_surface(_cairo_context, cairo_surface, 0, 0);
    
    SetBlittingFlags(_blitting);
    
    cairo_scale(_cairo_context, 1.0f/dx, 1.0f/dy);
  	cairo_rectangle(_cairo_context, 0, 0, dst.size.x, dst.size.y);
  	cairo_fill(_cairo_context);
    cairo_restore(_cairo_context);
  } else {
    std::shared_ptr<Image> scl = img->Scale(dst.size);

    if (scl == nullptr) {
      return false;
    }

    uint32_t *rgb = new uint32_t[dst.size.x*dst.size.y];

    scl->GetRGBArray(rgb, {0, 0, dst.size.x, dst.size.y});
  
    SetRGBArray(rgb, dst);
  
    delete [] rgb;
  }

  return true;
}

bool Graphics::DrawImage(std::shared_ptr<Image> img, jrect_t<int> src, jpoint_t<int> dst)
{
  if (img == nullptr) {
    return false;
  }

  return DrawImage(img, src, {dst, src.size});
}

bool Graphics::DrawImage(std::shared_ptr<Image> img, jrect_t<int> src, jrect_t<int> dst)
{
  if (img == nullptr or dst.size.x <= 0 or dst.size.x > IMAGE_LIMIT or dst.size.y <= 0 or dst.size.y > IMAGE_LIMIT) {
    return false;
  }

  Graphics *g = img->GetGraphics();

  if (g != nullptr) {
    struct jpoint_t<int> t = Translate();

    float dx = dst.size.x/(float)src.size.x;
    float dy = dst.size.y/(float)src.size.y;

    cairo_surface_t *cairo_surface = g->GetCairoSurface();

    cairo_save(_cairo_context);
		cairo_translate(_cairo_context, dst.point.x + t.x, dst.point.y + t.y);
    cairo_scale(_cairo_context, dx, dy);
    cairo_set_source_surface(_cairo_context, cairo_surface, -src.point.x, -src.point.y);

    SetBlittingFlags(_blitting);

    cairo_scale(_cairo_context, 1.0f/dx, 1.0f/dy);
  	cairo_rectangle(_cairo_context, 0, 0, dst.size.x, dst.size.y);
  	cairo_fill(_cairo_context);
    cairo_restore(_cairo_context);
  } else {
    std::shared_ptr<Image> aux = img->Crop({src.point, src.size});

    if (aux == nullptr) {
      return false;
    }

    std::shared_ptr<Image> scl = aux->Scale(dst.size);

    if (scl == nullptr) {
      return false;
    }

    uint32_t *rgb = new uint32_t[dst.size.x*dst.size.y];

    scl->GetRGBArray(rgb, {0, 0, dst.size.x, dst.size.y});
  
    SetRGBArray(rgb, dst);
  
    delete [] rgb;
  }

  return true;
}

void Graphics::SetPattern(std::shared_ptr<Image> image)
{
  if (image == nullptr or image->GetGraphics() == nullptr) {
    return;
  }

  // struct jpoint_t<int> t = Translate();

  // int x0 = xp+t.x;
  // int y0 = yp+t.y;
  // int x1 = wp+t.x;
  // int y1 = hp+t.y;

  cairo_pattern_t *pattern = cairo_pattern_create_for_surface(image->GetGraphics()->GetCairoSurface());

  cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
  cairo_set_source(_cairo_context, pattern);
  // cairo_rectangle(_cairo_context, x0, y0, x1-x0, y1-y0);
  cairo_fill(_cairo_context);
  cairo_pattern_destroy(pattern);
}

void Graphics::SetPattern(jpoint_t<int> p0, jpoint_t<int> p1)
{
  struct jpoint_t<int> t = Translate();

  int x0 = p0.x + t.x;
  int y0 = p0.y + t.y;
  int x1 = p1.x + t.x;
  int y1 = p1.y + t.y;
    
  // int x = x0;
  // int y = y0;
  // int w = x1-x0;
  // int h = y1-y0;

  // if (w < 0) {
  //  x = x1;
  //  w = -w;
  // }

  // if (h < 0) {
  //  y = y1;
  //  h = -h;
  // }

  cairo_pattern_t *pattern = cairo_pattern_create_linear(x0, y0, x1, y1);

  for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
    jgradient_t 
      gradient = (*i);
    jcolor_t<float> 
      &color = gradient.color;

    cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, color.red, color.green, color.blue, color.alpha);
  }

  // cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_set_source(_cairo_context, pattern);
  cairo_fill(_cairo_context);
  cairo_pattern_destroy(pattern);
}

void Graphics::SetPattern(jpoint_t<int> p0, int rad0, jpoint_t<int> p1, int rad1)
{
  cairo_t *cairo_context = _cairo_context;

  struct jpoint_t<int> t = Translate();

  int x0 = p0.x + t.x;
  int y0 = p0.y + t.y;
  int x1 = p1.x + t.x;
  int y1 = p1.y + t.y;
    
  cairo_pattern_t *pattern = cairo_pattern_create_radial(x0, y0, rad0, x1, y1, rad1);

  for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
    jgradient_t 
      gradient = (*i);
    jcolor_t<float> 
      &color = gradient.color;

    cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, color.red, color.green, color.blue, color.alpha);
  }

  // cairo_arc(cairo_context, x0, y0, rad0, 0.0, 2 * M_PI);
  cairo_set_source(cairo_context, pattern);
  cairo_fill(cairo_context);
  cairo_pattern_destroy(pattern);
}

void Graphics::MoveTo(jpoint_t<int> point)
{
  struct jpoint_t<int> t = Translate();

  cairo_move_to(_cairo_context, point.x + t.x, point.y);
}

void Graphics::LineTo(jpoint_t<int> point)
{
  struct jpoint_t<int> t = Translate();

  cairo_line_to(_cairo_context, point.x + t.x, point.y + t.y);
}

void Graphics::CurveTo(jpoint_t<int> p0, jpoint_t<int> p1, jpoint_t<int> p2)
{
  struct jpoint_t<int> t = Translate();

  cairo_curve_to(_cairo_context, p0.x + t.x, p0.y + t.y, p1.x + t.x, p1.y + t.y, p2.x + t.x, p2.y + t.y);
}

void Graphics::ArcTo(jpoint_t<int> point, int radius, float arc0, float arc1, bool negative)
{
  struct jpoint_t<int> t = Translate();

  arc0 = M_2PI - arc0;
  arc1 = M_2PI - arc1;

  if (negative == false) {
    cairo_arc(_cairo_context, point.x + t.x, point.y + t.y, radius, arc0, arc1);
  } else {
    cairo_arc_negative(_cairo_context, point.x + t.x, point.y + t.y, radius, arc0, arc1);
  }
}

void Graphics::TextTo(std::string text, jpoint_t<int> point)
{
  std::shared_ptr<Font> font = GetFont();

  if (font == nullptr) {
    return;
  }

  struct jpoint_t<int> t = Translate();
  std::string utf8 = text;
  cairo_glyph_t *glyphs = nullptr;
  int glyphs_len = 0;
  cairo_status_t status;

  if (font->GetEncoding() == jfont_encoding_t::Latin1) {
    utf8 = jmixin::latin1_to_utf8(text);
  }

  status = cairo_scaled_font_text_to_glyphs(
      font->GetScaledFont(), point.x + t.x, point.y + t.y + font->GetAscender(), utf8.c_str(), utf8.size(), &glyphs, &glyphs_len, nullptr, nullptr, nullptr);

  if (status == CAIRO_STATUS_SUCCESS) {
    cairo_glyph_path(_cairo_context, glyphs, glyphs_len);
    cairo_glyph_free(glyphs);
  }
}

void Graphics::Close()
{
  cairo_close_path(_cairo_context);
}

void Graphics::Stroke()
{
  int width = _pen.size;

  if (width < 0) {
    width = -width;
  }

  cairo_set_line_width(_cairo_context, width);
  cairo_stroke(_cairo_context);
}

void Graphics::Fill()
{
  cairo_fill(_cairo_context);
}

void Graphics::SetSource(std::shared_ptr<Image> image)
{
  if (image == nullptr or image->GetGraphics() == nullptr) {
    return;
  }

  cairo_set_source_surface(_cairo_context, image->GetGraphics()->GetCairoSurface(), 0, 0);
  
  SetBlittingFlags(_blitting);
}

void Graphics::SetMask(std::shared_ptr<Image> image)
{
  if (image == nullptr or image->GetGraphics() == nullptr) {
    return;
  }

  cairo_mask_surface(_cairo_context, image->GetGraphics()->GetCairoSurface(), 0, 0);
}

void Graphics::SetMatrix(float *)
{
}

void Graphics::GetMatrix(float **)
{
}

void Graphics::Reset()
{
  // _translate.x = 0;
  // _translate.y = 0;

  _pen.dashes.clear();
  _pen.join = jline_join_t::Miter;
  _pen.style = jline_style_t::Butt;
  _pen.size = 1;

  // ReleaseClip();
  SetAntialias(jantialias_t::Normal);
  SetPen(_pen);
  SetColor(0x00000000);
  ResetGradientStop();
  SetCompositeFlags(jcomposite_flags_t::SrcOver);
  SetBlittingFlags(jblitting_flags_t::Bilinear);
}

void Graphics::SetVerticalSyncEnabled(bool enabled)
{
  _is_vertical_sync_enabled = enabled;
}

bool Graphics::IsVerticalSyncEnabled()
{
  return _is_vertical_sync_enabled;
}

void Graphics::Flush()
{
  // cairo_surface_mark_dirty(_cairo_surface);
  cairo_surface_flush(_cairo_surface);
}

}
