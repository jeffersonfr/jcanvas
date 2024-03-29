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
#ifndef J_COLOR_H
#define J_COLOR_H

#include "jcanvas/algebra/jvector.h"

#include "jmixin/jstring.h"

#include <iostream>
#include <cmath>
#include <cstdint>
#include <stdexcept>

#define CLIP_COLOR(c) (((c) < T(0.0))?T(0.0):((c) > T(1.0))?T(1.0):(c))

namespace jcanvas {

enum class jcolor_name_t : uint32_t {
  AliceBlue = 0xfff0f8ff,
  AntiqueWhite = 0xfffaebd7,
  Aqua = 0xff00ffff,
  Aquamarine = 0xff7fffd4,
  Azure = 0xfff0ffff,
  Beige = 0xfff5f5dc,
  Bisque = 0xffffe4c4,
  Black = 0xff000000,
  BlanchedAlmond = 0xffffebcd,
  Blue = 0xff0000ff,
  BlueViolet = 0xff8a2be2,
  Brown = 0xffa52a2a,
  BurlyWood = 0xffdeb887,
  CadetBlue = 0xff5f9ea0,
  Chartreuse = 0xff7fff00,
  Chocolate = 0xffd2691e,
  Coral = 0xffff7f50,
  CornflowerBlue = 0xff6495ed,
  Cornsilk = 0xfffff8dc,
  Crimson = 0xffdc143c,
  Cyan = 0xff00ffff,
  DarkBlue = 0xff00008b,
  DarkCyan = 0xff008b8b,
  DarkGoldenRod = 0xffb8860b,
  DarkGray = 0xffa9a9a9,
  DarkGrey = 0xffa9a9a9,
  DarkGreen = 0xff006400,
  DarkKhaki = 0xffbdb76b,
  DarkMagenta = 0xff8b008b,
  DarkOliveGreen = 0xff556b2f,
  DarkOrange = 0xffff8c00,
  DarkOrchid = 0xff9932cc,
  DarkRed = 0xff8b0000,
  DarkSalmon = 0xffe9967a,
  DarkSeaGreen = 0xff8fbc8f,
  DarkSlateBlue = 0xff483d8b,
  DarkSlateGray = 0xff2f4f4f,
  DarkSlateGrey = 0xff3f4f4f,
  DarkTurquoise = 0xff00ced1,
  DarkViolet = 0xff9400d3,
  DeepPink = 0xffff1493,
  Deepskyblue = 0xff00bf00,
  DimGray = 0xff696969,
  DimGrey = 0xff696969,
  DodgerBlue = 0xff1e90ff,
  FireBrick = 0xffb22222,
  FloralWhite = 0xfffffaf0,
  ForestGreen = 0xff228b22,
  Fuchsia = 0xffff00ff,
  Gainsboro = 0xffdcdcdc,
  GhostWhite = 0xfff8f8ff,
  Gold = 0xffffd700,
  GoldenRod = 0xffdaa520,
  Gray = 0xff808080,
  Grey = 0xff808080,
  Green = 0xff008000,
  GreenYellow = 0xffadff2f,
  HoneyDew = 0xfff0fff0,
  HotPink = 0xffff69b4,
  IndianRed = 0xffcd5c5c,
  Indigo = 0xff4b0082,
  Ivory = 0xfffffff0,
  Khaki = 0xfff0e68c,
  Lavender = 0xffe6e6fa,
  kavenderBlush = 0xfffff0f5,
  LawnGreen = 0xff7cfc00,
  LemonChiffon = 0xfffffacd,
  LightBlue = 0xffadd8e6,
  LightCoral = 0xfff08080,
  LightCyan = 0xffe0ffff,
  LightGoldenRodYellow = 0xfffafad2,
  LightGray = 0xffd3d3d3,
  LightGrey = 0xffd3d3d3,
  LightGreen = 0xff90ee90,
  LightPink = 0xffffb6c1,
  LightSalmon = 0xffffa07a,
  LightSeaGreen = 0xff20b2aa,
  LightSkyBlue = 0xff87cefa,
  LightSlateGray = 0xff778899,
  LightSlateGrey = 0xff778899,
  LightSteelBlue = 0xffb0c4de,
  LightYellow = 0xffffffe0,
  Lime = 0xff00ff00,
  LimeGreen = 0xff32cd32,
  Linen = 0xfffaf0e6,
  Magenta = 0xffff00ff,
  Maroon = 0xff800000,
  MediumAquaMarine = 0xff6ccdaa,
  MediumBlue = 0xff0000cd,
  MediumOrchid = 0xffba55d3,
  MediumPurple = 0xff9370d6,
  MediumSeaGreen = 0xff3cb371,
  MediumSlateBlue = 0xff7b68ee,
  MediumSpringGreen = 0xff00fa9a,
  MediumTurquoise = 0xff48d1cc,
  MediumVioletRed = 0xffc71585,
  MidnightBlue = 0xff191970,
  MintCream = 0xfff5fffa,
  MistyRose = 0xffffe4e1,
  Moccasin = 0xffffe4b5,
  NavajoWhite = 0xffffdead,
  Navy = 0xff000080,
  OldLace = 0xfffdf5e6,
  Olive = 0xff808000,
  OliveDrab = 0xff6b8e23,
  Orange = 0xffffa500,
  OrangeRed = 0xffff4500,
  Orchid = 0xffda70d6,
  PaleGoldenRod = 0xffeee8aa,
  PaleGreen = 0xff98fb98,
  PaleTurquoise = 0xffafeeee,
  PaleVioletRed = 0xffd87093,
  PapayaWhip = 0xffffefd5,
  PeachPuff = 0xffffdab9,
  Peru = 0xffcd853f,
  Pink = 0xffffc0cb,
  PowderBlue = 0xffb0e0e6,
  Plum = 0xffdda0dd,
  Purple = 0xff800080,
  Red = 0xffff0000,
  RosyBrown = 0xffbc8f8f,
  RoyalBlue = 0xff4169e1,
  SaddlebBown = 0xff8b4513,
  Salmon = 0xfffa8072,
  SandyBrown = 0xfff4a460,
  SeaGreen = 0xff2e8b57,
  SeaShell = 0xfffff5ee,
  Sienna = 0xffa0522d,
  Silver = 0xffc0c0c0,
  Skyblue = 0xff87ceeb,
  StateBlue = 0xff6a5acd,
  StateGray = 0xff708090,
  StateGrey = 0xff708090,
  Snow = 0xfffffafa,
  SpringGreen = 0xff00ff7f,
  SteelBlue = 0xff4682b4,
  Tan = 0xffd2b48c,
  Teal = 0xff008080,
  Thistle = 0xffd8bfd8,
  Tomato = 0xffff6347,
  Turquoise = 0xff40e0d0,
  Violet = 0xffee82ee,
  Wheat = 0xfff5deb3,
  White = 0xffffffff,
  WhiteSmoke = 0xfff5f5f5,
  Yellow = 0xffffff00,
  Yellowgreen = 0xff9acd32,
  Transparent = 0x00000000
};


static bool Compare(std::string name, std::string ref)
{
  if (name == jmixin::String(ref).lower_case()) {
    return true;
  }

  return false;
}

/**
 * \brief
 *
 * \author Jeff Ferr
 */
template<typename T = float, typename = typename std::enable_if<std::is_floating_point<T>::value, T>::type>
struct jcolor_t {

  T red;
  T green;
  T blue;
  T alpha;

  jcolor_t(std::string name)
  {
    jcolor_name_t color = jcolor_name_t::Black;

    name = jmixin::String(name).lower_case();

    if (Compare(name, "AliceBlue")) {
      color = jcolor_name_t::AliceBlue;
    } else if (Compare(name, "AntiqueWhite")) {
      color = jcolor_name_t::AntiqueWhite;
    } else if (Compare(name, "Aqua")) {
      color = jcolor_name_t::Aqua;
    } else if (Compare(name, "Aquamarine")) {
      color = jcolor_name_t::Aquamarine;
    } else if (Compare(name, "Azure")) {
      color = jcolor_name_t::Azure;
    } else if (Compare(name, "Beige")) {
      color = jcolor_name_t::Beige;
    } else if (Compare(name, "Bisque")) {
      color = jcolor_name_t::Bisque;
    } else if (Compare(name, "Black")) {
      color = jcolor_name_t::Black;
    } else if (Compare(name, "BlanchedAlmond")) {
      color = jcolor_name_t::BlanchedAlmond;
    } else if (Compare(name, "Blue")) {
      color = jcolor_name_t::Blue;
    } else if (Compare(name, "BlueViolet")) {
      color = jcolor_name_t::BlueViolet;
    } else if (Compare(name, "Brown")) {
      color = jcolor_name_t::Brown;
    } else if (Compare(name, "BurlyWood")) {
      color = jcolor_name_t::BurlyWood;
    } else if (Compare(name, "CadetBlue")) {
      color = jcolor_name_t::CadetBlue;
    } else if (Compare(name, "Chartreuse")) {
      color = jcolor_name_t::Chartreuse;
    } else if (Compare(name, "Chocolate")) {
      color = jcolor_name_t::Chocolate;
    } else if (Compare(name, "Coral")) {
      color = jcolor_name_t::Coral;
    } else if (Compare(name, "CornflowerBlue")) {
      color = jcolor_name_t::CornflowerBlue;
    } else if (Compare(name, "Cornsilk")) {
      color = jcolor_name_t::Cornsilk;
    } else if (Compare(name, "Crimson")) {
      color = jcolor_name_t::Crimson;
    } else if (Compare(name, "Cyan")) {
      color = jcolor_name_t::Cyan;
    } else if (Compare(name, "DarkBlue")) {
      color = jcolor_name_t::DarkBlue;
    } else if (Compare(name, "DarkCyan")) {
      color = jcolor_name_t::DarkCyan;
    } else if (Compare(name, "DarkGoldenRod")) {
      color = jcolor_name_t::DarkGoldenRod;
    } else if (Compare(name, "DarkGray")) {
      color = jcolor_name_t::DarkGray;
    } else if (Compare(name, "DarkGrey")) {
      color = jcolor_name_t::DarkGrey;
    } else if (Compare(name, "DarkGreen")) {
      color = jcolor_name_t::DarkGreen;
    } else if (Compare(name, "DarkKhaki")) {
      color = jcolor_name_t::DarkKhaki;
    } else if (Compare(name, "DarkMagenta")) {
      color = jcolor_name_t::DarkMagenta;
    } else if (Compare(name, "DarkOliveGreen")) {
      color = jcolor_name_t::DarkOliveGreen;
    } else if (Compare(name, "DarkOrange")) {
      color = jcolor_name_t::DarkOrange;
    } else if (Compare(name, "DarkOrchid")) {
      color = jcolor_name_t::DarkOrchid;
    } else if (Compare(name, "DarkRed")) {
      color = jcolor_name_t::DarkRed;
    } else if (Compare(name, "DarkSalmon")) {
      color = jcolor_name_t::DarkSalmon;
    } else if (Compare(name, "DarkSeaGreen")) {
      color = jcolor_name_t::DarkSeaGreen;
    } else if (Compare(name, "DarkSlateBlue")) {
      color = jcolor_name_t::DarkSlateBlue;
    } else if (Compare(name, "DarkSlateGray")) {
      color = jcolor_name_t::DarkSlateGray;
    } else if (Compare(name, "DarkSlateGrey")) {
      color = jcolor_name_t::DarkSlateGrey;
    } else if (Compare(name, "DarkTurquoise")) {
      color = jcolor_name_t::DarkTurquoise;
    } else if (Compare(name, "DarkViolet")) {
      color = jcolor_name_t::DarkViolet;
    } else if (Compare(name, "DeepPink")) {
      color = jcolor_name_t::DeepPink;
    } else if (Compare(name, "Deepskyblue")) {
      color = jcolor_name_t::Deepskyblue;
    } else if (Compare(name, "DimGray")) {
      color = jcolor_name_t::DimGray;
    } else if (Compare(name, "DimGrey")) {
      color = jcolor_name_t::DimGrey;
    } else if (Compare(name, "DodgerBlue")) {
      color = jcolor_name_t::DodgerBlue;
    } else if (Compare(name, "FireBrick")) {
      color = jcolor_name_t::FireBrick;
    } else if (Compare(name, "FloralWhite")) {
      color = jcolor_name_t::FloralWhite;
    } else if (Compare(name, "ForestGreen")) {
      color = jcolor_name_t::ForestGreen;
    } else if (Compare(name, "Fuchsia")) {
      color = jcolor_name_t::Fuchsia;
    } else if (Compare(name, "Gainsboro")) {
      color = jcolor_name_t::Gainsboro;
    } else if (Compare(name, "GhostWhite")) {
      color = jcolor_name_t::GhostWhite;
    } else if (Compare(name, "Gold")) {
      color = jcolor_name_t::Gold;
    } else if (Compare(name, "GoldenRod")) {
      color = jcolor_name_t::GoldenRod;
    } else if (Compare(name, "Gray")) {
      color = jcolor_name_t::Gray;
    } else if (Compare(name, "Grey")) {
      color = jcolor_name_t::Grey;
    } else if (Compare(name, "Green")) {
      color = jcolor_name_t::Green;
    } else if (Compare(name, "GreenYellow")) {
      color = jcolor_name_t::GreenYellow;
    } else if (Compare(name, "HoneyDew")) {
      color = jcolor_name_t::HoneyDew;
    } else if (Compare(name, "HotPink")) {
      color = jcolor_name_t::HotPink;
    } else if (Compare(name, "IndianRed")) {
      color = jcolor_name_t::IndianRed;
    } else if (Compare(name, "Indigo")) {
      color = jcolor_name_t::Indigo;
    } else if (Compare(name, "Ivory")) {
      color = jcolor_name_t::Ivory;
    } else if (Compare(name, "Khaki")) {
      color = jcolor_name_t::Khaki;
    } else if (Compare(name, "Lavender")) {
      color = jcolor_name_t::Lavender;
    } else if (Compare(name, "kavenderBlush")) {
      color = jcolor_name_t::kavenderBlush;
    } else if (Compare(name, "LawnGreen")) {
      color = jcolor_name_t::LawnGreen;
    } else if (Compare(name, "LemonChiffon")) {
      color = jcolor_name_t::LemonChiffon;
    } else if (Compare(name, "LightBlue")) {
      color = jcolor_name_t::LightBlue;
    } else if (Compare(name, "LightCoral")) {
      color = jcolor_name_t::LightCoral;
    } else if (Compare(name, "LightCyan")) {
      color = jcolor_name_t::LightCyan;
    } else if (Compare(name, "LightGoldenRodYellow")) {
      color = jcolor_name_t::LightGoldenRodYellow;
    } else if (Compare(name, "LightGray")) {
      color = jcolor_name_t::LightGray;
    } else if (Compare(name, "LightGrey")) {
      color = jcolor_name_t::LightGrey;
    } else if (Compare(name, "LightGreen")) {
      color = jcolor_name_t::LightGreen;
    } else if (Compare(name, "LightPink")) {
      color = jcolor_name_t::LightPink;
    } else if (Compare(name, "LightSalmon")) {
      color = jcolor_name_t::LightSalmon;
    } else if (Compare(name, "LightSeaGreen")) {
      color = jcolor_name_t::LightSeaGreen;
    } else if (Compare(name, "LightSkyBlue")) {
      color = jcolor_name_t::LightSkyBlue;
    } else if (Compare(name, "LightSlateGray")) {
      color = jcolor_name_t::LightSlateGray;
    } else if (Compare(name, "LightSlateGrey")) {
      color = jcolor_name_t::LightSlateGrey;
    } else if (Compare(name, "LightSteelBlue")) {
      color = jcolor_name_t::LightSteelBlue;
    } else if (Compare(name, "LightYellow")) {
      color = jcolor_name_t::LightYellow;
    } else if (Compare(name, "Lime")) {
      color = jcolor_name_t::Lime;
    } else if (Compare(name, "LimeGreen")) {
      color = jcolor_name_t::LimeGreen;
    } else if (Compare(name, "Linen")) {
      color = jcolor_name_t::Linen;
    } else if (Compare(name, "Magenta")) {
      color = jcolor_name_t::Magenta;
    } else if (Compare(name, "Maroon")) {
      color = jcolor_name_t::Maroon;
    } else if (Compare(name, "MediumAquaMarine")) {
      color = jcolor_name_t::MediumAquaMarine;
    } else if (Compare(name, "MediumBlue")) {
      color = jcolor_name_t::MediumBlue;
    } else if (Compare(name, "MediumOrchid")) {
      color = jcolor_name_t::MediumOrchid;
    } else if (Compare(name, "MediumPurple")) {
      color = jcolor_name_t::MediumPurple;
    } else if (Compare(name, "MediumSeaGreen")) {
      color = jcolor_name_t::MediumSeaGreen;
    } else if (Compare(name, "MediumSlateBlue")) {
      color = jcolor_name_t::MediumSlateBlue;
    } else if (Compare(name, "MediumSpringGreen")) {
      color = jcolor_name_t::MediumSpringGreen;
    } else if (Compare(name, "MediumTurquoise")) {
      color = jcolor_name_t::MediumTurquoise;
    } else if (Compare(name, "MediumVioletRed")) {
      color = jcolor_name_t::MediumVioletRed;
    } else if (Compare(name, "MidnightBlue")) {
      color = jcolor_name_t::MidnightBlue;
    } else if (Compare(name, "MintCream")) {
      color = jcolor_name_t::MintCream;
    } else if (Compare(name, "MistyRose")) {
      color = jcolor_name_t::MistyRose;
    } else if (Compare(name, "Moccasin")) {
      color = jcolor_name_t::Moccasin;
    } else if (Compare(name, "NavajoWhite")) {
      color = jcolor_name_t::NavajoWhite;
    } else if (Compare(name, "Navy")) {
      color = jcolor_name_t::Navy;
    } else if (Compare(name, "OldLace")) {
      color = jcolor_name_t::OldLace;
    } else if (Compare(name, "Olive")) {
      color = jcolor_name_t::Olive;
    } else if (Compare(name, "OliveDrab")) {
      color = jcolor_name_t::OliveDrab;
    } else if (Compare(name, "Orange")) {
      color = jcolor_name_t::Orange;
    } else if (Compare(name, "OrangeRed")) {
      color = jcolor_name_t::OrangeRed;
    } else if (Compare(name, "Orchid")) {
      color = jcolor_name_t::Orchid;
    } else if (Compare(name, "PaleGoldenRod")) {
      color = jcolor_name_t::PaleGoldenRod;
    } else if (Compare(name, "PaleGreen")) {
      color = jcolor_name_t::PaleGreen;
    } else if (Compare(name, "PaleTurquoise")) {
      color = jcolor_name_t::PaleTurquoise;
    } else if (Compare(name, "PaleVioletRed")) {
      color = jcolor_name_t::PaleVioletRed;
    } else if (Compare(name, "PapayaWhip")) {
      color = jcolor_name_t::PapayaWhip;
    } else if (Compare(name, "PeachPuff")) {
      color = jcolor_name_t::PeachPuff;
    } else if (Compare(name, "Peru")) {
      color = jcolor_name_t::Peru;
    } else if (Compare(name, "Pink")) {
      color = jcolor_name_t::Pink;
    } else if (Compare(name, "PowderBlue")) {
      color = jcolor_name_t::PowderBlue;
    } else if (Compare(name, "Plum")) {
      color = jcolor_name_t::Plum;
    } else if (Compare(name, "Purple")) {
      color = jcolor_name_t::Purple;
    } else if (Compare(name, "Red")) {
      color = jcolor_name_t::Red;
    } else if (Compare(name, "RosyBrown")) {
      color = jcolor_name_t::RosyBrown;
    } else if (Compare(name, "RoyalBlue")) {
      color = jcolor_name_t::RoyalBlue;
    } else if (Compare(name, "SaddlebBown")) {
      color = jcolor_name_t::SaddlebBown;
    } else if (Compare(name, "Salmon")) {
      color = jcolor_name_t::Salmon;
    } else if (Compare(name, "SandyBrown")) {
      color = jcolor_name_t::SandyBrown;
    } else if (Compare(name, "SeaGreen")) {
      color = jcolor_name_t::SeaGreen;
    } else if (Compare(name, "SeaShell")) {
      color = jcolor_name_t::SeaShell;
    } else if (Compare(name, "Sienna")) {
      color = jcolor_name_t::Sienna;
    } else if (Compare(name, "Silver")) {
      color = jcolor_name_t::Silver;
    } else if (Compare(name, "Skyblue")) {
      color = jcolor_name_t::Skyblue;
    } else if (Compare(name, "StateBlue")) {
      color = jcolor_name_t::StateBlue;
    } else if (Compare(name, "StateGray")) {
      color = jcolor_name_t::StateGray;
    } else if (Compare(name, "StateGrey")) {
      color = jcolor_name_t::StateGrey;
    } else if (Compare(name, "Snow")) {
      color = jcolor_name_t::Snow;
    } else if (Compare(name, "SpringGreen")) {
      color = jcolor_name_t::SpringGreen;
    } else if (Compare(name, "SteelBlue")) {
      color = jcolor_name_t::SteelBlue;
    } else if (Compare(name, "Tan")) {
      color = jcolor_name_t::Tan;
    } else if (Compare(name, "Teal")) {
      color = jcolor_name_t::Teal;
    } else if (Compare(name, "Thistle")) {
      color = jcolor_name_t::Thistle;
    } else if (Compare(name, "Tomato")) {
      color = jcolor_name_t::Tomato;
    } else if (Compare(name, "Turquoise")) {
      color = jcolor_name_t::Turquoise;
    } else if (Compare(name, "Violet")) {
      color = jcolor_name_t::Violet;
    } else if (Compare(name, "Wheat")) {
      color = jcolor_name_t::Wheat;
    } else if (Compare(name, "White")) {
      color = jcolor_name_t::White;
    } else if (Compare(name, "WhiteSmoke")) {
      color = jcolor_name_t::WhiteSmoke;
    } else if (Compare(name, "Yellow")) {
      color = jcolor_name_t::Yellow;
    } else if (Compare(name, "Yellowgreen")) {
      color = jcolor_name_t::Yellowgreen;
    } else if (Compare(name, "Transparent")) {
      color = jcolor_name_t::Transparent;
    }

    *this = jcolor_t<float>{color};
  }

  jcolor_t(jcolor_name_t color):
    jcolor_t((uint32_t)color)
  {
  }

  jcolor_t(uint32_t color = 0xff000000)
  {
    uint8_t
      r = (color >> 0x10) & 0xff,
      g = (color >> 0x08) & 0xff,
      b = (color >> 0x00) & 0xff,
      a = (color >> 0x18) & 0xff;

    red = r/T(255.0);
    green = g/T(255.0);
    blue = b/T(255.0);
    alpha = a/T(255.0);
  }

  jcolor_t(T r, T g, T b, T a)
  {
    red = r;
    green = g;
    blue = b;
    alpha = a;
  }

  template<typename U> jcolor_t(U r, U g, U b, U a = 255)
  {
    red = r/T(255.0);
    green = g/T(255.0);
    blue = b/T(255.0);
    alpha = a/T(255.0);
  }

  template<typename U> operator jcolor_t<U>()
  {
    return {
      (U)red,
      (U)green,
      (U)blue,
      (U)alpha
    };
  }

  operator jvector_t<3, T>()
  {
    return {
      red,
      green,
      blue
    };
  }

  operator jvector_t<4, T>()
  {
    return {
      red,
      green,
      blue,
      alpha
    };
  }

  operator uint32_t()
  {
    jcolor_t<T> 
      clip = Clip();
    int
      r = (int)(clip.red*T(255.0)),
      g = (int)(clip.green*T(255.0)),
      b = (int)(clip.blue*T(255.0)),
      a = (int)(clip.alpha*T(255.0));

    return (uint32_t)(a << 0x18 | r << 0x10 | g << 0x08 | b);
  }

  uint8_t operator[](size_t n) const
  {
    if (n >= 4) {
      throw std::out_of_range("Element index is out of bounds");
    }

    switch (n) {
      case 0: return (uint8_t)(blue*T(255.0));
      case 1: return (uint8_t)(green*T(255.0));
      case 2: return (uint8_t)(red*T(255.0));
    }

    return (uint8_t)(alpha*T(255.0));
  }

  template<typename U, typename = typename std::enable_if<std::is_integral<U>::value, U>::type> jcolor_t & operator()(size_t n, U value)
  {
    if (n >= 4) {
      throw std::out_of_range("Element index is out of bounds");
    }

    T k = value/T(255.0);

    switch (n) {
      case 0: blue = k; break;
      case 1: green = k; break;
      case 2: red = k; break;
      default: alpha = k; break;
    }

    return *this;
  }

  bool operator==(const jcolor_t &param)
  {
    if (red != param.red or green != param.green or blue != param.blue or alpha != param.alpha) {
      return false;
    }

    return true;
  }

  bool operator!=(const jcolor_t &param)
  {
    if (red != param.red or green != param.green or blue != param.blue or alpha != param.alpha) {
      return true;
    }

    return false;
  }

  jcolor_t & operator=(const T &param)
  {
    red = param;
    green = param;
    blue = param;

    return *this;
  }

  template<typename U> jcolor_t<T> operator+(const U &param)
  {
    return {
      (T)(red + param), 
      (T)(green + param), 
      (T)(blue + param)
    };
  }

  template<typename U> jcolor_t<T> operator-(const U &param)
  {
    return {
      (T)(red - param), 
      (T)(green - param), 
      (T)(blue - param)
    };
  }

  template<typename U> jcolor_t<T> operator*(const U &param)
  {
    return {
      (T)(red*param), 
      (T)(green*param), 
      (T)(blue*param)
    };
  }

  template<typename U> jcolor_t<T> operator/(const U &param)
  {
    return {
      (T)(red/param), 
      (T)(green/param), 
      (T)(blue/param)
    };
  }

  template<typename U> jcolor_t<T> operator+=(const U &param)
  {
    return (*this = *this + param);
  }

  template<typename U> jcolor_t<T> operator-=(const U &param)
  {
    return (*this = *this - param);
  }

  template<typename U> jcolor_t<T> operator*=(const U &param)
  {
    return (*this = *this*param);
  }

  template<typename U> jcolor_t<T> operator/=(const U &param)
  {
    return (*this = *this/param);
  }

  template<typename U> jcolor_t<T> operator+(const jcolor_t<U> &param)
  {
    return {
      (T)(red + param.red),
      (T)(green + param.green),
      (T)(blue + param.blue),
      (T)(alpha + param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator-(const jcolor_t<U> &param)
  {
    return {
      (T)(red - param.red),
      (T)(green - param.green),
      (T)(blue - param.blue),
      (T)(alpha - param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator*(const jcolor_t<U> &param)
  {
    return {
      (T)(red*param.red),
      (T)(green*param.green),
      (T)(blue*param.blue),
      (T)(alpha*param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator/(const jcolor_t<U> &param)
  {
    return {
      (T)(red/param.red),
      (T)(green/param.green),
      (T)(blue/param.blue),
      (T)(alpha/param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator+=(const jcolor_t<U> &param)
  {
    return (*this = *this + param);
  }

  template<typename U> jcolor_t<T> operator-=(const jcolor_t<U> &param)
  {
    return (*this = *this - param);
  }

  template<typename U> jcolor_t<T> operator*=(const jcolor_t<U> &param)
  {
    return (*this = *this*param);
  }

  template<typename U> jcolor_t<T> operator/=(const jcolor_t<U> &param)
  {
    return (*this = *this/param);
  }

  jcolor_t<T> Clip()
  {
    T
      r = (red < T(0.0))?T(0.0):(red > T(1.0))?T(1.0):red,
      g = (green < T(0.0))?T(0.0):(green > T(1.0))?T(1.0):green,
      b = (blue < T(0.0))?T(0.0):(blue > T(1.0))?T(1.0):blue,
      a = (alpha < T(0.0))?T(0.0):(alpha > T(1.0))?T(1.0):alpha;

    return {r, g, b, a};
  }

  jcolor_t<T> Normalize()
  {
    float
      min = std::min(std::min(std::min(std::min(T(0.0), red), green), blue), alpha),
      max = std::max(std::max(std::max(std::max(T(1.0), red), green), blue), alpha);

    return {
      (red - min)/(max - min),
      (green - min)/(max - min),
      (blue - min)/(max - min),
      (alpha - min)/(max - min)
    };
  }

  static jcolor_t<T> FromGray(float g)
  {
    jcolor_t {
      g,
      g,
      g,
      0xff
    };
  }

  /**
   * \brief Converts the components of a color, as specified by the HSB model, to an equivalent set of values for the 
   * default RGB model. 
   * <p>
   * The <code>saturation</code> and <code>brightness</code> components should be doubleing-point values between zero 
   * and one (numbers in the range T(0.0)-T(1.0)).  The <code>hue</code> component can be any doubleing-point number.  The floor 
   * of this number is subtracted from it to create a fraction between 0 and 1.  This fractional number is then multiplied 
   * by 360 to produce the hue angle in the HSB color model.
   * <p>
   * The integer that is returned by <code>HSBtoRGB</code> encodes the value of a color in bits 0-23 of an integer value 
   * that is the same format used by the method {@link #getRGB() <code>getRGB</code>}. This integer can be supplied as an 
   * argument to the <code>jcolor_t</code> constructor that takes a single integer argument. 
   *
   * \param hue the hue component of the color
   * \param saturation the saturation of the color
   * \param brightness the brightness of the color
   *
   * \return the RGB value of the color with the indicated hue, saturation, and brightness.
   */
  static jcolor_t<T> FromHSB(const jvector_t<3, T> &hsb)
  {
    T 
      red, 
      green, 
      blue;

    if (hsb[1] == T(0.0)) {
      red = green = blue = hsb[2];
    } else {
      double h = (hsb[0] - std::floor(hsb[0]))*6.0;
      double f = h - std::floor(h);
      double p = hsb[2]*(T(1.0) - hsb[1]);
      double q = hsb[2]*(T(1.0) - hsb[1]*f);
      double t = hsb[2]*(T(1.0) - (hsb[1]*(T(1.0) - f)));

      switch ((int)(h)) {
        case 0:
          red = hsb[2];
          green = t;
          blue = p;
          break;
        case 1:
          red = q;
          green = hsb[2];
          blue = p;
          break;
        case 2:
          red = p;
          green = hsb[2];
          blue = t;
          break;
        case 3:
          red = p;
          green = q;
          blue = hsb[2];
          break;
        case 4:
          red = t;
          green = p;
          blue = hsb[2];
          break;
        case 5:
          red = hsb[2];
          green = p;
          blue = q;
          break;
      }
    }
    
    return jcolor_t {
      red,
      green,
      blue,
      0xff
    };
  }

  static jcolor_t<T> FromXYZ(const jvector_t<3, T> &xyz)
  {
    T 
      red, 
      green, 
      blue;
    T
      x = xyz[0],
      y = xyz[1],
      z = xyz[2];

    x = 0.95047 * ((x * x * x > 0.008856) ? x * x * x : (x - 16/116) / 7.787);
    y = 1.00000 * ((y * y * y > 0.008856) ? y * y * y : (y - 16/116) / 7.787);
    z = 1.08883 * ((z * z * z > 0.008856) ? z * z * z : (z - 16/116) / 7.787);

    red = x* 3.2406 - y*1.5372 - z*0.4986;
    green = -x*0.9689 + y*1.8758 + z*0.0415;
    blue = x*0.0557 - y*0.2040 + z*1.0570;

    red = (red > 0.0031308)?(1.055 * std::pow(red, 1/2.4) - 0.055):(12.92*red);
    green = (green > 0.0031308)?(1.055 * std::pow(green, 1/2.4) - 0.055):(12.92*green);
    blue = (blue > 0.0031308)?(1.055 * std::pow(blue, 1/2.4) - 0.055):(12.92*blue);

    jcolor_t {
      red,
      green,
      blue,
      0xff
    };
  }

  static jcolor_t<T> FromLab(const jvector_t<3, T> &lab)
  {
    T
      y = (lab[0] + 16) / 116,
      x = lab[1] / 500 + y,
      z = y - lab[2] / 200;

    return FromXYZ({x, y, z});
  }

  T ToGray(float r = 0.30f, float g = 0.59f, float b = 0.11f)
  {
    return {red*r, green*g, blue*b, alpha};
  }

  /**
   * Converts the components of a color, as specified by the default RGB model, to an equivalent set of values for hue, saturation, 
   * and brightness that are the three components of the HSB model. 
   * <p>
   * If the <code>hsbvals</code> argument is <code>null</code>, then a new array is allocated to return the result. Otherwise, the 
   * method returns the array <code>hsbvals</code>, with the values put into that array. 
   *
   * \param r the red component of the color
   * \param g the green component of the color
   * \param b the blue component of the color
   *
   * \return the HSB value of the color with the indicated red, green, blue.
   */
  jvector_t<3, T> ToHSB()
  {
    T 
      cmin = std::min(std::min(red, green), blue),
      cmax = std::max(std::max(red, green), blue);
    T 
      hue, 
      saturation, 
      brightness = cmax;

    if (cmax != T(0.0)) {
      saturation = (cmax - cmin)/cmax;

      float
        rc = (cmax - red)/(cmax - cmin),
        gc = (cmax - green)/(cmax - cmin),
        bc = (cmax - blue)/(cmax - cmin);

      if (red == cmax) {
        hue = bc - gc;
      } else if (green == cmax) {
        hue = 2.0 + rc - bc;
      } else {
        hue = 4.0 + gc - rc;
      }

      hue = hue/6.0;

      if (hue < T(0.0)) {
        hue = hue + T(1.0);
      }
    } else {
      saturation = T(0.0);
      hue = T(0.0);
    }

    return {hue, saturation, brightness};
  }

  jvector_t<3, T> ToXYZ()
  {
    T x, y, z;

    red = (red > 0.04045)?std::pow((red + 0.055)/1.055, 2.4):red/12.92;
    green = (green > 0.04045)?std::pow((green + 0.055)/1.055, 2.4):green/12.92;
    blue = (blue > 0.04045)?std::pow((blue + 0.055)/1.055, 2.4):blue/12.92;

    x = (red*0.4124 + green*0.3576 + blue*0.1805)/0.95047;
    y = (red*0.2126 + green*0.7152 + blue*0.0722)/1.00000;
    z = (red*0.0193 + green*0.1192 + blue*0.9505)/1.08883;

    x = (x > 0.008856)?std::pow(x, 1/3):(7.787*x) + 16/116;
    y = (y > 0.008856)?std::pow(y, 1/3):(7.787*y) + 16/116;
    z = (z > 0.008856)?std::pow(z, 1/3):(7.787*z) + 16/116;

    return {x, y, z};
  }

  jvector_t<3, T> ToLab()
  {
    jvector_t<3, T>
      xyz = ToXYZ();

    return {(116*xyz[1]) - 16, 500*(xyz[0] - xyz[1]), 200*(xyz[1] - xyz[2])};
  }

  static jcolor_t<T> LerpRGB(jcolor_t<T> a, jcolor_t<T> b, float t)
  {
    jcolor_t {
      a.red + t*(b.red - a.red),
      a.green + t*(b.green - a.green),
      a.blue + t*(b.blue - a.blue),
      a.alpha + t*(b.alpha - a.alpha),
    };
  }

  static jvector_t<3, T> LerpGray(float a, float b, float t)
  {
    return a + t*(b - a);
  }

  static jvector_t<3, T> LerpLab(jvector_t<3, T> a, jvector_t<3, T> b, float t)
  {
    jvector_t<3, T> {
      a[0] + t*(b[0] - a[0]),
      a[1] + t*(b[1] - a[1]),
      a[2] + t*(b[2] - a[2]),
    };
  }

  static jvector_t<3, T> LerpHSB(jvector_t<3, T> a, jvector_t<3, T> b, float t)
  {
    float h = 0, d = b[0] - a[0];

    if (a[0] > b[0]) {
      std::swap(a[0], b[0]);

      d = -d;
      t = 1.0f - t;
    }

    if (d > 0.5f) {
      a[0] = a[0] + 1.0f;
      h = fmod((a[0] + t*(b[0] - a[0]))%1.0f);
    }

    if (d <= 0.5f) {
      h = a[0] - t*d;
    }
  
    jvector_t {
      h,
      a[1] + t*(b[1] - a[1]),
      a[2] + t*(b[2] - a[2]),
    };
  }

  friend std::ostream & operator<<(std::ostream& out, const jcolor_t<T> &param)
  {
    out << std::hex << "0x" << (int)param[3] << (int)param[2] << (int)param[1] << (int)param[0];

    return out;
  }

  friend std::istream & operator>>(std::istream& is, jcolor_t<T> &param) 
  {
    is >> param.red >> param.green >> param.blue >> param.alpha;

    return is;
  }

};

}

#endif 
