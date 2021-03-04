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
#include "jcanvas/widgets/jmarquee.h"

#include <thread>

#include <unistd.h>

#define STEP 16

namespace jcanvas {

Marquee::Marquee(std::string text, std::chrono::milliseconds interval):
   Animation(std::chrono::milliseconds(0), interval)
{
  _text = text;
  _position = 0;
}

Marquee::~Marquee()
{
}

void Marquee::SetText(std::string text)
{
  _text = text;
}

std::string Marquee::GetText()
{
  return _text;
}

void Marquee::Update(std::chrono::milliseconds tick)
{
  jtheme_t
    theme = GetTheme();

  jpoint_t<int>
    size = GetSize();
  int 
    width = theme.font.primary->GetStringWidth(_text.c_str());

  _position = _position - STEP;

  if (_position <= -width) {
    _position = size.x;
  }
}

void Marquee::Render(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();

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

    g->DrawString(GetText(), jpoint_t<int>{_position, (size.y - theme.font.primary->GetSize())/2});
  }
}

}
