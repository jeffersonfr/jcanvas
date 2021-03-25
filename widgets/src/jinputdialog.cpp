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

InputDialog::InputDialog(Container *parent, std::string title, std::string msg):
  Dialog(title, parent, {0, 0, 560, 280})
{
  jtheme_t &theme = GetTheme();

  theme.font.primary = Font::Big;

  _label.SetText(msg);
  _label.SetEditable(false);
  _label.SetHorizontalAlign(jhorizontal_align_t::Left);
  
  _ok.RegisterActionListener(this);
  _cancel.RegisterActionListener(this);
  
  auto layout = _container.SetLayout<FlowLayout>();

  layout->SetAlign(jflowlayout_align_t::Right);

  _container.Add(&_ok);
  _container.Add(&_cancel);

  _container.SetPreferredSize(layout->GetPreferredLayoutSize(&_container));

  Add(&_label, jborderlayout_align_t::North);
  Add(&_field, jborderlayout_align_t::Center);
  Add(&_container, jborderlayout_align_t::South);

  _cancel.RequestFocus();
}

InputDialog::~InputDialog() 
{
}

std::string InputDialog::GetText()
{
  return _field.GetText();
}

void InputDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label.SetHorizontalAlign(align);
}

jhorizontal_align_t InputDialog::GetHorizontalAlign()
{
  return _label.GetHorizontalAlign();
}

void InputDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label.SetVerticalAlign(align);
}

jvertical_align_t InputDialog::GetVerticalAlign()
{
  return _label.GetVerticalAlign();
}

void InputDialog::ActionPerformed(ActionEvent *event)
{
  Close();
}

}
