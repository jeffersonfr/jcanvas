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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

using namespace jcanvas;

class BorderTest : public Frame{

	private:
		std::vector<Component *> 
      _buttons;
    Container 
      _top,
      _bottom;
    FlowLayout
      _layout;

	public:
		BorderTest():
			Frame({960, 540})
		{
			_buttons.push_back(new Button("Empty"));
			_buttons.push_back(new Button("Line"));
			_buttons.push_back(new Button("Bevel"));
			_buttons.push_back(new Button("Round"));
			_buttons.push_back(new Button("Raised Gradient"));
			
			_buttons.push_back(new Button("Lowered Gradient"));
			_buttons.push_back(new Button("Raised Bevel"));
			_buttons.push_back(new Button("Lowered Bevel"));
			_buttons.push_back(new Button("Raised Etched"));
			_buttons.push_back(new Button("Lowered Etched"));

      _top.SetLayout(&_layout);
      _bottom.SetLayout(&_layout);

			for (int i=0; i<(int)_buttons.size(); i++) {
        Component *cmp = _buttons[i];
        jtheme_t &theme = cmp->GetTheme();

        cmp->SetPadding({16, 16, 16, 16});

			  theme.border.size = {8, 8};

        if (i == 0) {
  			  theme.border.type = jtheme_border_t::style::Empty;
        } else if (i == 1) {
  			  theme.border.type = jtheme_border_t::style::Line;
        } else if (i == 2) {
  			  theme.border.type = jtheme_border_t::style::Bevel;
        } else if (i == 3) {
  			  theme.border.type = jtheme_border_t::style::Round;
        } else if (i == 4) {
  			  theme.border.type = jtheme_border_t::style::RaisedGradient;
        } else if (i == 5) {
  			  theme.border.type = jtheme_border_t::style::LoweredGradient;
        } else if (i == 6) {
  			  theme.border.type = jtheme_border_t::style::RaisedBevel;
        } else if (i == 7) {
  			  theme.border.type = jtheme_border_t::style::LoweredBevel;
        } else if (i == 8) {
  			  theme.border.type = jtheme_border_t::style::RaisedEtched;
        } else if (i == 9) {
  			  theme.border.type = jtheme_border_t::style::LoweredEtched;
        }

        if (i < (int)_buttons.size()/2) {
  				_top.Add(cmp);
        } else {
  				_bottom.Add(cmp);
        }
			}
  		
      _top.SetPreferredSize(_layout.GetPreferredLayoutSize(&_top));
      _bottom.SetPreferredSize(_layout.GetPreferredLayoutSize(&_bottom));

      Add(&_top, jborderlayout_align_t::North);
      Add(&_bottom, jborderlayout_align_t::South);
    }

		virtual ~BorderTest()
		{
			RemoveAll();

			for (std::vector<Component *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
        delete (*i);
			}
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	BorderTest app;

	app.SetTitle("Border");
	
	Application::Loop();

	return 0;
}

