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
/*
 *  Original Code from:
 *
 * Let's make 16 games in C++: TETRIS
 *
 * youtube.com/watch?v=zH_omFPqMO4
 *
 */

#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jbufferedimage.h"

#include <time.h>

using namespace jcanvas;

const int M = 20;
const int N = 10;

int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

class Tetris : public Window, public KeyListener {

  private:
    std::shared_ptr<Image> s1;
    std::shared_ptr<Image> s2;
    jpoint_t<int> a[4];
    jpoint_t<int> b[4];
    int colorNum;
    int field[M][N];
    int dx; 
    int timer;
    bool rotate;

  private:
    bool check()
    {
      for (int i=0;i<4;i++)
        if (a[i].x<0 || a[i].x>=N || a[i].y>=M) return 0;
        else if (field[a[i].y][a[i].x]) return 0;

      return 1;
    };

  public:
		Tetris():
			Window({320, 480})
		{
      SetFramesPerSecond(30);

      s1 = std::make_shared<BufferedImage>("images/tetris/tiles.png");
      s2 = std::make_shared<BufferedImage>("images/tetris/background.png");
    
      colorNum = 0;
      rotate = false;
      dx=0; 
      timer = 0;

      for (int j=0; j<M; j++) {
        for (int i=0; i<N; i++) {
          field[j][i] = 0;
        }
      }

      for (int i=0; i<4; i++) {
        a[i].x = -1;
        a[i].y = -1;

        b[i] = a[i];
      }
    }

    virtual ~Tetris()
    {
    }

    bool KeyPressed(KeyEvent *event)
    {
			if (event->GetSymbol() == jkeyevent_symbol_t::CursorUp) {
        rotate = true;
      }

      return true;
    }

    bool KeyReleased(KeyEvent *event)
    {
			if (event->GetSymbol() == jkeyevent_symbol_t::CursorUp) {
        rotate = false;
      }

      return true;
    }
		void Paint(Graphics *g) 
		{
      EventManager 
        &ev = GetEventManager();

      dx = 0;

      if (ev.IsKeyDown(jkeyevent_symbol_t::CursorLeft)) {
        dx = -1;
      } else if (ev.IsKeyDown(jkeyevent_symbol_t::CursorRight)) {
        dx = 1;
      } else if (ev.IsKeyDown(jkeyevent_symbol_t::CursorDown)) {
        timer = timer + 4;
      }

      g->SetCompositeFlags(jcomposite_flags_t::SrcOver);

      //// <- Move -> ///
      for (int i=0; i<4; i++)  { 
        b[i] = a[i]; 
        a[i].x += dx; 
      }

      if (!check()) {
        for (int i=0; i<4; i++) {
          a[i] = b[i];
        }
      }

      // rotate
      if (rotate) {
        jpoint_t<int> p = a[1]; //center of rotation

        for (int i=0; i<4; i++) {
          int x = a[i].y - p.y;
          int y = a[i].x - p.x;

          a[i].x = p.x - x;
          a[i].y = p.y + y;
        }

        if (!check()) {
          for (int i=0; i<4; i++) {
            a[i] = b[i];
          }
        }
      }

      // timer tick
      if (timer++ > 4) {
        for (int i=0; i<4; i++) { 
          b[i] = a[i]; 
          a[i].y += 1; 
        }

        if (!check()) {
          for (int i=0; i<4; i++) {
            field[b[i].y][b[i].x] = colorNum;
          }

          colorNum = 1 + rand()%7;

          int n = rand()%7;

          for (int i=0; i<4; i++) {
            a[i].x = figures[n][i] % 2 + N/2 - 1;
            a[i].y = figures[n][i] / 2;
          }
        }

        timer = 0;
      }

      // check lines
      int k = M-1;

      for (int i=M - 1; i>0; i--) {
        int count = 0;

        for (int j=0; j<N; j++) {
          if (field[i][j]) {
            count++;
          }

          field[k][j] = field[i][j];
        }

        if (count < N) {
          k--;
        }
      }

      rotate = false;
      dx = 0;

      // draw
      g->DrawImage(s2, jpoint_t<int>{0, 0});

      for (int i=0; i<M; i++) {
        for (int j=0; j<N; j++) {
          if (field[i][j] == 0) {
            continue;
          }

          g->DrawImage(s1, {field[i][j]*18, 0, 18, 18}, {j*18 + 28, i*18 + 31, 18, 18});
        }
      }

      for (int i=0; i<4; i++) {
        g->DrawImage(s1, {colorNum*18, 0, 18, 18}, {a[i].x*18 + 28, a[i].y*18 + 31, 18, 18});
      }
      
      Repaint();
    }

};

int main(int argc, char **argv)
{
  Application::Init(argc, argv);

  Tetris app;

  app.SetTitle("Tetris");
  app.Exec();

  Application::Loop();

  return 0;
}
