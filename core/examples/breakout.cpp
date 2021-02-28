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
 * \brief Original code by Brian Postma
 *
 */
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jbufferedimage.h"

using namespace jcanvas;

int	borderwidth = 5;
int	batwidth = 50;
int	ballsize = 5;
int	batheight = 5;
int	scoreheight = 20;
int	screendelay = 300;
int	brickwidth = 15;
int	brickheight = 8;
int	brickspace = 2;
int	numlines = 4;
int	startline = 32;
int	delay = 10;

double myrandom()
{
	return ((double)(random()%1000))/1000.0;
}

class Breakout : public Window, public KeyListener {

	private:
		Image 
      *off;
		Graphics 
      *goff;
		int	player1score,
	  	ballx,
      bally,
      batpos,
      batdpos,
      balldx ,
      balldy,
      dxval,
      ballsleft,
      count,
      bricksperline;
		bool ingame,
		  showtitle,
		  *showbrick;

	public:
		Breakout():
			Window(/*"BreakOut", */ {720, 480})
		{
      SetFramesPerSecond(60);

			off = nullptr;
			goff = nullptr;

			player1score = 0;
			ballx = 0;
			bally = 0;
			batpos = 0;
			batdpos = 0;
			balldx = 0;
			balldy = 0;
			dxval = 0;
			ballsleft = 0;
			count = 0;
			ingame = false;
			showtitle = true;

			bricksperline = (720 - 2*borderwidth)/(brickwidth + brickspace);

			showbrick = new bool[bricksperline*numlines];

			GameInit();
		}

		virtual ~Breakout()
		{
      delete off;

      delete [] showbrick;
		}

		void GameInit()
		{
      jpoint_t<int>
        size = GetSize();

			batpos = (size.x-batwidth)/2;
			ballx = (size.x-ballsize)/2;
			bally = (size.y-ballsize-scoreheight-2*borderwidth);
			player1score = 0;
			ballsleft = 3;
			dxval = 2;

			if (myrandom() < 0.5) {
				balldx = dxval;
			} else {
				balldx = -dxval;
			}

			balldy = -dxval;
			count = screendelay;
			batdpos = 0;

			InitBricks();
		}

		void InitBricks()
		{
			for (int i=0; i<numlines*bricksperline; i++)
				showbrick[i] = true;
		}

		virtual bool KeyPressed(jcanvas::KeyEvent *event)
		{
			if (Window::KeyPressed(event) == true) {
				return true;
			}

			if (ingame) {
				if (event->GetSymbol() == jcanvas::JKS_CURSOR_LEFT) {
					batdpos = -3;
				}

				if (event->GetSymbol() == jcanvas::JKS_CURSOR_RIGHT) {
					batdpos = 3;
				}
			} else {
				if (event->GetSymbol() == jcanvas::JKS_SPACE) {
					ingame = true;

					GameInit();
				}
			}

			return true;
		}

		/*
		virtual bool KeyReleased(KeyEvent *event)
		{
			if (Window::KeyReleased(event) == true) {
				return true;
			}

			batdpos = 0;

			return true;
		}
		*/

		virtual void Paint(Graphics *g)
		{
      jpoint_t<int>
        size = GetSize();

			if (goff == nullptr && size.x > 0 && size.y > 0) {
				off = new BufferedImage(JPF_RGB32, size);

				goff = off->GetGraphics();
			}

			if (goff == nullptr || off == nullptr) {
				return;
			}

			if (ingame) {
				PlayGame();
			} else {
				ShowIntroScreen();
			}

			g->DrawImage(off, jpoint_t<int>{0, 0});

      Repaint();
		}


		void PlayGame()
		{
      goff->Clear();

			MoveBall();
			CheckBat();
			CheckBricks();
			DrawPlayField();
			DrawBricks();
			ShowScore();
		}

		void ShowIntroScreen()
		{
      goff->Clear();

			MoveBall();
			CheckBat();
			CheckBricks();
			BatDummyMove();
			DrawPlayField();
			DrawBricks();
			ShowScore();
	
			goff->SetColor({0x60, 0x80, 0xff, 0xff});

			if (--count <= 0) { 
				count = screendelay; 
				showtitle = !showtitle;
			}
			
			Font *font = &Font::Size16;
			jpoint_t<int> size = GetSize();

			if (font != nullptr) {
        size.x = size.x - font->GetStringWidth("Pressione SPACE para iniciar");

				goff->DrawString("Pressione SPACE para iniciar", jpoint_t<int>{size.x/2, size.y/2});
			}
		}

		void DrawBricks()
		{
			int i,
					j,
					colordelta = 255/(numlines-1);
			bool nobricks = true;

			for (j=0; j<numlines; j++) {
				for (i=0; i<bricksperline; i++) {
					if (showbrick[j*bricksperline+i]) {
						nobricks = false;

						goff->SetColor({0xff, j*colordelta, 0xff-j*colordelta, 0xff});
						goff->FillRectangle({borderwidth+i*(brickwidth+brickspace), startline+j*(brickheight+brickspace), brickwidth, brickheight});
					}
				}
			}

			if (nobricks) {
				InitBricks();
				
				if (ingame) {
					player1score += 100;
				}
			}
		}

		void DrawPlayField()
		{
      jpoint_t<int>
        size = GetSize();

			goff->SetColor({0xff, 0xff, 0xff, 0xff});
			goff->FillRectangle({0, 0, size.x, borderwidth});
			goff->FillRectangle({0, 0, borderwidth, size.y});
			goff->FillRectangle({size.x-borderwidth, 0, borderwidth, size.y});
			goff->FillRectangle({0, size.y-borderwidth, size.x, borderwidth});
			goff->FillRectangle({batpos, size.y-2*borderwidth-scoreheight, batwidth,batheight}); // bat
			goff->FillRectangle({ballx, bally, ballsize, ballsize}); // ball
		}

		void ShowScore()
		{
			Font 
        *font = &Font::Size16;
      jpoint_t<int>
        size = GetSize();

			if (font == nullptr) {
				return;
			}

			char tmp[255];

			goff->SetFont(font);

			goff->SetColor({0xff, 0xff, 0xff, 0xff});

			sprintf(tmp, "Score: %d", player1score); 
			goff->DrawString(tmp, jpoint_t<int>{borderwidth, borderwidth});
			sprintf(tmp, "Balls left: %d", ballsleft); 
			goff->DrawString(tmp, jpoint_t<int>{size.x-borderwidth-font->GetStringWidth(tmp), borderwidth});
		}

		void MoveBall()
		{
      jpoint_t<int>
        size = GetSize();

			ballx += balldx;
			bally += balldy;

			if (bally <= borderwidth) {
				balldy = -balldy;
				bally = borderwidth;
			}

			if (bally >= (size.y-ballsize-scoreheight)) {
				if (ingame) {
					ballsleft--;
					
					if (ballsleft <= 0) {
						ingame = false;
					}
				}

				ballx = batpos+(batwidth-ballsize)/2;
				bally = startline+numlines*(brickheight+brickspace);
				balldy = dxval;
				balldx = 0;
			}

			if (ballx >= (size.x-borderwidth-ballsize)) {
				balldx = -balldx;
				ballx = size.x-borderwidth-ballsize;
			}

			if (ballx <= borderwidth) {
				balldx = -balldx;
				ballx = borderwidth;
			}
		}

		void BatDummyMove()
		{
			if (ballx < (batpos+2)) {
				batpos -= 3;
			} else if (ballx>(batpos+batwidth-3)) {
				batpos += 3;
			}
		}

		void CheckBat()
		{
      jpoint_t<int>
        size = GetSize();

			batpos += batdpos;

			if (batpos < borderwidth) {
				batpos = borderwidth;
			} else if (batpos > (size.x-borderwidth-batwidth)) {
				batpos = (size.x-borderwidth-batwidth);
			}

			if (bally >= (size.y-scoreheight-2*borderwidth-ballsize) && bally < (size.y-scoreheight-2*borderwidth) && (ballx+ballsize) >= batpos && ballx <= (batpos+batwidth)) {
				bally = size.y-scoreheight-ballsize-borderwidth*2;
				balldy = -dxval;
				balldx = CheckBatBounce(balldx,ballx-batpos);
			}
		}

		int CheckBatBounce(int dy, int delta)
		{
			int i = -ballsize, 
					j = 0,
					sign = 0,
					stepsize = (ballsize+batwidth)/8;

			if (dy > 0) {
				sign = 1;
			} else {
				sign = -1;
			}

			while (i < batwidth && delta > i) {
				i += stepsize;
				j++;
			}

			switch(j) {
				case 0:
				case 1:
					return -4;
				case 2:
					return -3;
				case 7:
					return 3;
				case 3:
				case 6:
					return sign*2;
				case 4:
				case 5:
					return sign*1;
				default:
					return 4;
			}
		}

		void CheckBricks()
		{
			int i,
					j,
					x,
					y,
					xspeed = balldx;

			if (xspeed < 0) {
				xspeed=-xspeed;
			}

			int ydir = balldy;

			if (bally < (startline-ballsize) || bally > (startline+numlines*(brickspace+brickheight))) {
				return;
			}

			for (j=0; j<numlines; j++) {
				for (i=0; i<bricksperline; i++) {
					if (showbrick[j*bricksperline+i]) {
						y = startline+j*(brickspace+brickheight);
						x = borderwidth+i*(brickspace+brickwidth);
						
						if (bally >= (y-ballsize) && bally < (y+brickheight) && ballx >= (x-ballsize) && ballx < (x+brickwidth)) {
							showbrick[j*bricksperline+i] = false;

							if (ingame) {
								player1score+=(numlines-j);
							}

							// Where did we hit the brick
							if (ballx >= (x-ballsize) && ballx <= (x-ballsize+3)) { // leftside
								balldx = -xspeed;
							} else if (ballx <= (x+brickwidth-1) && ballx >= (x+brickwidth-4)) { // rightside
								balldx = xspeed;
							}

							balldy = -ydir;
						}
					}
				}
			}
		}

};

int main(int argc, char **argv) 
{
	Application::Init(argc, argv);

	Breakout app;

	app.SetTitle("Breakout");
	app.Exec();

	Application::Loop();

	return 0;
}
