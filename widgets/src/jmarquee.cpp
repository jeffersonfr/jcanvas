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
#include "jcanvas/widgets/jtheme.h"

#include <thread>

#include <unistd.h>

#define STEP 16

namespace jcanvas {

Marquee::Marquee(std::string text):
   Animation()
{
  SetText(text);
}

Marquee::~Marquee()
{
}

void Marquee::SetBounceEnabled(bool enabled)
{
  _is_bounce_enabled = enabled;

  if (_is_bounce_enabled == false) {
    _direction = -std::abs(_direction);
  }
}

bool Marquee::IsBounceEnabled()
{
  return _is_bounce_enabled;
}

void Marquee::SetRatio(float ratio)
{
  _ratio = ratio;
}

float Marquee::GetRatio()
{
  return _ratio;
}

void Marquee::SetText(std::string text)
{
  jtheme_t theme = GetTheme();

  _text = text;
  _text_size = theme.font.primary->GetStringWidth(_text);
}

std::string Marquee::GetText()
{
  return _text;
}

void Marquee::Update(std::chrono::milliseconds tick)
{
  jpoint_t<int> size = GetSize();
  std::chrono::duration<float> duration = tick;

  _position = _position + _direction*size.x*duration.count()*GetRatio();

  if (_is_bounce_enabled == false) {
    if (_position <= -_text_size) {
      _position = size.x;
    }
  } else {
    if (_position < 0) {
      _position = 0;
      _direction = -_direction;
    }
    
    if (_position > (size.x - _text_size)) {
      _position = size.x - _text_size;
      _direction = -_direction;
    }
  }

  Repaint();
}

void Marquee::Paint(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();

  if (theme.font.primary != nullptr) {
    g->SetFont(theme.font.primary);
    g->SetColor(theme.fg.normal);
    g->DrawString(GetText(), jpoint_t<int>{_position, (size.y - theme.font.primary->GetSize())/2});
  }
}

}
