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
#include "jcanvas/widgets/jgridlayout.h"
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/jbutton.h"

#include <list>

using namespace jcanvas;

class Display : public Component {

		private:
				std::string 
          _text,
					_operation;
				int 
          _draw;

		public:
				Display(int x, int y, int width, int height);
				virtual ~Display();

				void SetText(std::string text);
				void SetOperation(std::string text);
				void Clear();

				virtual void Paint(Graphics *g);

};

Display::Display(int x, int y, int width, int height):
   	Component({x, y, width, height})
{
  SetPreferredSize({0, 64});

  jtheme_t
    &theme = GetTheme();

  theme.font.primary = &Font::Big;
}

Display::~Display()
{
}

void Display::SetText(std::string text)
{
  _text = text;
  _draw = 0;

  Repaint();
} 

void Display::Paint(Graphics *g)
{
  if ((void *)g == nullptr) {
    return;
  }

  Component::Paint(g);

  jtheme_t
    theme = GetTheme();

  g->SetColor({0xf0, 0xf0, 0xf0, 0xff});

  if (theme.font.primary != nullptr) {
    g->SetFont(theme.font.primary);
  }

  jpoint_t 
    size = GetSize();

  g->DrawString(_text, {-8, 0, size}, jhorizontal_align_t::Right, jvertical_align_t::Center);
  g->DrawString(_operation, {8, 0, size}, jhorizontal_align_t::Left, jvertical_align_t::Center);
}

void Display::SetOperation(std::string text)
{
  _operation = text;
  _draw = 1;

  Repaint();
}

void Display::Clear()
{
  SetText("0");
}

class Calculator : public Frame, public ActionListener {

		private:
			std::list<Button *> _buttons;
			std::string _number0;
			std::string _number1;
			std::string _operation;
			Display *_display;
      Container *_container;
			int _state;

		public:
			Calculator();
			virtual ~Calculator();

			void Process(std::string type);

			virtual bool KeyPressed(KeyEvent *event);
			virtual void ActionPerformed(ActionEvent *event);

};

Calculator::Calculator():
	Frame(/*"Calculator", */ {500, 400})
{
	_number0 = "";
	_number1 = "";
	_operation = -1;
	_state = 1;

	_display = new Display(0, 0, 0, 0);

	_display->Clear();

	Button *b[] = { 
		new Button("7"),
		new Button("8"),
		new Button("9"),
		new Button("/"),
		new Button("C"),
		new Button("4"),
		new Button("5"),
		new Button("6"),
		new Button("x"),
		new Button("raiz"),
		new Button("1"),
		new Button("2"),
		new Button("3"),
		new Button("-"),
		new Button("del"),
		new Button("0"),
		new Button("."),
		new Button("%"),
		new Button("+"),
		new Button("=")
	};

	SetLayout(new BorderLayout());

	Add(_display, jborderlayout_align_t::North);

	_container = new Container();

	_container->SetLayout(new GridLayout(4, 5, 2, 2));

	for (int i=0; i<20; i++) {
		_buttons.push_back(b[i]);

		b[i]->RegisterActionListener(this);

		_container->Add(b[i]);
	}

	Add(_container, jborderlayout_align_t::Center);

	b[10]->RequestFocus();
}

Calculator::~Calculator() 
{
  Layout *layout = GetLayout();

  RemoveAll();

  delete layout;
  layout = nullptr;

	delete _display;
  _display = nullptr;
  
  _container->RemoveAll();

  layout = _container->GetLayout();

  delete layout;
  layout = nullptr;

  delete _container;
  _container = nullptr;

	while (_buttons.size() > 0) {
		Button *b = _buttons.back();

		_buttons.pop_back();

		delete b;
	}
}

void Calculator::Process(std::string type)
{
	Button *button = (Button *)GetFocusOwner();

  std::string text = button->GetText();

	if (type == ".") {
		if (_state == 2 || _state == 5) {
			if (_number0.size() < 9) {
				if (strchr(_number0.c_str(), '.') == nullptr) {
					_number0 += ".";
				}
			}
		} else if (_state == 4) {
			if (_number1.size() < 9) {
				if (strchr(_number1.c_str(), '.') == nullptr) {
					_number1 += ".";
				}
			}
		}
	} else if (type == "1" ||
			type == "2" ||
			type == "3" ||
			type == "4" ||
			type == "5" ||
			type == "6" ||
			type == "7" ||
			type == "8" ||
			type == "9" ||
			type == "0") {
		if (_state == 1 || _state == 2 || _state == 5 || _state == 7 || _state == 8) {
			if (_state == 1 || _state == 5 || _state == 7) {
				_number0 = text;
			} else {
				if (_number0.size() < 9 && (_number0 != "0" || text != "0")) {
					if (_number0 == "0") {
						_number0 = text;
					} else {
						_number0 += text;
					}
				}
			}

			_state = 2;
		} else if (_state == 3 || _state == 4 || _state == 6) {
			if (_state == 3 || _state == 6) {
				_number1 = text;
			} else {
				if (_number1.size() < 9 && (_number1 != "0" || text != "0")) {
					if (_number1 == "0") {
						_number1 = text;
					} else {
						_number1 += text;
					}
				}
			}

			_state = 4;
		}
	} else if (type == "+" ||
			type == "-" ||
			type == "x" ||
			type == "/") {
		if (_state == 1) {
			if (type == "-") {
				_state = 8;
				_number0 = "-";
			}
		} else if (_state == 2) {
			_state = 3;
			_operation = type;
		} else if (_state == 3) {
			_state = 3;
			_operation = type;
		} else if (_state == 5) {
			_state = 6;
			_operation = type;
		} else if (_state == 6) {
			_state = 6;
			_operation = type;
		} else if (_state == 7) {
			_state = 3;
			_operation = type;
		} else if (_state == 4) {
			Process("=");

			_operation = type;
			_state = 3;
		}

		if (_state == 3 || _state == 6) {
			_display->SetOperation(text);
		}
	} else if (type == "raiz") {
		if (_state == 2 || _state == 3 || _state == 4 || _state == 5 || _state == 6 || _state == 7) {
			if (_state == 4) {
				_number0 = _number1;
			}

			_state = 7;

			double a1 = atof(_number0.c_str());
			const char *i1;
			char tmp[255];

			if (a1 < 0) {
				_state = 255;
				_display->SetText("Erro");

				return;
			}

			a1 = sqrt(a1);

			if ((a1-(int)a1) > 0.0) {
				if (a1 >= 100000000) {
					sprintf(tmp, "%g", a1);
				} else {
					sprintf(tmp, "%f", a1);
				}
			} else {
				sprintf(tmp, "%d", (int)a1);
			}

			// INFO:: tirar zeros a direitaa
			std::string zeros = tmp;

			if (strchr(tmp, '.') != nullptr) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != nullptr) {
					int d1 = (int)(i1-zeros.c_str());

					if (d1 == (int)(zeros.size()-1)) {
						zeros = zeros.substr(0, zeros.size()-1);
					} else {
						break;
					}
				}

				if (zeros.size() > 1 && zeros[zeros.size()-1] == '.') {
					zeros = zeros.substr(0, zeros.size()-1);
				}
			}

			_number0 = zeros;
		}
	} else if (type == "=") {
		_display->SetOperation("");

		if (_state == 4) {
			_state = 5;

			double a1 = atof(_number0.c_str()),
						 a2 = atof(_number1.c_str());
			const char *i1;
			char tmp[255];

			if (_operation == "/") {
				if (a2 == 0) {
					_state = 255;

					_display->SetText("Erro");
				} else {
					a1 /= a2;
				}
			} else if (_operation == "x") {
				a1 *= a2;
			} else if (_operation == "+") {
				a1 += a2;
			} else if (_operation == "-") {
				a1 -= a2;
			}

			if ((a1-(int)a1) > 0.0) {
				if (a1 >= 100000000) {
					sprintf(tmp, "%g", a1);
				} else {
					sprintf(tmp, "%f", a1);
				}
			} else {
				sprintf(tmp, "%d", (int)a1);
			}

			// INFO:: tirar zeros a direitaa
			std::string zeros = tmp;

			if (strchr(tmp, '.') != nullptr) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != nullptr) {
					int d1 = (int)(i1-zeros.c_str());

					if (d1 == (int)(zeros.size()-1)) {
						zeros = zeros.substr(0, zeros.size()-1);
					} else {
						break;
					}
				}

				if (zeros.size() > 1 && zeros[zeros.size()-1] == '.') {
					zeros = zeros.substr(0, zeros.size()-1);
				}
			}

			_number0 = zeros;
		}
	} else if (type == "%") {
		if (_state == 4) {
			_state = 5;

			double a1 = atof(_number0.c_str()),
						 a2 = atof(_number1.c_str());
			const char *i1;
			char tmp[255];

			a2 = a1*(a2/100.0);

			if (_operation == "/") {
				if (a2 == 0) {
					_state = 255;

					_display->SetText("Erro");
				} else {
					a1 /= a2;
				}
			} else if (_operation == "x") {
				a1 *= a2;
			} else if (_operation == "+") {
				a1 += a2;
			} else if (_operation == "-") {
				a1 -= a2;
			}

			if ((a1-(int)a1) > 0.0) {
				if (a1 >= 100000000) {
					sprintf(tmp, "%g", a1);
				} else {
					sprintf(tmp, "%f", a1);
				}
			} else {
				sprintf(tmp, "%d", (int)a1);
			}

			// INFO:: tirar zeros a direitaa
			std::string zeros = tmp;

			if (strchr(tmp, '.') != nullptr) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != nullptr) {
					int d1 = (int)(i1-zeros.c_str());

					if (d1 == (int)(zeros.size()-1)) {
						zeros = zeros.substr(0, zeros.size()-1);
					} else {
						break;
					}
				}

				if (zeros.size() > 1 && zeros[zeros.size()-1] == '.') {
					zeros = zeros.substr(0, zeros.size()-1);
				}
			}

			_number0 = zeros;
		}
	} else if (type == "C") {
		_number0 = "";
		_number1 = "";
		_state = 1;
	} else if (type == "del") {
		if (_state == 2 || _state == 5 || _state == 7) {
			_number0 = _number0.substr(0, _number0.size()-1);

			if (_number0 == "") {
				_number0 = "0";
			}
		} else if (_state == 4) {
			_number1 = _number1.substr(0, _number1.size()-1);

			if (_number1 == "") {
				_number1 = "0";
			}
		}
	}
}

bool Calculator::KeyPressed(KeyEvent *event)
{
	if (Frame::KeyPressed(event) == true) {
		return true;
	}

	std::string num = "";

	if (event->GetSymbol() == jkeyevent_symbol_t::Number1) {
		num = "1";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number2) {
		num = "2";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number3) {
		num = "3";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number4) {
		num = "4";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number5) {
		num = "5";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number6) {
		num = "6";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number7) {
		num = "7";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number8) {
		num = "8";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number9) {
		num = "9";
	} else if (event->GetSymbol() == jkeyevent_symbol_t::Number0) {
		num = "0";
	}

	if (num != "") {
		if (_state == 1 || _state == 2 || _state == 5 || _state == 7 || _state == 8) {
			if (_state == 1 || _state == 5 || _state == 7) {
				_number0 = num;
			} else {
				if (_number0.size() < 9 && (_number0 != "0" || num != "0")) {
					if (_number0 == "0") {
						_number0 = num;
					} else {
						_number0 += num;
					}
				}
			}

			_state = 2;
		} else if (_state == 3 || _state == 4 || _state == 6) {
			if (_state == 3 || _state == 6) {
				_number1 = num;
			} else {
				if (_number1.size() < 9 && (_number1 != "0" || num != "0")) {
					if (_number1 == "0") {
						_number1 = num;
					} else {
						_number1 += num;
					}
				}
			}

			_state = 4;
		}
	}

	if (_state == 1) {
		_display->Clear();
	} else if (_state == 2 || _state == 3 || _state == 5 || _state == 7) {
		char 
      *tmp = strdup(_number0.c_str()),
			*i1;

		if (strlen(tmp) > 9) {
			i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number0 = tmp;
					_display->SetText(_number0);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number0);
		}

		if (_state == 3) {
			if (_operation == "+") {
				_display->SetOperation("+");
			} else if (_operation == "-") {
				_display->SetOperation("-");
			} else if (_operation == "x") {
				_display->SetOperation("x");
			} else if (_operation == "/") {
				_display->SetOperation("/");
			}
		}

		free(tmp);
	} else if (_state == 4) {
		char *tmp = strdup(_number1.c_str());

		if (strlen(tmp) > 9) {
			char *i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number1 = tmp;
					_display->SetText(_number1);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number1);
		}

		free(tmp);
	}

	return true;
}

void Calculator::ActionPerformed(ActionEvent *event)
{
	Button *button = (Button *)event->GetSource();

	Process(button->GetText());

	if (_state == 1) {
		_display->Clear();
	} else if (_state == 2 || _state == 3 || _state == 5 || _state == 7) {
		char 
      *tmp = strdup(_number0.c_str()),
			*i1;

		if (strlen(tmp) > 9) {
			i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number0 = tmp;
					_display->SetText(_number0);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number0);
		}

		if (_state == 3) {
			if (_operation == "+") {
				_display->SetOperation("+");
			} else if (_operation == "-") {
				_display->SetOperation("-");
			} else if (_operation == "x") {
				_display->SetOperation("x");
			} else if (_operation == "/") {
				_display->SetOperation("/");
			}
		}

		free(tmp);
	} else if (_state == 4) {
		char *tmp = strdup(_number1.c_str());

		if (strlen(tmp) > 9) {
			char *i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number1 = tmp;
					_display->SetText(_number1);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number1);
		}

		free(tmp);
	}
}

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Calculator app;

	app.SetTitle("Calculator");

	Application::Loop();

	return 0;
}
