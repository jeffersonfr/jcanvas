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

using namespace jcanvas;

class Text : public Window {

  public:
    Text():
      Window(jpoint_t<int>{1600, 900})
    {
    }

    virtual ~Text()
    {
    }

    virtual void Paint(Graphics *g) 
    {
      Window::Paint(g);

      std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam eget ligula eu lectus lobortis condimentum. Aliquam nonummy auctor massa. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nulla at risus. Quisque purus magna, auctor et, sagittis ac, posuere eu, lectus. Nam mattis, felis ut adipiscing.";

      jrect_t<int> rect = GetBounds();
      int dw = rect.size.x/4;
      int dh = rect.size.y/4;

      for (int j=0; j<4; j++) {
        for (int i=0; i<4; i++) {
          g->DrawRectangle(jrect_t<int>{i*dw, j*dh, dw, dh});
        }
      }

      g->SetFont(&Font::Size16);
      g->SetColor(jcolorname::White);

      g->DrawString(text, jrect_t<int>{0*dw, 0*dh, dw, dh}, JHA_LEFT, JVA_TOP);
      g->DrawString(text, jrect_t<int>{1*dw, 0*dh, dw, dh}, JHA_CENTER, JVA_TOP);
      g->DrawString(text, jrect_t<int>{2*dw, 0*dh, dw, dh}, JHA_RIGHT, JVA_TOP);
      g->DrawString(text, jrect_t<int>{3*dw, 0*dh, dw, dh}, JHA_JUSTIFY, JVA_TOP);

      g->DrawString(text, jrect_t<int>{0*dw, 1*dh, dw, dh}, JHA_LEFT, JVA_CENTER);
      g->DrawString(text, jrect_t<int>{1*dw, 1*dh, dw, dh}, JHA_CENTER, JVA_CENTER);
      g->DrawString(text, jrect_t<int>{2*dw, 1*dh, dw, dh}, JHA_RIGHT, JVA_CENTER);
      g->DrawString(text, jrect_t<int>{3*dw, 1*dh, dw, dh}, JHA_JUSTIFY, JVA_CENTER);

      g->DrawString(text, jrect_t<int>{0*dw, 2*dh, dw, dh}, JHA_LEFT, JVA_BOTTOM);
      g->DrawString(text, jrect_t<int>{1*dw, 2*dh, dw, dh}, JHA_CENTER, JVA_BOTTOM);
      g->DrawString(text, jrect_t<int>{2*dw, 2*dh, dw, dh}, JHA_RIGHT, JVA_BOTTOM);
      g->DrawString(text, jrect_t<int>{3*dw, 2*dh, dw, dh}, JHA_JUSTIFY, JVA_BOTTOM);

      g->DrawString(text, jrect_t<int>{0*dw, 3*dh, dw, dh}, JHA_LEFT, JVA_JUSTIFY);
      g->DrawString(text, jrect_t<int>{1*dw, 3*dh, dw, dh}, JHA_CENTER, JVA_JUSTIFY);
      g->DrawString(text, jrect_t<int>{2*dw, 3*dh, dw, dh}, JHA_RIGHT, JVA_JUSTIFY);
      g->DrawString(text, jrect_t<int>{3*dw, 3*dh, dw, dh}, JHA_JUSTIFY, JVA_JUSTIFY);
    }

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  Text app;

  Application::Loop();

  return 0;
}

