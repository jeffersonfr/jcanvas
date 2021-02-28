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

#include <iostream>

using namespace jcanvas;

#define GRID_A 40
#define GRID_B 30

#define CELL_SIZE 20

class Function : public Window {

	private:
    float 
      cx,
      cy,
      fx,
      fy,
      Rho,
      Scale,
      Theta,
      snt,
      cst,
      Phi,
      snp,
      csp,
      tx,
      ty,
      incremented,
      increment,
      dr;

	public:
		Function():
			Window({720, 480})
		{
      SetFramesPerSecond(60);

      jpoint_t<int>
        size = GetSize();

      cx = (int)(size.x);
      cy = (int)(size.y);

      fx = cx - 30;
      fy = cy - 114;

      Rho = 45; // view elevation
      Scale = 500; // scaling factor

      Theta = 1.0;

      snt = sin(Theta);
      cst = cos(Theta);

      Phi = 1.0;

      snp = sin(Phi);
      csp = cos(Phi);

      tx = size.x/2;
      ty = size.y/2; // x & y translates

      incremented = 3.1;
      dr = -1;
      increment = .2;
		}

		virtual ~Function()
		{
		}

		void Paint(Graphics *g) 
		{
      jpoint_t<int>
        size = GetSize();

      g->Clear();

      for (float x=-15; x<15; x+=0.125) {
        float x2 = x * x;

        for (float y=-15; y<15; y+=0.125) {
          float xy2 = (x2 + y*y)/20.0;
          float z = incremented * cos(xy2);
          float xe = -x * snt + y * cst;
          float ye = -x * cst * csp - y * snt * csp + z * snp;
          float ze = -x * snp * cst - y * snt * snp - z * csp + Rho;
          float sx = Scale * xe / ze;
          float sy = Scale * ye / ze;
          float ix = (int)(sx + tx);
          float iy = size.y - (int)(sy + ty);

          g->SetRGB(0xff808080, {(int)ix, (int)iy});
        }
      }

      incremented = incremented + increment * dr;

      if (incremented > 5) {
        incremented = 5;
        dr = dr * -1;
      }

      if (incremented < -5) {
        incremented = -5;
        dr = dr * -1;
      }

      Repaint();
    }

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Function app;

	app.SetTitle("Function");
  app.Exec();

	Application::Loop();

	return 0;
}

