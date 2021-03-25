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

class App : public Frame, public ActionListener {

  private:
    std::vector<Button *> buttons {
      new Button("Calendar"),
      new Button("FileChooser"),
      new Button("Input"),
      new Button("Keyboard"),
      new Button("Messsage"),
      new Button("Toast"),
      new Button("Yes/No")
    };
    Dialog *dialog {nullptr};

	public:
		App():
			Frame({1280, 720})
	  {
    }

    virtual ~App()
    {
      for (auto cmp : buttons) {
        delete cmp;
      }

      delete dialog;
    }

    void Init()
    {
      SetLayout<FlowLayout>();

      for (auto &button : buttons) {
        button->RegisterActionListener(this);
      
        Add(button);
      }

      buttons[0]->RequestFocus();
    }

    virtual void ActionPerformed(ActionEvent *event)
    {
      static Dialog *dialog;

      if (dialog != nullptr) {
        delete dialog;
        dialog = nullptr;
      }

      if (event->GetSource() == buttons[0]) {
        dialog = new CalendarDialog(this);
      } else if (event->GetSource() == buttons[1]) {
        dialog = new FileChooserDialog(this, "File Chooser", "/tmp", jfilechooser_type_t::SaveFile);
      } else if (event->GetSource() == buttons[2]) {
        dialog = new InputDialog(this, "Input", "Warning");
      } else if (event->GetSource() == buttons[3]) {
        dialog = new KeyboardDialog(this, jkeyboard_type_t::Qwerty);
      } else if (event->GetSource() == buttons[4]) {
        dialog = new MessageDialog(this, "Message", "Some message ...");
      } else if (event->GetSource() == buttons[5]) {
        ToastDialog *toast = new ToastDialog(this, "Toast");

        toast->SetTimeout(2000);

        dialog = toast;
      } else if (event->GetSource() == buttons[6]) {
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

	auto app = std::make_shared<App>();

  app->Init();
  app->Exec();

  Application::Loop();

	return 0;
}
