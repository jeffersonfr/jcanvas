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

YesNoDialog::YesNoDialog(Container *parent, std::string title, std::string msg):
  Dialog(title, parent, {0, 0, 560, 280})
{
  _label.SetText(msg);
  _label.SetEditable(false);
  _label.SetHorizontalAlign(jhorizontal_align_t::Left);

  _yes.RegisterActionListener(this);
  _no.RegisterActionListener(this);
 
  _container.SetLayout<FlowLayout>();

  auto layout = _container.GetLayout<FlowLayout>();
  
  layout->SetAlign(jflowlayout_align_t::Right);

  _container.Add(&_yes);
  _container.Add(&_no);

  _container.SetPreferredSize(layout->GetPreferredLayoutSize(&_container));
  
  Add(&_label, jborderlayout_align_t::Center);
  Add(&_container, jborderlayout_align_t::South);

  _no.RequestFocus();
}

YesNoDialog::~YesNoDialog() 
{
}

void YesNoDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label.SetHorizontalAlign(align);
}

jhorizontal_align_t YesNoDialog::GetHorizontalAlign()
{
  return _label.GetHorizontalAlign();
}

void YesNoDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label.SetVerticalAlign(align);
}

jvertical_align_t YesNoDialog::GetVerticalAlign()
{
  return _label.GetVerticalAlign();
}

std::string YesNoDialog::GetResponse()
{
  std::string response = "no";

  if (GetFocusOwner() == &_yes) {
    response = "yes";
  }

  return response;
}

void YesNoDialog::ActionPerformed(ActionEvent *event)
{
  Button *button = reinterpret_cast<Button *>(event->GetSource());

  if (button->IsPressed() == false) {
    return;
  }

  Close();
}

}
