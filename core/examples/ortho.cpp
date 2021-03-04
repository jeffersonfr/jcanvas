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
#include "jcanvas/core/jraster.h"
#include "jcanvas/algebra/jmatrix.h"

using namespace jcanvas;

const float ANGLE_STEP = 0.05;

float f1(float x, float y)
{
	return x*x + y*y;
}

float f2(float x, float y)
{
	return x*x - y*y;
}

float f3(float x, float y)
{
	return sin(x)*cos(y);
}

float (*f)(float, float) = f1;

const int sizeT = 360;
float cosT[sizeT + 1];
float sinT[sizeT + 1];

class Grid : public Window, public KeyListener, public MouseListener {

	private:
    jmatrix_t<3, 3, float>
      _transformation;
    jmatrix_t<3, 1, float>
      _rotate {+1.2f, +0.0f, +2.4f},
      _camera {+0.0f, +0.0f, +1.0f};
    jpoint_t<int> 
      _pointer;
		float
			_scale = 100.0f;
    bool 
      _drag = false;


  private:
    void UpdateValues()
    {
      _rotate(0, 0) = fmod(_rotate(0, 0), 2*M_PI);

      if (_rotate(0, 0) < 0.0f) {
        _rotate(0, 0) = _rotate(0, 0) + 2*M_PI;
      }

      _rotate(1, 0) = fmod(_rotate(1, 0), 2*M_PI);

      if (_rotate(1, 0) < 0.0f) {
        _rotate(1, 0) = _rotate(1, 0) + 2*M_PI;
      }

      _rotate(2, 0) = fmod(_rotate(2, 0), 2*M_PI);

      if (_rotate(2, 0) < 0.0f) {
        _rotate(2, 0) = _rotate(2, 0) + 2*M_PI;
      }
    }

	public:
		Grid():
			Window({720, 480})
		{
      SetFramesPerSecond(60);

      float step = 2*M_PI/sizeT;

      for (int i=0; i<=sizeT; i++) {
        cosT[i] = std::cos(i*step);
        sinT[i] = std::sin(i*step);
      }
		}

		virtual ~Grid()
		{
		}

    float Cos(float angle)
    {
      constexpr float M = sizeT/(2*M_PI);

      return cosT[(int)(angle*M)];
    }

    float Sin(float angle)
    {
      constexpr float M = sizeT/(2*M_PI);

      return sinT[(int)(angle*M)];
    }

    void UpdateProjectionMatrix()
    {
      float
        cx = Cos(_rotate(0, 0)),
        sx = Sin(_rotate(0, 0)),
        cy = Cos(_rotate(1, 0)),
        sy = Sin(_rotate(1, 0)),
        cz = Cos(_rotate(2, 0)),
        sz = Sin(_rotate(2, 0));

      jmatrix_t<3, 3, float> 
        xrotation = {
          1.0f, 0.0f, 0.0f,
          0.0f, cx, sx,
          0.0f, -sx, cx
        },
        yrotation = {
          cy, 0.0f, -sy,
          0.0f, 1.0f, 0.0f,
          sy, 0.0f, cy
        },
        zrotation = {
          cz, sz, 0.0f,
          -sz, cz, 0.0f,
          0.0f, 0.0f, 1.0f
        };

      _transformation = xrotation*yrotation*zrotation;
    }

    jpoint_t<float> Project(jmatrix_t<3, 1, float> p) 
		{
      jpoint_t<int>
        size = GetSize();

      p = _transformation*(p - _camera);
  
      return jpoint_t<float>{p(0, 0)/_camera(2, 0), -p(1, 0)/_camera(2, 0)}*_scale + jpoint_t<int>{size.x/2, size.y/2};
		}

		bool MousePressed(MouseEvent *event)
		{
      _pointer = event->GetLocation();

      _drag = true;

      return true;
    }

		bool MouseReleased(MouseEvent *event)
		{
      _drag = false;

      return true;
    }

		bool MouseMoved(MouseEvent *event)
		{
      if (_drag == false) {
        return false;
      }

      jpoint_t<int>
        diff = _pointer - event->GetLocation();
      jmatrix_t<3, 1, float>
        ref {(float)diff.y, 0.0f, (float)diff.x};
     
      _pointer = event->GetLocation();

      _rotate = _rotate + ref/100.f;

      UpdateValues();

      return true;
    }

		bool KeyPressed(KeyEvent *event)
		{
			if (event->GetSymbol() == jkeyevent_symbol_t::Escape) {
        _camera = {0.0f, 0.0f, 1.0f};
				_rotate(0, 0) = 0.0f;
				_rotate(1, 0) = 0.0f;
				_rotate(2, 0) = 0.0f;
      } else if (event->GetSymbol() == jkeyevent_symbol_t::CursorLeft) {
				_rotate(1, 0) += ANGLE_STEP;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorRight) {
				_rotate(1, 0) -= ANGLE_STEP;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorUp) {
				_rotate(0, 0) += ANGLE_STEP;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorDown) {
				_rotate(0, 0) -= ANGLE_STEP;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::SquareBracketLeft) {
				_rotate(2, 0) -= ANGLE_STEP;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::SquareBracketRight) {
				_rotate(2, 0) += ANGLE_STEP;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::q) {
				_scale *= 0.9f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::w) {
				_scale *= 1.1f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::a) {
				_camera(2, 0) -= 1.0f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::s) {
				_camera(2, 0) += 1.0f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::j) {
				_camera(0, 0) -= 1.0f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::l) {
				_camera(0, 0) += 1.0f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::i) {
				_camera(1, 0) += 1.0f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::k) {
				_camera(1, 0) -= 1.0f;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::Number1) {
				f = f1;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::Number2) {
				f = f2;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::Number3) {
				f = f3;
			}

      UpdateValues();

			return true;
		}

		void Paint(Graphics *g) 
		{
      static auto start0 = std::chrono::steady_clock::now();
      static auto frame0 = 0;

			g->Clear();

      Raster 
				raster(g->GetCairoSurface());

			static float k = 0.0f;
			static float l = 0.01f;

			k += l;

			if (k < -1 or k > 1.0) {
				l = -l;
			}

      const float axis = 2.0f;

      UpdateProjectionMatrix();

      jpoint_t<float> 
        px0 = Project({-axis, 0.0f, 0.0f}),
        px1 = Project({+axis, 0.0f, 0.0f}),
        pxt0 = Project({+axis, 0.1f, 0.0f}),
        pxt1 = Project({+2.1f, 0.0f, 0.0f}),
        pxt2 = Project({+axis, -0.1f, 0.0f}),
        py0 = Project({0.0f, -axis, 0.0f}),
        py1 = Project({0.0f, +axis, 0.0f}),
        pyt0 = Project({-0.1f, +axis, 0.0f}),
        pyt1 = Project({0.0f, +2.1f, 0.0f}),
        pyt2 = Project({0.1f, +axis, 0.0f}),
        pz0 = Project({0.0f, 0.0f, -axis}),
        pz1 = Project({0.0f, 0.0f, +axis}),
        pzt0 = Project({0.0f, 0.1f, +axis}),
        pzt1 = Project({0.0f, 0.0f, +2.1f}),
        pzt2 = Project({0.0f, -0.1f, +axis});

      raster.SetColor(0xffff0000);
      raster.DrawLine(px0, px1);
      raster.DrawTriangle(pxt0, pxt1, pxt2);
      raster.SetColor(0xff00ff00);
      raster.DrawLine(py0, py1);
      raster.DrawTriangle(pyt0, pyt1, pyt2);
      raster.SetColor(0xff0000ff);
      raster.DrawLine(pz0, pz1);
      raster.DrawTriangle(pzt0, pzt1, pzt2);

			raster.SetColor(0xff808080);

			for (float y=axis; y>-axis; y-=.1) {
				for (float x=-axis; x<axis; x+=.1) {
          raster.SetPixel(Project({x, y, -axis}));
          raster.SetPixel(Project({x, -axis, y}));
          raster.SetPixel(Project({-axis, x, y}));

          // raster.SetPixel(Project({x, y, 0.0f}));
          // raster.SetPixel(Project({x, 0.0f, y}));
          // raster.SetPixel(Project({0.0f, x, y}));
				}
			}

			raster.SetColor(0xffffffff);

			for (float y=1; y>-1; y-=.1) {
				for (float x=-1; x<1; x+=.1) {
          raster.SetPixel(Project({x, y, f(x, y)*k}));
				}
			}

      Font *font = &Font::Size16;
      int fsize = font->GetSize();

      g->SetFont(font);
      g->SetColor(0xffffffff);

      g->DrawString("rotate: [" + std::to_string(_rotate(0, 0)) + ", " + std::to_string(_rotate(1, 0)) + ", " + std::to_string(_rotate(2, 0)) + "]", jpoint_t<int>{10, 0*fsize + 8});
      g->DrawString("camera: [" + std::to_string(_camera(0, 0)) + ", " + std::to_string(_camera(1, 0)) + ", " + std::to_string(_camera(2, 0)) + "]", jpoint_t<int>{10, 1*fsize + 8});

      auto count0 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start0).count();

      if (count0 != 0) {
        g->DrawString("fps: [" + std::to_string((++frame0*1000)/count0) + "]", jpoint_t<int>{10, 2*fsize + 8});
      }

      if (frame0 == 1000) { // INFO:: reset frames per second
        start0 = std::chrono::steady_clock::now();
        frame0 = 0;
      }

			Repaint();
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Grid app;

	app.SetTitle("Grid");
  app.Exec();

	Application::Loop();

	return 0;
}
