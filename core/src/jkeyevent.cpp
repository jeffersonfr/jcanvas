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
#include "jcanvas/core/jkeyevent.h"

namespace jcanvas {

KeyEvent::KeyEvent(void *source, jkeyevent_type_t type, jkeyevent_modifiers_t mod, int code, jkeyevent_symbol_t symbol):
  EventObject(source)
{
  _code = code;
  _symbol = symbol;
  _type = type;
  _mod = mod;
}

KeyEvent::~KeyEvent()
{
}

int KeyEvent::GetCodeFromSymbol(jkeyevent_symbol_t symbol)
{
  switch (symbol) {
    case jkeyevent_symbol_t::Unknown:
      return -1;
    case jkeyevent_symbol_t::Enter:
      return (int)'\n';
    case jkeyevent_symbol_t::Backspace:
      return (int)'\b';
    case jkeyevent_symbol_t::Tab:
      return (int)'\t';
    // case jkeyevent_symbol_t::Return:
    //  return (int)'\r';
    case jkeyevent_symbol_t::Cancel:
      return -1;
    case jkeyevent_symbol_t::Escape:
      return (int)'\x1b';
    case jkeyevent_symbol_t::Space:
      return (int)' ';
    case jkeyevent_symbol_t::ExclamationMark:
      return (int)'!';
    case jkeyevent_symbol_t::Quotation:
      return (int)'\"';
    case jkeyevent_symbol_t::Hash:
      return (int)'#';
    case jkeyevent_symbol_t::Dollar:
      return (int)'$';
    case jkeyevent_symbol_t::Percent:
      return (int)'%';
    case jkeyevent_symbol_t::Ampersand:
      return (int)'&';
    case jkeyevent_symbol_t::Aposthrophe:
      return (int)'\'';
    case jkeyevent_symbol_t::ParenthesisLeft:
      return (int)'(';
    case jkeyevent_symbol_t::ParenthesisRight:
      return (int)')';
    case jkeyevent_symbol_t::Star:
      return (int)'*';
    case jkeyevent_symbol_t::Plus:
      return (int)'+';
    case jkeyevent_symbol_t::Comma:
      return (int)',';
    case jkeyevent_symbol_t::Minus:
      return (int)'-';
    case jkeyevent_symbol_t::Period:
      return (int)'.';
    case jkeyevent_symbol_t::Slash:
      return (int)'/';
    case jkeyevent_symbol_t::Number0:
      return (int)'0';
    case jkeyevent_symbol_t::Number1:
      return (int)'1';
    case jkeyevent_symbol_t::Number2:
      return (int)'2';
    case jkeyevent_symbol_t::Number3:
      return (int)'3';
    case jkeyevent_symbol_t::Number4:
      return (int)'4';
    case jkeyevent_symbol_t::Number5:
      return (int)'5';
    case jkeyevent_symbol_t::Number6:
      return (int)'6';
    case jkeyevent_symbol_t::Number7:
      return (int)'7';
    case jkeyevent_symbol_t::Number8:
      return (int)'8';
    case jkeyevent_symbol_t::Number9:
      return (int)'9';
    case jkeyevent_symbol_t::Colon:
      return (int)':';
    case jkeyevent_symbol_t::SemiColon:
      return (int)';';
    case jkeyevent_symbol_t::LessThan:
      return (int)'<';
    case jkeyevent_symbol_t::Equals:
      return (int)'=';
    case jkeyevent_symbol_t::GreaterThan:
      return (int)'>';
    case jkeyevent_symbol_t::QuestionMark:
      return (int)'?';
    case jkeyevent_symbol_t::At:
      return (int)'@';
    case jkeyevent_symbol_t::A:
      return (int)'A';
    case jkeyevent_symbol_t::B:
      return (int)'B';
    case jkeyevent_symbol_t::C:
      return (int)'C';
    case jkeyevent_symbol_t::D:
      return (int)'D';
    case jkeyevent_symbol_t::E:
      return (int)'E';
    case jkeyevent_symbol_t::F:
      return (int)'F';
    case jkeyevent_symbol_t::G:
      return (int)'G';
    case jkeyevent_symbol_t::H:
      return (int)'H';
    case jkeyevent_symbol_t::I:
      return (int)'I';
    case jkeyevent_symbol_t::J:
      return (int)'J';
    case jkeyevent_symbol_t::K:
      return (int)'K';
    case jkeyevent_symbol_t::L:
      return (int)'L';
    case jkeyevent_symbol_t::M:
      return (int)'M';
    case jkeyevent_symbol_t::N:
      return (int)'N';
    case jkeyevent_symbol_t::O:
      return (int)'O';
    case jkeyevent_symbol_t::P:
      return (int)'P';
    case jkeyevent_symbol_t::Q:
      return (int)'Q';
    case jkeyevent_symbol_t::R:
      return (int)'R';
    case jkeyevent_symbol_t::S:
      return (int)'S';
    case jkeyevent_symbol_t::T:
      return (int)'T';
    case jkeyevent_symbol_t::U:
      return (int)'U';
    case jkeyevent_symbol_t::V:
      return (int)'V';
    case jkeyevent_symbol_t::W:
      return (int)'W';
    case jkeyevent_symbol_t::X:
      return (int)'X';
    case jkeyevent_symbol_t::Y:
      return (int)'Y';
    case jkeyevent_symbol_t::Z:
      return (int)'Z';
    case jkeyevent_symbol_t::SquareBracketLeft:
      return (int)'[';
    case jkeyevent_symbol_t::BackSlash:
      return (int)'\\';
    case jkeyevent_symbol_t::SquareBracketRight:
      return (int)']';
    case jkeyevent_symbol_t::CircumflexAccent:
      return (int)'^';
    case jkeyevent_symbol_t::Underscore:
      return (int)'_';
    case jkeyevent_symbol_t::GraveAccent:
      return (int)'`';
    case jkeyevent_symbol_t::a:
      return (int)'a';
    case jkeyevent_symbol_t::b:
      return (int)'b';
    case jkeyevent_symbol_t::c:
      return (int)'c';
    case jkeyevent_symbol_t::d:
      return (int)'d';
    case jkeyevent_symbol_t::e:
      return (int)'e';
    case jkeyevent_symbol_t::f:
      return (int)'f';
    case jkeyevent_symbol_t::g:
      return (int)'g';
    case jkeyevent_symbol_t::h:
      return (int)'h';
    case jkeyevent_symbol_t::i:
      return (int)'i';
    case jkeyevent_symbol_t::j:
      return (int)'j';
    case jkeyevent_symbol_t::k:
      return (int)'k';
    case jkeyevent_symbol_t::l:
      return (int)'l';
    case jkeyevent_symbol_t::m:
      return (int)'m';
    case jkeyevent_symbol_t::n:
      return (int)'n';
    case jkeyevent_symbol_t::o:
      return (int)'o';
    case jkeyevent_symbol_t::p:
      return (int)'p';
    case jkeyevent_symbol_t::q:
      return (int)'q';
    case jkeyevent_symbol_t::r:
      return (int)'r';
    case jkeyevent_symbol_t::s:
      return (int)'s';
    case jkeyevent_symbol_t::t:
      return (int)'t';
    case jkeyevent_symbol_t::u:
      return (int)'u';
    case jkeyevent_symbol_t::v:
      return (int)'v';
    case jkeyevent_symbol_t::w:
      return (int)'w';
    case jkeyevent_symbol_t::x:
      return (int)'x';
    case jkeyevent_symbol_t::y:
      return (int)'y';
    case jkeyevent_symbol_t::z:
      return (int)'z';
    case jkeyevent_symbol_t::CurlyBracketLeft:
      return (int)'{';
    case jkeyevent_symbol_t::VerticalBar:
      return (int)'|';
    case jkeyevent_symbol_t::CurlyBracketRight:
      return (int)'}';
    case jkeyevent_symbol_t::Tilde:
      return (int)'~';
    case jkeyevent_symbol_t::Delete:
      return (int)'\x7f';
    case jkeyevent_symbol_t::CursorLeft:
      return (int)0xf000;
    case jkeyevent_symbol_t::CursorRight:
      return (int)0xf001;
    case jkeyevent_symbol_t::CursorUp:
      return (int)0xf002;
    case jkeyevent_symbol_t::CursorDown:
      return (int)0xf003;
    case jkeyevent_symbol_t::Insert:
      return (int)0xf004;
    case jkeyevent_symbol_t::Home:
      return (int)0xf005;
    case jkeyevent_symbol_t::End:
      return (int)0xf006;
    case jkeyevent_symbol_t::PageUp:
      return (int)0xf007;
    case jkeyevent_symbol_t::PageDown:
      return (int)0xf008;
    case jkeyevent_symbol_t::Print:
      return (int)0xf009;
    case jkeyevent_symbol_t::Pause:
      return (int)0xf00a;
    case jkeyevent_symbol_t::Red:
      return (int)0x0190;
    case jkeyevent_symbol_t::Green:
      return (int)0x0191;
    case jkeyevent_symbol_t::Yellow:
      return (int)0x0192;
    case jkeyevent_symbol_t::Blue:
      return (int)0x0193;
    case jkeyevent_symbol_t::F1:
      return (int)0xf101;
    case jkeyevent_symbol_t::F2:
      return (int)0xf102;
    case jkeyevent_symbol_t::F3:
      return (int)0xf103;
    case jkeyevent_symbol_t::F4:
      return (int)0xf104;
    case jkeyevent_symbol_t::F5:
      return (int)0xf105;
    case jkeyevent_symbol_t::F6:
      return (int)0xf106;
    case jkeyevent_symbol_t::F7:
      return (int)0xf107;
    case jkeyevent_symbol_t::F8:
      return (int)0xf108;
    case jkeyevent_symbol_t::F9:
      return (int)0xf109;
    case jkeyevent_symbol_t::F10:
      return (int)0xf10a;
    case jkeyevent_symbol_t::F11:
      return (int)0xf10b;
    case jkeyevent_symbol_t::F12:
      return (int)0xf10c;
    case jkeyevent_symbol_t::Shift:
      return (int)0xf201;
    case jkeyevent_symbol_t::Control:
      return (int)0xf202;
    case jkeyevent_symbol_t::Alt:
      return (int)0xf204;
    case jkeyevent_symbol_t::AltGr:
      return (int)-1;
    case jkeyevent_symbol_t::Meta:
      return (int)-1;
    case jkeyevent_symbol_t::Super:
      return (int)0xF220;
    case jkeyevent_symbol_t::Hyper:
      return (int)-1;
    default: 
      break;
  }

  return (int)-1;
}

int KeyEvent::GetKeyCode()
{
  return _code;
}

jkeyevent_symbol_t KeyEvent::GetSymbol()
{
  return _symbol;
}

jkeyevent_type_t KeyEvent::GetType()
{
  return _type;
}

jkeyevent_modifiers_t KeyEvent::GetModifiers()
{
  return _mod;
}

}
