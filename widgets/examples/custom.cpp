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
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/jbutton.h"

using namespace jcanvas;

class CustomContainer : public Container {

	private:
    std::shared_ptr<Image> _image;

	public:
		CustomContainer(int x, int y, int w, int h):
			Container({x, y, w, h})
		{
			_image = std::make_shared<BufferedImage>("images/bubble.png");
		}

		virtual ~CustomContainer()
		{
		}

		virtual void PaintScrollbars(Graphics *g) 
		{
			if (IsScrollable() == false) {
				return;
			}

      jtheme_t
        theme = GetTheme();
      jpoint_t 
        scroll_dimension = GetScrollDimension();
      jpoint_t
        size = GetSize();
      jpoint_t 
        scroll_location = GetScrollLocation();
			int 
        scrollx = (IsScrollableX() == true)?scroll_location.x:0,
				scrolly = (IsScrollableY() == true)?scroll_location.y:0;

			g->SetColor(theme.scroll.color.normal);

			if (IsScrollableX() == true) {
				double 
          offset_ratio = (double)scrollx/(double)scroll_dimension.x,
				  block_size_ratio = (double)size.x/(double)scroll_dimension.x;
				int 
          offset = (int)(size.x*offset_ratio),
					block_size = (int)(size.x*block_size_ratio);

				g->DrawRectangle({theme.border.size.x, size.y - theme.scroll.size.y - theme.border.size.y, size.x - 2*theme.border.size.x, theme.scroll.size.y});
				g->DrawImage(_image, {offset, size.y - theme.scroll.size.y - theme.border.size.y, block_size, theme.scroll.size.y});
			}

			if (IsScrollableY() == true) {
				double 
          offset_ratio = (double)scrolly/(double)scroll_dimension.y,
				  block_size_ratio = (double)size.y/(double)scroll_dimension.y;
				int 
          offset = (int)(size.y*offset_ratio),
					block_size = (int)(size.y*block_size_ratio);

				g->DrawRectangle({size.x - theme.scroll.size.x - theme.border.size.x, theme.border.size.y, theme.scroll.size.x, size.y});
				g->DrawImage(_image, {size.x - theme.scroll.size.x - theme.border.size.x, offset, theme.scroll.size.x, block_size});
			}
		}

};

class App : public Frame {

  private:
      CustomContainer container1 {100, 100, 400, 400};
			CustomContainer container2 {100, 100, 400, 400};
      Button button1 {"Testing Clipping"};

	public:
		App(std::string title, int w, int h):
			Frame(/*title, */ {w, h})
		{
    }

		virtual ~App()
		{
		}

    void Init()
    {
			button1.SetBounds({200, 100, 300, 100});

			container2.Add(&button1);
			container1.Add(&container2);

			Add(&container1);
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<App>("Custom Frame", 720, 480);

  app->Init();
	app->SetTitle("Custom");

	Application::Loop();

	return 0;
}

