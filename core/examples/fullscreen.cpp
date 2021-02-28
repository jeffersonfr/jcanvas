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
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindowlistener.h"

using namespace jcanvas;

class Fullscreen : public Window, public KeyListener {

	private:
		Image *_bg;

	public:
		Fullscreen():
			Window(jpoint_t<int>{720, 480})
		{
			_bg = new BufferedImage("images/background.png");
		}

		virtual ~Fullscreen()
		{
      delete _bg;
		}

		virtual bool KeyReleased(jcanvas::KeyEvent *event)
		{
			if (Window::KeyReleased(event) == true) {
				return true;
			}
	
			if (event->GetSymbol() == jcanvas::JKS_F || event->GetSymbol() == jcanvas::JKS_f) {
        ToggleFullScreen();
			}

			return true;
		}

		virtual void Paint(Graphics *g)
		{
			Window::Paint(g);

      jpoint_t
        size = GetSize();
      int left = 0;
      int top = 0;
      int right = 0;
      int bottom = 0;

			printf("Window Bounds:: %d, %d, %d, %d\n", left, top, size.x-left-right, size.y-top-bottom);

			g->DrawImage(_bg, {left, top, size.x-left-right, size.y-top-bottom});

			g->SetColor(jcolorname::Red);
			g->FillRectangle({100, 100, 100, 100});
			
			g->SetColor(jcolorname::White);
      g->SetFont(&Font::Size16);
			g->DrawString("Press F to use enable/disable fullscreen mode", jpoint_t<int>{100, 100});
		}

};

int main( int argc, char *argv[] )
{
	Application::Init(argc, argv);

	Fullscreen app;

	app.SetTitle("Fullscreen");

	Application::Loop();

	return 0;
}
