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
#ifndef J_THEME_H
#define J_THEME_H

#include "jcanvas/core/jfont.h"
#include "jcanvas/core/jcolor.h"
#include "jcanvas/algebra/jinsets.h"

#include <map>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
struct jtheme_color_t {
  jcolor_t<float> normal;
  jcolor_t<float> focus;
  jcolor_t<float> disable;
  jcolor_t<float> select;
  jcolor_t<float> highlight;
};

struct jtheme_font_t {
  std::shared_ptr<Font> primary;
  std::shared_ptr<Font> secondary;
};

struct jtheme_border_t {
  enum class style {
    Empty,
    Line,
    Bevel,
    Round,
    RaisedGradient,
    LoweredGradient,
    RaisedBevel,
    LoweredBevel,
    RaisedEtched,
    LoweredEtched
  };

  jtheme_color_t color;
  jpoint_t<int> size;
  jtheme_border_t::style type;
};

struct jtheme_scroll_t {
  jtheme_color_t color;
  jpoint_t<int> size;
  jpoint_t<int> padding;
};

struct jtheme_t {
  jtheme_color_t bg {
    0xa0202020,
    0xff404040,
    0xff000000,
    0xff404040,
    0xa0202020
  };
  
  jtheme_color_t fg {
    0xfff0f0f0,
    0xffffffff,
    0xff808080,
    0xfff0f0f0,
    0x80f0f0f0
  };

  jtheme_border_t border {
    .color {
      0xff808080,
      0xfff0f0f0,
      0xff808080,
      0xfff0f0f0,
      0xff808080
    },
    .size {
      2, 2
    },
    .type = jtheme_border_t::style::Line
  };

  jtheme_scroll_t scroll {
    .color {
      0xff404040,
      0xff404040,
      0xff404040,
      0xff404040,
      0xff404040
    },
    .size {
      16, 16
    },
    .padding {
      4, 4
    }
  };

  jinsets_t<int> padding;

  jtheme_font_t font {
   Font::Small,
   Font::Tiny
  };

};

}

#endif

