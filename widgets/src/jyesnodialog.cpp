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
#include "jcanvas/widgets/jyesnodialog.h"

namespace jcanvas {

YesNoDialog::YesNoDialog(std::shared_ptr<Container> parent, std::string title, std::string msg):
  Dialog(title, parent, {0, 0, 560, 280})
{
  _label = std::make_shared<Text>("Yes or No ?");
  _yes = std::make_shared<Button>("Yes");
  _no = std::make_shared<Button>("No");

  _label->SetText(msg);
  _label->SetWrap(true);
  _label->SetHorizontalAlign(jhorizontal_align_t::Left);

  _yes->RegisterActionListener(this);
  _no->RegisterActionListener(this);
 
  _buttons_container = std::make_shared<Container>();

  std::shared_ptr<FlowLayout> layout = _buttons_container->SetLayout<FlowLayout>();
  
  layout->SetAlign(jflowlayout_align_t::Right);

  _buttons_container->Add(_yes);
  _buttons_container->Add(_no);

  _buttons_container->SetPreferredSize(layout->GetPreferredLayoutSize(_buttons_container));
}

YesNoDialog::~YesNoDialog() 
{
}

void YesNoDialog::Init()
{
  Add(_label, jborderlayout_align_t::Center);
  Add(_buttons_container, jborderlayout_align_t::South);

  _no->RequestFocus();
}

void YesNoDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label->SetHorizontalAlign(align);
}

jhorizontal_align_t YesNoDialog::GetHorizontalAlign()
{
  return _label->GetHorizontalAlign();
}

void YesNoDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label->SetVerticalAlign(align);
}

jvertical_align_t YesNoDialog::GetVerticalAlign()
{
  return _label->GetVerticalAlign();
}

std::string YesNoDialog::GetResponse()
{
  std::string response = "no";

  if (GetFocusOwner() == _yes) {
    response = "yes";
  }

  return response;
}

void YesNoDialog::ActionPerformed(ActionEvent *event)
{
  Close();
}

}
