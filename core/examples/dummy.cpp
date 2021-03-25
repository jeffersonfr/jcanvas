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

#include <chrono>

using namespace jcanvas;

class Dummy : public Window {

  public:
    Dummy():
      Window({720, 480})
    {
    }

    virtual ~Dummy()
    {
    }

    virtual void Paint(Graphics *g) 
    {
      Window::Paint(g);

      jpoint_t<int> size = GetSize();

      static std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
      
      static jpoint_t<float> pos = jpoint_t<long int>{random()%size.x, random()%size.y};
      static jpoint_t<float> dir {1, 1};

      static int block = 128;

      block = block + random()%9 - 4;

      g->SetColor(jcolor_name_t::Blue);
      g->FillRectangle({int(pos.x - block/2), int(pos.y - block/2), block, block});

      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
      std::chrono::duration<float> offset = end - now;

      now = end;

      pos.x = pos.x + 256*dir.x*offset.count();
      pos.y = pos.y + 256*dir.y*offset.count();

      if (pos.x < 0 or pos.x > size.x) {
        dir.x = -dir.x;
      }

      if (pos.y < 0 or pos.y > size.y) {
        dir.y = -dir.y;
      }

      Repaint();
    }

  private:

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  Dummy app;

  Application::Loop();

  return 0;
}

