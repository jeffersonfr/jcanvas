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

#include <mutex>

using namespace jcanvas;

double	COUNTRY = 1.00;

class Neuron {

  private:

	public:
  	double x;
    double y;
	  double wx;
    double wy;
  	int update;
    int choose;
	
	public:
    Neuron(double x,double y)
    {
      this->x = x;
      this->y = y;

      this->wx = random()%100;
      this->wy = random()%100;

      update = 0;
      choose = 0;
    }

    double dist(Neuron *c)
    {
      double dx = x - c->x;
      double dy = y - c->y;

      return sqrt(dx*dx + dy*dy);

    }

    double wdist(Neuron *c)
    {
      double dx = wx - c->wx;
      double dy = wy - c->wy;

      return sqrt(dx*dx + dy*dy);
    }

};

class SOM : public Window {

  private:
    std::mutex _mutex;

	public:
		enum {
			bkC = 0xff000090,
			bk2C = 0xff6060a0,
			lnC = 0xffff0000,
			ln2C = 0xffcccc00,
			fgC = 0xffffffff
		};
	
	public:
		Image 
      *offscreen;
		int	
      W,
      H,
      NGEONEURON,
      imagewidth,
      imageheight,
      counter;
		double 
      px,
      py,
      **r,
      theta, 
      phi, 
      momentum;
		bool 
      please_stop;
		Neuron 
      **gn;

	public:
		SOM():
			Window(/*"SOM", */ {400, 400})
		{
			kohonenInit();
		}

		virtual ~SOM()
		{
			please_stop = true;
			
      _mutex.unlock();

			for (int i=0; i<NGEONEURON; i++) {
        delete gn[i];
      }

      delete [] gn;

			for (int i=0; i<NGEONEURON; i++) {
				delete [] r[i];
			}
			
      delete [] r;

      delete offscreen;
		}

		void kohonenInit()
		{
			please_stop = false;
			theta = 0.5;
			phi   = 0.5;
			momentum = 0.999;

			// tamanho da grade
			W = 10;
			H = 10;

			NGEONEURON = W * H;

			gn = new Neuron*[NGEONEURON];

			for(int x = 0; x<W; x++)
				for(int y = 0; y<H; y++){
					gn[x*W+y] = new Neuron((double)x/(double)(W-1), (double)y/(double)(H-1));
				}

			r = new double*[NGEONEURON];

			for (int i=0; i<NGEONEURON; i++) {
				r[i] = new double[NGEONEURON];
			}

			makeR(theta);        

			counter = 0;
		}

		void makeR(double th)
		{
      float c0 = -2.0f*th*th;

#pragma omp parallel for
			for(int i=0; i<NGEONEURON; i++){
				r[i][i]= 1.0;

				for(int j=i+1; j<NGEONEURON; j++){
					float c1 = gn[i]->dist(gn[j]);
          
					r[i][j] = expf(c1*c1/c0);
					r[j][i] = r[i][j];
				}
			}
		}

		int toXReal(double val)
		{
      jpoint_t
        size = GetSize();

			return (int)(val *((double)size.x - 50.0) / COUNTRY + 25.0);
		}

		int toYReal(double val)
		{
      jpoint_t
        size = GetSize();

			return (int)(val *((double)size.y - 50.0) / COUNTRY + 25.0);
		}

		void paintLeft(Graphics *g) 
		{
      jpoint_t
        size = GetSize();
			int 
        w = size.x, 
				h = size.y;

			// CLEAR ALL
			g->SetColor(bkC);
			g->FillRectangle({0, 0, w, h});

			// DRAW GRID
			g->SetColor(bk2C);
			for(double i=0; i<COUNTRY+COUNTRY/20.0; i+=(COUNTRY/20.0)){
				g->DrawLine({toXReal(0.0), toYReal(i)}, {toXReal(COUNTRY), toYReal(i)});
				g->DrawLine({toXReal(i), toYReal(0.0)}, {toXReal(i), toYReal(COUNTRY)});
			}

			//DRAW PATH
			g->SetColor(lnC);

			for(int x=0; x<(W-1); x++) {
				for(int y=0; y<(H-1); y++){
					g->DrawCircle({toXReal(gn[x*W+y]->wx), toYReal(gn[x*W+y]->wy)}, 4);
					g->DrawLine({toXReal(gn[x*W+y]->wx), toYReal(gn[x*W+y]->wy)}, {toXReal(gn[(x+1)*W+y]->wx), toYReal(gn[(x+1)*W+y]->wy)});
					g->DrawLine({toXReal(gn[x*W+y]->wx), toYReal(gn[x*W+y]->wy)}, {toXReal(gn[x*W+y+1]->wx), toYReal(gn[x*W+y+1]->wy)});
				}
			}

			for(int x=0; x<(W-1); x++){
				g->DrawCircle({toXReal(gn[x*W+H-1]->wx), toYReal(gn[x*W+H-1]->wy)}, 4);
				g->DrawLine({toXReal(gn[x*W+H-1]->wx), toYReal(gn[x*W+H-1]->wy)} ,{toXReal(gn[(x+1)*W+H-1]->wx), toYReal(gn[(x+1)*W+H-1]->wy)});
			}

			for(int y=0; y<(H-1); y++){
				g->DrawCircle({toXReal(gn[(W-1)*W+y]->wx), toYReal(gn[(W-1)*W+y]->wy)}, 4);
				g->DrawLine({toXReal(gn[(W-1)*W+y]->wx), toYReal(gn[(W-1)*W+y]->wy)}, {toXReal(gn[(W-1)*W+y+1]->wx), toYReal(gn[(W-1)*W+y+1]->wy)});
			}

			g->DrawCircle({toXReal(gn[(W-1)*W+H-1]->wx), toYReal(gn[(W-1)*W+H-1]->wy)}, 4);

			g->SetColor(fgC);
			g->DrawLine({toXReal(0.0), toYReal(0.0)}, {toXReal(COUNTRY), toYReal(0.0)});
			g->DrawLine({toXReal(0.0), toYReal(0.0)}, {toXReal(0.0), toYReal(COUNTRY)});

			double 
			  dx = toXReal(0.0),
			  dy = toYReal(0.0);

			g->DrawArc({(int)dx, (int)dy}, {(int)(size.x-2*dx), (int)(size.y-2*dy)}, 3*M_PI_2,  2*M_PI);
			
			g->DrawCircle({toXReal(px), toYReal(py)}, 6);
		}    

		virtual void Paint(Graphics *g) 
		{
      jpoint_t
        size = GetSize();
			double 
        x1,
				x2,
				mindist;
			int 
        w = size.x, 
				h = size.y;
			int 
        j;

      // CHOSE A RANDOM PATTERN
      x1 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));
      x2 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));

      while ((x1*x1+x2*x2) > 1.0){
        x1 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));
        x2 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));
      }

      px = x1;
      py = x2;

      // SEARCH FOR MINIMAL
      j = -1;
      mindist = 100000.0;

      for(int i=0; i<NGEONEURON;i++){
        double d = (x1 - gn[i]->wx)*(x1 - gn[i]->wx) + (x2 - gn[i]->wy)*(x2 - gn[i]->wy);
        if(d < mindist){
          mindist = d;
          j = i;
        }
      }

      gn[j]->update++;

      // UPDATE WEIGHTS
      for(int i=0; i<NGEONEURON;i++){
        gn[i]->wx += (phi * r[i][j] * (x1 - gn[i]->wx));
        gn[i]->wy += (phi * r[i][j] * (x2 - gn[i]->wy));
      }

      // DECREASE LEARNING PARAMETERS
      phi *= momentum;
      theta *= momentum;

      // RE-COMPUTE r MATRIX
      makeR(theta);

			g->SetColor(bkC);
			g->FillRectangle({0, 0, size.x, size.y});

			if ((offscreen == nullptr) || ((imagewidth != w) || (imageheight != h))) {
				offscreen = new BufferedImage(jpixelformat_t::RGB32, {w, h});

				imagewidth = w;
				imageheight = h;
			}

			Graphics *goff = offscreen->GetGraphics();

			paintLeft(goff);

			g->DrawImage(offscreen, jpoint_t<int>{0, 0});

      Repaint();
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	srand(time(nullptr));

	SOM app;

	app.SetTitle("Som");
	app.kohonenInit();
	app.Exec();

	Application::Loop();

	return 0;
}

