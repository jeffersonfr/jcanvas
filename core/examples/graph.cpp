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

#include <iostream>

using namespace jcanvas;

class Graph : public Window {

	private:
    std::vector< std::map<std::string, float> > 
      nodes;

	public:
		Graph():
			Window({600, 600}), 
      nodes(100)
		{
      SetFramesPerSecond(60);

      jpoint_t
        size = GetSize();
        
      for (int i=0; i<(int)nodes.size(); i++) {
        nodes[i]["x"]   = random()%size.y;
        nodes[i]["y"]   = random()%size.x;
        nodes[i]["vx"]  = random()%3 - 1;
        nodes[i]["vy"]  = random()%3 - 1;
        nodes[i]["clr"] = 0xff000000 | random()%0xff000000;
      }
		}

		virtual ~Graph()
		{
		}

		void Paint(Graphics *g) 
		{
      jpoint_t
        size = GetSize();
        
      g->Clear();

      for (int x=0; x<(int)nodes.size(); x++) {
        nodes[x]["x"] = nodes[x]["x"] + nodes[x]["vx"];
        nodes[x]["y"] = nodes[x]["y"] + nodes[x]["vy"];

        if (nodes[x]["x"] > size.x) {
          nodes[x]["x"] = 0;
        } else if (nodes[x]["x"] < 0) {
          nodes[x]["x"] = size.x;
        }

        if (nodes[x]["y"] > size.y) {
          nodes[x]["y"] = 0;
        } else if (nodes[x]["y"] < 0) {
            nodes[x]["y"] = size.y;
        }
        
        g->SetColor(nodes[x]["clr"]);

        g->FillCircle({(int)nodes[x]["x"], (int)nodes[x]["y"]}, 3);
      }

      for (int i=0; i<(int)nodes.size(); i++) {
        for (int j=i+1; j<(int)nodes.size(); j++) {
          float dx = nodes[j]["x"] - nodes[i]["x"];
          float dy = nodes[j]["y"] - nodes[i]["y"];
          float dist = sqrt((dx*dx)+(dy*dy));

          if (dist < nodes.size()) {
            g->SetColor(nodes[i]["clr"]);

            g->DrawLine({(int)nodes[i]["x"] , (int)nodes[i]["y"]}, {(int)nodes[j]["x"], (int)nodes[j]["y"]});
          }

        }
      }

      Repaint();
    }

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Graph app;

	app.SetTitle("Graph");
  app.Exec();

	Application::Loop();

	return 0;
}

