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
#include "jcanvas/widgets/jtoastdialog.h"

#include <chrono>

namespace jcanvas {

ToastDialog::ToastDialog(Container *parent, std::string msg, bool wrap):
  Dialog(parent, {0, 0, 560, 280})
{
  _label.SetText(msg);
  _label.SetEditable(false);
  _label.SetHorizontalAlign(jhorizontal_align_t::Left);
  
  _timeout = 0;

  Add(&_label, jborderlayout_align_t::Center);
}

ToastDialog::~ToastDialog() 
{
}

void ToastDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label.SetHorizontalAlign(align);
}

jhorizontal_align_t ToastDialog::GetHorizontalAlign()
{
  return _label.GetHorizontalAlign();
}

void ToastDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label.SetVerticalAlign(align);
}

jvertical_align_t ToastDialog::GetVerticalAlign()
{
  return _label.GetVerticalAlign();
}

void ToastDialog::Exec(bool modal)
{
  Dialog::Exec(false);

  if (modal == true) {
    std::unique_lock<std::mutex> 
      lock(_modal_mutex);

    _modal_condition.wait_for(lock, std::chrono::milliseconds(_timeout));

    Close();
  } else {
    // TODO:: start thread to close after timeout
  }
}

void ToastDialog::SetTimeout(int timeout)
{
  _timeout = timeout;

  if (_timeout < 0) {
    _timeout = 0;
  }
}

int ToastDialog::GetTimeout()
{
  return _timeout;
}

}
