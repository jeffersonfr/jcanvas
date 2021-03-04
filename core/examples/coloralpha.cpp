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

class ColorAlphaTeste : public Window, public MouseListener {

	private:
		Image 
      *_fg;
		Image 
      *_bg;
    jcolor_t<float>
      _ref_color;

	private:
		void Color2Alpha(jcolor_t<float> *src, jcolor_t<float> *ref)
		{
      jcolor_t<float> alpha;

			alpha.alpha = src->alpha;

			if (ref->red < 0.0001)
				alpha.red = src->red;
			else if (src->red > ref->red)
				alpha.red = (src->red - ref->red) / (1.0 - ref->red);
			else if (src->red < ref->red)
				alpha.red = (ref->red - src->red) / ref->red;
			else alpha.red = 0.0;

			if (ref->green < 0.0001)
				alpha.green = src->green;
			else if (src->green > ref->green)
				alpha.green = (src->green - ref->green) / (1.0 - ref->green);
			else if (src->green < ref->green)
				alpha.green = (ref->green - src->green) / (ref->green);
			else alpha.green = 0.0;

			if (ref->blue < 0.0001)
				alpha.blue = src->blue;
			else if (src->blue > ref->blue)
				alpha.blue = (src->blue - ref->blue) / (1.0 - ref->blue);
			else if (src->blue < ref->blue)
				alpha.blue = (ref->blue - src->blue) / (ref->blue);
			else alpha.blue = 0.0;

			if (alpha.red > alpha.green) {
				if (alpha.red > alpha.blue) {
					src->alpha = alpha.red;
				} else {
					src->alpha = alpha.blue;
				}
			} else if (alpha.green > alpha.blue) {
				src->alpha = alpha.green;
			} else {
				src->alpha = alpha.blue;
			}

			if (src->alpha < 0.0001) {
				return;
			}

			src->red = (src->red - ref->red) / src->alpha + ref->red;
			src->green = (src->green - ref->green) / src->alpha + ref->green;
			src->blue = (src->blue - ref->blue) / src->alpha + ref->blue;

			src->alpha *= alpha.alpha;
		}

	public:
		ColorAlphaTeste():
			Window(/*"Color Alpha Teste", */ {320, 320})
		{
			_fg = new BufferedImage("images/image.bmp");
			_bg = new BufferedImage("images/tux-zombie.jpg");

			_ref_color.red = 0x00;
			_ref_color.green = 0x00;
			_ref_color.blue = 0x00;
		}

		virtual ~ColorAlphaTeste()
		{
			delete _fg;
			delete _bg;
		}

		virtual bool MousePressed(MouseEvent *event)
		{
      jpoint_t
        elocation = event->GetLocation();
			Graphics 
        *g = _fg->GetGraphics();
			jcolor_t<float>
        color(g->GetRGB({elocation.x, elocation.y}));

      _ref_color = color;

			Repaint();

			return true;
		}

		virtual void Paint(Graphics *g)
		{
			Window::Paint(g);

			jpoint_t 
        size = GetSize();
			jpoint_t 
        isize = _fg->GetSize();
			uint32_t 
        buffer[isize.x*isize.y];

			_fg->GetRGBArray(buffer, {0, 0, isize.x, isize.y});

			for (int i=0; i<isize.x*isize.y; i++) {
				int a = (buffer[i] >> 0x18) & 0xff;
				int r = (buffer[i] >> 0x10) & 0xff;
				int g = (buffer[i] >> 0x08) & 0xff;
				int b = (buffer[i] >> 0x00) & 0xff;
        jcolor_t<float> color(r, g, b, a);

				Color2Alpha(&color, &_ref_color);

				buffer[i] = uint32_t(color);
			}

      int left = 0;
      int top = 0;
      int right = 0;
      int bottom = 0;

      g->SetCompositeFlags(jcomposite_t::SrcOver);

			g->DrawImage(_bg, {left, top, size.x-left-right, size.y-top-bottom});

      Image *image = new BufferedImage(jpixelformat_t::ARGB, size);

      image->GetGraphics()->SetCompositeFlags(jcomposite_t::Src);
      image->GetGraphics()->SetRGBArray(buffer, {0, 0, size.x, size.y});

			g->DrawImage(image, {left, top, size.x-left-right, size.y-top-bottom});

			delete image;
		}

};

int main(int argc, char *argv[])
{
	Application::Init(argc, argv);

	ColorAlphaTeste app;

	app.SetTitle("Coloralpha");

	Application::Loop();

	return 0;
}

