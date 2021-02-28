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

class Mines : public Window, public KeyListener {

	enum game_status_t {
		NONE,
		WIN,
		LOSE
	};

	enum block_type_t {
		BOMB_BLOCK,
		SAFE_BLOCK
	};

	enum block_state_t {
		CLOSED_BLOCK,
		OPENED_BLOCK,
		MARKED_BLOCK
	};

	struct block_t {
		block_type_t type;
		block_state_t state;
		int value;
	};

	private:
		Image *small_bomb;
		Image *huge_bomb;
		Image *flag;
		Image *smile_face;
		Image *dead_face;
		Image *image1;
		Image *image2;
		Image *image3;
    std::mutex mines_mutex;
		block_t *board;
		int isize;
		int max_rows;
		int max_cols;
		int current_row;
		int current_col;
		int max_bombs;
		int hide_bombs;
		game_status_t game_state;

	public:
		Mines(int x, int y);
		virtual ~Mines();

		void InitializeFlags();
		void SetupBoard();
		void Expose(int row, int col);
		game_status_t GetResult();
		virtual bool KeyPressed(jcanvas::KeyEvent *event);
		virtual void Paint(Graphics *g);

};

Mines::Mines(int x, int y):
	Window(jpoint_t<int>{720, 480})
{
	srand((int)time(nullptr));

	isize = 36;
	max_rows = 10;
	max_cols = 10;
	max_bombs = 20;

	board = new block_t[max_rows*max_cols+10];

	Image *image;

	image = new BufferedImage("images/bomb.png");
	small_bomb = image->Scale({isize, isize});
	huge_bomb = image->Scale({4*isize, 4*isize});
	delete image;

	image = new BufferedImage("images/flag.png");
	flag = image->Scale({isize, isize});
	delete image;

	image = new BufferedImage("images/smile_face.png");
	smile_face = image->Scale({4*isize, 4*isize});
	delete image;

	image = new BufferedImage("images/dead_face.png");
	dead_face = image->Scale({4*isize, 4*isize});
	delete image;

	SetupBoard();

  image1 = new BufferedImage("images/blue_icon.png");
  image2 = new BufferedImage("images/yellow_icon.png");
  image3 = new BufferedImage("images/green_icon.png");

	// AddSubtitle(image1, "Novo Jogo");
	// AddSubtitle(image2, "Flag");
	// AddSubtitle(image3, "Ajuda");
}

Mines::~Mines() 
{
	delete [] board;

	delete small_bomb;
	delete huge_bomb;
	delete flag;
	delete smile_face;
	delete dead_face;

  delete image1;
  delete image2;
  delete image3;
}

void Mines::Paint(Graphics *g)
{
	Window::Paint(g);

	Font *font = &Font::Size16;
	int delta = 4;
  int left = 0;
  int top = 0;
  int right = 0;
  int bottom = 0;

	g->SetFont(font);

	for (int i=0; i<max_cols; i++) {
		for (int j=0; j<max_rows; j++) {
			block_t block = board[j*max_cols+i];

			g->SetColor({0x80, 0x80, 0x80, 0xff});

			if (block.state == CLOSED_BLOCK) {
				g->FillRectangle({left+i*(isize+delta), top+j*(isize+delta), isize, isize});
			} else if (block.state == OPENED_BLOCK) {
				g->SetColor({0xd0, 0xd0, 0xd0, 0xff});
				g->FillRectangle({left+i*(isize+delta), top+j*(isize+delta), isize, isize});

				if (block.type == BOMB_BLOCK) {
					g->FillRectangle({left+i*(isize+delta), top+j*(isize+delta), isize, isize});
					g->DrawImage(small_bomb, jpoint_t<int>{left+i*(isize+delta), top+j*(isize+delta)});
				} else {
					if (block.value != 0) {
						char tmp[256];

						sprintf(tmp, "%d", block.value);

						g->SetColor({0xff, 0x00, 0x00, 0xff});
						g->DrawString(tmp, {left+i*(isize+delta), top+j*(isize+delta), isize, isize}, JHA_CENTER, JVA_CENTER);
					}
				}
			} else if (block.state == MARKED_BLOCK) {
				g->FillRectangle({left+i*(isize+delta), top+j*(isize+delta), isize, isize});
				g->DrawImage(flag, jpoint_t<int>{left+i*(isize+delta), top+j*(isize+delta)});
			}
		}
	}

	g->SetColor(jcanvas::jcolorname::White);

	jpoint_t
		size = GetSize();
	int 
		x = size.x - 180,
		y = 260,
		w = 160,
		h = 140;
	char tmp[255];


	if (GetResult() != LOSE) {
		g->DrawString("You Win", jpoint_t<int>{size.x - 150, 80});
		g->DrawImage(smile_face, {size.x - 200, 80, 160, 140});
	} else {
		g->DrawString("You Lost", jpoint_t<int>{size.x - 150, 80});
		g->DrawImage(dead_face, {size.x - 200, 80, 160, 140});
	}

	sprintf(tmp, "bombs [ %02d ]", hide_bombs);

	g->DrawImage(huge_bomb, {x, y, w, h});

	g->SetColor({0x00, 0x00, 0x00, 0xff});
	g->FillRectangle({x, y+h+10, w, font->GetSize()});
	g->SetColor({0xf0, 0xf0, 0xf0, 0xff});
	g->DrawString(tmp, jpoint_t<int>{x+10, y+h+10});
				
	g->SetColor({0x20, 0x40, 0xa0, 0x80});
	g->FillRectangle({left+current_col*(isize+delta), top+current_row*(isize+delta), isize, isize});
}

bool Mines::KeyPressed(jcanvas::KeyEvent *event)
{
	if (Window::KeyPressed(event) == true) {
		return true;
	}

	if (GetResult() == NONE) {
		if (event->GetSymbol() == jcanvas::JKS_CURSOR_RIGHT) {
			if (current_col < max_cols-1) {
				current_col++;
			}
		} else if (event->GetSymbol() == jcanvas::JKS_CURSOR_LEFT) {
			if (current_col > 0) {
				current_col--;
			}
		} else if (event->GetSymbol() == jcanvas::JKS_CURSOR_UP) {
			if (current_row > 0) {
				current_row--;
			}
		} else if (event->GetSymbol() == jcanvas::JKS_CURSOR_DOWN) {
			if (current_row < max_rows-1) {
				current_row++;
			}
		} else if (event->GetSymbol() == jcanvas::JKS_ENTER) {
			block_t *block = &board[current_row*max_cols+current_col];

			// INFO:: just open a block
			if (block->type == SAFE_BLOCK) {
				if (block->state == MARKED_BLOCK) {
					hide_bombs++;

					block->state = CLOSED_BLOCK;
				}

				if (block->state == CLOSED_BLOCK) {
					Expose(current_row, current_col);
				}
			} else if (block->type == BOMB_BLOCK) {
				game_state = LOSE;
			}

			int count = 0;

			for (int i=0; i<max_rows*max_cols; i++) {
				if (board[i].state == CLOSED_BLOCK || board[i].state == MARKED_BLOCK) {
					count++;
				}
			}

			if (count == max_bombs) {
				game_state = WIN;
			}
		} else if (event->GetSymbol() == jcanvas::JKS_RED || event->GetSymbol() == jcanvas::JKS_F1) {
		} else if (event->GetSymbol() == jcanvas::JKS_GREEN || event->GetSymbol() == jcanvas::JKS_F2) {
			// Window *widget = new MessageDialogBox("Ajuda", "O jogo termina quando todos os blocos seguros forem revelados. Utilize as bandeirinhas para ajudar a identificar as minas escondidas.");
		} else if (event->GetSymbol() == jcanvas::JKS_YELLOW || event->GetSymbol() == jcanvas::JKS_F3) {
			block_t *block = &board[current_row*max_cols+current_col];

			if (block->state == CLOSED_BLOCK) {
				block->state = MARKED_BLOCK;

				hide_bombs--;
			} else if (block->state == MARKED_BLOCK) {
				block->state = CLOSED_BLOCK;

				hide_bombs++;
			}
		} else if (event->GetSymbol() == jcanvas::JKS_BLUE || event->GetSymbol() == jcanvas::JKS_F4) {
			SetupBoard();
		}

		if (GetResult() == WIN) {
			for (int i=0; i<max_rows*max_cols; i++) {
				if (board[i].type == BOMB_BLOCK) {
					board[i].state = MARKED_BLOCK;
				}
			}
		} else if (GetResult() == LOSE) {
			for (int i=0; i<max_rows*max_cols; i++) {
				if (board[i].type == BOMB_BLOCK) {
					board[i].state = OPENED_BLOCK;
				}
			}
		}

		Repaint();
	} else if (event->GetSymbol() == jcanvas::JKS_BLUE || event->GetSymbol() == jcanvas::JKS_F4) {
		SetupBoard();
	}

	return true;
}

void Mines::InitializeFlags()
{
	current_row = 0;
	current_col = 0;
	hide_bombs = max_bombs;
	game_state = NONE;
}

void Mines::SetupBoard()
{
	InitializeFlags();

	for (int i=0; i<max_rows*max_cols; i++) {
		board[i].type = SAFE_BLOCK;
		board[i].state = CLOSED_BLOCK;
		board[i].value = 0;
	}

	int bombs = max_bombs;

	while (bombs > 0) {
		int rx = rand() % max_cols,
				ry = rand() % max_rows;

		if (board[ry*max_cols+rx].type != BOMB_BLOCK) {
			board[ry*max_cols+rx].type = BOMB_BLOCK;

			bombs--;
		}
	}

	for (int i=0; i<max_rows*max_cols; i++) {
		int ry = i/max_cols,
				rx = i%max_cols;

		if (board[i].type != BOMB_BLOCK) {
			int count = 0;

			if (rx > 0) {
				count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
			}

			if (rx < (max_cols-1)) {
				count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
			}

			if (ry > 0) {
				count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;

				if (rx > 0) {
					count += (board[(ry-1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				}

				if (rx < (max_cols-1)) {
					count += (board[(ry-1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
				}
			}

			if (ry < (max_rows-1)) {
				count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;

				if (rx > 0) {
					count += (board[(ry+1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				}

				if (rx < (max_cols-1)) {
					count += (board[(ry+1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
				}
			}

			if (count > 0) {
				board[i].value = count;
			}
		}
	}

	Repaint();
}

void Mines::Expose(int row, int col)
{
	if (col < 0 || col > (max_cols-1) || row < 0 || row > (max_rows-1)) {
		return;
	}

	block_t *block = &board[row*max_cols+col];

	if (block->type == SAFE_BLOCK && block->state == CLOSED_BLOCK) {
		block->state = OPENED_BLOCK;

		if (block->value != 0) {
			return;
		}

		Expose(row+0, col-1);
		Expose(row+0, col+1);
		Expose(row+1, col-1);
		Expose(row+1, col+0);
		Expose(row+1, col+1);
		Expose(row-1, col-1);
		Expose(row-1, col+0);
		Expose(row-1, col+1);
	}
}

Mines::game_status_t Mines::GetResult()
{
	return game_state;
}

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Mines app(100, 100);

  app.SetTitle("Mines");

	Application::Loop();

	return 0;
}

