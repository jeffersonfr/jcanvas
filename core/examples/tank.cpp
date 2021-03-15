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

class Main : public Window, public KeyListener {

	private:
    std::shared_ptr<Image> _image;
		std::shared_ptr<Image> _tiles;
		double _tx;
		double _ty;
		double _tw;
		double _th;
		double _tc;
		double _has_bullet;
		double _bullet_x;
		double _bullet_y;
		double _bullet_angle;
		double _tile_w;
		double _tile_h;
		double _step;
		double _angle;
		bool _running;

	public:
		Main(int n, int p):
			Window(jpoint_t<int>{720, 480})
		{
      SetFramesPerSecond(30);

			_running = true;
			_tx = 200;
			_ty = 200;
			_tw = 60;
			_th = 50;
			_tile_w = 48;
			_tile_h = 48;
			_step = 0;
			_angle = 0.0;
			_has_bullet = false;

			if (_th > _tw) {
				_tc = _th;
			}

			_tc = _tc/2;

			_image = std::make_shared<BufferedImage>("images/tank2.png")->Scale({(int)_tw, (int)_th});
			_tiles = std::make_shared<BufferedImage>("images/tiles.png")->Scale({(int)(10*_tile_w), (int)(4*_tile_h)});

			SetSize(jpoint_t<int>{(int)(10.0*_tile_w), (int)(8.0*_tile_h)});
		}

		virtual ~Main()
		{
      SetVisible(false);

			_running = false;
		}

		virtual void Paint(Graphics *g)
		{
			Window::Paint(g);

			KeyPressed();

      jpoint_t
        size = GetSize();

      _tx = _tx + _step*cos(_angle); // +M_PI_2);
      _ty = _ty - _step*sin(_angle); // +M_PI_2);

      if (_tx < _tw/2) {
        _tx = _tw/2;
      }

      if (_tx > (size.x - _tw/2)) {
        _tx = (size.x - _tw/2);
      }

      if (_ty < _th/2) {
        _ty = _th/2;
      }

      if (_ty > (size.y - _th/2)) {
        _ty = (size.y - _th/2);
      }

      _bullet_x = _bullet_x + 12*cos(_bullet_angle);
      _bullet_y = _bullet_y - 12*sin(_bullet_angle);

      if (_bullet_x < 0 || _bullet_x > size.x || _bullet_y < 0 || _bullet_y > size.y) {
        _has_bullet = false;
      }

      g->SetBlittingFlags(jblitting_t::Nearest);
      g->SetCompositeFlags(jcomposite_t::SrcOver);

			for (int j=0; j<8; j++) {
				for (int i=0; i<10; i++) {
					g->DrawImage(_tiles,
							{(int)(2*_tile_w), (int)(0*_tile_h), (int)(_tile_w), (int)(_tile_h)}, 
							{(int)(i*_tile_w), (int)(j*_tile_h), (int)_tile_w, (int)_tile_h});
				}
			}

			if (_has_bullet == true) {
				g->SetColor({0xf0, 0xf0, 0xf0, 0xff});
				g->FillCircle({(int)_bullet_x, (int)_bullet_y}, 3);
			}
			
      std::shared_ptr<Image> image = _image->Rotate(_angle);
			jpoint_t<int> isize = image->GetSize();

			g->DrawImage(image, jpoint_t<int>{(int)_tx - isize.x/2, (int)_ty - isize.y/2});

      Repaint();
		}

		virtual void KeyPressed()
		{
			EventManager &ev = GetEventManager();
			double angle_step = 0.1;

			if (ev.IsKeyDown(jkeyevent_symbol_t::Space)) {
				if (_has_bullet == false) {
					_bullet_x = _tx;
					_bullet_y = _ty;
					_bullet_angle = _angle;
					_has_bullet = true;
				}
			}
			
			_step = 0.0;

			if (ev.IsKeyDown(jkeyevent_symbol_t::CursorUp)) {
				_step = _step + 2;

				if (_step > 8) {
					_step = 8;
				}
			}
			
			if (ev.IsKeyDown(jkeyevent_symbol_t::CursorDown)) {
				_step = _step - 2;

				if (_step < -8) {
					_step = -8;
				}
			}
			
			if (ev.IsKeyDown(jkeyevent_symbol_t::CursorRight)) {
				// _step = 0.0;
				_angle = (_angle-angle_step);
				
				if (_angle < 0.0) {
					_angle = 2*M_PI;
				}
			}
			
			if (ev.IsKeyDown(jkeyevent_symbol_t::CursorLeft)) {
				// _step = 0.0;
				_angle = fmod(_angle+angle_step, 2*M_PI);
			}
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Main app(50, 100);

	app.SetTitle("Tank");
	app.Exec();

	Application::Loop();

	return 0;
}

