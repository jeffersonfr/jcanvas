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
#include "jcanvas/core/jindexedimage.h"
#include "jcanvas/core/jbufferedimage.h"

using namespace jcanvas;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

uint32_t palette[37] = {
  0xff070707, 0xff1f0707, 0xff2f0f07, 0xff470f07, 
  0xff571707, 0xff671f07, 0xff771f07, 0xff8f2707, 
  0xff9f2f07, 0xffaf3f07, 0xffbf4707, 0xffc74707, 
  0xffdf4f07, 0xffdf5707, 0xffdf5707, 0xffd75f07, 
  0xffd75f07, 0xffd7670f, 0xffcf6f0f, 0xffcf770f, 
  0xffcf7f0f, 0xffcf8717, 0xffc78717, 0xffc78f17, 
  0xffc7971f, 0xffbf9f1f, 0xffbf9f1f, 0xffbfa727, 
  0xffbfa727, 0xffbfaf2f, 0xffb7af2f, 0xffb7b72f, 
  0xffb7b737, 0xffcfcf6f, 0xffdfdf9f, 0xffefefc7, 
  0xffffffff
};

uint8_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

class Doom : public Window {

  public:
    Doom():
      Window(jpoint_t<int>{SCREEN_WIDTH, SCREEN_HEIGHT})
    {
      SetFramesPerSecond(30);

      srand(time(NULL));

      for (int j=0; j<SCREEN_HEIGHT; j++) {
      	for (int i=0; i<SCREEN_WIDTH; i++) {
					buffer[j][i] = 36;
				}
      }
    }

    virtual ~Doom()
    {
    }

    virtual void Paint(Graphics *g) 
    {
      jpoint_t<int>
        size = GetSize();
 
      for (int j=0; j<SCREEN_HEIGHT - 1; j++) {
        for (int i=0; i<SCREEN_WIDTH; i++) {
					int decay = random()%3;
          int intensity = buffer[j + 1][i] - decay;

					if (intensity < 0) {
						intensity = 0;
					}

					buffer[j][i + decay] = intensity;
        }
      }
      
			IndexedImage image(
				palette, 37, (uint8_t *)buffer, {SCREEN_WIDTH, SCREEN_HEIGHT});

      g->SetCompositeFlags(JCF_SRC);
      g->SetBlittingFlags(JBF_NEAREST);

			g->DrawImage(&image, {0, 0, size.x, size.y});

      Repaint();
    }

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  Doom app;

  app.SetTitle("Doom");
  app.Exec();

  Application::Loop();

  return 0;
}

