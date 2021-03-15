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
#include "jcanvas/core/jwindowlistener.h"
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/jflatimage.h"
#include "jcanvas/widgets/jtext.h"

#include <iostream>
#include <mutex>

using namespace jcanvas;

class App : public Frame {

	private:
    std::shared_ptr<Image>
      _image;

	public:
		App():
			Frame({1280, 720})
	  {
    }

    void Init()
    {
      SetLayout<GridLayout>(4, 3);

      _image = std::make_shared<BufferedImage>("images/image.png");
      // _image = new BufferedImage("images/red_icon.png");

      for (int i=0; i<11; i++) {
        jrect_align_t align;
        std::string id;

        if (i == 0) {
          align = jrect_align_t::Center;
          id = "center";
        } else if (i == 1) {
          align = jrect_align_t::North;
          id = "north";
        } else if (i == 2) {
          align = jrect_align_t::South;
          id = "south";
        } else if (i == 3) {
          align = jrect_align_t::East;
          id = "east";
        } else if (i == 4) {
          align = jrect_align_t::West;
          id = "west";
        } else if (i == 5) {
          align = jrect_align_t::NorthEast;
          id = "northeast";
        } else if (i == 6) {
          align = jrect_align_t::NorthWest;
          id = "northwest";
        } else if (i == 7) {
          align = jrect_align_t::SouthEast;
          id = "southeast";
        } else if (i == 8) {
          align = jrect_align_t::SouthWest;
          id = "southwest";
        } else if (i == 9) {
          align = jrect_align_t::Contains;
          id = "contains";
        } else if (i == 10) {
          align = jrect_align_t::Cover;
          id = "cover";
        }

        auto title = std::make_shared<Text>(id);
        auto flatimage = std::make_shared<FlatImage>(_image);
        
        flatimage->SetAlign(static_cast<jrect_align_t>(i));

        auto container = std::make_shared<Container>();

        container->SetLayout<BorderLayout>();

        container->Add(title, jborderlayout_align_t::North);
        container->Add(flatimage, jborderlayout_align_t::Center);

        Add(container);
      }
    }

    virtual ~App()
    {
    }

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

  auto app = std::make_shared<App>();

  app->Init();
  app->Exec();

  Application::Loop();

	return 0;
}
