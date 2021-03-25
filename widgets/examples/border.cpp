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
#include "jcanvas/widgets/jflowlayout.h"

#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

using namespace jcanvas;

class App : public Frame{

  private:
    std::vector<Button *> buttons {
      new Button{"Empty"},
      new Button{"Line"},
      new Button{"Bevel"},
      new Button{"Round"},
      new Button{"Raised Gradient"},

      new Button{"Lowered Gradient"},
      new Button{"Raised Bevel"},
      new Button{"Lowered Bevel"},
      new Button{"Raised Etched"},
      new Button{"Lowered Etched"}
    };
    Container top {};
    Container bottom {};

	public:
		App():
			Frame({960, 540})
		{
       top.SetLayout<FlowLayout>();
       bottom.SetLayout<FlowLayout>();

       for (int i=0; i<(int)buttons.size(); i++) {
         Button *button = buttons[i];
         jtheme_t &theme = button->GetTheme();

         theme.padding = {16, 16, 16, 16};
         theme.border.size = {8, 8};
         theme.border.type = static_cast<jtheme_border_t::style>(i);

         if (i < (int)buttons.size()/2) {
           top.Add(button);
         } else {
           bottom.Add(button);
         }
       }

       top.SetPreferredSize(top.GetLayout()->GetPreferredLayoutSize(&top));
       bottom.SetPreferredSize(bottom.GetLayout()->GetPreferredLayoutSize(&bottom));

       Add(&top, jborderlayout_align_t::North);
       Add(&bottom, jborderlayout_align_t::South);
    }

    virtual ~App()
    {
      for (auto cmp : buttons) {
        delete cmp;
      }

      buttons.clear();
    }
};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	App app;

	app.SetTitle("Border");
	
	Application::Loop();

	return 0;
}

