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
#include "jcanvas/widgets/jscrollbar.h"
#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jresources.h"

namespace jcanvas {

ScrollBar::ScrollBar()
{
  SetLayout<BorderLayout>();
  SetVertical(false);
  SetInsets({2, 2, 2, 2});
  SetScrollable(false);
  SetFocusable(true);
  SetPreferredSize({128, 32});
  SetBackground(std::make_shared<SolidBackground>());
  SetBorder(std::make_shared<RectangleBorder>());
  
  _previous.RegisterActionListener(this);
  _next.RegisterActionListener(this);
}

ScrollBar::~ScrollBar()
{
  _previous.RemoveActionListener(this);
  _next.RemoveActionListener(this);
}

void ScrollBar::SetValue(float value)
{
  _slider.SetValue(value);
}

float ScrollBar::GetValue()
{
  return _slider.GetValue();
}

void ScrollBar::SetRange(jrange_t range)
{
  _slider.SetRange(range);
}

jrange_t ScrollBar::GetRange()
{
  return _slider.GetRange();
}

void ScrollBar::SetTicks(jrange_t ticks)
{
  _slider.SetTicks(ticks);
}

jrange_t ScrollBar::GetTicks()
{
  return _slider.GetTicks();
}

void ScrollBar::SetVertical(bool vertical)
{
  _slider.SetVertical(vertical);

  if (vertical == false) {
    _previous.SetImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/left-arrow.png"));
    _next.SetImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/right-arrow.png"));

    Add(&_previous, jborderlayout_align_t::West);
    Add(&_next, jborderlayout_align_t::East);
  } else {
    _previous.SetImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/down-arrow.png"));
    _previous.SetPreferredSize(_previous.GetImageComponent()->GetPreferredSize());

    _next.SetImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/up-arrow.png"));
    _next.SetPreferredSize(_next.GetImageComponent()->GetPreferredSize());

    Add(&_previous, jborderlayout_align_t::South);
    Add(&_next, jborderlayout_align_t::North);
  }
  
  // _slider.SetBackground(nullptr);
  _slider.SetBorder(nullptr);
  _slider.SetMetricVisible(false);
  _slider.SetStoneImage(std::make_shared<BufferedImage>(Resources::Path() + "/images/stone.png"));

  _previous.SetBackground(nullptr);
  _previous.SetBorder(nullptr);
  _previous.SetPreferredSize(_previous.GetImageComponent()->GetPreferredSize());

  _next.SetBackground(nullptr);
  _next.SetBorder(nullptr);
  _next.SetPreferredSize(_next.GetImageComponent()->GetPreferredSize());

  Add(&_slider, jborderlayout_align_t::Center);
}

bool ScrollBar::IsVertical()
{
  return _slider.IsVertical();
}

void ScrollBar::ActionPerformed(ActionEvent *event)
{
  Button *button = reinterpret_cast<Button *>(event->GetSource());

  if (button == &_previous) {
    SetValue(GetValue() - GetTicks().min);
  } else {
    SetValue(GetValue() + GetTicks().min);
  }
}

void ScrollBar::RegisterAdjustmentListener(AdjustmentListener *listener)
{
  _slider.RegisterAdjustmentListener(listener);
}

void ScrollBar::RemoveAdjustmentListener(AdjustmentListener *listener)
{
  _slider.RemoveAdjustmentListener(listener);
}

void ScrollBar::DispatchAdjustmentEvent(AdjustmentEvent *event)
{
  _slider.DispatchAdjustmentEvent(event);
}

const std::vector<AdjustmentListener *> & ScrollBar::GetAdjustmentListeners()
{
  return _slider.GetAdjustmentListeners();
}

}
