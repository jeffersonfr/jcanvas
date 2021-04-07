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

#include <chrono>

using namespace jcanvas;

class Particle {

  public:
    std::chrono::duration<float> lifetime {10.0f};
    jpoint_t<float> pos;
    jpoint_t<float> vel;
    jpoint_t<float> acc;

    Particle()
    {
      int v = 16;

      pos = jpoint_t<long>{random()%720, 0};
      vel = jpoint_t<long>{random()%v - v/2, 0};
      acc = jpoint_t<float>{random()%2, random()%16 + 16};
    }

    virtual ~Particle()
    {
    }

    void Update(std::chrono::duration<float> tick)
    {
      float t = tick.count()*10;

      pos = pos + vel*t + acc*t*t/2.0f;
      lifetime = lifetime - tick;
    }

    void Render(Graphics *g)
    {
      g->SetColor(jcolor_t {0xf0, 0xf0, 0xf0, int(0x20*lifetime.count())});
      g->FillCircle(pos, 2);
    }

};

class App : public Window {

  std::vector<Particle> mParticles {
    {}
  };
  std::shared_ptr<Image> _image = std::make_shared<BufferedImage>("images/tux-santa.png");

  public:
    App():
      Window({720, 480})
    {
      srand(time(nullptr));
    }

    virtual ~App()
    {
    }

    virtual void Paint(Graphics *g) 
    {
      static std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

      Window::Paint(g);

      std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

      mParticles.emplace_back();
      for (auto &p : mParticles) {
        p.Update(current - now);
      }

      now = current;

      g->DrawImage(_image, GetSize() - _image->GetSize());
      g->SetCompositeFlags(jcomposite_flags_t::SrcOver);

      for (auto &p : mParticles) {
        p.Render(g);
      }

      mParticles.erase(std::remove_if(mParticles.begin(), mParticles.end(), 
            [](auto &particle) {
              return particle.lifetime < std::chrono::duration<float>{0};
            }), mParticles.end());

      Repaint();
    }

};

int main(int argc, char *argv[])
{
  Application::Init(argc, argv);

  App app;

  Application::Loop();

  return 0;
}

