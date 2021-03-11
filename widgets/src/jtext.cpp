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
#include "jcanvas/widgets/jtext.h"

namespace jcanvas {

Text::Text(std::string text):
   Component()
{
  _text = text;
  _halign = jhorizontal_align_t::Center;
  _valign = jvertical_align_t::Center;
  _is_wrap = false;
}

Text::~Text()
{
}

void Text::UpdatePreferredSize()
{
  jpoint_t<int> 
    t = {
      0, 0
    };

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();

  if (theme.font.primary != nullptr) {
    if (IsWrap() == false) {
      jfont_extends_t extends = theme.font.primary->GetStringExtends(GetText());

      t.x = int(extends.size.x - extends.bearing.x);
      t.y = int(extends.size.y - extends.bearing.y);
    } else {
      std::vector<std::string> lines;

      theme.font.primary->GetStringBreak(&lines, GetText(), {bounds.size.x, INT_MAX});

      t.y = lines.size()*theme.font.primary->GetSize();
    }
  }

  SetPreferredSize(t + jpoint_t<int>{theme.padding.left + theme.padding.right + 2*theme.border.size.x, theme.padding.top + theme.padding.bottom + 2*theme.border.size.y});
}

void Text::SetWrap(bool wrap)
{
  if (_is_wrap == wrap) {
    _is_wrap = wrap;

    UpdatePreferredSize();
    Repaint();
  }
}

bool Text::IsWrap()
{
  return _is_wrap;
}

std::string Text::GetText()
{
  return _text;
}

void Text::SetText(std::string text)
{
  if (_text != text) {
    _text = text;

    UpdatePreferredSize();
    Repaint();
  }
}

void Text::SetHorizontalAlign(jhorizontal_align_t align)
{
  if (_halign != align) {
    _halign = align;

    Repaint();
  }
}

jhorizontal_align_t Text::GetHorizontalAlign()
{
  return _halign;
}

void Text::SetVerticalAlign(jvertical_align_t align)
{
  if (_valign != align) {
    _valign = align;

    Repaint();
  }
}

jvertical_align_t Text::GetVerticalAlign()
{
  return _valign;
}

void Text::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jrect_t<int>
    bounds = GetBounds();

  if (theme.font.primary != nullptr) {
    g->SetFont(theme.font.primary);

    if (IsEnabled() == true) {
      if (HasFocus() == true) {
        g->SetColor(theme.fg.focus);
      } else {
        g->SetColor(theme.fg.normal);
      }
    } else {
      g->SetColor(theme.fg.disable);
    }

    std::string text = GetText();

    if (_is_wrap == false) {
      text = theme.font.primary->TruncateString(text, "...", bounds.size.x);
    }

    g->DrawString(text, theme.padding.bounds(jrect_t<int>{{0, 0}, bounds.size}), _halign, _valign);
  }
}

}
