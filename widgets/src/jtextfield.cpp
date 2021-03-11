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
#include "jcanvas/widgets/jtextfield.h"

#include "jmixin/jstring.h"

namespace jcanvas {

TextField::TextField():
  TextComponent()
{
  SetFocusable(true);
}

TextField::~TextField()
{
}

bool TextField::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEditable() == false) {
    return false;
  }

  KeyMap 
    *keymap = GetKeyMap();
  // jpoint_t<int>
  //   size = GetSize();
  jkeyevent_symbol_t 
    action = event->GetSymbol();

  if (keymap != nullptr && keymap->HasKey(action) == false) {
    return false;
  }

  bool catched = false;

  if (action == jkeyevent_symbol_t::CursorLeft) {
    DecrementCaretPosition(1);
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::CursorRight) {
    IncrementCaretPosition(1);
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::Home) {
    _caret_position = 0;

    Repaint();
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::End) {
    _caret_position = _text.size();

    Repaint();
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::Backspace) {
    Backspace();
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::Delete) {
    Delete();
    
    catched = true;
  } else {
    std::string s;

    catched = true;

    int code = KeyEvent::GetCodeFromSymbol(action);

    if (code > 0x00 and code < 0xff) {
      Insert(std::string(1, code));
      
      // _caret_position = _caret_position + 1;
    }
  }

  return catched;
}

bool TextField::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return true;
}

bool TextField::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}

bool TextField::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool TextField::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  return false;
}

void TextField::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  
  jrect_t<int>
    bounds = GetBounds();
  int
    w = bounds.size.x - theme.padding.left - theme.padding.right,
    h = bounds.size.y - theme.padding.top - theme.padding.bottom;
  std::string 
    paint_text = _text,
    text = paint_text,
    cursor,
    previous,
    temp;
  int 
    caret_size = 0,
    current_text_size;

  text = jmixin::String(text).replace("\t", "    ").replace("\n", "").replace("\x8", "");

  if (EchoCharIsSet() == true) {
    paint_text = paint_text.replace(paint_text.begin(), paint_text.end(), paint_text.size(), _echo_char);
  }

  if (IsEnabled() == true) {
    if (HasFocus() == true) {
      g->SetColor(theme.fg.focus);
    } else {
      g->SetColor(theme.fg.normal);
    }
  } else {
    g->SetColor(theme.fg.disable);
  }

  current_text_size = 0;

  if (theme.font.primary != nullptr) {
    g->SetFont(theme.font.primary);

    if (_caret_visible == true) {
      if (_caret_type == jcaret_type_t::Underscore) {
        cursor = "_";
      } else if (_caret_type == jcaret_type_t::Stick) {
        cursor = "|";
      } else if (_caret_type == jcaret_type_t::Block) {
        cursor = "?";
      }

      caret_size = theme.font.primary->GetStringWidth(cursor);
    }

    current_text_size = theme.font.primary->GetStringWidth(text.substr(0, _caret_position));
  }

  int offset = 0;

  if (w > 0 && h > 0) { 
    if (current_text_size > (w-caret_size)) {
      int count = 0;

      do {
        count++;

        current_text_size = theme.font.primary->GetStringWidth(text.substr(_caret_position-count, count));
      } while (current_text_size < (w-caret_size));

      count = count-1;
      text = text.substr(_caret_position-count, count);
      current_text_size = theme.font.primary->GetStringWidth(text);
      offset = (w-current_text_size-caret_size)-caret_size;

      if (_caret_position < (int)paint_text.size()) {
        text = text + paint_text[_caret_position];
      }
    } else {
      int count = 1;

      do {
        current_text_size = theme.font.primary->GetStringWidth(text.substr(0, count));

        if (count++ > (int)paint_text.size()) {
          break;
        }
      } while (current_text_size < (w-caret_size));

      count = count-1;

      text = text.substr(0, count);

      if (_halign == jhorizontal_align_t::Left) {
        offset = 0;
      } else if (_halign == jhorizontal_align_t::Center) {
        offset = (w-current_text_size)/2;
      } else if (_halign == jhorizontal_align_t::Right) {
        offset = w-current_text_size;
      } else if (_halign == jhorizontal_align_t::Justify) {
        offset = 0;
      }

      current_text_size = theme.font.primary->GetStringWidth(text.substr(0, _caret_position));
    }

    g->DrawString(text, {theme.padding.left + offset, theme.padding.top, w, h}, jhorizontal_align_t::Left, _valign);

    if (_caret_visible == true) {
      if (HasFocus() == true && IsEditable() == true) {
        g->SetColor(_caret_color);
      }

      g->DrawString(cursor, {theme.padding.left + current_text_size + offset, theme.padding.top, w, h}, jhorizontal_align_t::Left, _valign);
    }
  }
}

}
