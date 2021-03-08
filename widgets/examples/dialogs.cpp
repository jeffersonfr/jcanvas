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
#include "jcanvas/core/japplication.h"
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jcalendardialog.h"
#include "jcanvas/widgets/jfilechooserdialog.h"
#include "jcanvas/widgets/jinputdialog.h"
#include "jcanvas/widgets/jkeyboarddialog.h"
#include "jcanvas/widgets/jmessagedialog.h"
#include "jcanvas/widgets/jtoastdialog.h"
#include "jcanvas/widgets/jyesnodialog.h"
#include "jcanvas/widgets/jflowlayout.h"

using namespace jcanvas;

class Dialogs : public Frame, public ActionListener {

	private:
		Button 
			_button1 = {"Calendar"},
			_button2 = {"FileChooser"},
			_button3 = {"Input"},
			_button4 = {"Keyboard"},
			_button5 = {"Messsage"},
			_button6 = {"Toast"},
			_button7 = {"Yes/No"};

	public:
		Dialogs():
			Frame({1280, 720})
	{
    SetLayout<FlowLayout>();

    _button1.RegisterActionListener(this);
    _button2.RegisterActionListener(this);
    _button3.RegisterActionListener(this);
    _button4.RegisterActionListener(this);
    _button5.RegisterActionListener(this);
    _button6.RegisterActionListener(this);
    _button7.RegisterActionListener(this);

    Add(&_button1);
    Add(&_button2);
    Add(&_button3);
    Add(&_button4);
    Add(&_button5);
    Add(&_button6);
    Add(&_button7);

		_button1.RequestFocus();
	}

	virtual ~Dialogs()
	{
	}

	virtual void ActionPerformed(ActionEvent *event)
	{
    static Dialog *dialog = nullptr;

    if (dialog != nullptr) {
      delete dialog;
      dialog = nullptr;
    }

		if (event->GetSource() == &_button1) {
      dialog = new CalendarDialog(this);
		} else if (event->GetSource() == &_button2) {
      dialog = new FileChooserDialog(this, "File Chooser", "/tmp", jfilechooser_type_t::SaveFile);
		} else if (event->GetSource() == &_button3) {
      dialog = new InputDialog(this, "Input", "Warning");
		} else if (event->GetSource() == &_button4) {
      dialog = new KeyboardDialog(this, jkeyboard_type_t::Qwerty);
		} else if (event->GetSource() == &_button5) {
      dialog = new MessageDialog(this, "Message", "Some message ...");
		} else if (event->GetSource() == &_button6) {
      ToastDialog *toast = new ToastDialog(this, "Toast");

      toast->SetTimeout(2000);

      dialog = toast;
		} else if (event->GetSource() == &_button7) {
      dialog = new YesNoDialog(this, "Yes/No", "Si or no ?");
		}

    if (dialog != nullptr) {
      dialog->SetLocation(100, 100);
      dialog->Exec();
    }
	}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Dialogs app;

  app.Exec();

  Application::Loop();

	return 0;
}
