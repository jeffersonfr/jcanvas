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

#include <mutex>

#include <math.h>

using namespace jcanvas;

const uint32_t POINTS = 4000;

int GetClass(double x, double y)
{
	double xAux;

	if (x >= 0.0 && y >= 0.0) {
		xAux = -x + 1;
		if (xAux > y) {
			return 1;
		} else {
			return 5;
		}
	} else if (x < 0.0 && y >= 0.0) {
		xAux = x + 1;
		if (xAux > y) {
			return 2;
		} else {
			return 6;
		}
	} else if (x >= 0.0 && y < 0.0) {
		xAux  = x - 1;
		if (xAux < y) {
			return 4;
		} else {
			return 8;
		}
	} else if (x < 0.0 && y < 0.0) {
		xAux = -x - 1;
		if (xAux < y) {
			return 3;
		} else {
			return 7;
		}
	}

	return -1;
}

uint32_t GetColor(int classe)
{
	uint32_t color = 0xff000000;

	if (classe == 1) {
		color = 0xfff00000;
	} else if (classe == 2) {
		color = 0xff00f000;
	} else if (classe == 3) {
		color = 0xff0000f0;
	} else if (classe == 4) {
		color = 0xfff0f000;
	} else if (classe == 5) {
		color = 0xfff000f0;
	} else if (classe == 6) {
		color = 0xff00f0f0;
	} else if (classe == 7) {
		color = 0xff404040;
	} else if (classe == 8) {
		color = 0xffa0a0a0;
	}

	return color;
}

double ** CreatePoints(int npoints)
{
	double **points = new double*[npoints*8];

	for (int j=0; j<8; j++) {
		int i = 0,
				k = j*npoints;

		while (i < npoints) {
			double raio = 1.0,
						 yMax,
						 rx = (1*((double)rand()/((double)(RAND_MAX)))),
						 ry = (1*((double)rand()/((double)(RAND_MAX))));
			double x,
						 y;

			x = (2 * rx) - 1; 
			yMax = sqrt(raio*raio - x*x);
			y = (2 * yMax * ry) - yMax;

			if (GetClass(x, y) == 1 && j != 0) {
				continue;
			} else if (GetClass(x, y) == 5 && j != 4) {
				continue;
			} else if (GetClass(x, y) == 2 && j != 1) {
				continue;
			} else if (GetClass(x, y) == 6 && j != 5) {
				continue;
			} else if (GetClass(x, y) == 4 && j != 3) {
				continue;
			} else if (GetClass(x, y) == 8 && j != 7) {
				continue;
			} else if (GetClass(x, y) == 3 && j != 2) {
				continue;
			} else if (GetClass(x, y) == 7 && j != 6) {
				continue;
			}

			points[i+k] = new double[4];

			points[i+k][0] = x;
			points[i+k][1] = y;
			points[i+k][2] = j+1.0;
			points[i+k][3] = j+1.0;

			i++;
		}
	}

	return points;
}

class Neuron {

	private:
		double *_weights;
		int _weights_size;

	public:
		Neuron(int ninputs)
		{
			_weights = new double[ninputs];

			_weights_size = ninputs;

			for (int i=0; i<ninputs; i++) {
				double d = (1*((double)rand()/((double)(RAND_MAX))));

				if (d <= 0.1) {
					d = 0.1;
				}

				_weights[i] = d;
			}

		}

		virtual ~Neuron()
		{
      delete [] _weights;
		}

		void UpdateWeight(int index, double delta)
		{
			_weights[index] += delta;
		}

		void SetWeight(int index, double w)
		{
			_weights[index] = w;
		}

		double GetWeight(int index)
		{
			return _weights[index];
		}

		int GetNumberOfWeights()
		{
			return _weights_size;
		}

		double Distance(Neuron *neuron)
		{
			double d = 0;

			for (int i=0; i<_weights_size; i++) {
				double diff = neuron->GetWeight(i)-_weights[i];

				d = d + diff*diff;
			}

			return sqrt(d);
		}

};

class SOM : public Window {

	private:
    std::mutex
      _mutex;
		Neuron 
      **_neurons;
		double 
      **_train_input,
			**_classify_input;
		double 
      _learning_rate;
		int 
      _train_input_size,
			_classify_input_size,
      _neurons_size;

	public:
		SOM(int ninputs, int neurons):
			Window(jpoint_t<int>{3*240, 3*128})
		{
      _train_input = nullptr;
      _train_input_size = 0;

			_classify_input = nullptr;
			_classify_input_size = 0;

			_learning_rate = 0.1;

			_neurons = new Neuron*[neurons];

			for (int i=0; i<neurons; i++) {
				_neurons[i] = new Neuron(ninputs);
			}

			_neurons_size = neurons;
		}

		virtual ~SOM()
		{
      _mutex.unlock();
		}

		void SetLearningRate(double n)
		{
			_learning_rate = n;
		}

		void Init(double **train_input, int train_input_size)
		{
			_train_input = train_input;
			_train_input_size = train_input_size;

			for (int i=0; i<_neurons_size; i++) {
				/*
				int r = random()%train_input_size;

				_neurons[i]->SetWeight(0, train_input[r][0]);
				_neurons[i]->SetWeight(1, train_input[r][1]);
				_neurons[i]->SetWeight(2, train_input[r][2]);
				_neurons[i]->SetWeight(3, 0);//train_input[r][3]);
				*/
	
				double domain = 0.1,
							 r1 = (domain*((double)rand()/((double)(RAND_MAX)))),
							 r2 = (domain*((double)rand()/((double)(RAND_MAX))));

				_neurons[i]->SetWeight(0, r1);
				_neurons[i]->SetWeight(1, r2);
				_neurons[i]->SetWeight(2, 0);
				_neurons[i]->SetWeight(3, 0);
			}
		}

		void Training(double **train_input, int train_input_size)
		{
			_train_input = train_input;
			_train_input_size = train_input_size;

			for (int u=0; u<100000; u++) {
				// Amostragem: retira uma amostra x do espaco de entrada
				int t = u/100,
						r = random()%train_input_size;
				// int r = t%train_input_size;

				double *input = train_input[r],
							 min_value = 99999999.0;
				int min_index = -1;

				// Casamento por similaridade: encontra o neuronio com o melhor casamento i(x) no passo de tempo n
				for (int j=0; j<_neurons_size; j++) {
					Neuron *neuron = _neurons[j];

					double d1 = input[0]-neuron->GetWeight(0),
								 d2 = input[1]-neuron->GetWeight(1),
								 d3 = input[2]-neuron->GetWeight(2),
								 d = d1*d1+d2*d2+d3*d3;
	
					if (d < min_value) {
						min_index = j;
						min_value = d;
					}
				}
				if (min_index < 0) {
					min_index = 0;
				}

				double eta0 = 0.1,//(((u/1000)%2)==0)?0.1:0.01,
							 sigma0 = 18.0,
							 t1 = 1000.0/log(sigma0),
							 t2 = 1000.0,
							 eta = eta0*exp(-((double)t/t2)),
							 sigma = sigma0*exp(-((double)t/t1)),
							 sigma2 = 2*sigma*sigma;

				// Atualizacao: ajusta os pesos sinapticos de todos os neuronios
				for (int j=0; j<_neurons_size; j++) {
					// { int j = min_index;
					Neuron *neuron = _neurons[j];

					double d = min_index-j,
								 t = (d*d)/sigma2,
								 h = exp(-t),
								 m = eta*h;

					neuron->UpdateWeight(0, m*(input[0]-neuron->GetWeight(0)));
					neuron->UpdateWeight(1, m*(input[1]-neuron->GetWeight(1)));
					neuron->UpdateWeight(2, m*(input[2]-neuron->GetWeight(2)));
				}
			
				if ((u%1000) == 0) {
					Classify(train_input, train_input_size);
				}

        if (IsVisible() == false) {
          break;
        }
			}

			puts("Traininging complete");
		}

		void Classify(double **train_input, int train_input_size)
		{
			_classify_input = train_input;
			_classify_input_size = train_input_size;

			int k = 0;

			for (int t=0; t<train_input_size; t++) {
				// Amostragem: retira uma amostra x do espaco de entrada
				double *input = train_input[t],
							 min_value = 99999999.0;
				int min_index = -1;

				// Casamento por similaridade: encontra o neuronio com o melhor casamento i(x) no passo de tempo n
				for (int j=0; j<_neurons_size; j++) {
					Neuron *neuron = _neurons[j];

					double d1 = input[0]-neuron->GetWeight(0),
								 d2 = input[1]-neuron->GetWeight(1),
								 d3 = 0,//input[2]-neuron->GetWeight(2),
								 d = d1*d1+d2*d2+d3*d3;
	
					if (d < min_value) {
						min_index = j;
						min_value = d;
					}
				}

				// CHANGE:: alterando o classify altera o input
				input[3] = _neurons[min_index]->GetWeight(2);

				int c1 = (int)round(input[2]),
						c2 = (int)round(input[3]);

				if (c1 != c2) {
					k++;
				}

        if (IsVisible() == false) {
          break;
        }
			}

			printf("Acerto:: Total[%d], Erro[%d], Acerto[%.2f%%], Erro[%.2f%%]\n", 
					train_input_size, k, 100.0*((train_input_size-k)/(double)train_input_size), 100.0*(k/(double)train_input_size));

      _mutex.lock();

			Repaint();
		}

		virtual void Paint(Graphics *g)
		{
      if (_train_input == nullptr) {
        _mutex.unlock();

        return;
      }

      jpoint_t
        size = GetSize();

			g->Clear();
			g->SetColor({0xf0, 0xf0, 0xf0, 0xff});
			g->FillRectangle({0, 0, size.x, size.y});

			int 
        dx = size.x/4,
				dy = size.y/2,
				ex = 3*dx,
				ey = dy,
				s = dx;

			for (int i=0; i<_train_input_size; i++) {
				// identifica a regiao do ponto
				double 
          x = _train_input[i][0],
					y = _train_input[i][1],
					c = _train_input[i][2];
				uint32_t 
          classe = (int)c,
					color = GetColor(classe);

				g->SetRGB(color, {(int)(x*s+dx), (int)(y*s+dy)});
			}
			
			if (_classify_input != nullptr) {
				// classifica os pontos
				for (int i=0; i<_classify_input_size; i++) {
					// identifica a regiao do ponto
					double 
            x = _classify_input[i][0],
						y = _classify_input[i][1],
						c = _classify_input[i][3];
					uint32_t 
            classe = (int)round(c),
						color = GetColor(classe);

					g->SetRGB(color, {(int)(x*s+ex), (int)(y*s+ey)});
				}
			}
			
			g->SetColor({0x00, 0x00, 0x00, 0xff});

			for (int i=0; i<_neurons_size; i++) {
				Neuron 
          *neuron = _neurons[i];
				uint32_t 
          classe = (int)round(neuron->GetWeight(2)),
					color = GetColor(classe);

				g->SetColor(color);
				g->FillRectangle({(int)(neuron->GetWeight(0)*s+dx), (int)(neuron->GetWeight(1)*s+dy), 8, 8});
			}

			g->SetColor({0x00, 0x00, 0x00, 0xff});
			g->DrawCircle({dx, dy}, 1*s);
			g->DrawCircle({3*dx, dy}, 1*s);

      _mutex.unlock();
		}

    virtual void ShowApp()
    {
      double **points1 =  CreatePoints(POINTS);
      double **points2 =  CreatePoints(POINTS);
      double **points3 =  CreatePoints(4*POINTS);

      Init(points1, 8*POINTS);
      Training(points2, 8*POINTS);
      Classify(points3, 32*POINTS);

      SetVisible(false);

      for (size_t i=0; i<POINTS; i++) {
        delete [] points1[i];
      }

      delete [] points1;
      
      for (size_t i=0; i<POINTS; i++) {
        delete [] points2[i];
      }

      delete [] points2;
      
      for (size_t i=0; i<4*POINTS; i++) {
        delete [] points3[i];
      }

      delete [] points3;

			for (int i=0; i<_neurons_size; i++) {
				delete _neurons[i];
			}

      delete [] _neurons;

      Application::Quit();
    }

};

int main(int argc, char **argv)
{
  Application::Init(argc, argv);

  srand(time(nullptr));

  SOM app(4, 2*256);

  app.SetTitle("K-means");
  app.Exec();

  Application::Loop();

  return 0;
}

