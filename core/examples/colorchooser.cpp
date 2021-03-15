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

class ColorChooser {

	private:
    std::shared_ptr<Image> _image;
    uint32_t _color = 0xff000000;

	public:
		ColorChooser()
		{
			jpoint_t<int>
				size {360, 360};
			int 
				border = std::min(size.x, size.y),
				cx = border / 2 + 1,
				cy = border / 2 + 1;
			double border2 = border / 2;

			_image = std::make_shared<BufferedImage>(jpixelformat_t::ARGB, size);

			for (double i = 0; i<360.; i+=.15) {
				for (double j = 0; j<border2; j++) {
					_image->GetGraphics()->SetRGB(HLS2RGB(i, 0.5, j/border2), {(int)(cx - cos(M_PI * i / 180.0)*j), (int)(cy - sin(M_PI * i / 180.0)*j)});
				}
			}
		}

		virtual ~ColorChooser()
		{
		}

		uint8_t FixRGB(double rm1, double rm2, double rh)
		{
			if (rh > 360.0f) {
				rh -= 360.0f;
			} else if (rh < 0.0f) {
				rh += 360.0f;
			}

			if (rh <  60.0f) {
				rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
			} else if (rh < 180.0f) {
				rm1 = rm2;
			} else if (rh < 240.0f) {
				rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;
			}

			return static_cast<uint8_t>(rm1 * 255);
		}

		uint32_t HLS2RGB(double hue, double luminance, double saturation)
		{
			int m_red,
					m_green,
					m_blue;

			if (hue < 0.0 && hue > 360.0) {
				hue = ((int)hue) % 360;
			}

			if (luminance < 0.0 && luminance > 1.0) {
				luminance = 0.5;
			}

			if (saturation < 0.0 && saturation > 1.0) {
				saturation = 0.5;
			}

			// converting hls to rgb
			if (saturation == 0.0) {
				m_red = m_green = m_blue = (uint8_t)(luminance * 255.0);
			} else {
				double rm1, rm2;

				if (luminance <= 0.5) {
					rm2 = luminance + luminance * saturation;
				} else {
					rm2 = luminance + saturation - luminance * saturation;
				}

				rm1 = 2.0 * luminance - rm2;
				m_red   = FixRGB(rm1, rm2, hue + 120.0);
				m_green = FixRGB(rm1, rm2, hue);
				m_blue  = FixRGB(rm1, rm2, hue - 120.0);
			}

			return (uint32_t)(0xff000000 | (m_red << 0x10) | (m_green << 0x08) | (m_blue << 0x00));
		}

		virtual bool MouseMoved(MouseEvent *event)
		{
      jpoint_t
        elocation = event->GetLocation();

      Graphics *g = _image->GetGraphics();

			_color = g->GetRGB({elocation.x, elocation.y});

			return true;
		}

		virtual void Paint(Graphics *g)
    {
      jpoint_t<int> size {360, 360};

      g->SetColor(_color);
      g->FillRectangle(jrect_t<int>{0, 0, size.x, size.y});
      g->SetCompositeFlags(jcomposite_t::SrcOver);
			g->DrawImage(_image, jpoint_t<int>{0, 0});
		}
		
};

class FrameTest : public Window, public MouseListener {

	private:
		ColorChooser *_color_chooser;

	public:
		FrameTest():
			Window({360, 360})
		{
			jpoint_t
				size = GetSize();
			int 
				m = std::min(size.x, size.y);

			_color_chooser = new ColorChooser();
		}

		virtual ~FrameTest()
		{
			delete _color_chooser;
		}

		virtual bool MouseMoved(MouseEvent *event)
		{
      _color_chooser->MouseMoved(event);

      Repaint();

      return true;
		}

		virtual void Paint(Graphics *g)
    {
      _color_chooser->Paint(g);
    }

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	FrameTest app;

	app.SetTitle("Color Chooser");

	Application::Loop();

	return 0;
}

