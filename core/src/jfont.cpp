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
#include "jcanvas/core/jfont.h"
#include "jcanvas/core/jenum.h"

#include "jmixin/jstring.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace jcanvas {

static FT_Library sg_freetype;
static int sg_freetype_refcounter = 0;

#define DEFAULT_FONT_NAME "default"

Font::Font(std::string name, jfont_attributes_t attributes, int size, const jmatrix_t<3, 2, float> &m)
{
  _name = name;
  _size = size;
  _attributes = attributes;
  _encoding = jfont_encoding_t::Utf8;
  _cairo_face = nullptr;

  _font = nullptr;
  _is_builtin = false;

  if (sg_freetype_refcounter == 0) {
    FT_Init_FreeType(&sg_freetype);
  }

  int fd = open(name.c_str(), O_RDONLY);

  if (fd > 0) {
    close(fd);
  }

  if (fd < 0) {
    _is_builtin = true;
  } else {
    if (FT_New_Face(sg_freetype, name.c_str(), 0, &_face) != 0) {
      if (sg_freetype_refcounter == 0) {
        // FT_Done_FreeType(sg_freetype); // TODO:: use FT_New_Library instead of FT_Init_FreeType
      }

      throw std::runtime_error("Cannot load a native font");
    }

    FT_Select_Charmap(_face, ft_encoding_unicode);

    _font = cairo_ft_font_face_create_for_ft_face(_face, FT_LOAD_NO_AUTOHINT);
  }

  sg_freetype_refcounter = sg_freetype_refcounter + 1;

  cairo_surface_t *surface_ref = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);

  _context_ref = cairo_create(surface_ref);

  cairo_surface_destroy(surface_ref);

  // INFO:: DEFAULT, NONE, GRAY, SUBPIXEL, FAST, GOOD, BEST
  _options = cairo_font_options_create();

  cairo_font_options_set_antialias(_options, CAIRO_ANTIALIAS_SUBPIXEL);

  // INFO:: initializing font parameters
  if (_is_builtin == false) {
    int attr = 0;

    if (jenum_t<jfont_attributes_t>{_attributes}.And(jfont_attributes_t::Bold)) {
      attr = attr | CAIRO_FT_SYNTHESIZE_BOLD;
    }

    if (jenum_t<jfont_attributes_t>{_attributes}.And(jfont_attributes_t::Italic)) {
      attr = attr | CAIRO_FT_SYNTHESIZE_OBLIQUE;
    }

    cairo_ft_font_face_set_synthesize(_font, attr);
    cairo_set_font_face(_context_ref, (cairo_font_face_t *)_font);
  } else {
    cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
    cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;

    if (jenum_t<jfont_attributes_t>{_attributes}.And(jfont_attributes_t::Bold)) {
      weight = CAIRO_FONT_WEIGHT_BOLD;
    }

    if (jenum_t<jfont_attributes_t>{_attributes}.And(jfont_attributes_t::Italic)) {
      slant = CAIRO_FONT_SLANT_ITALIC;
    }

    cairo_font_face_t 
      *_cairo_face = cairo_toy_font_face_create(_name.c_str(), slant, weight);

    cairo_set_font_face(_context_ref, _cairo_face);
  }

  cairo_font_extents_t t;

  cairo_set_font_options(_context_ref, _options);
  cairo_set_font_size(_context_ref, _size);
  cairo_font_extents(_context_ref, &t);

  _ascender = t.ascent;
  _descender = t.descent;
  _leading = t.height - t.ascent - t.descent;
  _max_advance_width = t.max_x_advance;
  _max_advance_height = t.max_y_advance;
  
  // INFO:: creating a scaled font
  cairo_font_face_t *font_face = cairo_get_font_face(_context_ref);
  cairo_matrix_t fm;
  cairo_matrix_t tm;

  cairo_get_matrix(_context_ref, &tm);
  cairo_get_font_matrix(_context_ref, &fm);

  fm.xx *= m(0, 0); fm.yx *= m(0, 1);
  fm.xy *= m(1, 0); fm.yy *= m(1, 1);
  fm.x0 = m(2, 0); fm.y0 = m(2, 1);

  _scaled_font = cairo_scaled_font_create(font_face, &fm, &tm, _options);

  cairo_destroy(_context_ref);

  // INFO:: intializing the first 256 characters withs
  for (int i=0; i<256; i++) {
    jfont_extends_t t = Font::GetGlyphExtends(i);

    _widths[i] = t.bearing.x + t.size.x;
  }
}

Font::~Font()
{
  if (_cairo_face != nullptr) {
    cairo_font_face_destroy(_cairo_face);
  }

  cairo_scaled_font_destroy(_scaled_font);
  cairo_font_options_destroy(_options);
  
  if (_font != nullptr) {
    cairo_font_face_destroy(_font);
  }
  
  if (_is_builtin == false) {
    FT_Done_Face(_face);
  }

  if (--sg_freetype_refcounter == 0) {
    // FT_Done_FreeType(sg_freetype);
  }
}

void Font::ApplyContext(void *ctx)
{
  cairo_t *context = (cairo_t *)ctx;

  cairo_set_scaled_font(context, _scaled_font);
}

jfont_attributes_t Font::GetAttributes()
{
  return _attributes;
}

cairo_scaled_font_t * Font::GetScaledFont()
{
  return _scaled_font;
}

void Font::SetEncoding(jfont_encoding_t encoding)
{
  _encoding = encoding;
}

jfont_encoding_t Font::GetEncoding()
{
  return _encoding;
}

std::string Font::GetName()
{
  return _name;
}

int Font::GetSize()
{
  return GetAscender() + GetDescender() + GetLeading();
}

int Font::GetAscender()
{
  return _ascender;
}

int Font::GetDescender()
{
  return abs(_descender);
}

int Font::GetMaxAdvanceWidth()
{
  return _max_advance_width;
}

int Font::GetMaxAdvanceHeight()
{
  return _max_advance_height;
}

int Font::GetLeading()
{
  return _leading;
}

int Font::GetStringWidth(std::string text)
{
  jfont_extends_t t = GetStringExtends(text);

  return (int)(t.bearing.x + t.size.x);
}

jfont_extends_t Font::GetStringExtends(std::string text)
{
  std::string utf8 = text;
  cairo_text_extents_t t;

  if (GetEncoding() == jfont_encoding_t::Latin1) {
    utf8 = jmixin::latin1_to_utf8(text);
  }

  cairo_scaled_font_text_extents(_scaled_font, utf8.c_str(), &t);

  return jfont_extends_t {
    .bearing {
      (float)t.x_bearing, 
      (float)t.y_bearing
    },
    .advance {
      (float)t.x_advance, 
      (float)t.y_advance
    },
    .size {
      (float)t.width, 
      (float)t.height
    }
  };
}

jfont_extends_t Font::GetGlyphExtends(int symbol)
{
  cairo_glyph_t glyph;
  cairo_text_extents_t t;

  glyph.index = symbol;
  glyph.x = 0;
  glyph.y = 0;

  cairo_scaled_font_glyph_extents(_scaled_font, &glyph, 1, &t);

  return jfont_extends_t {
    .bearing {
      (float)t.x_bearing, 
      (float)t.y_bearing
    },
    .advance {
      (float)t.x_advance, 
      (float)t.y_advance
    },
    .size {
      (float)t.width, 
      (float)t.height
    }
  };
}

void Font::GetStringBreak(std::vector<std::string> *lines, std::string text, jpoint_t<int> size)
{
  if (size.x < 0 || size.y < 0) {
    return;
  }

  jmixin::String(text)
    .split(std::string("\n"))
    .for_each(
        [&](auto line) {
          std::vector<std::string> words;

          line = jmixin::String(line)
            .replace(std::string("\r"), std::string(""))
            .replace(std::string("\n"), std::string(""))
            .replace(std::string("\t"), std::string("    "));

          std::string temp, previous;

          jmixin::String(line)
            .split(std::string(" "))
            .for_each(
                [&](auto word) {
                  temp = word;

                  if (GetStringWidth(temp) > size.x) {
                    int p = 1;

                    while (p < (int)temp.size()) {
                      if (GetStringWidth(temp.substr(0, ++p)) > size.x) {
                        words.push_back(temp.substr(0, p - 1));

                        temp = temp.substr(p - 1);

                        p = 1;
                      }
                    }

                    if (temp != "") {
                      words.push_back(temp.substr(0, p));
                    }
                  } else {
                    words.push_back(temp);
                  }
                });

          temp = words[0];

          for (int j=1; j<(int)words.size(); j++) {
            previous = temp;
            temp = temp + " " + words[j];

            if (GetStringWidth(temp.c_str()) > size.x) {
              temp = words[j];

              lines->push_back(previous);
            }
          }

          lines->push_back(temp);
        });
}

std::string Font::TruncateString(std::string text, std::string extension, int width)
{
  if (text.size() <= 1 || width <= 0) {
    return text;
  }

  if (GetStringWidth(text) <= width) {
    return text;
  }

  bool flag = false;

  while (GetStringWidth(text + extension) > width) {
    flag = true;

    text = text.substr(0, text.size()-1);

    if (text.size() <= 1) {
      break;
    }
  }

  if (flag == true) {
    return text + extension;
  }

  return text;
}

bool Font::CanDisplay(int ch)
{
  return true;
}

int Font::GetCharWidth(char ch)
{
  return _widths[(int)ch];
}

const int * Font::GetCharWidths()
{
  return (int *)_widths;
}

void Font::Release()
{
}

void Font::Restore()
{
}

}
