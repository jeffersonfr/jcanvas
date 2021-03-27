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
#include "jcanvas/widgets/jpanel.h"
#include "jcanvas/widgets/jsolidbackground.h"

namespace jcanvas {

Panel::Panel(jrect_t<int> bounds):
  Panel("", bounds)
{
}

Panel::Panel(std::string title, jrect_t<int> bounds):
  Container(bounds)
{
  _title = title;

  jinsets_t<int>
    insets;

  insets.left = 8;
  insets.right = 8;
  insets.top = 64;
  insets.bottom = 8;

  SetInsets(insets);
  SetFocusable(true);
  SetBackground(std::make_shared<SolidBackground>());
}

Panel::~Panel()
{
}

void Panel::SetTitle(std::string title)
{
  _title = title;
}

std::string Panel::GetTitle()
{
  return _title;
}
    
void Panel::Paint(Graphics *g)
{
  Container::Paint(g);
}

void Panel::PaintGlassPane(Graphics *g)
{
  jtheme_t
    theme = GetTheme();
  jpoint_t<int>
    size = GetSize();
  jinsets_t<int>
    insets = GetInsets();

  if (_title != "") {
    g->SetGradientStop(0.0, theme.bg.normal);
    g->SetGradientStop(1.0, theme.scroll.color.normal);
    g->FillLinearGradient({0, 0, size.x, insets.top}, {0, 0}, {0, insets.top});
    g->ResetGradientStop();

    if (theme.font.primary != nullptr) {
      int y = (insets.top - theme.font.primary->GetSize())/2;

      if (y < 0) {
        y = 0;
      }

      std::string text = _title;
      
      // if (_wrap == false) {
        text = theme.font.primary->TruncateString(text, "...", (size.x-insets.left-insets.right));
      // }

      g->SetFont(theme.font.primary);
      g->SetColor(theme.fg.normal);
      g->DrawString(text, jpoint_t<int>{insets.left + (size.x - insets.left - insets.right - theme.font.primary->GetStringWidth(text))/2, y});
    }
  }
}

}
