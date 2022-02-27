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
/**
 * \author RosettaCode
 *
 * \address http://rosettacode.org/wiki/2048
 *
 */
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jbufferedimage.h"

#include <sstream>

using namespace jcanvas;

enum move_direction_t { 
	UP, 
	DOWN, 
	LEFT, 
	RIGHT 
};

class Tile {

	public:
		int val;
		bool blocked;

	public:
		Tile() {
			val = 0;
			blocked = false;
		}

};

class Game2048 : public Window, public KeyListener {
	
	private:
		std::map<int, std::shared_ptr<Image>> _images;
		Tile _board[4][4];
		int _score;
		bool _is_win;
		bool _is_done;
		bool _is_moved;

	private:
		virtual void Paint(Graphics *g) 
		{
			Window::Paint(g);

			int left = 4;
      int top = 4;
      int right = 0;
      int bottom = 0;

			g->SetColor(jcolor_name_t::White);
			g->SetFont(Font::Size16);

			jpoint_t<int>
				size = GetSize();

			if (_is_done) {
				std::string s = "Game Over!";

				if (_is_win) {
					s = "You've made it!";
				}

				g->DrawString(s, jpoint_t<int>{left, size.y - bottom - 16});
			} else {
				std::ostringstream o;

				o << "score:: " << _score;

				g->DrawString(o.str(), jpoint_t<int>{left, size.y - bottom - 16});
			}

			int gap = 4;
			int bw = (size.x - left - right - 4*gap)/4;
			int bh = (size.y - top - bottom - 4*gap - 16)/4;

			g->SetColor(jcolor_name_t::Gray);

			for (int i=0; i<4; i++) {
				for (int j=0; j<4; j++) {
					g->DrawRectangle({left+i*(bw+gap), top+j*(bh+gap), bw, bh});
				}
			}

			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (_board[x][y].val > 0) {
						std::ostringstream o;
						
						g->DrawImage(_images[_board[x][y].val], {x*(bw+gap)+left, y*(bh+gap)+top, bw, bh});
					}
				}
			}
		}

		virtual bool KeyPressed(KeyEvent *event)
		{
			_is_moved = false; 
			
			if (event->GetSymbol() == jkeyevent_symbol_t::CursorLeft) {
				move(LEFT);
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorRight) {
				move(RIGHT);
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorUp) {
				move(UP);
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorDown) {
				move(DOWN);
			} else {
				return false;
			}
			
			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					_board[x][y].blocked = false;
				}
			}

			addTile();

			Repaint();

			return true;
		}

		void addTile()
		{
			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (!_board[x][y].val) {
						int a, b;

						do { 
							a = rand() % 4; b = rand() % 4; 
						} while (_board[a][b].val);

						int s = rand() % 100;

						if (s > 89) 
							_board[a][b].val = 4;
						else 
							_board[a][b].val = 2;

						if (canMove()) 
							return;
					}
				}
			}

			_is_done = true;
		}

		bool canMove()
		{
			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (!_board[x][y].val) {
						return true;
					}
				}
			}

			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (testAdd(x+1, y, _board[x][y].val)) {
						return true;
					}

					if (testAdd(x-1, y, _board[x][y].val)) {
						return true;
					}

					if (testAdd(x, y+1, _board[x][y].val)) {
						return true;
					}

					if (testAdd(x, y-1, _board[x][y].val)) {
						return true;
					}
				}
			}

			return false;
		}

		bool testAdd(int x, int y, int v)
		{
			if (x < 0 || x > 3 || y < 0 || y > 3) {
				return false;
			}

			return _board[x][y].val == v;
		}

		void moveVert( int x, int y, int d )
		{
			if (_board[x][y+d].val && _board[x][y+d].val == _board[x][y].val && !_board[x][y].blocked && !_board[x][y+d].blocked) {
				_board[x][y].val = 0;
				_board[x][y + d].val *= 2;
				_score += _board[x][y + d].val;
				_board[x][y + d].blocked = true;
				_is_moved = true;
			} else if (!_board[x][y+d].val && _board[x][y].val) {
				_board[x][y + d].val = _board[x][y].val;
				_board[x][y].val = 0;
				_is_moved = true;
			}
		
			if (d > 0) { 
				if (y+d < 3) {
					moveVert(x, y+d, 1); 
				}
			} else { 
				if (y+d > 0) {
					moveVert(x, y+d, -1); 
				}
			}
		}
		
		void moveHori( int x, int y, int d )
		{
			if (_board[x+d][y].val && _board[x+d][y].val == _board[x][y].val && !_board[x][y].blocked && !_board[x+d][y].blocked) {
				_board[x][y].val = 0;
				_board[x + d][y].val *= 2;
				_score += _board[x + d][y].val;
				_board[x + d][y].blocked = true;
				_is_moved = true;
			} else if (!_board[x+d][y].val && _board[x][y].val) {
				_board[x + d][y].val = _board[x][y].val;
				_board[x][y].val = 0;
				_is_moved = true;
			}

			if (d > 0) { 
				if (x+d < 3) {
					moveHori( x + d, y,  1 ); 
				}
			} else { 
				if (x+d > 0) {
					moveHori(x+d, y, -1); 
				}
			}
		}

		void move(move_direction_t d)
		{
			switch(d) {
				case UP:
					for (int x=0; x<4; x++) {
						int y = 1;

						while (y < 4) { 
							if (_board[x][y].val) {
								moveVert(x, y, -1);
							}

							y++;
						}
					}
					break;
				case DOWN:
					for (int x=0; x<4; x++) {
						int y = 2;

						while (y >= 0) { 
							if (_board[x][y].val) {
								moveVert(x, y, 1);
							}

							y--;
						}
					}
					break;
				case LEFT:
					for (int y=0; y<4; y++) {
						int x = 1;

						while (x < 4) { 
							if (_board[x][y].val) {
								moveHori(x, y, -1);
							}

							x++;
						}
					}
					break;
				case RIGHT:
					for (int y=0; y<4; y++) {
						int x = 2;

						while (x >= 0) { 
							if (_board[x][y].val) {
								moveHori(x, y, 1);
							}

							x--;
						}
					}
			}
		}

	public:
		Game2048():
			Window(jpoint_t<int>{400, 400})
		{
			_is_done = false;
			_is_win = false;
			_is_moved = true;
			_score = 0;
			
			for (int i=2; i<=8192;) {
				std::ostringstream o;

				o << i;

				_images[i] = std::make_shared<BufferedImage>("images/2048/tux-n" + o.str() + ".png");

				i = i*2;
			}

			addTile(); 
		}

		virtual ~Game2048()
		{
		}

};

int main( int argc, char* argv[] )
{
	Application::Init(argc, argv);

	srand(static_cast<int>(time(nullptr)));

	Game2048 app; 

	app.SetTitle("2048");

	Application::Loop();

	return 0;
}
