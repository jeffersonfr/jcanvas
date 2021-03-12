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
#include "jcanvas/widgets/jflowlayout.h"

namespace jcanvas {

FlowLayout::FlowLayout(jflowlayout_align_t align, int hgap, int vgap)
{
  _hgap = hgap;
  _vgap = vgap;

  SetAlign(align);
}

FlowLayout::~FlowLayout()
{
}

jflowlayout_align_t FlowLayout::GetAlign()
{
  return _newalign;
}

int FlowLayout::GetHGap()
{
  return _hgap;
}

int FlowLayout::GetVGap()
{
  return _vgap;
}

bool FlowLayout::GetAlignOnBaseline()
{
    return (bool)_align_on_baseline;
}

void FlowLayout::SetAlign(jflowlayout_align_t align)
{
  _newalign = align;

  switch (align) {
    case jflowlayout_align_t::Leading:
      _align = jflowlayout_align_t::Left;
      break;
    case jflowlayout_align_t::Trailing:
      _align = jflowlayout_align_t::Right;
      break;
    default:
      _align = align;
      break;
  }
}

void FlowLayout::SetHGap(int hgap)
{
  _hgap = hgap;
}

void FlowLayout::SetVGap(int vgap)
{
  _vgap = vgap;
}

void FlowLayout::SetAlignOnBaseline(bool align_on_baseline)
{
    _align_on_baseline = align_on_baseline;
}

int FlowLayout::MoveComponents(std::shared_ptr<Container> target, int x, int y, int width, int height, int rowStart, int rowEnd, bool ltr, bool useBaseline, int *ascent, int *descent)
{
  switch (_newalign) {
    case jflowlayout_align_t::Left:
      x += ltr ? 0 : width;
      break;
    case jflowlayout_align_t::Center:
      x += width / 2;
      break;
    case jflowlayout_align_t::Right:
      x += ltr ? width : 0;
      break;
    case jflowlayout_align_t::Leading:
      break;
    case jflowlayout_align_t::Trailing:
      x += width;
      break;
  }

  int maxAscent = 0,
      nonbaselineHeight = 0,
      baselineOffset = 0;

  if (useBaseline) {
    int maxDescent = 0;

    for (int i = rowStart ; i < rowEnd ; i++) {
      std::shared_ptr<Component> m = target->GetComponents()[i];
      jpoint_t<int> size = m->GetSize();

      if (m->IsVisible() == true) {
        if (ascent[i] >= 0) {
          maxAscent = std::max(maxAscent, ascent[i]);
          maxDescent = std::max(maxDescent, descent[i]);
        } else {
          nonbaselineHeight = std::max(size.y, nonbaselineHeight);
        }
      }
    }

    height = std::max(maxAscent + maxDescent, nonbaselineHeight);

    baselineOffset = (height - maxAscent - maxDescent) / 2;
  }

  for (int i = rowStart ; i < rowEnd ; i++) {
    std::shared_ptr<Component> m = target->GetComponents()[i];
    jpoint_t<int> size = m->GetSize();

    if (m->IsVisible() == true) {
      int cy;

      if (useBaseline && ascent[i] >= 0) {
        cy = y + baselineOffset + maxAscent - ascent[i];
      } else {
        cy = y + (height - size.y) / 2;
      }

      if (ltr) {
        m->SetLocation(x, cy);
      } else {
        jpoint_t<int> size2 = target->GetSize();

        m->SetLocation(size2.x - x - size.x, cy);
      }

      x += size.x + _hgap;
    }
  }

  return height;
}

jpoint_t<int> FlowLayout::GetMinimumLayoutSize(std::shared_ptr<Container> target)
{
  jpoint_t<int> t = {0, 0};

  // WARN:: sync parent
  int 
    nmembers = target->GetComponentCount(),
    maxAscent = 0,
    maxDescent = 0;
  bool 
    useBaseline = GetAlignOnBaseline(),
    firstVisibleComponent = true;

  for (int i = 0 ; i < nmembers ; i++) {
    std::shared_ptr<Component> m = target->GetComponents()[i];

    if (m->IsVisible()) {
      jpoint_t<int> d = m->GetMinimumSize();

      t.y = std::max(t.y, d.y);

      if (firstVisibleComponent) {
        firstVisibleComponent = false;
      } else {
        t.x += _hgap;
      }

      t.x += d.x;

      if (useBaseline) {
        int baseline = m->GetBaseline(d.x, d.y);

        if (baseline >= 0) {
          maxAscent = std::max(maxAscent, baseline);
          maxDescent = std::max(maxDescent, t.y - baseline);
        }
      }
    }
  }

  if (useBaseline) {
    t.y = std::max(maxAscent + maxDescent, t.y);
  }

  jinsets_t insets = target->GetInsets();

  t.x += insets.left + insets.right + _hgap*2;
  t.y += insets.top + insets.bottom + _vgap*2;

  return t;
}

jpoint_t<int> FlowLayout::GetMaximumLayoutSize(std::shared_ptr<Container> target)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> FlowLayout::GetPreferredLayoutSize(std::shared_ptr<Container> target)
{
  jpoint_t<int> t = {0, 0};

  // WARN:: sync parent
  int 
    nmembers = target->GetComponentCount(),
    maxAscent = 0,
    maxDescent = 0;
  bool 
    firstVisibleComponent = true,
    useBaseline = GetAlignOnBaseline();

  for (int i = 0 ; i < nmembers ; i++) {
    std::shared_ptr<Component> m = target->GetComponents()[i];

    if (m->IsVisible()) {
      jpoint_t<int> d = m->GetPreferredSize();

      t.y = std::max(t.y, d.y);

      if (firstVisibleComponent) {
        firstVisibleComponent = false;
      } else {
        t.x += _hgap;
      }

      t.x += d.x;

      if (useBaseline) {
        int baseline = m->GetBaseline(d.x, d.y);
        if (baseline >= 0) {
          maxAscent = std::max(maxAscent, baseline);
          maxDescent = std::max(maxDescent, d.y - baseline);
        }
      }
    }
  }

  if (useBaseline) {
    t.y = std::max(maxAscent + maxDescent, t.y);
  }

  jinsets_t insets = target->GetInsets();

  t.x += insets.left + insets.right + _hgap*2;
  t.y += insets.top + insets.bottom + _vgap*2;
  
  return t;
}

void FlowLayout::DoLayout(std::shared_ptr<Container> target)
{
  // WARN:: syn with jframe
  jinsets_t 
    insets = target->GetInsets();
  jpoint_t<int> 
    size = target->GetSize();
  int 
    maxwidth = size.x - (insets.left + insets.right + _hgap*2),
    nmembers = target->GetComponentCount();
  int
    x = 0, 
    y = insets.top + _vgap;
  int
    rowh = 0, 
    start = 0;
  int 
    *ascent = nullptr,
    *descent = nullptr;
  bool 
    ltr = (target->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight),
    useBaseline = GetAlignOnBaseline();

  if (useBaseline) {
    ascent = new int[nmembers];
    descent = new int[nmembers];
  }

  for (int i = 0 ; i < nmembers ; i++) {
    std::shared_ptr<Component> m = target->GetComponents()[i];

    if (m->IsVisible() == true) {
      jpoint_t<int> psize = m->GetPreferredSize();

      m->SetSize(psize.x, psize.y);

      if (useBaseline) {
        int baseline = m->GetBaseline(psize.x, psize.y);

        if (baseline >= 0) {
          ascent[i] = baseline;
          descent[i] = psize.y - baseline;
        } else {
          ascent[i] = -1;
        }
      }

      if ((x == 0) || ((x + psize.x) <= maxwidth)) {
        if (x > 0) {
          x += _hgap;
        }
        x += psize.x;

        rowh = std::max(rowh, psize.y);
      } else {
        rowh = MoveComponents(target, insets.left + _hgap, y, maxwidth - x, rowh, start, i, ltr, useBaseline, ascent, descent);
        x = psize.x;
        y += _vgap + rowh;
        rowh = psize.y;
        start = i;
      }
    }
  }

  MoveComponents(target, insets.left + _hgap, y, maxwidth - x, rowh, start, nmembers, ltr, useBaseline, ascent, descent);

  if (ascent != nullptr) {
    delete ascent;
  }

  if (descent != nullptr) {
    delete descent;
  }
}

}

