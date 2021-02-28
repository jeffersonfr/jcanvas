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

#define ENABLE_GUI	1
#define DRAW_ANTS	0

class Main : public Window {

	private:
		double _angle;

	public:
		Main():
			Window(/*"Image Test", */ {1280, 720})
		{
			_angle = M_PI/6;

			// SetBackgroundColor(0xa0, 0x40, 0x60, 0xa0);
		}

		virtual ~Main()
		{
		}

		void SetAngle(double angle) 
		{
			_angle = angle;
		}

    void UpdateRotation()
    {
      static double j = 0.0;

      SetAngle(j);

      j = fmod(j + 2*M_PI/30.0f, 2*M_PI);
    }

    virtual void Paint(Graphics *g)
		{
			// |-----------------------------------------------------------------------|
			// |       Translate -     |       Translate 0     |       Translate +     |
			// |-----------------------------------------------------------------------|
			// | Rot - | Rot 0 | Rot + | Rot - | Rot 0 | Rot + | Rot - | Rot 0 | Rot + |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, x, y)                                                 |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, x, y, w, h)                                           |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, sx, sy, sw, sh, x, y)                                 |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, sx, sy, sw, sh, x, y, w, h)                           |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, x, y)                                            |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, x, y, w, h)                                      |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, sx, sy, sw, sh, x, y)                            |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, sx, sy, sw, sh, x, y, w, h)                      |
			// |-----------------------------------------------------------------------|

			Window::Paint(g);

      UpdateRotation();

      jpoint_t
        size = GetSize();
      int left = 0;
      int top = 0;
      int right = 0;
      int bottom = 0;
			int 
        x = left,
				y = top,
				w = size.x-left-right,
				h = size.y-top-bottom;
			int 
        gapx = 40,
				gapy = 20;
			double 
        boxw = w/9.0,
				boxh = h/8.0;
			int 
        iw = (int)(boxw-2*gapx),
				ih = (int)(boxh-2*gapy);
			double 
        radians[3] = {-_angle, 0, _angle};

			g->SetColor({0xf0, 0xf0, 0xf0, 0xff});

			for (int i=0; i<=9; i++) {
				g->DrawLine({(int)(x+i*boxw), y}, {(int)(x+i*boxw), y+h});
			}

			for (int i=0; i<=8; i++) {
				g->DrawLine({x, (int)(y+i*boxh)}, {x+w, (int)(y+i*boxh)});
			}

			g->SetColor({0x80, 0x80, 0x80, 0xff});

			for (int i=0; i<9; i++) {
				for (int j=0; j<8; j++) {
					g->FillRectangle({(int)(x+gapx+i*boxw), (int)(y+gapy+j*boxh), iw, ih});
				}
			}

			static Image *off = nullptr;
			static Image *offsub = nullptr;
			
      g->SetCompositeFlags(JCF_SRC_OVER);

			if (off == nullptr) {
				Image *image = new BufferedImage("images/square.png");

				off = image->Scale({iw, ih});
				offsub = off->Crop({0, 0, iw/2, ih/2});

				delete image;
			}
		 
			for (int k=0,j=-1; j<=1; k++,j++) {
				g->Translate({j*gapx, j*gapy});

				for (int i=0; i<3; i++) {
					Image *image = off->Rotate(radians[i], false);
					Image *simage = offsub->Rotate(radians[i], false);
 
					g->DrawImage(image, jpoint_t<int>{(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+0*boxh)});
					g->DrawImage(image, {(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+1*boxh), iw, ih});
					g->DrawImage(simage, jpoint_t<int>{(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+2*boxh)});
					g->DrawImage(simage, {(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+3*boxh), iw, ih});

					g->DrawImage(image, jpoint_t<int>{(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+4*boxh)});
					g->DrawImage(image, {(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+5*boxh), iw, ih});
					g->DrawImage(simage, jpoint_t<int>{(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+6*boxh)});
					g->DrawImage(simage, {(int)(x+gapx+(i+k*3)*boxw), (int)(y+gapy+7*boxh), iw, ih});

					delete image;
					delete simage;
				}

				g->Translate({-j*gapx, -j*gapy});
			}
      
      Repaint();
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Main app;

	app.SetTitle("Rotate Image");
  app.Exec();

	Application::Loop();

	return 0;
}
