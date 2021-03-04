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

using namespace jcanvas;

class Barrier {

  private:
    jline_t<int> _line;

  public:
    Barrier(jline_t<int> line)
    {
      _line = line;
    }

    virtual ~Barrier()
    {
    }

    jline_t<int> GetBounds()
    {
      return _line;
    }

    void Paint(Raster &raster)
    {
      raster.DrawLine(_line.p0, _line.p1);
    }

};

class Ray {

  private:
    jpoint_t<int> _p0;
    jpoint_t<float> _p1;

  public:
    Ray(jpoint_t<int> p0, float angle)
    {
      angle = 2*M_PI*angle/360.0f;

      _p0 = p0;
      _p1 = jpoint_t<float>{cos(angle), sin(angle)};
    }

    virtual ~Ray()
    {
    }

    jpoint_t<int> GetPosition()
    {
      return _p0;
    }

    jpoint_t<float> GetDirection()
    {
      return _p1;
    }

    void SetPoint(jpoint_t<int> point)
    {
      _p0 = point;
    }

    std::optional<jpoint_t<int>> Cast(Barrier &barrier)
    {
      std::optional<std::pair<float, float>>
        pair = barrier.GetBounds().Intersection(jline_t<float>{_p0, jpoint_t<float>(_p0) + _p1});

      if (pair != std::nullopt and pair->first >= 0.0f and pair->first <= 1.0f and pair->second >= 0.0f) {
        return barrier.GetBounds().Point(pair->first);
      }

      return std::nullopt;
    }

    void Paint(Raster &raster)
    {
      raster.DrawLine(_p0, {(int)(_p0.x + 10*_p1.x), (int)(_p0.y + 10*_p1.y)});
    }

};

class Light {

  private:
    std::vector<Ray> _rays;

  public:
    Light()
    {
      for (float i=0.0f; i<360.0f; i+=1.0f) {
        _rays.emplace_back(jpoint_t<int>{0, 0}, i);
      }
    }

    virtual ~Light()
    {
    }

    void SetPoint(jpoint_t<int> point)
    {
      for (auto &ray : _rays) {
        ray.SetPoint(point);
      }
    }

    void Paint(std::vector<Barrier> &barriers, Raster &raster)
    {
      for (auto &ray : _rays) {
        ray.Paint(raster);

        jpoint_t<int> 
          best = {-1, -1};
        int
          d0 = INT_MAX;

        for (auto &barrier : barriers) {
          std::optional<jpoint_t<int>> 
            point = ray.Cast(barrier);

          if (point != std::nullopt) {
            int 
              d1 = (ray.GetPosition() - *point).Norm();

            if (d1 < d0) {
              d0 = d1;
              best = *point;
            }
          }
        }

        raster.DrawLine(ray.GetPosition(), best);
      }
    }

};

class Scene : public Window, public MouseListener {

  private:
    std::vector<Barrier> _barriers;
    Light _light;

  public:
    Scene():
      Window({720, 480})
    {
      _barriers.emplace_back(jline_t<int>{{0, 0}, {720, 0}});
      _barriers.emplace_back(jline_t<int>{{720, 0}, {720, 480}});
      _barriers.emplace_back(jline_t<int>{{720, 480}, {0, 480}});
      _barriers.emplace_back(jline_t<int>{{0, 480}, {0, 0}});

      for (int i=0; i<3; i++) {
        _barriers.emplace_back(
            jline_t<long int>{
              {random()%720, random()%480}, 
              {random()%720, random()%480}
            });
      }
      
      _light.SetPoint(jpoint_t<int>{200, 250});
    }

    virtual ~Scene()
    {
    }

    bool MouseMoved(MouseEvent *event)
    {
      _light.SetPoint(event->GetLocation());

      Repaint();

      return true;
    }

    void Paint(Graphics *g) 
    {
      Window::Paint(g);

      Raster 
        raster(g->GetCairoSurface());

      for (auto &i : _barriers) {
        i.Paint(raster);
      }
      
      _light.Paint(_barriers, raster);
    }

};

int main(int argc, char **argv)
{
  Application::Init(argc, argv);

  srandom(time(NULL));

  Scene app;

  app.SetTitle("Scene");
  app.Exec();

  Application::Loop();

  return 0;
}
