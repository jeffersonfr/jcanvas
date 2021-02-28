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

using namespace jcanvas;

class GraphicsTeste : public Window, public MouseListener {

	private:
		std::vector<jpoint_t<int>> 
      points;
    jpoint_t<int>
      last = {-1, -1};
    jpoint_t<int>
      blocks;
    bool
      pressed = false;

	public:
		GraphicsTeste():
			Window({720, 480})
		{
      blocks = {720/32, 480/32};
		}

		virtual ~GraphicsTeste()
		{
		}

    void DrawPoint(jcanvas::MouseEvent *event)
    {
      if (pressed == false) {
        return;
      }

      jpoint_t<int>
        point = event->GetLocation()/jpoint_t<int>(blocks);

      if (last == point) {
        return;
      }

      last = point;

      decltype(points)::iterator 
        i = std::find_if(points.begin(), points.end(), [=](const jpoint_t<int> &param){
          return param == point;
          });

			if (i == points.end()) {
				points.push_back(point);
			} else {
				points.erase(i);
			}

      Repaint();
    }

		virtual bool MousePressed(jcanvas::MouseEvent *event)
		{
      pressed = true;

      DrawPoint(event);

			return true;
		}

		virtual bool MouseReleased(jcanvas::MouseEvent *event)
		{
      pressed = false;
      last = {-1, -1};

			return true;
		}

		virtual bool MouseMoved(jcanvas::MouseEvent *event)
		{
      DrawPoint(event);

      return true;
    }

		virtual void Paint(Graphics *g)
		{
      Window::Paint(g);

			jpoint_t
				size = GetSize();

      g->SetColor(jcolorname::Blue);

      for (int j=0; j<size.y; j+=blocks.y) {
        g->DrawLine({0, j}, {size.x, j});
      }

      for (int i=0; i<size.x; i+=blocks.x) {
        g->DrawLine({i, 0}, {i, size.y});
      }

      for (auto &point : points) {
        g->FillRectangle({point*jpoint_t<int>(blocks), blocks});
      }
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	GraphicsTeste app;

	app.SetTitle("Mouse");

	Application::Loop();

	return 0;
}

