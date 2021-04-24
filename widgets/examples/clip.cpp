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

class RectangleContainer : public Container {

	public:
		RectangleContainer(int x, int y, int w, int h):
			Container({x, y, w, h})
		{
		}

		virtual ~RectangleContainer()
		{
		}

		virtual void Paint(Graphics *g) 
		{
			Container::Paint(g);

			jpoint_t
				size = GetSize();

			g->SetColor({(int)(random()%0xff), (int)(random()%0xff), (int)(random()%0xff), 0xff});
			g->DrawRectangle({0, 0, size.x, size.y});
		}

		virtual std::string What()
		{
			return "RC:: ";
		}
};

class App : public Frame {

  private:
      RectangleContainer container1 {100, 100, 400, 400};
      RectangleContainer container2 {100, 100, 400, 400};
      Button button1 {"Testing Clipping"};

	public:
		App(int w, int h):
			Frame({w, h})
		{
    }

		virtual ~App()
		{
		}

    void Init()
    {
      button1.SetBounds({200, 100, 300, 100});

      container2.Add(&button1);
      container1.Add(&container2);

      Add(&container1);
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<App>(720, 480);

  app->Init();
	app->SetTitle("Clip");

	Application::Loop();

	return 0;
}

