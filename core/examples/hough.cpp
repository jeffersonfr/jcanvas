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

#include <iostream>

using namespace jcanvas;

#define DEG2RAD 0.017453293f

class Hough {

	private:
		uint32_t 
      *_accu;
		int 
      _accu_w,
		  _accu_h,
		  _img_w,
		  _img_h;

	public:
		Hough():
			_accu(0), _accu_w(0), _accu_h(0), _img_w(0), _img_h(0)
		{
	
		}
		
		virtual ~Hough()
		{
			if (_accu) {
				free(_accu);
			}
		}

		int Transform(unsigned char* img_data, int w, int h)
		{
			_img_w = w;
			_img_h = h;

			//Create the accu
			double hough_h = ((sqrt(2.0) * (double)(h>w?h:w)) / 2.0);

			_accu_h = hough_h * 2.0; // -r -> +r
			_accu_w = 180;

			_accu = (unsigned int*)calloc(_accu_h * _accu_w, sizeof(unsigned int));

			double center_x = w/2;
			double center_y = h/2;

			for (int y=0;y<h;y++) {
				for (int x=0;x<w;x++) {
					if ( img_data[ (y*w) + x] > 250 ) {
						for (int t=0;t<180;t++) {
							double r = ( ((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));

							_accu[ (int)((round(r + hough_h) * 180.0)) + t]++;
						}
					}
				}
			}

			return 0;
		}

		std::vector<jline_t<int>> GetLines(int threshold)
		{
			std::vector<jline_t<int>> lines;

			if (_accu == 0) {
				return lines;
			}

			for (int r=0;r<_accu_h;r++) {
				for (int t=0;t<_accu_w;t++) {
					if ((int)_accu[(r*_accu_w) + t] >= threshold) {
						// Is this point a local maxima (9x9)
						int max = _accu[(r*_accu_w) + t];
						
						for (int ly=-4;ly<=4;ly++) {
							for (int lx=-4;lx<=4;lx++) {
								if ( (ly+r>=0 && ly+r<_accu_h) && (lx+t>=0 && lx+t<_accu_w) ) {
									if ( (int)_accu[( (r+ly)*_accu_w) + (t+lx)] > max ) {
										max = _accu[( (r+ly)*_accu_w) + (t+lx)];
										ly = lx = 5;
									}
								}
							}
						}

						if (max > (int)_accu[(r*_accu_w) + t]) {
							continue;
						}

						int x1, y1, x2, y2;

						x1 = y1 = x2 = y2 = 0;

						if (t >= 45 && t <= 135) {
							//y = (r - x cos(t)) / sin(t)
							x1 = 0;
							y1 = ((double)(r-(_accu_h/2)) - ((x1 - (_img_w/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (_img_h / 2);
							x2 = _img_w - 0;
							y2 = ((double)(r-(_accu_h/2)) - ((x2 - (_img_w/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (_img_h / 2);
						} else {
							//x = (r - y sin(t)) / cos(t);
							y1 = 0;
							x1 = ((double)(r-(_accu_h/2)) - ((y1 - (_img_h/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (_img_w / 2);
							y2 = _img_h - 0;
							x2 = ((double)(r-(_accu_h/2)) - ((y2 - (_img_h/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (_img_w / 2);
						}

						jline_t<int> t;

						t.p0.x = x1;
						t.p0.y = y1;
						t.p1.x = x2;
						t.p1.y = y2;

						lines.push_back(t);
					}
				}
			}

			std::cout << "lines: " << lines.size() << " " << threshold << std::endl;

			return lines;
		}

		const unsigned int* GetAccuracy(int *w, int *h)
		{
			(*w) = _accu_w;
			(*h) = _accu_h;

			return _accu;
		}

};

class Test : public Window, public KeyListener {
	
	private:
		Hough _transform;
    std::shared_ptr<Image> _image;
		std::shared_ptr<Image> _hough;
		int _binary_threshold;
		int _lines_threshold;

	public:
		Test():
			Window(/*"Hough Test", */ {960, 540})
		{
			_binary_threshold = 195;
			_lines_threshold = 195;

			_image = std::make_shared<BufferedImage>("images/robin.png");

      jpoint_t
        size = _image->GetSize();

      _hough = std::make_shared<BufferedImage>(jpixelformat_t::RGB32, size);

		}

		virtual ~Test()
		{
		}

		virtual void ProcessFrame()
		{
			jpoint_t<int> 
        size;

			size = _image->GetSize();

			uint32_t 
        *data = new uint32_t[size.x*size.y];
      
			_image->GetGraphics()->GetRGBArray(data, {0, 0, size.x, size.y});

			// INFO:: convert to gray and binarize
			uint8_t *gray = new uint8_t[size.x*size.y];
	
			for (int j=0; j<size.y-1; j++) {
				for (int i=0; i<size.x-1; i++) {
					int 
            index = j*size.x+i;
					uint8_t 
            r = (data[index] >> 0x10) & 0xff,
					  g = (data[index] >> 0x08) & 0xff,
					  b = (data[index] >> 0x00) & 0xff;
					double 
            p = (r * 0.299) + (g * 0.587) + (b * 0.114);

					if (p < _binary_threshold) {
						gray[index] = 0x00;
					} else {
						gray[index] = 0xff;
					}
				}
			}

      delete [] data;

			// INFO:: converto to edges
			uint8_t 
        *edges = new uint8_t[size.x*size.y];
			int 
        k, 
        offset, 
        sigma, 
        gamma;

			for (int i=1; i<size.x-1; i++) {
				for (int j=1; j<size.y-1; j++) {
					offset = j*size.x+i;

					if (gray[offset] == 0) {
						sigma =
							gray[((j-1)*size.x+(i-1))] +
							gray[((j-1)*size.x+(i+0))] +
							gray[((j-1)*size.x+(i+1))] +
							gray[((j+0)*size.x+(i-1))] +
							gray[((j+0)*size.x+(i+1))] +
							gray[((j+1)*size.x+(i-1))] +
							gray[((j+1)*size.x+(i+0))] +
							gray[((j+1)*size.x+(i+1))];

						gamma =
							gray[((j+0)*size.x+(i+0))] +
							gray[((j-1)*size.x+(i+1))] +
							gray[((j+1)*size.x+(i-1))] +
							gray[((j+1)*size.x+(i+1))];

						if ((gamma>0 && gamma==sigma) || sigma==0) {
							k = 0;
						} else {
							k = 255;
						}

						edges[offset] = k;
					} else {
						edges[offset] = 0;
					}
				}
			}

			// INFO:: convert to rgba
			int 
        count = size.x*size.y;
			uint32_t 
        *gray32 = new uint32_t[count];

			for (int i=0; i<count; i++) {
				gray32[i] = 0xff000000 | (edges[i] << 0x10) | (edges[i] << 0x08) | (edges[i] << 0x00);
			}

      _hough->GetGraphics()->SetCompositeFlags(jcomposite_flags_t::Src);
      _hough->GetGraphics()->SetRGBArray(gray32, {0, 0, size.x, size.y});

      delete [] gray32;
			delete [] gray;

			// INFO:: initializing hough transform
			_transform.Transform(edges, size.x, size.y);

      delete [] edges;
		}

		virtual void Paint(Graphics *g) 
		{
			Window::Paint(g);

			ProcessFrame();

			std::vector<jline_t<int>> 
        lines = _transform.GetLines(_lines_threshold);
			jpoint_t<int> 
				size = _hough->GetSize();

			g->DrawImage(_image, {0, 0, size.x, size.y});
			g->DrawImage(_hough, {size.x, 0, size.x, size.y});

			g->SetColor(jcolor_name_t::Red);
			g->SetClip({0, 0, size.x, size.y});

			for (std::vector<jline_t<int>>::iterator i=lines.begin(); i!=lines.end(); i++) {
				jline_t<int> t = (*i);

				g->DrawLine({t.p0.x, t.p0.y}, {t.p1.x, t.p1.y});
			}
		}

		virtual bool KeyPressed(KeyEvent *event)
		{
			if (event->GetSymbol() == jkeyevent_symbol_t::CursorLeft) {
				_lines_threshold -= 10;
      } else if (event->GetSymbol() == jkeyevent_symbol_t::CursorUp) {
				_binary_threshold += 10;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorRight) {
				_lines_threshold += 10;
			} else if (event->GetSymbol() == jkeyevent_symbol_t::CursorDown) {
				_binary_threshold -= 10;
			}

			Repaint();

			return true;
		}
};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Test app;

	app.SetTitle("Hough");

	Application::Loop();

	return 0;
}

