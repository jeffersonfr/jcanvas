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
#include "jcanvas/widgets/jcheckbutton.h"
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jresources.h"

#include <algorithm>

namespace jcanvas {

CheckButton::CheckButton(jcheckbutton_type_t type, std::string text):
  Button(text, std::make_shared<BufferedImage>(Resources::Path() + "/images/check-image.png"))
{
  _check_image = std::make_shared<BufferedImage>(Resources::Path() + "/images/check-image.png")->Scale({64, 32});
  _radio_image = std::make_shared<BufferedImage>(Resources::Path() + "/images/radio-image.png")->Scale({64, 32});

  SetPressed(false);
  SetType(type);
  SetImage(_unchecked_image);
  SetInsets({4, 0, 0, 0});

  OnClick(
    [this](Button *thiz, bool down) {
      if (down == true) {
        thiz->SetPressed(!thiz->IsPressed());
      }

      if (thiz->IsPressed() == false) {
        thiz->SetImage(_unchecked_image);
      } else {
        thiz->SetImage(_checked_image);
      }
    });
}

CheckButton::~CheckButton()
{
}

void CheckButton::SetType(jcheckbutton_type_t type)
{
  _type = type;
  
  if (_type == jcheckbutton_type_t::Check) {
    jpoint_t<int> size = _check_image->GetSize()/jpoint_t<int>{2, 1};

    _unchecked_image = _check_image->Crop({size*jpoint_t<int>{0, 0}, size});
    _checked_image = _check_image->Crop({size*jpoint_t<int>{1, 0}, size});
  } else if (_type == jcheckbutton_type_t::Radio) {
    jpoint_t<int> size = _radio_image->GetSize()/jpoint_t<int>{2, 1};

    _unchecked_image = _radio_image->Crop({size*jpoint_t<int>{0, 0}, size});
    _checked_image = _radio_image->Crop({size*jpoint_t<int>{1, 0}, size});
  }

  SetImage(_checked_image);
}

jcheckbutton_type_t CheckButton::GetType()
{
  return _type;
}

}
