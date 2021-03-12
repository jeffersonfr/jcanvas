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
#include "jcanvas/widgets/jtextarea.h"

#include "jmixin/jstring.h"

namespace jcanvas {

TextArea::TextArea():
  TextComponent()
{
  _rows_string = true;
  _valign = jvertical_align_t::Top;
  _rows_gap = 0;
  _current_row = 0;
  _is_wrap = true;

  SetFocusable(true);
}

TextArea::~TextArea()
{
}

int TextArea::GetRowsGap()
{
  return _rows_gap;
}

void TextArea::SetRowsGap(int gap)
{
  _rows_gap = gap;

  Repaint();
}

void TextArea::SetWrap(bool b)
{
  if (b != _is_wrap) {
    _is_wrap = b;

    Repaint();
  }
}

bool TextArea::IsWrap()
{
  return _is_wrap;
}

void TextArea::SetEchoChar(char echo_char)
{
  _rows_string = true;

  TextComponent::SetEchoChar(echo_char);
}

void TextArea::SetText(std::string text)
{
  _rows_string = true;

  TextComponent::SetText(text);
}
  
void TextArea::Insert(std::string text)
{
  _rows_string = true;

  TextComponent::Insert(text);
}

void TextArea::Delete()
{
  _rows_string = true;

  TextComponent::Delete();
}

bool TextArea::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEditable() == false) {
    return false;
  }

  // TODO:: usar scroll para exibir o texto
  KeyMap *
    keymap = GetKeyMap();
  jkeyevent_symbol_t 
    action = event->GetSymbol();
  bool 
    catched = false;

  if (keymap != nullptr && keymap->HasKey(action) == false) {
    return false;
  }

  if (action == jkeyevent_symbol_t::CursorLeft) {
    DecrementCaretPosition(1);

    catched = true;
  } else if (action == jkeyevent_symbol_t::CursorRight) {
    IncrementCaretPosition(1);

    catched = true;
  } else if (action == jkeyevent_symbol_t::CursorUp) {
    IncrementLines(1);

    catched = true;
  } else if (action == jkeyevent_symbol_t::PageUp) {
    // TODO:: IncrementLines((_size.y-2*(bordersize+_vertical_gap))/(font->GetSize()+_rows_gap));

    catched = true;
  } else if (action == jkeyevent_symbol_t::CursorDown) {
    DecrementLines(1);

    catched = true;
  } else if (action == jkeyevent_symbol_t::PageDown) {
    // TODO:: DecrementLines((_size.y-2*(bordersize+_vertical_gap))/(font->GetSize()+_rows_gap));

    catched = true;
  } else if (action == jkeyevent_symbol_t::Home) {
    _caret_position = 0;

    IncrementLines(_lines.size());
    
    catched = true;
  } else if (action == jkeyevent_symbol_t::End) {
    _caret_position = _text.size();
    
    DecrementLines(_lines.size());
    
    catched = true;
  } else {
    int code = KeyEvent::GetCodeFromSymbol(action);

    if (code > 0x00 and code < 0xff) {
      Insert(std::string(1, code));
    }
  }

  /* TODO::
  if (font != nullptr) {
    int w = font->GetStringWidth(GetText().substr(0, _caret_position));

    if ((w-_size.x) > 0) {
      SetScrollX(w - _size.x);
    }
  }
  */

  return catched;
}


bool TextArea::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return true;
}

bool TextArea::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return true;
}

bool TextArea::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return true;
}

bool TextArea::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  return true;
}

void TextArea::IncrementLines(int lines)
{
  if (_lines.size() == 0) {
    return;
  }

  jtheme_t
    theme = GetTheme();
  int 
    current_length = 0;

  _current_row = _current_row - lines;

  if (_current_row < 0) {
    _current_row = 0;
  }

  // INFO:: define a nova posicao do caret
  for (int i=0; i<_current_row; i++) {
    current_length += _lines[i].size();
  }

  _caret_position = current_length;

  if (theme.font.primary != nullptr) {
    jpoint_t 
      slocation = GetScrollLocation();
    int 
      fs = theme.font.primary->GetSize();

    if (slocation.y > 0) {
      SetScrollLocation(slocation.x, (std::max)(0, (fs+_rows_gap)*_current_row));
    }
  }

  Repaint();
}

void TextArea::DecrementLines(int lines)
{
  if (_lines.size() == 0) {
    return;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();
  int 
    current_length = 0;

  _current_row = _current_row + lines;

  if (_current_row >= (int)(_lines.size())) {
    _current_row = _lines.size()-1;
  }

  // INFO:: define a nova posicao do caret
  for (int i=0; i<_current_row; i++) {
    current_length += _lines[i].size();
  }

  _caret_position = current_length;

  if (theme.font.primary != nullptr) {
    jpoint_t 
      slocation = GetScrollLocation();
    int 
      fs = theme.font.primary->GetSize();

    if ((slocation.y + size.y) < (theme.font.primary->GetSize() + _rows_gap)*GetRows()) {
      SetScrollLocation(slocation.x, (std::max)(0, (fs + _rows_gap)*_current_row));
    }
  }

  Repaint();
}

void TextArea::InitRowsString()
{
  jtheme_t
    theme = GetTheme();

  if (theme.font.primary == nullptr) {
    return;
  }

  jpoint_t<int>
    size = GetSize();
  int
    w = size.x - theme.padding.left - theme.padding.right;

  std::string 
    text = _text;

  _lines.clear();

  if (EchoCharIsSet() == true) {
    text = text.replace(text.begin(), text.end(), text.size(), _echo_char);
  }

  if (_is_wrap == false) {
    _lines.push_back(jmixin::String(text).replace("\n", " ") + " ");

    return;
  }

  std::vector<std::string> 
    lines;

  jmixin::String(text)
    .split("\n")
    .for_each([&](auto const &token) {
          std::vector<std::string> words;
          std::string line = token + "\n";

          jmixin::String(line)
            .split(" ")
            .for_each([&](auto const &word) {
                  words.push_back(word);
                });

          std::string temp, previous;

          temp = words[0];

          for (int j=1; j<(int)words.size(); j++) {
            previous = temp;
            temp = temp + " " + words[j];

            if (theme.font.primary->GetStringWidth(temp.c_str()) > w) {
              temp = words[j];

              _lines.push_back(previous);
            }
          }

          _lines.push_back(temp);
        });

  int length = _caret_position;

  for (int i=0; i<=(int)_lines.size()-1; i++) {
    std::string line = _lines[i];
    int size = (int)line.size();

    if (length >= size) {
      length -= size;
    } else {
      _current_row = i;
      
      break;
    }
  }

  _rows_string = false;
}

const std::vector<std::string> & TextArea::GetLines()
{
  return _lines;
}

void TextArea::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t 
    slocation = GetScrollLocation();

  InitRowsString();

  if (theme.font.primary == nullptr) {
    return;
  }

  g->SetFont(theme.font.primary);

  int current_text_size,
      current_length = _caret_position,
      fs = theme.font.primary->GetSize()+_rows_gap;

  theme.padding.left = theme.padding.left - slocation.x;
  theme.padding.top = theme.padding.top - slocation.y;

  // INFO:: Draw text
  for (int i=0, k=0; i<=(int)_lines.size()-1; i++) {
    std::string s = _lines[i];

    char *c = (char *)strchr(s.c_str(), '\n');

    if (c != nullptr) {
      c[0] = ' ';
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

    g->DrawString(s, jpoint_t<int>{theme.padding.left, theme.padding.top+k*fs});

    if (_caret_visible == true && current_length < (int)s.size() && current_length >= 0) {
      std::string cursor;

      if (_caret_type == jcaret_type_t::Underscore) {
        cursor = "_";
      } else if (_caret_type == jcaret_type_t::Stick) {
        cursor = "|";
      } else if (_caret_type == jcaret_type_t::Block) {
        cursor = "?";
      }

      current_text_size = theme.font.primary->GetStringWidth(_lines[i].substr(0, current_length).c_str());

      if (HasFocus() == true && IsEditable() == true) {
        g->SetColor(_caret_color);
      }

      g->DrawString(cursor, jpoint_t<int>{theme.padding.left+current_text_size, theme.padding.top+k*fs});

      current_length = -1;
    }

    k++;

    if (current_length >= (int)s.size()) {
      current_length -= s.size();
    }
  }
}

std::string TextArea::GetLineAt(int row)
{
  if (row < 0) {
    return "";
  }

  if (row >= (int)_lines.size()) {
    return "";
  }

  return _lines[row];
}

int TextArea::GetRows()
{
  return _lines.size();
}

void TextArea::SetCurrentRow(int row)
{
  int size = _lines.size();

  if (size == 0) {
    row = 0;
  } else {

    if (row < 0) {
      row = 0;
    }

    if (row >= size) {
      row = size-1;
    }
  }

  _current_row = row;

  Repaint();
}

int TextArea::GetCurrentRow()
{
  return _current_row;
}

jpoint_t<int> TextArea::GetScrollDimension()
{
  jtheme_t
    theme = GetTheme();
  jrect_t<int> 
    bounds = GetBounds();

  if (theme.font.primary == nullptr) {
    return bounds.size;
  }

  if (_is_wrap == false) {
    bounds.size.x = theme.font.primary->GetStringWidth(GetText());
  } else {
    bounds.size.y = GetRows()*(theme.font.primary->GetSize()) + theme.padding.top + theme.padding.bottom;
  }

  return  bounds.size;
}

}

