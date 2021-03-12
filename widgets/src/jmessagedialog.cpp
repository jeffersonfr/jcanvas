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
#include "jcanvas/widgets/jmessagedialog.h"

namespace jcanvas {

MessageDialog::MessageDialog(std::shared_ptr<Container> parent, std::string title, std::string msg):
  Dialog(title, parent, {0, 0, 560, 280})
{
  _label = std::make_shared<Text>("Message");
  _ok = std::make_shared<Button>("Ok");

  _label->SetText(msg);
  _label->SetWrap(true);
  _label->SetHorizontalAlign(jhorizontal_align_t::Left);

  _ok->RegisterActionListener(this);
  
  _buttons_container = std::make_shared<Container>();

  std::shared_ptr<FlowLayout> layout = _buttons_container->SetLayout<FlowLayout>();

  _buttons_container->Add(_ok);

  _buttons_container->SetPreferredSize(layout->GetPreferredLayoutSize(_buttons_container));
}

MessageDialog::~MessageDialog() 
{
}

void MessageDialog::Init()
{
  Add(_label, jborderlayout_align_t::Center);
  Add(_buttons_container, jborderlayout_align_t::South);

  _ok->RequestFocus();
}

void MessageDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label->SetHorizontalAlign(align);
}

jhorizontal_align_t MessageDialog::GetHorizontalAlign()
{
  return _label->GetHorizontalAlign();
}

void MessageDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label->SetVerticalAlign(align);
}

jvertical_align_t MessageDialog::GetVerticalAlign()
{
  return _label->GetVerticalAlign();
}

void MessageDialog::ActionPerformed(ActionEvent *event)
{
  Close();
}

}
