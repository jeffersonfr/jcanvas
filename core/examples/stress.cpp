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
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindow.h"

using namespace jcanvas;

class GraphicPanel : public Window {

	private:
    std::shared_ptr<Image>
      _buffer;

	public:
		GraphicPanel():
			Window({1600, 900})
	{
		srand(time(nullptr));

    _buffer = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{1920, 1080});
	}

	virtual ~GraphicPanel()
	{
	}

	void Clear() 
  {
    Graphics 
      *g = _buffer->GetGraphics();
    jpoint_t 
      t = _buffer->GetSize();

		g->SetColor(jcolor_name_t::Black);
		g->FillRectangle({0, 0, t.x, t.y});
	}

	void DrawString(std::string msg) 
	{
    Graphics 
      *g = _buffer->GetGraphics();

    Clear();

		g->SetColor({0xf0, 0xf0, 0xf0, 0xff});
		g->DrawString(msg, jpoint_t<int>{16, 0});
	}

	virtual void Paint(Graphics *g)
	{
		Window::Paint(g);

    jpoint_t 
      t = GetSize();

    g->DrawImage(_buffer, {0, 0, t.x, t.y});
	}

  virtual void ShowApp()
  {
    std::shared_ptr<Image>
      off,
      pimage;
    Graphics 
      *g = _buffer->GetGraphics();
    std::shared_ptr<Font>
      font = std::make_shared<Font>("default", jfont_attributes_t::None, 48);
    jpoint_t
      t = GetSize();
		
    g->SetFont(font);

		int 
      x, 
      y, 
      z,
  		w = 200,
	  	h = 200,
		  r1, 
      g1, 
      b1, 
      a1,
		  size, 
      iterations = 1000;
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    do {
    //////////////////////////////////////////////
    DrawString("DrawString");

    int 
      wfont = font->GetStringWidth("Font Testing"),
      hfont = font->GetSize();

    for (int i=0; i<iterations*4; i++) {
      x = rand()%(t.x-wfont-left-right);
      y = rand()%(t.y-hfont-top-bottom);
      r1 = rand()%0xff;
      g1 = rand()%0xff;
      b1 = rand()%0xff;

      g->SetColor({r1, g1, b1, 0xff});
      g->DrawString("Font Testing", jpoint_t<int>{x+left, y+top});

      Repaint();
    }

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("DrawString [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-wfont-left-right);
			y = rand()%(t.y-hfont-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->DrawString("Font Testing", jpoint_t<int>{x+left, y+top});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Rotate String");

		jfont_extends_t 
      extends = font->GetStringExtends("Rotate String");
		double 
      angle = 0.1;
		int 
      sw = extends.bearing.x + extends.size.x,
			sh = extends.bearing.x + extends.size.y;

		x = (t.x - sw)/2;
		y = (t.y + sh)/2;

    std::shared_ptr<Image>
      fimage = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{2*sw, sh});
		Graphics 
      *gf = fimage->GetGraphics();

		gf->SetFont(font);
		gf->SetColor({0xff, 0xff, 0xff, 0xff});
		gf->DrawString("Rotate String", jpoint_t<int>{0, 0});

		for (int i=0; i<iterations; i++) {
      std::shared_ptr<Image> rotate = fimage->Rotate(angle, true);

			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			// a1 = rand()%0x80;

      std::shared_ptr<Image>
        colorize = rotate->Colorize({r1, g1, b1});
      jpoint_t 
        t = rotate->GetSize();
			
			g->DrawImage(colorize, jpoint_t<int>{x - (t.x)/2, y - (t.y)/2});

			angle = angle + 0.1;

			if (angle > 2*M_PI) {
				angle = 0.1;
			}

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		jpen_t 
      pen = g->GetPen();

		for (int k=0; k<3; k++) {
		  DrawString("Draw Lines");

			if (k == 0) {
				pen.size = 1;
			} else if (k == 1) {
				pen.size = 20;
			} else if (k == 2) {
				pen.size = 40;
			}

			g->SetPen(pen);

			for (int i=0; i<iterations; i++) {
				int 
          w = rand()%(800),
					h = rand()%(800),
          mx = t.x-w-left-right,
					my = t.y-h-top-bottom;

				mx = (mx == 0)?1:mx;
				my = (my == 0)?1:my;

				x = rand()%(mx);
				y = rand()%(my);
				r1 = rand()%0xff;
				g1 = rand()%0xff;
				b1 = rand()%0xff;

				g->SetColor({r1, g1, b1, 0xff});
				g->DrawLine({x+left, y+top}, {x+left+w, y+top+h});

        Repaint();
			}

			DrawString("Draw Lines [BLEND]");

			for (int i=0; i<iterations; i++) {
				int 
          w = rand()%(800),
					h = rand()%(800),
				  mx = t.x-w-left-right,
					my = t.y-h-top-bottom;

				mx = (mx == 0)?1:mx;
				my = (my == 0)?1:my;

				x = rand()%(mx);
				y = rand()%(my);
				r1 = rand()%0xff;
				g1 = rand()%0xff;
				b1 = rand()%0xff;
				a1 = rand()%0x80;

				g->SetColor({r1, g1, b1, a1+0x80});
				g->DrawLine({x+left, y+top}, {x+left+w, y+top+h});

        Repaint();
			}
		}

		pen.size = 1;

		g->SetPen(pen);

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Triangles");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			x = x+left;
			y = y+top+h;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillTriangle({x, y}, {x+w/2, y-h}, {x+w, y});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawTriangle({x, y}, {x+w/2, y-h}, {x+w, y});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Triangles [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			x = x+left;
			y = y+top+h;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillTriangle({x, y}, {x+w/2, y-h}, {x+w, y});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawTriangle({x, y}, {x+w/2, y-h}, {x+w, y});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Rectangles");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillRectangle({x+left, y+top, w, h});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawRectangle({x+left, y+top, w, h});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Rectangles [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillRectangle({x+left, y+top, w, h});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawRectangle({x+left, y+top, w, h});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Round Rectangles");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillRoundRectangle({x+left, y+top, w, h}, 40, 40);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawRoundRectangle({x+left, y+top, w, h}, 40, 40);

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Round Rectangles [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillRoundRectangle({x+left, y+top, w, h});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawRoundRectangle({x+left, y+top, w, h});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Bevel Rectangles");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillBevelRectangle({x+left, y+top, w, h});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawBevelRectangle({x+left, y+top, w, h});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Bevel Rectangles [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillBevelRectangle({x+left, y+top, w, h});
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawBevelRectangle({x+left, y+top, w, h});

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Circles");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right-200);
			y = rand()%(t.y-h-top-bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillCircle({x+left+200, y+top+200}, z);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawCircle({x+left+200, y+top+200}, z);

      Repaint();
		}
		
    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Circles [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right-200);
			y = rand()%(t.y-h-top-bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillCircle({x+left+200, y+top+200}, z);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawCircle({x+left+200, y+top+200}, z);

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Arcs");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right-200);
			y = rand()%(t.y-h-top-bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillArc({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawArc({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);

      Repaint();
		}
		
    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Arcs [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right-200);
			y = rand()%(t.y-h-top-bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillArc({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawArc({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Chords");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right-200);
			y = rand()%(t.y-h-top-bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor({r1, g1, b1, 0xff});
			g->FillChord({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawChord({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);

      Repaint();
		}
		
    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Fill Chords [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(t.x-w-left-right-200);
			y = rand()%(t.y-h-top-bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor({r1, g1, b1, a1+0x80});
			g->FillChord({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);
			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawChord({x+left+200, y+top+200}, {z, z}, M_PI/6, -M_PI/6);

      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Blits [offscreen]");

		off = std::make_shared<BufferedImage>("images/tux-zombie.png");

		for (int i=0; i<iterations; i++) {
			uint32_t color = (rand()%0xf0f0f0) | 0xff000000;

			x = rand()%(t.x-w-left-right);
			y = rand()%(t.y-h-top-bottom);

			g->SetColor(color);
			g->DrawImage(off, {x+left, y+top, w, h});
			
      Repaint();
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Stretch Blits [offscreen]");

		uint32_t color = 0xffffffff;;

		size = 10;

		for (int i=0; i<iterations; i++) {
			size = size + 5;

			if ((size%20) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			x = (t.x-size)/2;
			y = (t.y-size)/2;

      std::shared_ptr<Image> colorize = off->Colorize(color);

			g->DrawImage(colorize, {x, y, size, size});

      Repaint();
			
			if (size > 900) {
				size = 10;
			}
		}

    if (IsVisible() == false) {
      break;
    }

    //////////////////////////////////////////////
		DrawString("Rotates [offscreen]");

		size = 10;
		color = 0xffffffff;;

		angle = 0.1;

		g->Translate({0, 0});
			
		pimage = std::make_shared<BufferedImage>("images/tux-zombie.png");

		for (int i=0; i<iterations; i++) {
      std::shared_ptr<Image> image = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, jpoint_t<int>{size, size});

			image->GetGraphics()->DrawImage(pimage, {0, 0, size, size});

      std::shared_ptr<Image> rotate = image->Rotate(angle, true);

			if (fmod(angle, 0.1) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			x = (t.x-size)/2;
			y = (t.y-size)/2;

      jpoint_t
        t = rotate->GetSize();

			g->SetColor(color);
			g->DrawImage(rotate, jpoint_t<int>{x-(t.x-size)/2, y-(t.y-size)/2});

      Repaint();

			size = size + 4;

			if (size > 900) {
				size = 10;
			}

			angle = angle + 0.1;

			if (angle > 2*M_PI) {
				angle = 0.1;
			}
		}
    } while (false);

	  Application::Quit();
  }

};

int main( int argc, char *argv[] )
{
	Application::Init(argc, argv);

	GraphicPanel app;

	app.SetTitle("Stress Test");
	app.Exec();

	Application::Loop();

	return 0;
}

