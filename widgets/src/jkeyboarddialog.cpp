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
#include "jcanvas/widgets/jkeyboarddialog.h"
#include "jcanvas/widgets/jgridlayout.h"
#include "jcanvas/widgets/jflowlayout.h"

#include <algorithm>

#define KEY_SIZE_1  32
#define KEY_SIZE_2 (4 * (KEY_SIZE_1))
#define KEY_SIZE_3 (6 * (KEY_SIZE_1))

#define KEYBOARD_LAYOUT                                     \
  _display = new Text(_text);                               \
                                                            \
  if (_is_password == true) {                               \
    _display->SetEchoChar('*');                             \
  }                                                         \
                                                            \
  _display->SetFocusable(false);                            \
                                                            \
  Add(_display, jborderlayout_align_t::Center);             \
                                                            \
  Container                                                 \
    *container = new Container();                           \
  Container                                                 \
    *lines[5];                                              \
                                                            \
  container->SetLayout<GridLayout>(5, 1, 4, 0);             \
                                                            \
  container->SetScrollableX(false);                         \
  container->SetScrollableY(false);                         \
  container->SetSize({720, 5*(KEY_SIZE_1 + 4)});            \
                                                            \
  for (int i=0; i<5; i++) {                                 \
    lines[i] = new Container();                             \
                                                            \
    lines[i]->SetLayout<FlowLayout>(jflowlayout_align_t::Center, 4, 0); \
                                                            \
    lines[i]->SetScrollableX(false);                        \
    lines[i]->SetScrollableY(false);                        \
    lines[i]->SetSize(0, KEY_SIZE_1);                       \
                                                            \
    container->Add(lines[i]);                               \
  }                                                         \
                                                            \
  Add(container, jborderlayout_align_t::North);             \

namespace jcanvas {

class KeyButton : public Button {

  public:
    KeyButton(std::string label1, std::string label2, ActionListener *listener, int width, int height):
      Button(label1) 
    {
      jpoint_t<int> t = {
        .x = width,
        .y = height
      };

      SetName(label2);
      SetPreferredSize(t);

      RegisterActionListener(listener);
    }

};

KeyboardDialog::KeyboardDialog(Container *parent, jkeyboard_type_t type, bool text_visible, bool is_password):
   Dialog("Keyboard", parent)
{
  _display = NULL;
  _shift_pressed = false;
  _type = type;
  _is_password = is_password;

  SetLayout<BorderLayout>();
  
  if (_type == jkeyboard_type_t::Qwerty) {
    BuildQWERTYKeyboard();
  } else if (_type == jkeyboard_type_t::AlphaNumeric) {
    BuildAlphaNumericKeyboard();
  } else if (_type == jkeyboard_type_t::Numeric) {
    BuildNumericKeyboard();
  } else if (_type == jkeyboard_type_t::Phone) {
    BuildPhoneKeyboard();
  } else if (_type == jkeyboard_type_t::Internet) {
    BuildInternetKeyboard();
  }

  SetSize(720, 400);
  Pack(false);
}

KeyboardDialog::~KeyboardDialog() 
{
  RemoveAll();
}

void KeyboardDialog::ActionPerformed(ActionEvent *event)
{
  Button *button = reinterpret_cast<Button *>(event->GetSource());
  std::string label = button->GetTextComponent()->GetText();
  jkeyevent_modifiers_t modifiers = jkeyevent_modifiers_t::None;
  jkeyevent_symbol_t symbol = jkeyevent_symbol_t::Unknown;
  int code = -1;
  bool any = true;

  if (label == "caps") {
    ProcessCaps(button);
    
    modifiers = jkeyevent_modifiers_t::CapsLock;
    code = 20;
    any = false;
  } else if (label == "shift") {
    if (_shift_pressed == true) {
      _shift_pressed = false;
    } else {
      _shift_pressed = true;
    }

    ProcessCaps(button);

    modifiers = jkeyevent_modifiers_t::Shift;
    code = 16;
    any = false;
  } else if (label == "enter") {
    symbol = jkeyevent_symbol_t::Enter;
    code = '\n';
    any = false;
  } else if (label == "space") {
    symbol = jkeyevent_symbol_t::Space;
    code = ' ';
    any = false;
  } else if (label == "tab") {
    symbol = jkeyevent_symbol_t::Tab;
    code = '\t';
    any = false;
  } else if (label == "left") {
    symbol = jkeyevent_symbol_t::CursorLeft;
    code = 37;
  } else if (label == "right") {
    symbol = jkeyevent_symbol_t::CursorRight;
    code = 39;
    any = false;
  } else if (label == "up") {
    symbol = jkeyevent_symbol_t::CursorUp;
    code = 38;
    any = false;
  } else if (label == "down") {
    symbol = jkeyevent_symbol_t::CursorDown;
    code = 40;
    any = false;
  } else if (label == "del") {
    symbol = jkeyevent_symbol_t::Delete;
    code = 46;
    any = false;
  } else if (label == "ins") {
    symbol = jkeyevent_symbol_t::Insert;
    code = 45;
    any = false;
  } else if (label == "esc") {
    symbol = jkeyevent_symbol_t::Escape;
    code = 27;
    any = false;
  } else if (label == "back") {
    symbol = jkeyevent_symbol_t::Backspace;
    code = '\b';
    any = false;
  } else if (label == "0") {
    symbol = jkeyevent_symbol_t::Number0;
    code = '0';
  } else if (label == "1") {
    symbol = jkeyevent_symbol_t::Number1;
    code = '1';
  } else if (label == "2") {
    symbol = jkeyevent_symbol_t::Number2;
    code = '2';
  } else if (label == "3") {
    symbol = jkeyevent_symbol_t::Number3;
    code = '3';
  } else if (label == "4") {
    symbol = jkeyevent_symbol_t::Number4;
    code = '4';
  } else if (label == "5") {
    symbol = jkeyevent_symbol_t::Number5;
    code = '5';
  } else if (label == "6") {
    symbol = jkeyevent_symbol_t::Number6;
    code = '6';
  } else if (label == "7") {
    symbol = jkeyevent_symbol_t::Number7;
    code = '7';
  } else if (label == "8") {
    symbol = jkeyevent_symbol_t::Number8;
    code = '8';
  } else if (label == "9") {
    symbol = jkeyevent_symbol_t::Number9;
    code = '9';
  } else if (label == "A") {
    symbol = jkeyevent_symbol_t::A;
    code = 'A';
  } else if (label == "B") {
    symbol = jkeyevent_symbol_t::B;
    code = 'B';
  } else if (label == "C") {
    symbol = jkeyevent_symbol_t::C;
    code = 'C';
  } else if (label == "D") {
    symbol = jkeyevent_symbol_t::D;
    code = 'D';
  } else if (label == "E") {
    symbol = jkeyevent_symbol_t::E;
    code = 'E';
  } else if (label == "F") {
    symbol = jkeyevent_symbol_t::F;
    code = 'F';
  } else if (label == "G") {
    symbol = jkeyevent_symbol_t::G;
    code = 'G';
  } else if (label == "H") {
    symbol = jkeyevent_symbol_t::H;
    code = 'H';
  } else if (label == "I") {
    symbol = jkeyevent_symbol_t::I;
    code = 'I';
  } else if (label == "J") {
    symbol = jkeyevent_symbol_t::J;
    code = 'J';
  } else if (label == "K") {
    symbol = jkeyevent_symbol_t::K;
    code = 'K';
  } else if (label == "L") {
    symbol = jkeyevent_symbol_t::L;
    code = 'L';
  } else if (label == "M") {
    symbol = jkeyevent_symbol_t::M;
    code = 'M';
  } else if (label == "N") {
    symbol = jkeyevent_symbol_t::N;
    code = 'N';
  } else if (label == "O") {
    symbol = jkeyevent_symbol_t::O;
    code = 'O';
  } else if (label == "P") {
    symbol = jkeyevent_symbol_t::P;
    code = 'P';
  } else if (label == "Q") {
    symbol = jkeyevent_symbol_t::Q;
    code = 'Q';
  } else if (label == "R") {
    symbol = jkeyevent_symbol_t::R;
    code = 'R';
  } else if (label == "S") {
    symbol = jkeyevent_symbol_t::S;
    code = 'S';
  } else if (label == "T") {
    symbol = jkeyevent_symbol_t::T;
    code = 'T';
  } else if (label == "U") {
    symbol = jkeyevent_symbol_t::U;
    code = 'U';
  } else if (label == "V") {
    symbol = jkeyevent_symbol_t::V;
    code = 'V';
  } else if (label == "W") {
    symbol = jkeyevent_symbol_t::W;
    code = 'W';
  } else if (label == "X") {
    symbol = jkeyevent_symbol_t::X;
    code = 'X';
  } else if (label == "Y") {
    symbol = jkeyevent_symbol_t::Y;
    code = 'Y';
  } else if (label == "Z") {
    symbol = jkeyevent_symbol_t::Z;
    code = 'Z';
  } else if (label == "a") {
    symbol = jkeyevent_symbol_t::a;
    code = 'a';
  } else if (label == "b") {
    symbol = jkeyevent_symbol_t::b;
    code = 'b';
  } else if (label == "c") {
    symbol = jkeyevent_symbol_t::c;
    code = 'c';
  } else if (label == "d") {
    symbol = jkeyevent_symbol_t::d;
    code = 'd';
  } else if (label == "e") {
    symbol = jkeyevent_symbol_t::e;
    code = 'e';
  } else if (label == "f") {
    symbol = jkeyevent_symbol_t::f;
    code = 'f';
  } else if (label == "g") {
    symbol = jkeyevent_symbol_t::g;
    code = 'g';
  } else if (label == "h") {
    symbol = jkeyevent_symbol_t::h;
    code = 'h';
  } else if (label == "i") {
    symbol = jkeyevent_symbol_t::i;
    code = 'i';
  } else if (label == "j") {
    symbol = jkeyevent_symbol_t::j;
    code = 'j';
  } else if (label == "k") {
    symbol = jkeyevent_symbol_t::k;
    code = 'k';
  } else if (label == "l") {
    symbol = jkeyevent_symbol_t::l;
    code = 'l';
  } else if (label == "m") {
    symbol = jkeyevent_symbol_t::m;
    code = 'm';
  } else if (label == "n") {
    symbol = jkeyevent_symbol_t::n;
    code = 'n';
  } else if (label == "o") {
    symbol = jkeyevent_symbol_t::o;
    code = 'o';
  } else if (label == "p") {
    symbol = jkeyevent_symbol_t::p;
    code = 'p';
  } else if (label == "q") {
    symbol = jkeyevent_symbol_t::q;
    code = 'q';
  } else if (label == "r") {
    symbol = jkeyevent_symbol_t::r;
    code = 'r';
  } else if (label == "s") {
    symbol = jkeyevent_symbol_t::s;
    code = 's';
  } else if (label == "t") {
    symbol = jkeyevent_symbol_t::t;
    code = 't';
  } else if (label == "u") {
    symbol = jkeyevent_symbol_t::u;
    code = 'u';
  } else if (label == "v") {
    symbol = jkeyevent_symbol_t::v;
    code = 'v';
  } else if (label == "w") {
    symbol = jkeyevent_symbol_t::w;
    code = 'w';
  } else if (label == "x") {
    symbol = jkeyevent_symbol_t::x;
    code = 'x';
  } else if (label == "y") {
    symbol = jkeyevent_symbol_t::y;
    code = 'y';
  } else if (label == "z") {
    symbol = jkeyevent_symbol_t::z;
    code = 'z';
  } else if (label == "!") {
    symbol = jkeyevent_symbol_t::ExclamationMark;
    code = '!';
  } else if (label == "\"") {
    symbol = jkeyevent_symbol_t::Quotation;
    code = '\"';
  } else if (label == "$") {
    symbol = jkeyevent_symbol_t::Dollar;
    code = '$';
  } else if (label == "%") {
    symbol = jkeyevent_symbol_t::Percent;
    code = '%';
  } else if (label == "&") {
    symbol = jkeyevent_symbol_t::Ampersand;
    code = '&';
  } else if (label == "'") {
    symbol = jkeyevent_symbol_t::Aposthrophe;
    code = '\'';
  } else if (label == "(") {
    symbol = jkeyevent_symbol_t::ParenthesisLeft;
    code = '(';
  } else if (label == ")") {
    symbol = jkeyevent_symbol_t::ParenthesisRight;
    code = ')';
  } else if (label == "*") {
    symbol = jkeyevent_symbol_t::Star;
    code = '*';
  } else if (label == "#") {
    symbol = jkeyevent_symbol_t::Sharp;
    code = '#';
  } else if (label == "#") {
    symbol = jkeyevent_symbol_t::Hash;
    code = '#';
  } else if (label == "+") {
    symbol = jkeyevent_symbol_t::Plus;
    code = '+';
  } else if (label == ",") {
    symbol = jkeyevent_symbol_t::Comma;
    code = ',';
  } else if (label == "-") {
    symbol = jkeyevent_symbol_t::Minus;
    code = '-';
  } else if (label == ".") {
    symbol = jkeyevent_symbol_t::Period;
    code = '.';
  } else if (label == "/") {
    symbol = jkeyevent_symbol_t::Slash;
    code = '/';
  } else if (label == ":") {
    symbol = jkeyevent_symbol_t::Colon;
    code = ':';
  } else if (label == ";") {
    symbol = jkeyevent_symbol_t::SemiColon;
    code = ';';
  } else if (label == "<") {
    symbol = jkeyevent_symbol_t::LessThan;
    code = '<';
  } else if (label == "=") {
    symbol = jkeyevent_symbol_t::Equals;
    code = '=';
  } else if (label == ">") {
    symbol = jkeyevent_symbol_t::GreaterThan;
    code = '>';
  } else if (label == "?") {
    symbol = jkeyevent_symbol_t::QuestionMark;
    code = '?';
  } else if (label == "@") {
    symbol = jkeyevent_symbol_t::At;
    code = '@';
  } else if (label == "[") {
    symbol = jkeyevent_symbol_t::SquareBracketLeft;
    code = '[';
  } else if (label == "\\") {
    symbol = jkeyevent_symbol_t::BackSlash;
    code = '\\';
  } else if (label == "]") {
    symbol = jkeyevent_symbol_t::SquareBracketRight;
    code = ']';
  } else if (label == "^") {
    symbol = jkeyevent_symbol_t::CircumflexAccent;
    code = '^';
  } else if (label == "_") {
    symbol = jkeyevent_symbol_t::Underscore;
    code = '_';
  } else if (label == "`") {
    symbol = jkeyevent_symbol_t::GraveAccent;
    code = '`';
  } else if (label == "{") {
    symbol = jkeyevent_symbol_t::CurlyBracketLeft;
    code = '{';
  } else if (label == "|") {
    symbol = jkeyevent_symbol_t::VerticalBar;
    code = '|';
  } else if (label == "}") {
    symbol = jkeyevent_symbol_t::CurlyBracketRight;
    code = '}';
  } else if (label == "~") {
    symbol = jkeyevent_symbol_t::Tilde;
    code = '~';
  } else if (label == "F1") {
    symbol = jkeyevent_symbol_t::F1;
    code = 112;
  } else if (label == "F2") {
    symbol = jkeyevent_symbol_t::F2;
    code = 113;
  } else if (label == "F3") {
    symbol = jkeyevent_symbol_t::F3;
    code = 114;
  } else if (label == "F4") {
    symbol = jkeyevent_symbol_t::F4;
    code = 115;
  } else if (label == "F5") {
    symbol = jkeyevent_symbol_t::F5;
    code = 116;
  } else if (label == "F6") {
    symbol = jkeyevent_symbol_t::F6;
    code = 117;
  } else if (label == "F7") {
    symbol = jkeyevent_symbol_t::F7;
    code = 118;
  } else if (label == "F8") {
    symbol = jkeyevent_symbol_t::F8;
    code = 119;
  } else if (label == "F9") {
    symbol = jkeyevent_symbol_t::F9;
    code = 120;
  } else if (label == "F10") {
    symbol = jkeyevent_symbol_t::F10;
    code = 121;
  } else if (label == "F11") {
    symbol = jkeyevent_symbol_t::F11;
    code = 122;
  } else if (label == "F12") {
    symbol = jkeyevent_symbol_t::F12;
    code = 123;
  }

  if (_shift_pressed == true) {
    if (any == true) {
      ProcessCaps(button);

      _shift_pressed = false;
    }
  }

  KeyEvent *kevent1 = new KeyEvent(this, jkeyevent_type_t::Pressed, modifiers, code, symbol);
  KeyEvent *kevent2 = new KeyEvent(this, jkeyevent_type_t::Released, modifiers, code, symbol);

  _display->KeyPressed(kevent1);
  _display->KeyReleased(kevent2);

  DispatchKeyEvent(kevent1);
  DispatchKeyEvent(kevent2);
}

TextComponent * KeyboardDialog::GetTextComponent()
{
  return _display;
}

void KeyboardDialog::BuildInternetKeyboard()
{
  KEYBOARD_LAYOUT

  lines[0]->Add(new KeyButton("@", "#", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("1", "1", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("2", "2", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("3", "3", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("4", "4", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("5", "5", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("6", "6", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("7", "7", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("8", "8", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("9", "9", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("0", "0", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("+", "=", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[1]->Add(new KeyButton("q", "Q", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("w", "W", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("e", "E", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("r", "R", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("t", "T", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("y", "Y", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("u", "U", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("i", "I", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("o", "O", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("p", "P", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("(", "[", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton(")", "]", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[2]->Add(new KeyButton("a", "A", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("s", "S", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("d", "D", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("f", "F", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("g", "G", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("h", "H", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("j", "J", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("k", "K", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("l", "L", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("~", "^", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("-", "_", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[3]->Add(new KeyButton("caps", "caps", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("/", "|", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("z", "Z", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("x", "X", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("c", "C", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("v", "V", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("b", "B", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("n", "N", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("m", "M", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(".", ":", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("&", "%", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("?", "!", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[4]->Add(new KeyButton("shift", "shift", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("back", "back", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("http://", "http://", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("www.", "www.", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton(".com", ".com", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("enter", "enter", this, KEY_SIZE_2, KEY_SIZE_1));

  lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildAlphaNumericKeyboard()
{
  KEYBOARD_LAYOUT

  lines[0]->Add(new KeyButton("a", "A", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("b", "B", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("c", "C", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("d", "D", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("e", "E", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("f", "F", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("g", "G", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("h", "H", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[1]->Add(new KeyButton("i", "I", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("j", "J", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("k", "K", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("l", "L", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("m", "M", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("n", "N", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("o", "O", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("p", "P", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[2]->Add(new KeyButton("q", "Q", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("r", "R", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("s", "S", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("t", "T", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("u", "U", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("v", "V", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("x", "X", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("w", "W", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[3]->Add(new KeyButton("y", "Y", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("z", "Z", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("@", "#", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("(", "*", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(")", "-", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(".", ",", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(";", ":", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("/", "?", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[4]->Add(new KeyButton("caps", "caps", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("space", "space", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("back", "back", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("enter", "enter", this, KEY_SIZE_2, KEY_SIZE_1));

  lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildQWERTYKeyboard()
{
  KEYBOARD_LAYOUT

  lines[0]->Add(new KeyButton("'", "\"", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("1", "1", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("2", "2", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("3", "3", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("4", "4", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("5", "5", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("6", "6", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("7", "7", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("8", "8", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("9", "9", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("0", "0", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("-", "_", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("=", "+", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[1]->Add(new KeyButton("tab", "tab", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("q", "Q", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("w", "W", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("e", "E", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("r", "R", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("t", "T", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("y", "Y", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("u", "U", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("i", "I", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("o", "O", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("p", "P", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("'", "`", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("[", "{", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[2]->Add(new KeyButton("caps", "caps", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("a", "A", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("s", "S", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("d", "D", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("f", "F", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("g", "G", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("h", "H", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("j", "J", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("k", "K", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("l", "L", this, KEY_SIZE_1, KEY_SIZE_1));
  // lines[2]->Add(new KeyButton("ç", "Ç", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("~", "^", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("]", "}", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[3]->Add(new KeyButton("shift", "shift", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("\\", "|", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("z", "Z", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("x", "X", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("c", "C", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("v", "V", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("b", "B", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("n", "N", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("m", "M", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(",", "<", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(".", ">", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(";", ":", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("/", "?", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[4]->Add(new KeyButton("back", "back", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("space", "space", this, KEY_SIZE_3, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("enter", "enter", this, KEY_SIZE_2, KEY_SIZE_1));

  lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildNumericKeyboard()
{
  KEYBOARD_LAYOUT

  lines[0]->Add(new KeyButton("(", "(", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("7", "7", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("8", "8", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("9", "9", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("+", "+", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[1]->Add(new KeyButton(")", ")", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("4", "4", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("5", "5", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("6", "6", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("-", "-", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[2]->Add(new KeyButton("%", "%", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("1", "1", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("2", "2", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("3", "3", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("*", "*", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[3]->Add(new KeyButton("back", "back", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[3]->Add(new KeyButton(".", ".", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("=", "=", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("/", "/", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[4]->Add(new KeyButton("space", "space", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("enter", "enter", this, KEY_SIZE_2, KEY_SIZE_1));

  lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildPhoneKeyboard()
{
  KEYBOARD_LAYOUT

  lines[0]->Add(new KeyButton("7", "7", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("8", "8", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("9", "9", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[0]->Add(new KeyButton("(", "(", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[1]->Add(new KeyButton("4", "4", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("5", "5", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton("6", "6", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[1]->Add(new KeyButton(")", ")", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[2]->Add(new KeyButton("1", "1", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("2", "2", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("3", "3", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[2]->Add(new KeyButton("-", "-", this, KEY_SIZE_1, KEY_SIZE_1));
  
  lines[3]->Add(new KeyButton("*", "*", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("0", "0", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("+", "+", this, KEY_SIZE_1, KEY_SIZE_1));
  lines[3]->Add(new KeyButton("#", "#", this, KEY_SIZE_1, KEY_SIZE_1));

  lines[4]->Add(new KeyButton("space", "space", this, KEY_SIZE_2, KEY_SIZE_1));
  lines[4]->Add(new KeyButton("back", "back", this, KEY_SIZE_2, KEY_SIZE_1));

  lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::ProcessCaps(Button *button)
{
  SetIgnoreRepaint(true);

  std::vector<Component *> components;

  GetInternalComponents(this, components);

  for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
    KeyButton *btn = dynamic_cast<KeyButton *>(*i);

    if (btn != nullptr) {
      std::string name = btn->GetName();

      btn->SetName(btn->GetTextComponent()->GetText());
      btn->SetText(name);
    }
  }
  
  SetIgnoreRepaint(false);

  button->RequestFocus();

  Repaint();
}

void KeyboardDialog::RegisterKeyListener(KeyListener *listener)
{
  if (listener == NULL) {
    return;
  }

   std::lock_guard<std::mutex> guard(_key_listeners_mutex);

  if (std::find(_key_listeners.begin(), _key_listeners.end(), listener) == _key_listeners.end()) {
    _key_listeners.push_back(listener);
  }
}

void KeyboardDialog::RemoveKeyListener(KeyListener *listener)
{
  if (listener == NULL) {
    return;
  }

   std::lock_guard<std::mutex> guard(_key_listeners_mutex);

  _key_listeners.erase(std::remove(_key_listeners.begin(), _key_listeners.end(), listener), _key_listeners.end());
}

void KeyboardDialog::DispatchKeyEvent(KeyEvent *event)
{
  if (event == NULL) {
    return;
  }

  std::vector<KeyListener *> listeners;
  
  _key_listeners_mutex.lock();

  listeners = _key_listeners;

  _key_listeners_mutex.unlock();

  for (std::vector<KeyListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    KeyListener *listener = (*i);

    if (event->GetType() == jkeyevent_type_t::Pressed) {
      listener->KeyPressed(event);
    } else if (event->GetType() == jkeyevent_type_t::Released) {
      listener->KeyReleased(event);
    }
  }

  delete event;
}

std::vector<KeyListener *> & KeyboardDialog::GetKeyListeners()
{
  return _key_listeners;
}

}
