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

using namespace jcanvas;

class Dummy : public Window {

  Image
    *buffer;

  public:
    Dummy():
      Window({256, 256})
    {
      buffer = new BufferedImage(JPF_RGB32, {256, 256});
    }

    virtual ~Dummy()
    {
      delete buffer;
      buffer = nullptr;
    }

    virtual void Paint(Graphics *g) 
    {
      Window::Paint(g);

      static float angle = 0.0f;

      buffer->GetGraphics()->SetColor(jcolorname::Green);
      buffer->GetGraphics()->FillArc({128, 128}, {128, 128}, angle, angle + 0.2f);

      for (int i=0; i<256*256; i++) {
        jpoint_t<int>
          point {i%256, i/256};
        jcolor_t<float>
          color = buffer->GetGraphics()->GetRGB(point);

        color.green = color.green*0.90f;

        buffer->GetGraphics()->SetRGB(color, point);
      }

      buffer->GetGraphics()->SetColor(jcolorname::Green);
      buffer->GetGraphics()->DrawCircle({128, 128}, 128);

      g->DrawImage(buffer, jpoint_t<int>{0, 0});

      angle = angle + 0.06f;

      Repaint();
    }

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  Dummy app;

  Application::Loop();

  return 0;
}

