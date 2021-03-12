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
#include "jcanvas/widgets/jinputdialog.h"

namespace jcanvas {

InputDialog::InputDialog(std::shared_ptr<Container> parent, std::string title, std::string msg):
  Dialog(title, parent, {0, 0, 560, 280})
{
  jtheme_t &theme = GetTheme();

  theme.font.primary = &Font::Big;

  _label = std::make_shared<Text>(msg);

  _label->SetWrap(true);
  _label->SetHorizontalAlign(jhorizontal_align_t::Left);
  
  _label = std::make_shared<Text>("Input");
  _ok = std::make_shared<Button>("Ok");
  _cancel = std::make_shared<Button>("Cancel");

  _ok->RegisterActionListener(this);
  _cancel->RegisterActionListener(this);
  
  _buttons_container = std::make_shared<Container>();

  std::shared_ptr<FlowLayout> layout = _buttons_container->SetLayout<FlowLayout>();

  layout->SetAlign(jflowlayout_align_t::Right);

  _buttons_container->Add(_ok);
  _buttons_container->Add(_cancel);

  _buttons_container->SetPreferredSize(layout->GetPreferredLayoutSize(_buttons_container));

  _field = std::make_shared<TextField>();
}

InputDialog::~InputDialog() 
{
}

void InputDialog::Init()
{
  Add(_label, jborderlayout_align_t::North);
  Add(_field, jborderlayout_align_t::Center);
  Add(_buttons_container, jborderlayout_align_t::South);

  _cancel->RequestFocus();
}

std::string InputDialog::GetText()
{
  return _field->GetText();
}

void InputDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label->SetHorizontalAlign(align);
}

jhorizontal_align_t InputDialog::GetHorizontalAlign()
{
  return _label->GetHorizontalAlign();
}

void InputDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label->SetVerticalAlign(align);
}

jvertical_align_t InputDialog::GetVerticalAlign()
{
  return _label->GetVerticalAlign();
}

void InputDialog::ActionPerformed(ActionEvent *event)
{
  Close();
}

}
