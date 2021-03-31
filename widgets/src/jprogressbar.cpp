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
#include "jcanvas/widgets/jprogressbar.h"

namespace jcanvas {

ProgressBar::ProgressBar():
  Component()
{
  SetValue(40);
  SetVertical(false);
  SetContinuous(false);
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::Update(std::chrono::milliseconds tick)
{
  if (_is_continuous == false) {
    return;
  }

  std::chrono::duration<float> offset = tick;

  SetValue(GetValue() + 100*offset.count()*0.5f);

  if (GetValue() >= 100.0f) {
    SetValue(0.0f);
  }

  Repaint();
}

void ProgressBar::SetVertical(bool vertical)
{
  _is_vertical = vertical;
}

bool ProgressBar::IsVertical()
{
  return _is_vertical;
}

void ProgressBar::SetContinuous(bool continuous)
{
  _is_continuous = continuous;

  if (_is_continuous == true) {
    Start();
  } else {
    Stop();
  }
}

bool ProgressBar::IsContinuous()
{
  return _is_continuous;
}

float ProgressBar::GetValue()
{
  return _value;
}

void ProgressBar::SetValue(float value)
{
  if (value < 0.0f) {
    value = 0.0f;
  }

  if (value > 100.0f) {
    value = 100.0f;
  }

  _value = value;
}

void ProgressBar::Paint(Graphics *g)
{
  Component::Paint(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();

  g->SetColor(theme.bg.select);

  size.x = size.x - theme.padding.left - theme.padding.right;
  size.y = size.y - theme.padding.top - theme.padding.bottom;

  if (IsVertical() == false) {
    g->FillRectangle({theme.padding.left, theme.padding.top, static_cast<int>(size.x*_value/100.0f), size.y});
  } else {
    g->FillRectangle({theme.padding.left, theme.padding.top + static_cast<int>(size.y*(100.0f - _value)/100.0f), size.x, static_cast<int>(size.y*_value/100)});
  }

  if (_is_continuous == false) {
    g->SetColor(theme.fg.normal);
    g->DrawString(std::to_string(static_cast<int>(_value)) + '%', {0, 0, size}, jhorizontal_align_t::Center, jvertical_align_t::Center);
  }
}

}
