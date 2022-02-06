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

class Main : public Window {

	private:
		std::map<std::string, std::shared_ptr<Image>> _types;

  protected:
    virtual void LoadImage(std::string format)
    {
      try {
        std::string path = "images/image." + format;

			  _types[format] = std::make_shared<BufferedImage>(path, jcanvas::jpoint_t<int>{256, 256});
      } catch (...) {
			  _types[format] = nullptr;
      }
    }

	public:
		Main():
			Window({1280, 720})
		{
      LoadImage("bmp");
      // LoadImage("gif");
      LoadImage("ico");
      LoadImage("jpg");
      LoadImage("pcx");
      LoadImage("png");
      LoadImage("ppm");
      LoadImage("tga");
      LoadImage("xbm");
      LoadImage("xpm");
      LoadImage("svg");
      LoadImage("jp2");
      LoadImage("heic");
      // LoadImage("bpg");
      LoadImage("webp");
      LoadImage("mjpeg");
      LoadImage("tiff");
		}

		virtual ~Main()
		{
			_types.clear();
		}

		virtual void Paint(Graphics *g)
		{
			Window::Paint(g);

      jpoint_t
        size = GetSize();
      // int left = 0;
      int top = 0;
      // int right = 0;
      int bottom = 0;
			int 
        items = 5,
			  gap = 32,
			  bs = (size.x-6*gap-top-bottom)/(items),
			  count = 0;

      g->SetFont(Font::Size16);

			for (std::map<std::string, std::shared_ptr<Image>>::iterator i=_types.begin(); i!=_types.end(); i++) {
        std::shared_ptr<Image>
          image = i->second;
				int 
          x = count%items,
				  y = count/items;

				g->DrawImage(image, {x*(bs+gap)+gap, y*(bs+gap)+16, bs, bs});
				g->SetColor(jcolor_name_t::Black);
				g->FillRectangle({x*(bs+gap)+gap, y*(bs+gap)+16, 48, 24});
				g->SetColor(jcolor_name_t::White);
				g->DrawString(i->first, jpoint_t<int>{x*(bs+gap)+gap, y*(bs+gap)+16});

				count = count + 1;
			}
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Main app;

	app.SetTitle("Draw Types");

	Application::Loop();

	return 0;
}
