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
#include "jcanvas/core/jcolor.h"
#include "jmixin/jstring.h"

namespace jcanvas {

static bool CompareName(std::string name, std::string ref)
{
  if (name == jmixin::String(ref).lower_case()) {
    return true;
  }

  return false;
}

jcolor_name_t GetColor(std::string name) {
	jcolor_name_t color = jcolor_name_t::Black;

	name = jmixin::String(name).lower_case();

	if (CompareName(name, "AliceBlue")) {
		color = jcolor_name_t::AliceBlue;
	} else if (CompareName(name, "AntiqueWhite")) {
		color = jcolor_name_t::AntiqueWhite;
	} else if (CompareName(name, "Aqua")) {
		color = jcolor_name_t::Aqua;
	} else if (CompareName(name, "Aquamarine")) {
		color = jcolor_name_t::Aquamarine;
	} else if (CompareName(name, "Azure")) {
		color = jcolor_name_t::Azure;
	} else if (CompareName(name, "Beige")) {
		color = jcolor_name_t::Beige;
	} else if (CompareName(name, "Bisque")) {
		color = jcolor_name_t::Bisque;
	} else if (CompareName(name, "Black")) {
		color = jcolor_name_t::Black;
	} else if (CompareName(name, "BlanchedAlmond")) {
		color = jcolor_name_t::BlanchedAlmond;
	} else if (CompareName(name, "Blue")) {
		color = jcolor_name_t::Blue;
	} else if (CompareName(name, "BlueViolet")) {
		color = jcolor_name_t::BlueViolet;
	} else if (CompareName(name, "Brown")) {
		color = jcolor_name_t::Brown;
	} else if (CompareName(name, "BurlyWood")) {
		color = jcolor_name_t::BurlyWood;
	} else if (CompareName(name, "CadetBlue")) {
		color = jcolor_name_t::CadetBlue;
	} else if (CompareName(name, "Chartreuse")) {
		color = jcolor_name_t::Chartreuse;
	} else if (CompareName(name, "Chocolate")) {
		color = jcolor_name_t::Chocolate;
	} else if (CompareName(name, "Coral")) {
		color = jcolor_name_t::Coral;
	} else if (CompareName(name, "CornflowerBlue")) {
		color = jcolor_name_t::CornflowerBlue;
	} else if (CompareName(name, "Cornsilk")) {
		color = jcolor_name_t::Cornsilk;
	} else if (CompareName(name, "Crimson")) {
		color = jcolor_name_t::Crimson;
	} else if (CompareName(name, "Cyan")) {
		color = jcolor_name_t::Cyan;
	} else if (CompareName(name, "DarkBlue")) {
		color = jcolor_name_t::DarkBlue;
	} else if (CompareName(name, "DarkCyan")) {
		color = jcolor_name_t::DarkCyan;
	} else if (CompareName(name, "DarkGoldenRod")) {
		color = jcolor_name_t::DarkGoldenRod;
	} else if (CompareName(name, "DarkGray")) {
		color = jcolor_name_t::DarkGray;
	} else if (CompareName(name, "DarkGrey")) {
		color = jcolor_name_t::DarkGrey;
	} else if (CompareName(name, "DarkGreen")) {
		color = jcolor_name_t::DarkGreen;
	} else if (CompareName(name, "DarkKhaki")) {
		color = jcolor_name_t::DarkKhaki;
	} else if (CompareName(name, "DarkMagenta")) {
		color = jcolor_name_t::DarkMagenta;
	} else if (CompareName(name, "DarkOliveGreen")) {
		color = jcolor_name_t::DarkOliveGreen;
	} else if (CompareName(name, "DarkOrange")) {
		color = jcolor_name_t::DarkOrange;
	} else if (CompareName(name, "DarkOrchid")) {
		color = jcolor_name_t::DarkOrchid;
	} else if (CompareName(name, "DarkRed")) {
		color = jcolor_name_t::DarkRed;
	} else if (CompareName(name, "DarkSalmon")) {
		color = jcolor_name_t::DarkSalmon;
	} else if (CompareName(name, "DarkSeaGreen")) {
		color = jcolor_name_t::DarkSeaGreen;
	} else if (CompareName(name, "DarkSlateBlue")) {
		color = jcolor_name_t::DarkSlateBlue;
	} else if (CompareName(name, "DarkSlateGray")) {
		color = jcolor_name_t::DarkSlateGray;
	} else if (CompareName(name, "DarkSlateGrey")) {
		color = jcolor_name_t::DarkSlateGrey;
	} else if (CompareName(name, "DarkTurquoise")) {
		color = jcolor_name_t::DarkTurquoise;
	} else if (CompareName(name, "DarkViolet")) {
		color = jcolor_name_t::DarkViolet;
	} else if (CompareName(name, "DeepPink")) {
		color = jcolor_name_t::DeepPink;
	} else if (CompareName(name, "Deepskyblue")) {
		color = jcolor_name_t::Deepskyblue;
	} else if (CompareName(name, "DimGray")) {
		color = jcolor_name_t::DimGray;
	} else if (CompareName(name, "DimGrey")) {
		color = jcolor_name_t::DimGrey;
	} else if (CompareName(name, "DodgerBlue")) {
		color = jcolor_name_t::DodgerBlue;
	} else if (CompareName(name, "FireBrick")) {
		color = jcolor_name_t::FireBrick;
	} else if (CompareName(name, "FloralWhite")) {
		color = jcolor_name_t::FloralWhite;
	} else if (CompareName(name, "ForestGreen")) {
		color = jcolor_name_t::ForestGreen;
	} else if (CompareName(name, "Fuchsia")) {
		color = jcolor_name_t::Fuchsia;
	} else if (CompareName(name, "Gainsboro")) {
		color = jcolor_name_t::Gainsboro;
	} else if (CompareName(name, "GhostWhite")) {
		color = jcolor_name_t::GhostWhite;
	} else if (CompareName(name, "Gold")) {
		color = jcolor_name_t::Gold;
	} else if (CompareName(name, "GoldenRod")) {
		color = jcolor_name_t::GoldenRod;
	} else if (CompareName(name, "Gray")) {
		color = jcolor_name_t::Gray;
	} else if (CompareName(name, "Grey")) {
		color = jcolor_name_t::Grey;
	} else if (CompareName(name, "Green")) {
		color = jcolor_name_t::Green;
	} else if (CompareName(name, "GreenYellow")) {
		color = jcolor_name_t::GreenYellow;
	} else if (CompareName(name, "HoneyDew")) {
		color = jcolor_name_t::HoneyDew;
	} else if (CompareName(name, "HotPink")) {
		color = jcolor_name_t::HotPink;
	} else if (CompareName(name, "IndianRed")) {
		color = jcolor_name_t::IndianRed;
	} else if (CompareName(name, "Indigo")) {
		color = jcolor_name_t::Indigo;
	} else if (CompareName(name, "Ivory")) {
		color = jcolor_name_t::Ivory;
	} else if (CompareName(name, "Khaki")) {
		color = jcolor_name_t::Khaki;
	} else if (CompareName(name, "Lavender")) {
		color = jcolor_name_t::Lavender;
	} else if (CompareName(name, "kavenderBlush")) {
		color = jcolor_name_t::kavenderBlush;
	} else if (CompareName(name, "LawnGreen")) {
		color = jcolor_name_t::LawnGreen;
	} else if (CompareName(name, "LemonChiffon")) {
		color = jcolor_name_t::LemonChiffon;
	} else if (CompareName(name, "LightBlue")) {
		color = jcolor_name_t::LightBlue;
	} else if (CompareName(name, "LightCoral")) {
		color = jcolor_name_t::LightCoral;
	} else if (CompareName(name, "LightCyan")) {
		color = jcolor_name_t::LightCyan;
	} else if (CompareName(name, "LightGoldenRodYellow")) {
		color = jcolor_name_t::LightGoldenRodYellow;
	} else if (CompareName(name, "LightGray")) {
		color = jcolor_name_t::LightGray;
	} else if (CompareName(name, "LightGrey")) {
		color = jcolor_name_t::LightGrey;
	} else if (CompareName(name, "LightGreen")) {
		color = jcolor_name_t::LightGreen;
	} else if (CompareName(name, "LightPink")) {
		color = jcolor_name_t::LightPink;
	} else if (CompareName(name, "LightSalmon")) {
		color = jcolor_name_t::LightSalmon;
	} else if (CompareName(name, "LightSeaGreen")) {
		color = jcolor_name_t::LightSeaGreen;
	} else if (CompareName(name, "LightSkyBlue")) {
		color = jcolor_name_t::LightSkyBlue;
	} else if (CompareName(name, "LightSlateGray")) {
		color = jcolor_name_t::LightSlateGray;
	} else if (CompareName(name, "LightSlateGrey")) {
		color = jcolor_name_t::LightSlateGrey;
	} else if (CompareName(name, "LightSteelBlue")) {
		color = jcolor_name_t::LightSteelBlue;
	} else if (CompareName(name, "LightYellow")) {
		color = jcolor_name_t::LightYellow;
	} else if (CompareName(name, "Lime")) {
		color = jcolor_name_t::Lime;
	} else if (CompareName(name, "LimeGreen")) {
		color = jcolor_name_t::LimeGreen;
	} else if (CompareName(name, "Linen")) {
		color = jcolor_name_t::Linen;
	} else if (CompareName(name, "Magenta")) {
		color = jcolor_name_t::Magenta;
	} else if (CompareName(name, "Maroon")) {
		color = jcolor_name_t::Maroon;
	} else if (CompareName(name, "MediumAquaMarine")) {
		color = jcolor_name_t::MediumAquaMarine;
	} else if (CompareName(name, "MediumBlue")) {
		color = jcolor_name_t::MediumBlue;
	} else if (CompareName(name, "MediumOrchid")) {
		color = jcolor_name_t::MediumOrchid;
	} else if (CompareName(name, "MediumPurple")) {
		color = jcolor_name_t::MediumPurple;
	} else if (CompareName(name, "MediumSeaGreen")) {
		color = jcolor_name_t::MediumSeaGreen;
	} else if (CompareName(name, "MediumSlateBlue")) {
		color = jcolor_name_t::MediumSlateBlue;
	} else if (CompareName(name, "MediumSpringGreen")) {
		color = jcolor_name_t::MediumSpringGreen;
	} else if (CompareName(name, "MediumTurquoise")) {
		color = jcolor_name_t::MediumTurquoise;
	} else if (CompareName(name, "MediumVioletRed")) {
		color = jcolor_name_t::MediumVioletRed;
	} else if (CompareName(name, "MidnightBlue")) {
		color = jcolor_name_t::MidnightBlue;
	} else if (CompareName(name, "MintCream")) {
		color = jcolor_name_t::MintCream;
	} else if (CompareName(name, "MistyRose")) {
		color = jcolor_name_t::MistyRose;
	} else if (CompareName(name, "Moccasin")) {
		color = jcolor_name_t::Moccasin;
	} else if (CompareName(name, "NavajoWhite")) {
		color = jcolor_name_t::NavajoWhite;
	} else if (CompareName(name, "Navy")) {
		color = jcolor_name_t::Navy;
	} else if (CompareName(name, "OldLace")) {
		color = jcolor_name_t::OldLace;
	} else if (CompareName(name, "Olive")) {
		color = jcolor_name_t::Olive;
	} else if (CompareName(name, "OliveDrab")) {
		color = jcolor_name_t::OliveDrab;
	} else if (CompareName(name, "Orange")) {
		color = jcolor_name_t::Orange;
	} else if (CompareName(name, "OrangeRed")) {
		color = jcolor_name_t::OrangeRed;
	} else if (CompareName(name, "Orchid")) {
		color = jcolor_name_t::Orchid;
	} else if (CompareName(name, "PaleGoldenRod")) {
		color = jcolor_name_t::PaleGoldenRod;
	} else if (CompareName(name, "PaleGreen")) {
		color = jcolor_name_t::PaleGreen;
	} else if (CompareName(name, "PaleTurquoise")) {
		color = jcolor_name_t::PaleTurquoise;
	} else if (CompareName(name, "PaleVioletRed")) {
		color = jcolor_name_t::PaleVioletRed;
	} else if (CompareName(name, "PapayaWhip")) {
		color = jcolor_name_t::PapayaWhip;
	} else if (CompareName(name, "PeachPuff")) {
		color = jcolor_name_t::PeachPuff;
	} else if (CompareName(name, "Peru")) {
		color = jcolor_name_t::Peru;
	} else if (CompareName(name, "Pink")) {
		color = jcolor_name_t::Pink;
	} else if (CompareName(name, "PowderBlue")) {
		color = jcolor_name_t::PowderBlue;
	} else if (CompareName(name, "Plum")) {
		color = jcolor_name_t::Plum;
	} else if (CompareName(name, "Purple")) {
		color = jcolor_name_t::Purple;
	} else if (CompareName(name, "Red")) {
		color = jcolor_name_t::Red;
	} else if (CompareName(name, "RosyBrown")) {
		color = jcolor_name_t::RosyBrown;
	} else if (CompareName(name, "RoyalBlue")) {
		color = jcolor_name_t::RoyalBlue;
	} else if (CompareName(name, "SaddlebBown")) {
		color = jcolor_name_t::SaddlebBown;
	} else if (CompareName(name, "Salmon")) {
		color = jcolor_name_t::Salmon;
	} else if (CompareName(name, "SandyBrown")) {
		color = jcolor_name_t::SandyBrown;
	} else if (CompareName(name, "SeaGreen")) {
		color = jcolor_name_t::SeaGreen;
	} else if (CompareName(name, "SeaShell")) {
		color = jcolor_name_t::SeaShell;
	} else if (CompareName(name, "Sienna")) {
		color = jcolor_name_t::Sienna;
	} else if (CompareName(name, "Silver")) {
		color = jcolor_name_t::Silver;
	} else if (CompareName(name, "Skyblue")) {
		color = jcolor_name_t::Skyblue;
	} else if (CompareName(name, "StateBlue")) {
		color = jcolor_name_t::StateBlue;
	} else if (CompareName(name, "StateGray")) {
		color = jcolor_name_t::StateGray;
	} else if (CompareName(name, "StateGrey")) {
		color = jcolor_name_t::StateGrey;
	} else if (CompareName(name, "Snow")) {
		color = jcolor_name_t::Snow;
	} else if (CompareName(name, "SpringGreen")) {
		color = jcolor_name_t::SpringGreen;
	} else if (CompareName(name, "SteelBlue")) {
		color = jcolor_name_t::SteelBlue;
	} else if (CompareName(name, "Tan")) {
		color = jcolor_name_t::Tan;
	} else if (CompareName(name, "Teal")) {
		color = jcolor_name_t::Teal;
	} else if (CompareName(name, "Thistle")) {
		color = jcolor_name_t::Thistle;
	} else if (CompareName(name, "Tomato")) {
		color = jcolor_name_t::Tomato;
	} else if (CompareName(name, "Turquoise")) {
		color = jcolor_name_t::Turquoise;
	} else if (CompareName(name, "Violet")) {
		color = jcolor_name_t::Violet;
	} else if (CompareName(name, "Wheat")) {
		color = jcolor_name_t::Wheat;
	} else if (CompareName(name, "White")) {
		color = jcolor_name_t::White;
	} else if (CompareName(name, "WhiteSmoke")) {
		color = jcolor_name_t::WhiteSmoke;
	} else if (CompareName(name, "Yellow")) {
		color = jcolor_name_t::Yellow;
	} else if (CompareName(name, "Yellowgreen")) {
		color = jcolor_name_t::Yellowgreen;
	} else if (CompareName(name, "Transparent")) {
		color = jcolor_name_t::Transparent;
	}

	return color;
}

}

