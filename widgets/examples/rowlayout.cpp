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
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jrowlayout.h"
#include "jcanvas/widgets/jrectangleborder.h"

using namespace jcanvas;

class App : public Frame {

	private:
    Container _containers[18];

	public:
		App():
			Frame({1280, 720})
		{
			SetLayout<GridLayout>(6, 3);

      // line 0
      _containers[0].SetLayout<RowLayout>(jmainaxis_align_t::Start, jcrossaxis_align_t::Start);
      _containers[1].SetLayout<RowLayout>(jmainaxis_align_t::Center, jcrossaxis_align_t::Start);
      _containers[2].SetLayout<RowLayout>(jmainaxis_align_t::End, jcrossaxis_align_t::Start);
      _containers[3].SetLayout<RowLayout>(jmainaxis_align_t::SpaceAround, jcrossaxis_align_t::Start);
      _containers[4].SetLayout<RowLayout>(jmainaxis_align_t::SpaceBetween, jcrossaxis_align_t::Start);
      _containers[5].SetLayout<RowLayout>(jmainaxis_align_t::SpaceEvenly, jcrossaxis_align_t::Start);

      // line 1
      _containers[6].SetLayout<RowLayout>(jmainaxis_align_t::Start, jcrossaxis_align_t::End);
      _containers[7].SetLayout<RowLayout>(jmainaxis_align_t::Center, jcrossaxis_align_t::End);
      _containers[8].SetLayout<RowLayout>(jmainaxis_align_t::End, jcrossaxis_align_t::End);
      _containers[9].SetLayout<RowLayout>(jmainaxis_align_t::SpaceAround, jcrossaxis_align_t::End);
      _containers[10].SetLayout<RowLayout>(jmainaxis_align_t::SpaceBetween, jcrossaxis_align_t::End);
      _containers[11].SetLayout<RowLayout>(jmainaxis_align_t::SpaceEvenly, jcrossaxis_align_t::End);

      // line 2
      _containers[12].SetLayout<RowLayout>(jmainaxis_align_t::Start, jcrossaxis_align_t::Stretch);
      _containers[13].SetLayout<RowLayout>(jmainaxis_align_t::Center, jcrossaxis_align_t::Stretch);
      _containers[14].SetLayout<RowLayout>(jmainaxis_align_t::End, jcrossaxis_align_t::Stretch);
      _containers[15].SetLayout<RowLayout>(jmainaxis_align_t::SpaceAround, jcrossaxis_align_t::Stretch);
      _containers[16].SetLayout<RowLayout>(jmainaxis_align_t::SpaceBetween, jcrossaxis_align_t::Stretch);
      _containers[17].SetLayout<RowLayout>(jmainaxis_align_t::SpaceEvenly, jcrossaxis_align_t::Stretch);

      for (auto &container : _containers) {
        container.SetBorder<RectangleBorder>();

        Button *btn1 = new Button("Button 1");
        btn1->SetPreferredSize({100, 32});
        container.Add(btn1);
        
        Button *btn2 = new Button("Button 2");
        btn2->SetPreferredSize({120, 32});
        container.Add(btn2);
        
        Button *btn3 = new Button("Button 3");
        btn3->SetPreferredSize({140, 32});
        container.Add(btn3);

        Add(&container);
      }
    }

		virtual ~App()
		{
			RemoveAll();

      for (auto &container : _containers) {
        auto components = container.GetComponents();

        container.RemoveAll();

        for (auto component : components) {
          delete component;
        }
      }
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<App>();

	app->SetTitle("App");

	Application::Loop();

	return 0;
}

