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
    std::shared_ptr<Image> _img1;
		std::shared_ptr<Image> _img2;
		std::shared_ptr<Image> _bg;
		bool _is_drawimage;

	public:
		PorterTeste():
			Window(/*"Porter Teste", */ {1280, 960})
		{
			_is_drawimage = true;

			_bg = std::make_shared<BufferedImage>("images/background.png");

			_img1 = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{RECT_SIZE, RECT_SIZE});
			_img2 = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{RECT_SIZE, RECT_SIZE});

			_img1->GetGraphics()->SetColor(0xa0ff0000);
			_img1->GetGraphics()->FillRectangle({0, 0, RECT_SIZE, RECT_SIZE});

			_img2->GetGraphics()->SetColor(0xa00000ff);
			_img2->GetGraphics()->FillRectangle({0, 0, RECT_SIZE, RECT_SIZE});
		}

		virtual ~PorterTeste()
		{
		}

		virtual void PaintComposition(Graphics *g, std::string name, jcomposite_t t, int x, int y)
		{
			std::shared_ptr<Image> image = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{2*RECT_SIZE, 2*RECT_SIZE});
			Graphics *ig = image->GetGraphics();

			ig->SetCompositeFlags(jcomposite_t::SrcOver);
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

			g->SetCompositeFlags(jcomposite_t::SrcOver);
			g->DrawImage(image, jpoint_t<int>{x, y});
			
			g->SetColor(jcolorname::White);
			g->SetFont(Font::Size16);
			g->DrawString(name, jpoint_t<int>{x+RECT_SIZE/2, y});
		}

		virtual bool KeyPressed(KeyEvent *event)
		{
			if (event->GetSymbol() == jkeyevent_symbol_t::Number1) {
				_is_drawimage = true;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::Number2) {
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

			g->SetColor(jcolorname::White);
			g->SetFont(Font::Size16);
			g->DrawString("Press 1 to use DrawImage() and 2 to use SetRGBArray()", jpoint_t<int>{0, 0});

			PaintComposition(g, "CLEAR", jcomposite_t::Clear, 0*(2*RECT_SIZE+16), 1*(2*RECT_SIZE+16));

			PaintComposition(g, "SRC", jcomposite_t::Src, 0*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_OVER", jcomposite_t::SrcOver, 1*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_IN", jcomposite_t::SrcIn, 2*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_OUT", jcomposite_t::SrcOut, 3*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_ATOP", jcomposite_t::SrcAtop, 4*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			
			PaintComposition(g, "DST", jcomposite_t::Dst, 0*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_OVER", jcomposite_t::DstOver, 1*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_IN", jcomposite_t::DstIn, 2*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_OUT", jcomposite_t::DstOut, 3*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_ATOP", jcomposite_t::DstAtop, 4*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			
			PaintComposition(g, "XOR", jcomposite_t::Xor, 0*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "ADD", jcomposite_t::Add, 1*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "SATURATE", jcomposite_t::Saturate, 2*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "MULTIPLY", jcomposite_t::Multiply, 3*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "SCREEN", jcomposite_t::Screen, 4*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "OVERLAY", jcomposite_t::Overlay, 5*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "DARKEN", jcomposite_t::Darken, 6*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));

			PaintComposition(g, "LIGHTEN", jcomposite_t::Lighten, 0*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "DIFFERENCE", jcomposite_t::Difference, 1*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "EXCLUSION", jcomposite_t::Exclusion, 2*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "DODGE", jcomposite_t::Dodge, 3*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "BURN", jcomposite_t::Burn, 4*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "HARD", jcomposite_t::Hard, 5*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "LIGHT", jcomposite_t::Light, 6*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
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
