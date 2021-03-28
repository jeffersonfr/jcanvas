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
#include "jcanvas/widgets/jcheckbuttongroup.h"

#include <algorithm>

namespace jcanvas {

CheckButtonGroup::CheckButtonGroup()
{
}

CheckButtonGroup::~CheckButtonGroup()
{
}

void CheckButtonGroup::Add(CheckButton *button)
{
   std::lock_guard<std::recursive_mutex> guard(_group_mutex);

  if (std::find(_buttons.begin(), _buttons.end(), button) != _buttons.end()) {
    return;
  }

  button->RegisterActionListener(this);

  _buttons.push_back(button);
}

void CheckButtonGroup::Remove(CheckButton *button)
{
   std::lock_guard<std::recursive_mutex> guard(_group_mutex);

   std::vector<CheckButton *>::iterator i = std::find(_buttons.begin(), _buttons.end(), button);

  if (i == _buttons.end()) {
    return;
  }

  (*i)->RemoveActionListener(this);

  _buttons.erase(i);
}

void CheckButtonGroup::ActionPerformed(ActionEvent *event)
{
   std::lock_guard<std::recursive_mutex> guard(_group_mutex);

  CheckButton *cmp = reinterpret_cast<CheckButton *>(event->GetSource());

  if (cmp->IsPressed() == false) {
    cmp->Click();
  }

  for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
    CheckButton *cmp1 = (*i);

    if (cmp != cmp1) {
      if (cmp1->IsPressed()) {
        cmp1->Click();
      }
    }
  }
}

CheckButton * CheckButtonGroup::GetSelected()
{
   std::lock_guard<std::recursive_mutex> guard(_group_mutex);

  for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
    CheckButton *cmp = (*i);

    if (cmp->IsPressed() == true) {
      return cmp;
    }
  }

  return nullptr;
}

}
