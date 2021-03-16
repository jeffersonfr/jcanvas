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

class Primitives : public Window {

	private:
    int
      _index;

	public:
		Primitives():
			Window(jpoint_t<int>{1920, 1080})
		{
      _index = 0;
		}

		virtual ~Primitives()
		{
		}

		virtual bool KeyPressed(KeyEvent *event)
    {
      if (event->GetSymbol() == jkeyevent_symbol_t::CursorLeft) {
        _index = 0;
      } else if (event->GetSymbol() == jkeyevent_symbol_t::CursorRight) {
        _index = 1;
      }

      Repaint();

      return true;
    }

		virtual void Paint(Graphics *g)
		{
			g->Clear();

      if (_index == 0) {
        Paint1(g);
      } else if (_index == 1) {
        Paint2(g);
      }
    }

    virtual void Paint1(Graphics *g)
    {
      Window::Paint(g);

      // colored bar
      const int 
        num_colors = 512,
                   bar_width = 400;
      uint32_t 
        array[num_colors];
      int 
        sixth = num_colors/6,
              dx,
              red, 
              green, 
              blue;

      for (int i=0; i<num_colors; i++) {
        if (i <= 2*sixth) {
          array[i] = 0;
        } else if (i > 2*sixth && i < 3*sixth) {
          array[i] = (i-2*sixth)*255/sixth;
        } else if (i >= 3*sixth && i <= 5*sixth) {
          array[i] = 255;
        } else if (i > 5*sixth && i < 6*sixth) {
          array[i] = 255-(i-5*sixth)*255/sixth;
        } else if (i >= 6*sixth) {
          array[i] = 0;
        }
      }

      g->SetAntialias(jantialias_t::None);

      for (int i=0; i<num_colors; i++) {
        red = array[(i+4*sixth)%num_colors];
        green = array[(i+2*sixth)%num_colors];
        blue = array[i];

        dx = (bar_width*i)/num_colors;

        g->SetColor({red, green, blue, 0xff});
        g->DrawLine({10+dx, 20}, {10+dx, 20+100});
      }

      // gray bar
      for (int i=0; i<400; i++) {
        g->SetColor({i/2, i/2, i/2, 0xff});
        g->DrawLine({i+10, 1*(100+10)+20}, {i+10, 1*(100+10)+100+20});
      }

      g->SetAntialias(jantialias_t::Normal);

      // draw image
      std::shared_ptr<Image> path = std::make_shared<BufferedImage>("images/blue_icon.png");

      std::shared_ptr<Image> path1 = path->Blend(1.0);
      std::shared_ptr<Image> path2 = path->Blend(0.5);
      std::shared_ptr<Image> path3 = path->Blend(0.25);
      std::shared_ptr<Image> path4 = path->Blend(0.125);

      g->SetColor({0x00, 0x00, 0x00, 0xf0});
      g->DrawImage(path1, {400+40+0*(200+10), 0*(100+10)+20, 200, 100});
      g->SetColor({0x00, 0x00, 0x00, 0x80});
      g->DrawImage(path2, {400+40+1*(200+10), 0*(100+10)+20, 200, 100});
      g->SetColor({0x00, 0x00, 0x00, 0x40});
      g->DrawImage(path3, {400+40+0*(200+10), 1*(100+10)+20, 200, 100});
      g->SetColor({0x00, 0x00, 0x00, 0x10});
      g->DrawImage(path4, {400+40+1*(200+10), 1*(100+10)+20, 200, 100});

      jcolor_t<float> pcolor(0xf0, 0xf0, 0xf0, 0x80),
        ccolor(0x20, 0xf0, 0x20, 0x80),
        rcolor(0x00, 0x00, 0x00, 0x80);
      jpen_t pen = g->GetPen();

      pen.width = 1;
      g->SetPen(pen);

      g->SetColor(rcolor);
      for (int i=0; i<=9; i++) {
        g->DrawRectangle({10+i*(120+10)+10, 2*(120+10), 100, 100});
        g->DrawRectangle({10+i*(120+10)+10, 3*(120+10), 100, 100});
        g->DrawRectangle({10+i*(120+10)+10, 4*(120+10), 100, 100});
        g->DrawRectangle({10+i*(120+10)+10, 5*(120+10), 100, 100});
      }

      // draw circle
      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawCircle({10+0*(120+10)+60, 10+3*(120+10)+40}, 50);

      pen.width = 10;
      g->SetPen(pen);

      g->DrawCircle({10+0*(120+10)+60, 10+4*(120+10)+40}, 50);
      g->FillCircle({10+0*(120+10)+60, 10+5*(120+10)+40}, 50);
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawCircle({10+0*(120+10)+60, 10+2*(120+10)+40}, 50);
      g->DrawCircle({10+0*(120+10)+60, 10+3*(120+10)+40}, 50);
      g->DrawCircle({10+0*(120+10)+60, 10+4*(120+10)+40}, 50);
      g->DrawCircle({10+0*(120+10)+60, 10+5*(120+10)+40}, 50);

      // draw ellipse
      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawEllipse({10+1*(120+10)+60, 10+3*(120+10)+40}, {30, 50});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawEllipse({10+1*(120+10)+60, 10+4*(120+10)+40}, {30, 50});
      g->FillEllipse({10+1*(120+10)+60, 10+5*(120+10)+40}, {30, 50});
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawEllipse({10+1*(120+10)+60, 10+2*(120+10)+40}, {30, 50});
      g->DrawEllipse({10+1*(120+10)+60, 10+3*(120+10)+40}, {30, 50});
      g->DrawEllipse({10+1*(120+10)+60, 10+4*(120+10)+40}, {30, 50});
      g->DrawEllipse({10+1*(120+10)+60, 10+5*(120+10)+40}, {30, 50});

      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawEllipse({10+2*(120+10)+60, 10+3*(120+10)+40}, {50, 30});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawEllipse({10+2*(120+10)+60, 10+4*(120+10)+40}, {50, 30});
      g->FillEllipse({10+2*(120+10)+60, 10+5*(120+10)+40}, {50, 30});
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawEllipse({10+2*(120+10)+60, 10+2*(120+10)+40}, {50, 30});
      g->DrawEllipse({10+2*(120+10)+60, 10+3*(120+10)+40}, {50, 30});
      g->DrawEllipse({10+2*(120+10)+60, 10+4*(120+10)+40}, {50, 30});
      g->DrawEllipse({10+2*(120+10)+60, 10+5*(120+10)+40}, {50, 30});

      // draw arc
      double 
			  arc0 = M_PI/6.0,
        arc1 = -arc0;

      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawArc({10+3*(120+10)+60, 10+3*(120+10)+40}, {50, 50}, arc0, arc1);

      pen.width = 10;
      g->SetPen(pen);

      g->DrawArc({10+3*(120+10)+60, 10+4*(120+10)+40}, {50, 50}, arc0, arc1);
      g->FillArc({10+3*(120+10)+60, 10+5*(120+10)+40}, {50, 50}, arc0, arc1);
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawArc({10+3*(120+10)+60, 10+2*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawArc({10+3*(120+10)+60, 10+3*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawArc({10+3*(120+10)+60, 10+4*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawArc({10+3*(120+10)+60, 10+5*(120+10)+40}, {50, 50}, arc0, arc1);

      // draw chord
      arc0 = 3*M_PI_2+1*M_PI/3.0;
      arc1 = 1*M_PI_2+1*M_PI/3.0;

      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawChord({10+4*(120+10)+60, 10+3*(120+10)+40}, {50, 50}, arc0, arc1);

      pen.width = 10;
      g->SetPen(pen);

      g->DrawChord({10+4*(120+10)+60, 10+4*(120+10)+40}, {50, 50}, arc0, arc1);
      g->FillChord({10+4*(120+10)+60, 10+5*(120+10)+40}, {50, 50}, arc0, arc1);
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawChord({10+4*(120+10)+60, 10+2*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawChord({10+4*(120+10)+60, 10+3*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawChord({10+4*(120+10)+60, 10+4*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawChord({10+4*(120+10)+60, 10+5*(120+10)+40}, {50, 50}, arc0, arc1);

      // draw pie
      arc0 = M_PI/6.0;
      arc1 = -arc0;

      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawPie({10+5*(120+10)+60, 10+3*(120+10)+40}, {50, 50}, arc0, arc1);

      pen.width = 10;
      g->SetPen(pen);

      g->DrawPie({10+5*(120+10)+60, 10+4*(120+10)+40}, {50, 50}, arc0, arc1);
      g->FillPie({10+5*(120+10)+60, 10+5*(120+10)+40}, {50, 50}, arc0, arc1);
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawPie({10+5*(120+10)+60, 10+2*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawPie({10+5*(120+10)+60, 10+3*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawPie({10+5*(120+10)+60, 10+4*(120+10)+40}, {50, 50}, arc0, arc1);
      g->DrawPie({10+5*(120+10)+60, 10+5*(120+10)+40}, {50, 50}, arc0, arc1);

      // draw rectangle miter
      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawRectangle({10+6*(120+10)+10, 3*(120+10), 100, 100});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawRectangle({10+6*(120+10)+10, 4*(120+10), 100, 100});
      g->FillRectangle({10+6*(120+10)+10, 5*(120+10), 100, 100});
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawRectangle({10+6*(120+10)+10, 2*(120+10), 100, 100});
      g->DrawRectangle({10+6*(120+10)+10, 3*(120+10), 100, 100});
      g->DrawRectangle({10+6*(120+10)+10, 4*(120+10), 100, 100});
      g->DrawRectangle({10+6*(120+10)+10, 5*(120+10), 100, 100});

      // draw rectangle bevel
      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawBevelRectangle({10+7*(120+10)+10, 3*(120+10), 100, 100});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawBevelRectangle({10+7*(120+10)+10, 4*(120+10), 100, 100});
      g->FillBevelRectangle({10+7*(120+10)+10, 5*(120+10), 100, 100});
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawBevelRectangle({10+7*(120+10)+10, 2*(120+10), 100, 100});
      g->DrawBevelRectangle({10+7*(120+10)+10, 3*(120+10), 100, 100});
      g->DrawBevelRectangle({10+7*(120+10)+10, 4*(120+10), 100, 100});
      g->DrawBevelRectangle({10+7*(120+10)+10, 5*(120+10), 100, 100});

      // draw rectangle round
      pen.width = -10;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawRoundRectangle({10+8*(120+10)+10, 3*(120+10), 100, 100});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawRoundRectangle({10+8*(120+10)+10, 4*(120+10), 100, 100});
      g->FillRoundRectangle({10+8*(120+10)+10, 5*(120+10), 100, 100});
      g->SetColor(ccolor);

      pen.width = 1;
      g->SetPen(pen);

      g->DrawRoundRectangle({10+8*(120+10)+10, 2*(120+10), 100, 100});
      g->DrawRoundRectangle({10+8*(120+10)+10, 3*(120+10), 100, 100});
      g->DrawRoundRectangle({10+8*(120+10)+10, 4*(120+10), 100, 100});
      g->DrawRoundRectangle({10+8*(120+10)+10, 5*(120+10), 100, 100});

      // draw triangle
      pen.width = 1;
      g->SetPen(pen);

      g->SetColor(ccolor);
      g->DrawTriangle({10+9*(120+10)+10, 2*(120+10)+100}, {10+9*(120+10)+10+100, 2*(120+10)+100}, {10+9*(120+10)+10+100/2, 2*(120+10)});
      g->SetColor(pcolor);

      pen.width = 10;
      pen.join = jline_join_t::Bevel;
      g->SetPen(pen);

      g->DrawTriangle({10+9*(120+10)+10, 3*(120+10)+100}, {10+9*(120+10)+10+100, 3*(120+10)+100}, {10+9*(120+10)+10+100/2, 3*(120+10)});

      pen.join = jline_join_t::Round;
      g->SetPen(pen);

      g->DrawTriangle({10+9*(120+10)+10, 4*(120+10)+100}, {10+9*(120+10)+10+100, 4*(120+10)+100}, {10+9*(120+10)+10+100/2, 4*(120+10)});

      pen.join = jline_join_t::Miter;
      g->SetPen(pen);

      g->DrawTriangle({10+9*(120+10)+10, 5*(120+10)+100}, {10+9*(120+10)+10+100, 5*(120+10)+100}, {10+9*(120+10)+10+100/2, 5*(120+10)});

      // draw polygon
      std::vector<jpoint_t<int>> hourglass = {
        {0, 0},
        {100, 0},
        {0, 100},
        {100, 100}
      };

      std::vector<jpoint_t<int>> star = {
        {50, 0},
        {85, 100},
        {0, 40},
        {100, 40},
        {15, 100}
      };

      pen.width = 1;
      pen.join = jline_join_t::Bevel;
      g->SetPen(pen);

      g->SetColor(ccolor);
      g->DrawPolygon({10+11*(120+10)+30, 2*(120+10)}, hourglass, true);
      g->SetColor(pcolor);
      g->DrawPolygon({10+10*(120+10)+10, 2*(120+10)}, hourglass, true);
      g->FillPolygon({10+11*(120+10)+30, 2*(120+10)}, hourglass);
      g->FillPolygon({10+10*(120+10)+30, 3*(120+10)}, star, false);
      g->FillPolygon({10+11*(120+10)+30, 3*(120+10)}, star, true);

      // draw lines
      int 
        x0 = 10+10*(120+10)+10,
        y0 = 5*(120+10),
        w0 = 240+10,
        h0 = 100;

      x0 = 10+10*(120+10)+10;
      y0 = 4*(120+10);

      pen.width = 40;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawLine({x0, y0}, {x0+100, y0+(240-10)});
      g->DrawLine({x0, y0+(240-10)}, {x0+100, y0});
      g->DrawLine({x0, y0+(240-10)/2}, {x0+100, y0+(240-10)/2});

      pen.width = 40;
      g->SetPen(pen);

      g->DrawLine({x0+150, y0}, {x0+250, y0+(240-10)});

      pen.width = 20;
      g->SetPen(pen);

      g->DrawLine({x0+150, y0+(240-10)}, {x0+250, y0});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawLine({x0+150, y0+(240-10)/2}, {x0+250, y0+(240-10)/2});

      pen.width = 1;
      g->SetPen(pen);

      g->SetColor(rcolor);
      g->DrawRectangle({x0, y0, 250, (240-10)});
      g->DrawLine({x0, y0+(240-10)/2}, {x0+250, y0+(240-10)/2});

      // draw line dashes
      double dashes[] = {
        50.0,  // ink
        10.0,  // skip
        10.0,  // ink
        10.0   // skip
      };

      pen.width = 5;
      pen.dashes = dashes;
      pen.dashes_size = 4;
      g->SetPen(pen);

      g->SetColor(pcolor);
      g->DrawLine({10+0*(120+10)+10, 6*(120+10)+10}, {10+10*(120+10)-20, 6*(120+10)+10});

      pen.width = 10;
      g->SetPen(pen);

      g->DrawLine({10+0*(120+10)+10, 6*(120+10)+40}, {10+10*(120+10)-20, 6*(120+10)+40});

      pen.width = 20;

      g->SetPen(pen);
      g->DrawLine({10+0*(120+10)+10, 6*(120+10)+80}, {10+10*(120+10)-20, 6*(120+10)+80});

      pen.dashes = nullptr;
      pen.dashes_size = 0;
      g->SetPen(pen);

      // draw bezier curve
      x0 = 10+10*(120+10)+10;
      y0 = 6*(120+10);
      w0 = 240+10;
      h0 = 100;

      std::vector<jpoint_t<int>> pb1 = {
        {x0, y0},
        {x0, y0+h0},
        {x0+w0, y0+h0}
      };
      std::vector<jpoint_t<int>> pb2 = {
        {x0, y0},
        {x0+w0, y0},
        {x0+w0, y0+h0}
      };

      g->SetColor(rcolor);

      pen.width = 1;

      g->SetPen(pen);

      g->DrawRectangle({x0, y0, w0, h0});
      g->SetColor(ccolor);
      g->DrawBezierCurve(pb1, 100);
      g->DrawBezierCurve(pb2, 100);

      std::shared_ptr<Font> 
        f1 = std::make_shared<Font>("default", jfont_attributes_t::None, 50),
        f2 = std::make_shared<Font>("default", jfont_attributes_t::None, 40),
        f3 = std::make_shared<Font>("default", jfont_attributes_t::None, 30),
        f4 = std::make_shared<Font>("default", jfont_attributes_t::None, 20);
      int shadow = 4;

      std::string text = "DrawString";

      rcolor(3, 0x80);
      pcolor(3, 0x80);

      g->SetColor(rcolor);

      g->SetFont(f1); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10+shadow, 0*(45+10)+20+shadow});
      g->SetFont(f2); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10+shadow, 1*(45+10)+20+shadow});
      g->SetFont(f3); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10+shadow, 2*(45+10)+20+shadow});
      g->SetFont(f4); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10+shadow, 3*(45+10)+20+shadow});

      g->SetColor(pcolor);

      g->SetFont(f1); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10, 0*(45+10)+20});
      g->SetFont(f2); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10, 1*(45+10)+20});
      g->SetFont(f3); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10, 2*(45+10)+20});
      g->SetFont(f4); 
      g->DrawString(text, jpoint_t<int>{10+7*(120+10)+10, 3*(45+10)+20});

      g->Translate({320, 0});

      int sw = f1->GetStringWidth(text);
      int sh = (3*(45+10)+20+shadow)+f4->GetSize()-(0*(45+10)+20+shadow);

      std::shared_ptr<Image> timage = std::make_shared<BufferedImage>(jpixelformat_t::RGB32, jpoint_t<int>{sw, sh});
      Graphics *gt = timage->GetGraphics();

      gt->SetColor(rcolor);
      gt->SetFont(f1); gt->DrawString(text, jpoint_t<int>{shadow, 0*(45+10)+shadow});
      gt->SetFont(f2); gt->DrawString(text, jpoint_t<int>{shadow, 1*(45+10)+shadow});
      gt->SetFont(f3); gt->DrawString(text, jpoint_t<int>{shadow, 2*(45+10)+shadow});
      gt->SetFont(f4); gt->DrawString(text, jpoint_t<int>{shadow, 3*(45+10)+shadow});

      gt->SetColor(pcolor);
      gt->SetFont(f1); gt->DrawString(text, jpoint_t<int>{0, 0*(45+10)});
      gt->SetFont(f2); gt->DrawString(text, jpoint_t<int>{0, 1*(45+10)});
      gt->SetFont(f3); gt->DrawString(text, jpoint_t<int>{0, 2*(45+10)});
      gt->SetFont(f4); gt->DrawString(text, jpoint_t<int>{0, 3*(45+10)});

      std::shared_ptr<Image> rotate = timage->Rotate(M_PI);

      g->DrawImage(rotate, jpoint_t<int>{10+7*(120+10)+10, 1*(45+10)});

      g->Translate({-320, 0});

      g->SetFont(nullptr);
    }

    virtual void Paint2(Graphics *g) 
    {
      Window::Paint(g);

      // draw lines
      g->SetColor(jcolor_name_t::Green);
      g->MoveTo({0*(150+32)+32, 0*(150+32)+64});
      g->LineTo({0*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({0*(150+32)+32, 0*(150+32)+64+150});
      g->Stroke();

      g->MoveTo({1*(150+32)+32, 0*(150+32)+64});
      g->LineTo({1*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({1*(150+32)+32, 0*(150+32)+64+150});
      g->Close();
      g->Stroke();

      g->SetColor(jcolor_name_t::Gray);
      g->MoveTo({2*(150+32)+32, 0*(150+32)+64});
      g->LineTo({2*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({2*(150+32)+32, 0*(150+32)+64+150});
      g->Close();
      g->Fill();

      g->SetColor(jcolor_name_t::Gray);
      g->MoveTo({3*(150+32)+32, 0*(150+32)+64});
      g->LineTo({3*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({3*(150+32)+32, 0*(150+32)+64+150});
      g->Close();
      g->Fill();

      jpen_t pen = g->GetPen();

      pen.width = 16;
      g->SetPen(pen);

      g->SetColor(jcolor_name_t::Green);

      jcolor_t<float> color = g->GetColor();

      color(3, 0x80);

      g->SetColor(color);

      g->MoveTo({3*(150+32)+32, 0*(150+32)+64});
      g->LineTo({3*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({3*(150+32)+32, 0*(150+32)+64+150});
      g->Close();
      g->Stroke();

      pen.width = 1;

      g->SetPen(pen);

      // draw arcs
      g->SetColor(jcolor_name_t::Green);
      g->ArcTo({0*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
      g->Stroke();

      g->ArcTo({1*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
      g->Close();
      g->Stroke();

      g->SetColor(jcolor_name_t::Gray);
      g->ArcTo({2*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
      g->Close();
      g->Fill();

      g->SetColor(jcolor_name_t::Gray);
      g->ArcTo({3*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
      g->Close();
      g->Fill();

      pen = g->GetPen();

      pen.width = 16;
      g->SetPen(pen);

      g->SetColor(jcolor_name_t::Green);

      color = g->GetColor();

      color(3, 0x80);

      g->SetColor(color);

      g->ArcTo({3*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
      g->Close();
      g->Stroke();

      pen.width = 1;
      g->SetPen(pen);

      // draw curves
      g->SetColor(jcolor_name_t::Green);
      g->CurveTo({0*(150+32)+32, 2*(150+32)+64}, {0*(150+32)+32+150, 2*(150+32)+64+0}, {0*(150+32)+32+150, 2*(150+32)+64+150});
      g->Stroke();

      g->CurveTo({1*(150+32)+32, 2*(150+32)+64}, {1*(150+32)+32+150, 2*(150+32)+64+0}, {1*(150+32)+32+150, 2*(150+32)+64+150});
      g->Close();
      g->Stroke();

      g->SetColor(jcolor_name_t::Gray);
      g->CurveTo({2*(150+32)+32, 2*(150+32)+64}, {2*(150+32)+32+150, 2*(150+32)+64+0}, {2*(150+32)+32+150, 2*(150+32)+64+150});
      g->Close();
      g->Fill();

      g->SetColor(jcolor_name_t::Gray);
      g->CurveTo({3*(150+32)+32, 2*(150+32)+64}, {3*(150+32)+32+150, 2*(150+32)+64+0}, {3*(150+32)+32+150, 2*(150+32)+64+150});
      g->Close();
      g->Fill();

      pen = g->GetPen();

      pen.width = 16;
      g->SetPen(pen);

      g->SetColor(jcolor_name_t::Green);

      color = g->GetColor();

      color(3, 0x80);

      g->SetColor(color);

      g->CurveTo({3*(150+32)+32, 2*(150+32)+64}, {3*(150+32)+32+150, 2*(150+32)+64+0}, {3*(150+32)+32+150, 2*(150+32)+64+150});
      g->Close();
      g->Stroke();

      pen.width = 1;
      g->SetPen(pen);

      // draw text
      std::shared_ptr<Font> font = std::make_shared<Font>("Sans Serif", jfont_attributes_t::None, 64);
      std::shared_ptr<Font> old = g->GetFont();

      g->SetFont(font);

      std::string text = "Hello!";

      g->SetColor(jcolor_name_t::Green);
      g->TextTo(text, {0*(150+32)+32, 3*(150+32)+64});
      g->Stroke();

      g->TextTo(text, {1*(150+32)+32, 3*(150+32)+64});
      g->Close();
      g->Stroke();

      g->SetColor(jcolor_name_t::Gray);
      g->TextTo(text, {2*(150+32)+32, 3*(150+32)+64});
      g->Close();
      g->Fill();

      g->SetColor(jcolor_name_t::Gray);
      g->TextTo(text, {3*(150+32)+32, 3*(150+32)+64});
      g->Close();
      g->Fill();

      pen = g->GetPen();

      pen.width = 16;
      g->SetPen(pen);

      g->SetColor(jcolor_name_t::Green);

      color = g->GetColor();

      color(3, 0x80);

      g->SetColor(color);

      g->TextTo(text, {3*(150+32)+32, 3*(150+32)+64});
      g->Close();
      g->Stroke();

      pen.width = 1;
      g->SetPen(pen);

      g->SetFont(old);

      // draw text
      pen.width = 10;
      g->SetPen(pen);

      g->SetColor(jcolor_name_t::Gray);
      g->MoveTo({0*(150+32)+32, 3*(150+32)+72+96});
      g->LineTo({1*(150+32)+32, 4*(150+32)+72+32});
      g->LineTo({2*(150+32)+32, 4*(150+32)+72+32});
      g->ArcTo({2*(150+32)+32, 3*(150+32)+96+72+96-10}, 32, -M_PI/2.0, M_PI/2.0);
      g->LineTo({2*(150+32)+32, 3*(150+32)+96+72});
      g->CurveTo({2*(150+32)+32, 3*(150+32)+96+72}, {3*(150+32)+32, 3*(150+32)+96+72}, {4*(150+32)+32, 3*(150+32)+96+72+72+32});
      g->Stroke();

      pen.width = 1;
      g->SetPen(pen);

      g->SetColor(jcolor_name_t::Black);
      g->MoveTo({0*(150+32)+32, 3*(150+32)+72+96});
      g->LineTo({1*(150+32)+32, 4*(150+32)+72+32});
      g->LineTo({2*(150+32)+32, 4*(150+32)+72+32});
      g->ArcTo({2*(150+32)+32, 3*(150+32)+96+72+96-10}, 32, -M_PI/2.0, M_PI/2.0);
      g->LineTo({2*(150+32)+32, 3*(150+32)+96+72});
      g->CurveTo({2*(150+32)+32, 3*(150+32)+96+72}, {3*(150+32)+32, 3*(150+32)+96+72}, {4*(150+32)+32, 3*(150+32)+96+72+72+32});
      g->Stroke();

      // draw linear pattern
      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->MoveTo({4*(150+32)+32, 0*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 0*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({4*(150+32)+32, 0*(150+32)+64+150});
      g->Close();
      g->SetPattern({4*(150+32)+32, 0*(150+32)+64}, {4*(150+32)+32+150, 0*(150+32)+64+150});

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80f0f0f0);
      g->SetGradientStop(1.0, 0x80000000);

      g->MoveTo({4*(150+32)+32, 1*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 1*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 1*(150+32)+64+150});
      g->LineTo({4*(150+32)+32, 1*(150+32)+64+150});
      g->Close();
      g->SetPattern({4*(150+32)+32, 1*(150+32)+64+150}, {4*(150+32)+32+150, 1*(150+32)+64});

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80f0f0f0);
      g->SetGradientStop(1.0, 0x80000000);

      g->MoveTo({4*(150+32)+32, 2*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 2*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 2*(150+32)+64+150});
      g->LineTo({4*(150+32)+32, 2*(150+32)+64+150});
      g->Close();
      g->SetPattern({4*(150+32)+32, 2*(150+32)+64}, {4*(150+32)+32+150, 2*(150+32)+64+150});

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->MoveTo({4*(150+32)+32, 3*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 3*(150+32)+64});
      g->LineTo({4*(150+32)+32+150, 3*(150+32)+64+150});
      g->LineTo({4*(150+32)+32, 3*(150+32)+64+150});
      g->Close();
      g->SetPattern({4*(150+32)+32, 3*(150+32)+64+150}, {4*(150+32)+32+150, 3*(150+32)+64});

      // draw radial pattern
      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->MoveTo({5*(150+32)+32, 0*(150+32)+64});
      g->ArcTo({5*(150+32)+32+150/2, 0*(150+32)+64+150/2}, 150/2, 0.0, 2*M_PI);
      g->SetPattern({5*(150+32)+32+150/2, 0*(150+32)+64+150/2}, 150/2, {5*(150+32)+32+150/2-32, 0*(150+32)+64+150/2-32}, 150/6);

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->MoveTo({5*(150+32)+32, 1*(150+32)+64});
      g->ArcTo({5*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2, 0.0, 2*M_PI);
      g->SetPattern({5*(150+32)+32+150/2, 1*(150+32)+64+150/2}, 150/2, {5*(150+32)+32+150/2+32, 1*(150+32)+64+150/2-32}, 150/6);

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->MoveTo({5*(150+32)+32, 2*(150+32)+64});
      g->ArcTo({5*(150+32)+32+150/2, 2*(150+32)+64+150/2}, 150/2, 0.0, 2*M_PI);
      g->SetPattern({5*(150+32)+32+150/2, 2*(150+32)+64+150/2}, 150/2, {5*(150+32)+32+150/2+32, 2*(150+32)+64+150/2+32}, 150/6);

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->MoveTo({5*(150+32)+32, 3*(150+32)+64});
      g->ArcTo({5*(150+32)+32+150/2, 3*(150+32)+64+150/2}, 150/2, 0.0, 2*M_PI);
      g->SetPattern({5*(150+32)+32+150/2, 3*(150+32)+64+150/2}, 150/2, {5*(150+32)+32+150/2-32, 3*(150+32)+64+150/2+32}, 150/6);

      // draw image pattern
      std::shared_ptr<Image> image1 = std::make_shared<BufferedImage>("images/red_icon.png");
      std::shared_ptr<Image> image2 = std::make_shared<BufferedImage>("images/green_icon.png");
      std::shared_ptr<Image> image3 = std::make_shared<BufferedImage>("images/yellow_icon.png");
      std::shared_ptr<Image> image4 = std::make_shared<BufferedImage>("images/blue_icon.png");

      g->MoveTo({6*(150+32)+32, 0*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 0*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 0*(150+32)+64+150});
      g->LineTo({6*(150+32)+32, 0*(150+32)+64+150});
      g->Close();
      g->SetPattern(image1);

      g->MoveTo({6*(150+32)+32, 1*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 1*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 1*(150+32)+64+150});
      g->LineTo({6*(150+32)+32, 1*(150+32)+64+150});
      g->Close();
      g->SetPattern(image2);

      g->MoveTo({6*(150+32)+32, 2*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 2*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 2*(150+32)+64+150});
      g->LineTo({6*(150+32)+32, 2*(150+32)+64+150});
      g->Close();
      g->SetPattern(image3);

      g->MoveTo({6*(150+32)+32, 3*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 3*(150+32)+64});
      g->LineTo({6*(150+32)+32+150, 3*(150+32)+64+150});
      g->LineTo({6*(150+32)+32, 3*(150+32)+64+150});
      g->Close();
      g->SetPattern(image4);

      // draw mask
      g->SetFont(font);

      g->ResetGradientStop();
      g->SetGradientStop(0.0, 0x80000000);
      g->SetGradientStop(1.0, 0x80f0f0f0);

      g->TextTo(text, {7*(150+32)+32, 0*(150+32)+64});
      g->SetPattern({7*(150+32)+32, 0*(150+32)+64}, {7*(150+32)+32+150, 0*(150+32)+64+150});

      g->TextTo(text, {7*(150+32)+32, 1*(150+32)+64});
      g->SetPattern({7*(150+32)+32+150, 1*(150+32)+64}, {7*(150+32)+32, 1*(150+32)+64+150});

      g->TextTo(text, {7*(150+32)+32, 2*(150+32)+64});
      g->SetPattern({7*(150+32)+32+150, 2*(150+32)+64+150}, {7*(150+32)+32, 2*(150+32)+64});

      g->TextTo(text, {7*(150+32)+32, 3*(150+32)+64});
      g->SetPattern({7*(150+32)+32, 3*(150+32)+64+150}, {7*(150+32)+32+150, 3*(150+32)+64});
    }

};

int main(int argc, char **argv)
{
  Application::Init(argc, argv);

  Primitives app;

  app.SetTitle("Primitives");

  Application::Loop();

  return 0;
}
