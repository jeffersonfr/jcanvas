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
#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/widgets/jbeveledrectangleborder.h"
#include "jcanvas/widgets/jroundedrectangleborder.h"
#include "jcanvas/widgets/jraisedgradientrectangleborder.h"
#include "jcanvas/widgets/jloweredgradientrectangleborder.h"
#include "jcanvas/widgets/jraisedbeveledrectangleborder.h"
#include "jcanvas/widgets/jloweredbeveledrectangleborder.h"
#include "jcanvas/widgets/jraisedetchedrectangleborder.h"
#include "jcanvas/widgets/jloweredetchedrectangleborder.h"

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

         if (i == 0) {
           button->SetBorder(nullptr);
         } else if (i == 1) {
           button->SetBorder(std::make_shared<RectangleBorder>(8));
         } else if (i == 2) {
           button->SetBorder(std::make_shared<BeveledRectangleBorder>(8));
         } else if (i == 3) {
           button->SetBorder(std::make_shared<RoundedRectangleBorder>(8));
         } else if (i == 4) {
           button->SetBorder(std::make_shared<RaisedGradientRectangleBorder>(8));
         } else if (i == 5) {
           button->SetBorder(std::make_shared<LoweredGradientRectangleBorder>(8));
         } else if (i == 6) {
           button->SetBorder(std::make_shared<RaisedBeveledRectangleBorder>(8));
         } else if (i == 7) {
           button->SetBorder(std::make_shared<LoweredBeveledRectangleBorder>(8));
         } else if (i == 8) {
           button->SetBorder(std::make_shared<RaisedEtchedRectangleBorder>(8));
         } else if (i == 9) {
           button->SetBorder(std::make_shared<LoweredEtchedRectangleBorder>(8));
         }

         jtheme_t &theme = button->GetTheme();

         theme.padding = {16, 16, 16, 16};

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

