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
    std::vector<std::shared_ptr<Button>> buttons {
      std::make_shared<Button>("Calendar"),
      std::make_shared<Button>("FileChooser"),
      std::make_shared<Button>("Input"),
      std::make_shared<Button>("Keyboard"),
      std::make_shared<Button>("Messsage"),
      std::make_shared<Button>("Toast"),
      std::make_shared<Button>("Yes/No")
    };

	public:
		App():
			Frame({1280, 720})
	  {
    }

    virtual ~App()
    {
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
      static std::shared_ptr<Dialog> dialog;

      if (dialog != nullptr) {
        dialog.reset();
      }

      auto parent = GetSharedPointer<Container>();

      if (event->GetSource() == buttons[0].get()) {
        dialog = std::make_shared<CalendarDialog>(parent);
      } else if (event->GetSource() == buttons[1].get()) {
        dialog = std::make_shared<FileChooserDialog>(parent, "File Chooser", "/tmp", jfilechooser_type_t::SaveFile);
      } else if (event->GetSource() == buttons[2].get()) {
        dialog = std::make_shared<InputDialog>(parent, "Input", "Warning");
      } else if (event->GetSource() == buttons[3].get()) {
        dialog = std::make_shared<KeyboardDialog>(parent, jkeyboard_type_t::Qwerty);
      } else if (event->GetSource() == buttons[4].get()) {
        dialog = std::make_shared<MessageDialog>(parent, "Message", "Some message ...");
      } else if (event->GetSource() == buttons[5].get()) {
        std::shared_ptr<ToastDialog> toast = std::make_shared<ToastDialog>(parent, "Toast");

        toast->SetTimeout(2000);

        dialog = toast;
      } else if (event->GetSource() == buttons[6].get()) {
        dialog = std::make_shared<YesNoDialog>(parent, "Yes/No", "Si or no ?");
      }

      if (dialog != nullptr) {
        dialog->SetLocation(100, 100);
        dialog->Init();
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
