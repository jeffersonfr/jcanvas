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

#define RECT_SIZE	64

class PorterTeste : public Window {

	private:
		Image *_img1;
		Image *_img2;
		Image *_bg;
		bool _is_drawimage;

	public:
		PorterTeste():
			Window(/*"Porter Teste", */ {1280, 960})
		{
			_is_drawimage = true;

			_bg = new BufferedImage("images/background.png");

			_img1 = new BufferedImage(JPF_ARGB, {RECT_SIZE, RECT_SIZE});
			_img2 = new BufferedImage(JPF_ARGB, {RECT_SIZE, RECT_SIZE});

			_img1->GetGraphics()->SetColor(0xa0ff0000);
			_img1->GetGraphics()->FillRectangle({0, 0, RECT_SIZE, RECT_SIZE});

			_img2->GetGraphics()->SetColor(0xa00000ff);
			_img2->GetGraphics()->FillRectangle({0, 0, RECT_SIZE, RECT_SIZE});
		}

		virtual ~PorterTeste()
		{
      delete _img1;
      delete _img2;
      delete _bg;
		}

		virtual void PaintComposition(Graphics *g, std::string name, jcomposite_flags_t t, int x, int y)
		{
			Image *image = new BufferedImage(JPF_ARGB, {2*RECT_SIZE, 2*RECT_SIZE});
			Graphics *ig = image->GetGraphics();

			ig->SetCompositeFlags(JCF_SRC_OVER);
			ig->DrawImage(_img1, jpoint_t<int>{1*RECT_SIZE/2, 1*RECT_SIZE/2});
			ig->SetCompositeFlags(t);

			if (_is_drawimage == true) {
				ig->DrawImage(_img2, jpoint_t<int>{2*RECT_SIZE/2, 2*RECT_SIZE/2});
			} else {
				jpoint_t size = _img2->GetSize();
				uint32_t buffer[size.x*size.y];

				_img2->GetGraphics()->GetRGBArray(buffer, {0, 0, size.x, size.y});

				ig->SetRGBArray(buffer, {2*RECT_SIZE/2, 2*RECT_SIZE/2, size.x, size.y});
			}

			g->SetCompositeFlags(JCF_SRC_OVER);
			g->DrawImage(image, jpoint_t<int>{x, y});
			
			Font *font = &Font::Size16;

			g->SetColor(jcolorname::White);
			g->SetFont(font);
			g->DrawString(name, jpoint_t<int>{x+RECT_SIZE/2, y});

			delete image;
		}

		virtual bool KeyPressed(jcanvas::KeyEvent *event)
		{
			if (Window::KeyPressed(event) == true) {
				return true;
			}
	
			if (event->GetSymbol() == jcanvas::JKS_1) {
				_is_drawimage = true;
			} else if (event->GetSymbol() == jcanvas::JKS_2) {
				_is_drawimage = false;
			}

			Repaint();

			return true;
		}

		virtual void Paint(Graphics *g)
		{
			Window::Paint(g);

      jpoint_t
        size = GetSize();
      
			g->DrawImage(_bg, {0, 0, size.x, size.y});

			Font *font = &Font::Size16;

			g->SetColor(jcolorname::White);
			g->SetFont(font);
			g->DrawString("Press 1 to use DrawImage() and 2 to use SetRGBArray()", jpoint_t<int>{0, 0});

			PaintComposition(g, "CLEAR", JCF_CLEAR, 0*(2*RECT_SIZE+16), 1*(2*RECT_SIZE+16));

			PaintComposition(g, "SRC", JCF_SRC, 0*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_OVER", JCF_SRC_OVER, 1*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_IN", JCF_SRC_IN, 2*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_OUT", JCF_SRC_OUT, 3*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_ATOP", JCF_SRC_ATOP, 4*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			
			PaintComposition(g, "DST", JCF_DST, 0*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_OVER", JCF_DST_OVER, 1*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_IN", JCF_DST_IN, 2*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_OUT", JCF_DST_OUT, 3*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_ATOP", JCF_DST_ATOP, 4*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			
			PaintComposition(g, "XOR", JCF_XOR, 0*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "ADD", JCF_ADD, 1*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "SATURATE", JCF_SATURATE, 2*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "MULTIPLY", JCF_MULTIPLY, 3*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "SCREEN", JCF_SCREEN, 4*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "OVERLAY", JCF_OVERLAY, 5*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "DARKEN", JCF_DARKEN, 6*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));

			PaintComposition(g, "LIGHTEN", JCF_LIGHTEN, 0*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "DIFFERENCE", JCF_DIFFERENCE, 1*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "EXCLUSION", JCF_EXCLUSION, 2*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "DODGE", JCF_DODGE, 3*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "BURN", JCF_BURN, 4*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "HARD", JCF_HARD, 5*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "LIGHT", JCF_LIGHT, 6*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
		}

};

int main( int argc, char *argv[] )
{
	Application::Init(argc, argv);

	PorterTeste app;

	app.SetTitle("Porter");

	Application::Loop();

	return 0;
}
