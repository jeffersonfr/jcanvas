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

using namespace jcanvas;

class App : public Frame {

	private:
	public:
		App(std::string title, int width, int height):
			Frame({width, height})
		{
    }

		virtual ~App()
		{
			RemoveAll();
		}

    void Init()
    {
			int 
        ws = 128,
				hs = 48;

      auto button1 = std::make_shared<Button>("Button 1");
      auto button2 = std::make_shared<Button>("Button 2");
      auto button3 = std::make_shared<Button>("Button 3");
      auto button4 = std::make_shared<Button>("Button 4");
      auto button5 = std::make_shared<Button>("Button 5");

      button1->SetBounds({50, 100, ws, hs});
      button2->SetBounds({100, 100, ws, hs});
      button3->SetBounds({400, 100, ws, hs});
      button4->SetBounds({800, 100, ws, hs});
      button5->SetBounds({100, 800, ws, hs});

      auto container1 = std::make_shared<Container>(jrect_t<int>{100, 100, 960, 540});
      auto container2 = std::make_shared<Container>(jrect_t<int>{-200, 200, 960, 540});

      container1->Add(button1);
      container1->Add(button5);
      container1->Add(container2);

      container2->Add(button2);
      container2->Add(button3);
      container2->Add(button4);

      Add(container1);
    }

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<App>("Clip", 720, 480);

  app->Init();
	app->SetTitle("Scroll");

	Application::Loop();

	return 0;
}

