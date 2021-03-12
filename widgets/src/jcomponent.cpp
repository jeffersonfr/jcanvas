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
#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jframe.h"

#include <algorithm>

namespace jcanvas {

Component::Component(jrect_t<int> bounds):
  KeyListener(),
  MouseListener()
{
  _preferred_size = {
    .x = 128, 
    .y = 32
  };
  
  _minimum_size = {
    .x = 16, 
    .y = 16
  };
  
  _maximum_size = {
    .x = 16384, 
    .y = 16384
  };

  _is_cyclic_focus = false;
  _is_navigation_enabled = true;
  _is_background_visible = true;
  _is_focusable = false;
  _is_enabled = true;
  _is_visible = true;
  _is_ignore_repaint = false;
  
  _keymap = nullptr;
  _parent = nullptr;
  _left = nullptr;
  _right = nullptr;
  _up = nullptr;
  _down = nullptr;
  _gradient_level = 0x40;
  _alignment_x = jcomponent_alignment_t::Center;
  _alignment_y = jcomponent_alignment_t::Center;

  _location = bounds.point;
  _size = bounds.size;
  _scroll_location = {0, 0};

  _is_scrollable_x = true;
  _is_scrollable_y = true;

  _is_scroll_visible = true;

  _scroll_minor_increment = 8;
  _scroll_major_increment = 64;

  _component_state = 0;

  _relative_mouse = {
    .x = 0,
    .y = 0
  };

  jtheme_t
    &theme = GetTheme();

  theme.padding = {
    .left = 2,
    .top = 2,
    .right = 2,
    .bottom = 2
  };
}

Component::~Component()
{
}

void Component::ScrollToVisibleArea(jrect_t<int> rect, std::shared_ptr<Component> coordinateSpace) 
{
  if (IsScrollable()) {
    jpoint_t<int> 
      slocation = GetScrollLocation();
    jpoint_t<int> 
      size = GetSize();
    jrect_t<int> 
      view;
    int 
      scrollPosition = slocation.y;

    // if (IsSmoothScrolling()) {
      view.point = slocation;
      view.size = size;
    // }

    int relativeX = rect.point.x;
    int relativeY = rect.point.y;
    
    // component needs to be in absolute coordinates ...
    std::shared_ptr<Container> parent = nullptr;

    if (coordinateSpace != nullptr) {
      parent = coordinateSpace->GetParent();
    }

    if (parent == GetSharedPointer<Container>()) {
      if (view.Contains(rect) == true) {
        return;
      }
    } else {
      while (parent != GetSharedPointer<Container>()) {
        // mostly a special case for list
        if (parent == nullptr) {
          relativeX = rect.point.x;
          relativeY = rect.point.y;

          break;
        }
        
        jpoint_t<int> t = parent->GetLocation();

        relativeX += t.x;
        relativeY += t.y;

        parent = parent->GetParent();
      }

      if (view.Contains(jrect_t<int>{relativeX, relativeY, rect.size.x, rect.size.y}) == true) {
        return;
      }
    }

    jpoint_t<int>
      nslocation = slocation;

    if (IsScrollableX()) {
      int 
        rightX = relativeX + rect.size.x; // - s.getPadding(LEFT) - s.getPadding(RIGHT);

      if (slocation.x > relativeX) {
        nslocation.x = relativeX;
      }

      if (slocation.x + size.x < rightX) {
        nslocation.x = slocation.x + (rightX - (slocation.x + size.x));
      } else {
        if (slocation.x > relativeX) {
          nslocation.x = relativeX;
        }
      }
    }

    if (IsScrollableY()) {
      int 
        bottomY = relativeY + rect.size.y; // - s.getPadding(TOP) - s.getPadding(BOTTOM);

      if (slocation.y > relativeY) {
        scrollPosition = relativeY;
      }

      if (slocation.y + size.y < bottomY) {
        scrollPosition = slocation.y + (bottomY - (slocation.y + size.y));
      } else {
        if (slocation.y > relativeY)
          scrollPosition = relativeY;
      }

      if (IsSmoothScrolling()) {
        // initialScrollY = slocation.y;
        // destScrollY = scrollPosition;
        // initScrollMotion();
        nslocation.y = scrollPosition;
      } else {
        nslocation.y = scrollPosition;
      }
    }

    SetScrollLocation(nslocation);
  } else {
    // try to move parent scroll if you are not scrollable
    std::shared_ptr<Container> parent = GetParent();

    if (parent != nullptr) {
      parent->ScrollToVisibleArea(
          {GetAbsoluteLocation().x - parent->GetAbsoluteLocation().x + rect.point.x, GetAbsoluteLocation().y - parent->GetAbsoluteLocation().y + rect.point.y, rect.size.x, rect.size.y}, parent);
    }
  }
}

void Component::SetName(std::string name)
{
  _name = name;
}

std::string Component::GetName()
{
  return _name;
}

jtheme_t & Component::GetTheme()
{
  return _theme;
}

void Component::SetTheme(jtheme_t theme)
{
  _theme = theme;
}

void Component::SetKeyMap(KeyMap *keymap)
{
  _keymap = keymap;
}

KeyMap * Component::GetKeyMap()
{
  return _keymap;
}

void Component::SetCyclicFocus(bool b)
{
  _is_cyclic_focus = b;
}

bool Component::IsCyclicFocus()
{
  return _is_cyclic_focus;
}

bool Component::IsOpaque()
{
  jtheme_t
    theme = GetTheme();

  return (IsBackgroundVisible() == true) && ((theme.bg.normal & 0xff000000) == 0xff000000);
}

int Component::GetBaseline(int width, int height)
{
  return -1;
}

jcomponent_behavior_t Component::GetBaselineResizeBehavior() 
{
  return jcomponent_behavior_t::Other;
}

jcomponent_alignment_t Component::GetAlignmentX()
{
  return _alignment_x;
}

jcomponent_alignment_t Component::GetAlignmentY()
{
  return _alignment_y;
}

void Component::SetAlignmentX(jcomponent_alignment_t align)
{
  _alignment_x = align;
}

void Component::SetAlignmentY(jcomponent_alignment_t align)
{
  _alignment_y = align;
}

void Component::SetComponentOrientation(jcomponent_orientation_t orientation)
{
  _orientation = orientation;
}

jcomponent_orientation_t Component::GetComponentOrientation()
{
  return _orientation;
}

bool Component::IsScrollableX()
{
  jpoint_t<int>
    size = GetSize();

  return (_is_scrollable_x == true) && (GetScrollDimension().x > size.x);
}

bool Component::IsScrollableY()
{
  jpoint_t<int>
    size = GetSize();

  return (_is_scrollable_y == true) && (GetScrollDimension().y > size.y);
}

bool Component::IsScrollable()
{
  return (IsScrollableX() == true || IsScrollableY() == true);
}

bool Component::IsScrollVisible()
{
  return _is_scroll_visible;
}

void Component::SetScrollableX(bool scrollable)
{
  _is_scrollable_x = scrollable;
}

void Component::SetScrollableY(bool scrollable)
{
  _is_scrollable_y = scrollable;
}

void Component::SetScrollable(bool scrollable)
{
  _is_scrollable_x = scrollable;
  _is_scrollable_y = scrollable;
}

void Component::SetSmoothScrolling(bool smooth)
{
  _is_smooth_scroll = smooth;
}

bool Component::IsSmoothScrolling()
{
  return _is_smooth_scroll;
}

jpoint_t<int> Component::GetScrollLocation()
{
  jpoint_t<int>
    location = _scroll_location;

  if (IsScrollableX() == false) {
    location.x = 0;
  }
  
  if (IsScrollableY() == false) {
    location.y = 0;
  }

  return location;
}

jpoint_t<int> Component::GetScrollDimension()
{
  return GetSize();
}

jrect_t<int> Component::GetBounds()
{
  return {_location, _size};
}

jrect_t<int> Component::GetVisibleBounds()
{
  return GetBounds();
}

void Component::SetScrollLocation(int x, int y)
{
  jpoint_t<int>
    size = GetSize(),
    sdimention = GetScrollDimension();
  int 
    diffx = sdimention.x  - size.x,
    diffy = sdimention.y - size.y;

  _scroll_location.x = x;

  if (x < 0 || diffx < 0) {
    _scroll_location.x = 0;
  } else {
    if (_scroll_location.x > diffx) {
      _scroll_location.x = diffx;
    }
  }

  _scroll_location.y = y;

  if (y < 0 || diffy < 0) {
    _scroll_location.y = 0;
  } else {
    if (_scroll_location.y > diffy) {
      _scroll_location.y = diffy;
    }
  }

  Repaint();
}

void Component::SetScrollLocation(jpoint_t<int> t)
{
  SetScrollLocation(t.x, t.y);
}

int Component::GetMinorScrollIncrement()
{
  return _scroll_minor_increment;
}

int Component::GetMajorScrollIncrement()
{
  return _scroll_major_increment;
}

void Component::SetMinorScrollIncrement(int increment)
{
  _scroll_minor_increment = increment;
}

void Component::SetMajorScrollIncrement(int increment)
{
  _scroll_major_increment = increment;
}

void Component::PaintScrollbars(Graphics *g)
{
  if (IsScrollable() == false) {
    return;
  }

  jtheme_t
    theme = GetTheme();
  jpoint_t<int> 
    slocation = GetScrollLocation();
  jpoint_t<int>
    size = GetSize(),
    sdimention = GetScrollDimension();

  if (IsScrollableX() == true) {
    double 
      offset_ratio = (double)slocation.x/(double)sdimention.x,
      block_size_ratio = (double)size.x/(double)sdimention.x;
    int 
      offset = (int)(size.x*offset_ratio),
      block_size = (int)(size.x*block_size_ratio);

    g->SetColor(theme.fg.normal);
    g->FillRectangle({0, size.y - theme.scroll.size.y, size.x, theme.scroll.size.y});
    g->SetGradientStop(0.0, theme.fg.normal);
    g->SetGradientStop(1.0, theme.bg.normal);
    g->FillLinearGradient({offset, size.y - theme.scroll.size.y, block_size, theme.scroll.size.y}, {0, 0}, {0, theme.scroll.size.y});
    g->ResetGradientStop();
  }
  
  if (IsScrollableY() == true) {
    double 
      offset_ratio = (double)slocation.y/(double)sdimention.y,
      block_size_ratio = (double)size.y/(double)sdimention.y;
    int 
      offset = (int)(size.y*offset_ratio),
      block_size = (int)(size.y*block_size_ratio);

    g->SetColor(theme.fg.normal);
    g->FillRectangle({size.x - theme.scroll.size.x, 0, theme.scroll.size.x, size.y});

    g->SetGradientStop(0.0, theme.fg.normal);
    g->SetGradientStop(1.0, theme.bg.normal);
    g->FillLinearGradient({size.x - theme.scroll.size.x, offset, theme.scroll.size.x, block_size}, {0, 0}, {theme.scroll.size.x, 0});
    g->ResetGradientStop();
  }

  if (IsScrollableX() == true && IsScrollableY() == true) {
    int radius = std::min(theme.scroll.size.x, theme.scroll.size.y);
    int radius2 = radius/2;

    g->SetGradientStop(0.0, theme.bg.normal);
    g->SetGradientStop(1.0, theme.fg.normal);
    g->FillRadialGradient({size.x-radius2, size.y-radius2}, {radius, radius}, {0, 0}, 0);
    g->ResetGradientStop();
  }

  /*
  jpen_t 
    pen = g->GetPen();
  int 
    width = pen.width;

  pen.width = -border.size.x;
  g->SetPen(pen);

  g->DrawRectangle({0, 0, size.x, size.y});

  pen.width = width;

  g->SetPen(pen);
  */
}

void Component::PaintBackground(Graphics *g)
{
  if (IsBackgroundVisible() == false) {
    return;
  }
  
  jtheme_t
    theme = GetTheme();

  if (IsEnabled() == true) {
    if (HasFocus() == true) {
      g->SetColor(theme.bg.focus);
    } else {
      g->SetColor(theme.bg.normal);
    }
  } else {
    g->SetColor(theme.bg.disable);
  }

  if (theme.border.type == jtheme_border_t::style::Round) {
    g->FillRoundRectangle({0, 0, GetSize()});
  } else if (theme.border.type == jtheme_border_t::style::Bevel) {
    g->FillBevelRectangle({0, 0, GetSize()});
  } else {
    g->FillRectangle({0, 0, GetSize()});
  }
}

void Component::PaintBorders(Graphics *g)
{
  jtheme_border_t
    border = GetTheme().border;

  if (border.type == jtheme_border_t::style::Empty) {
    return;
  }

  jcolor_t<float>
    color,
    bordercolor = border.color.normal,
    borderfocus = border.color.focus,
    borderdisable = border.color.disable;
  jpoint_t<int>
    size = GetSize();
  int 
    xp = 0, 
    yp = 0,
    wp = size.x,
    hp = size.y;
  int 
    step = 0x20;

  if (IsEnabled() == true) {
    if (HasFocus() == true) {
      color = borderfocus;
    } else {
      color = bordercolor;
    }
  } else {
    color = borderdisable;
  }

  int 
    dr = color[2],
    dg = color[1],
    db = color[0],
    da = color[3];
  jpen_t 
    pen = g->GetPen();
  int 
    width = pen.width;

  if (border.type == jtheme_border_t::style::Line) {
    g->SetColor({dr, dg, db, da});
    pen.width = -border.size.x;
    g->SetPen(pen);
    g->DrawRectangle({xp, yp, wp, hp});
  } else if (border.type == jtheme_border_t::style::Bevel) {
    g->SetColor({dr, dg, db, da});
    pen.width = -border.size.x;
    g->SetPen(pen);
    g->DrawBevelRectangle({xp, yp, wp, hp});
  } else if (border.type == jtheme_border_t::style::Round) {
    g->SetColor({dr, dg, db, da});
    pen.width = -border.size.x;
    g->SetPen(pen);
    g->DrawRoundRectangle({xp, yp, wp, hp});
  } else if (border.type == jtheme_border_t::style::RaisedGradient) {
    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr+step*(border.size.x-i), dg+step*(border.size.x-i), db+step*(border.size.x-i)});
      g->DrawLine({xp+i, yp+i}, {xp+wp-i, yp+i}); //cima
      g->SetColor({dr-step*(border.size.x-i), dg-step*(border.size.x-i), db-step*(border.size.x-i)});
      g->DrawLine({xp+i, yp+hp-i}, {xp+wp-i, yp+hp-i}); //baixo
    }

    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr+step*(border.size.x-i), dg+step*(border.size.x-i), db+step*(border.size.x-i)});
      g->DrawLine({xp+i, yp+i}, {xp+i, yp+hp-i}); //esquerda
      g->SetColor({dr-step*(border.size.x-i), dg-step*(border.size.x-i), db-step*(border.size.x-i)});
      g->DrawLine({xp+wp-i, yp+i}, {xp+wp-i, yp+hp-i}); //direita
    }
  } else if (border.type == jtheme_border_t::style::LoweredGradient) {
    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr-step*(border.size.x-i), dg-step*(border.size.x-i), db-step*(border.size.x-i)});
      g->DrawLine({xp+i, yp+i}, {xp+wp-i, yp+i}); //cima
      g->SetColor({dr+step*(border.size.x-i), dg+step*(border.size.x-i), db+step*(border.size.x-i)});
      g->DrawLine({xp+i, yp+hp-i}, {xp+wp-i, yp+hp-i}); //baixo
    }

    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr-step*(border.size.x-i), dg-step*(border.size.x-i), db-step*(border.size.x-i)});
      g->DrawLine({xp+i, yp+i}, {xp+i, yp+hp-i}); //esquerda
      g->SetColor({dr+step*(border.size.x-i), dg+step*(border.size.x-i), db+step*(border.size.x-i)});
      g->DrawLine({xp+wp-i, yp+i}, {xp+wp-i, yp+hp-i}); //direita
    }
  } else if (border.type == jtheme_border_t::style::RaisedBevel) {
    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr+step, dg+step, db+step});
      g->DrawLine({xp+i, yp+i}, {xp+wp-i, yp+i}); //cima
      g->SetColor({dr-step, dg-step, db-step});
      g->DrawLine({xp+i, yp+hp-i}, {xp+wp-i, yp+hp-i}); //baixo
    }

    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr+step, dg+step, db+step});
      g->DrawLine({xp+i, yp+i}, {xp+i, yp+hp-i}); //esquerda
      g->SetColor({dr-step, dg-step, db-step});
      g->DrawLine({xp+wp-i, yp+i}, {xp+wp-i, yp+hp-i}); //direita
    }
  } else if (border.type == jtheme_border_t::style::LoweredBevel) {
    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr-step, dg-step, db-step});
      g->DrawLine({xp+i, yp+i}, {xp+wp-i, yp+i}); //cima
      g->SetColor({dr+step, dg+step, db+step});
      g->DrawLine({xp+i, yp+hp-i}, {xp+wp-i, yp+hp-i}); //baixo
    }

    for (int i=0; i<border.size.x && i<wp && i<hp; i++) {
      g->SetColor({dr-step, dg-step, db-step});
      g->DrawLine({xp+i, yp+i}, {xp+i, yp+hp-i}); //esquerda
      g->SetColor({dr+step, dg+step, db+step});
      g->DrawLine({xp+wp-i, yp+i}, {xp+wp-i, yp+hp-i}); //direita
    }
  } else if (border.type == jtheme_border_t::style::RaisedEtched) {
    g->SetColor({dr+step, dg+step, db+step, da});
    pen.width = -border.size.x;
    g->SetPen(pen);
    g->DrawRectangle({xp, yp, wp, hp});
    
    g->SetColor({dr-step, dg-step, db-step, da});
    pen.width = -border.size.x/2;
    g->SetPen(pen);
    g->DrawRectangle({xp, yp, wp-border.size.x/2, hp-border.size.x/2});
  } else if (border.type == jtheme_border_t::style::LoweredEtched) {
    g->SetColor({dr-step, dg-step, db-step, da});
    pen.width = -border.size.x;
    g->SetPen(pen);
    g->DrawRectangle({xp, yp, wp, hp});
    
    g->SetColor({dr+step, dg+step, db+step, da});
    pen.width = -border.size.x/2;
    g->DrawRectangle({xp, yp, wp-border.size.x/2, hp-border.size.x/2});
  }

  pen.width = width;
  g->SetPen(pen);

  if (_is_enabled == false) {
    g->SetColor({0x00, 0x00, 0x00, 0x80});
    g->FillRectangle({0, 0, size.x, size.y});
  }
}

void Component::Paint(Graphics *g)
{
}

std::shared_ptr<Container> Component::GetParent()
{
  return _parent;
}

std::shared_ptr<Container> Component::GetTopLevelAncestor()
{
  for (std::shared_ptr<Component> cmp = GetSharedPointer(); cmp != nullptr; cmp = cmp->GetParent()) {
    std::shared_ptr<Container> container = std::dynamic_pointer_cast<Frame>(cmp);
    
    if (container != nullptr) {
      return container;
    }
  }

  return nullptr;
}

bool Component::IsEnabled()
{
  return _is_enabled;
}

void Component::SetEnabled(bool b)
{
  _is_enabled = b;

  if (HasFocus() == true) {
    ReleaseFocus();
  }
}

void Component::SetNavigationEnabled(bool b)
{
  _is_navigation_enabled = b;
}

bool Component::IsNavigationEnabled()
{
  return _is_navigation_enabled;
}

void Component::SetNextComponentFocus(std::shared_ptr<Component> left, std::shared_ptr<Component> right, std::shared_ptr<Component> up, std::shared_ptr<Component> down)
{
  _left = left;
  _right = right;
  _up = up;
  _down = down;
}

std::shared_ptr<Component> Component::GetNextFocusLeft()
{
  return _left;
}

std::shared_ptr<Component> Component::GetNextFocusRight()
{
  return _right;
}

std::shared_ptr<Component> Component::GetNextFocusUp()
{
  return _up;
}

std::shared_ptr<Component> Component::GetNextFocusDown()
{
  return _down;
}

void Component::SetNextFocusLeft(std::shared_ptr<Component> cmp)
{
  _left = cmp;
}

void Component::SetNextFocusRight(std::shared_ptr<Component> cmp)
{
  _right = cmp;
}

void Component::SetNextFocusUp(std::shared_ptr<Component> cmp)
{
  _up = cmp;
}

void Component::SetNextFocusDown(std::shared_ptr<Component> cmp)
{
  _down = cmp;
}

void Component::SetParent(std::shared_ptr<Container> parent)
{
  _parent = parent;
}

bool Component::IsBackgroundVisible()
{
  return _is_background_visible;
}

void Component::SetBackgroundVisible(bool b)
{
  if (_is_background_visible == b) {
    return;
  }

  _is_background_visible = b;
}

void Component::SetIgnoreRepaint(bool b)
{
  _is_ignore_repaint = b;
}

bool Component::IsIgnoreRepaint()
{
  return _is_ignore_repaint;
}

void Component::Repaint(std::shared_ptr<Component> cmp)
{
  if (_is_ignore_repaint == true) {
    return;
  }

  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->Repaint(cmp);
  }

  DispatchComponentEvent(new ComponentEvent(this, jcomponentevent_type_t::Paint));
}

void Component::SetMinimumSize(jpoint_t<int> size)
{
  _minimum_size.x = size.x;
  _minimum_size.y = size.y;

  if (_minimum_size.x < 0) {
    _minimum_size.x = 0;
  }

  if (_minimum_size.y < 0) {
    _minimum_size.y = 0;
  }

  if (_minimum_size.x > _maximum_size.x) {
    _minimum_size.x = _maximum_size.x;
  }

  if (_minimum_size.y > _maximum_size.y) {
    _minimum_size.y = _maximum_size.y;
  }

  jpoint_t<int>
    size2 = GetSize();

  if (size2.x < _minimum_size.x || size2.y < _minimum_size.y) {
    int 
      w = size2.x,
      h = size2.y;

    if (w < _minimum_size.x) {
      w = _minimum_size.x;
    }
  
    if (h < _minimum_size.y) {
      h = _minimum_size.y;
    }

    SetSize(w, h);
  }
}

void Component::SetMaximumSize(jpoint_t<int> size)
{
  _maximum_size.x = size.x;
  _maximum_size.y = size.y;

  if (_maximum_size.x > 65535) {
    _maximum_size.x = 65535;
  }

  if (_maximum_size.y > 65535) {
    _maximum_size.y = 65535;
  }

  if (_minimum_size.x > _maximum_size.x) {
    _maximum_size.x = _minimum_size.x;
  }

  if (_minimum_size.y > _maximum_size.y) {
    _maximum_size.y = _minimum_size.y;
  }

  jpoint_t<int>
    size2 = GetSize();

  if (size2.x > _maximum_size.x || size2.y > _maximum_size.y) {
    int 
      w = _size.x,
      h = _size.y;

    if (w > _maximum_size.x) {
      w = _maximum_size.x;
    }
  
    if (h > _maximum_size.y) {
      h = _maximum_size.y;
    }

    SetSize(w, h);
  }
}

void Component::SetPreferredSize(jpoint_t<int> size)
{
  _preferred_size.x = size.x;
  _preferred_size.y = size.y;

  if (_preferred_size.x < _minimum_size.x) {
    _preferred_size.x = _minimum_size.x;
  }

  if (_preferred_size.y < _minimum_size.y) {
    _preferred_size.y = _minimum_size.y;
  }

  if (_preferred_size.x > _maximum_size.x) {
    _preferred_size.x = _maximum_size.x;
  }

  if (_preferred_size.y > _maximum_size.y) {
    _preferred_size.y = _maximum_size.y;
  }
}

jpoint_t<int> Component::GetMinimumSize()
{
  return _minimum_size;
}

jpoint_t<int> Component::GetMaximumSize()
{
  return _maximum_size;
}

jpoint_t<int> Component::GetPreferredSize()
{
  jtheme_t
    theme = GetTheme();

  return _preferred_size + jpoint_t<int>{theme.padding.left + theme.padding.right, theme.padding.top + theme.padding.bottom};
}

void Component::Move(int x, int y)
{
  jpoint_t<int>
    location = GetLocation();

  SetLocation(location.x + x, location.y + y);
}

void Component::Move(jpoint_t<int> point)
{
  Move(point.x, point.y);
}

void Component::SetBounds(int x, int y, int width, int height)
{
  jpoint_t<int>
    size = GetSize();

  if (_location.x == x && _location.y == y && size.x == width && size.y == height) {
    return;
  }

  bool moved = (_location.x != x) || (_location.y != y);
  bool sized = (_size.x != width) || (_size.y != height);

  _location.x = x;
  _location.y = y;
  _size.x = width;
  _size.y = height;

  if (_size.x < _minimum_size.x) {
    _size.x = _minimum_size.x;
  }

  if (_size.y < _minimum_size.y) {
    _size.y = _minimum_size.y;
  }

  if (_size.x > _maximum_size.x) {
    _size.x = _maximum_size.x;
  }

  if (_size.y > _maximum_size.y) {
    _size.y = _maximum_size.y;
  }

  if (moved == true) {
    DispatchComponentEvent(new ComponentEvent(this, jcomponentevent_type_t::Move));
  }
  
  if (sized == true) {
    DispatchComponentEvent(new ComponentEvent(this, jcomponentevent_type_t::Size));
  }
}

void Component::SetBounds(jpoint_t<int> point, jpoint_t<int> size)
{
  SetBounds(point.x, point.y, size.x, size.y);
}

void Component::SetBounds(jrect_t<int> region)
{
  SetBounds(region.point, region.size);
}

void Component::SetLocation(int x, int y)
{
  jpoint_t<int>
    size = GetSize();

  SetBounds(x, y, size.x, size.y);
}

void Component::SetLocation(jpoint_t<int> point)
{
  SetLocation(point.x, point.y);
}

void Component::SetSize(int width, int height)
{
  jpoint_t<int>
    location = GetLocation();

  SetBounds(location.x, location.y, width, height);
}

void Component::SetSize(jpoint_t<int> size)
{
  SetSize(size.x, size.y);
}

jpoint_t<int> Component::GetAbsoluteLocation()
{
  std::shared_ptr<Container>
    parent = GetParent();
  jpoint_t<int>
    location = {
      .x = 0, 
      .y = 0
    };

  if (parent == nullptr) {
    return location;
  }

  jpoint_t<int>
    slocation = GetScrollLocation();

  location = _location;

  do {
    slocation = parent->GetScrollLocation();

    location.x = location.x + slocation.x;  
    location.y = location.y + slocation.y;  
  
    if (parent->GetParent() != nullptr) {
      jpoint_t<int> t = parent->GetLocation();

      location.x = location.x + t.x;
      location.y = location.y + t.y;
    }
  } while ((parent = parent->GetParent()) != nullptr);

  return location;
}

jpoint_t<int> Component::GetLocation()
{
  return GetBounds().point;
}

jpoint_t<int> Component::GetSize()
{
	return GetBounds().size;
}

void Component::RaiseToTop()
{
  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->RaiseComponentToTop(GetSharedPointer());
  }
}

void Component::LowerToBottom()
{
  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->LowerComponentToBottom(GetSharedPointer());
  }
}

void Component::PutAtop(std::shared_ptr<Component> c)
{
  if (c == nullptr) {
    return;
  }

  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->PutComponentATop(GetSharedPointer(), c);
  }
}

void Component::PutBelow(std::shared_ptr<Component> c)
{
  if (c == nullptr) {
    return;
  }

  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->PutComponentBelow(GetSharedPointer(), c);
  }
}

void Component::SetGradientLevel(int level)
{
  _gradient_level = level;

  if (_gradient_level < 0) {
    _gradient_level = 0;
  }

  if (_gradient_level > 0xff) {
    _gradient_level = 0xff;
  }
}

int Component::GetGradientLevel()
{
  return _gradient_level;
}

bool Component::KeyPressed(KeyEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }

  return false;
}

bool Component::KeyReleased(KeyEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }

  return false;
}

bool Component::KeyTyped(KeyEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }

  return false;
}

bool Component::MousePressed(MouseEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }
 
  jtheme_t
    theme = GetTheme();
  jpoint_t<int> 
    size = GetSize(),
    sdimention = GetScrollDimension();
  jpoint_t<int> 
    slocation = GetScrollLocation();
  jpoint_t<int> 
    elocation = event->GetLocation();

  if (IsFocusable() == true) {
    RequestFocus();
  }

  if (IsScrollableY() && elocation.x > (size.x - theme.scroll.size.x)) {
    double 
      offset_ratio = (double)slocation.y/(double)sdimention.y,
      block_size_ratio = (double)size.y/(double)sdimention.y;
    int 
      offset = (int)(size.y*offset_ratio),
      block_size = (int)(size.y*block_size_ratio);

    if (elocation.y > offset && elocation.y < (offset+block_size)) {
      _component_state = 10;
      _relative_mouse.x = elocation.x;
      _relative_mouse.y = elocation.y;
    } else if (elocation.y < offset) {
      SetScrollLocation(slocation.x, slocation.y - _scroll_major_increment);
    } else if (elocation.y > (offset + block_size)) {
      SetScrollLocation(slocation.x, slocation.y + _scroll_major_increment);
    }

    return true;
  } else if (IsScrollableX() && elocation.y > (size.y - theme.scroll.size.y)) {
    double 
      offset_ratio = (double)slocation.x/(double)sdimention.x,
      block_size_ratio = (double)size.x/(double)sdimention.x;
    int 
      offset = (int)(size.x*offset_ratio),
      block_size = (int)(size.x*block_size_ratio);

    if (elocation.x > offset && elocation.x < (offset + block_size)) {
      _component_state = 11;
      _relative_mouse.x = elocation.x;
      _relative_mouse.y = elocation.y;
    } else if (elocation.x < offset) {
      SetScrollLocation(slocation.x - _scroll_major_increment, slocation.y);
    } else if (elocation.x > (offset + block_size)) {
      SetScrollLocation(slocation.x + _scroll_major_increment, slocation.y);
    }

    return true;
  } 

  return false;
}

bool Component::MouseReleased(MouseEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }

  // int mousex = event->GetX(),
  //     mousey = event->GetY();

  if (_component_state != 0) {
    _component_state = 0;

    return true;
  }

  return false;
}

bool Component::MouseMoved(MouseEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }
  
  jpoint_t<int> 
    slocation = GetScrollLocation(),
    elocation = event->GetLocation();
  jpoint_t<int> 
    size = GetSize(),
    sdimention = GetScrollDimension();

  if (_component_state == 10) {
    SetScrollLocation(slocation.x, slocation.y + (int)((elocation.y - _relative_mouse.y)*((double)sdimention.y/(double)size.y)));
    
    _relative_mouse.y = elocation.y;

    return true;
  } else if (_component_state == 11) {
    SetScrollLocation(slocation.x + (int)((elocation.x - _relative_mouse.x)*((double)sdimention.x/(double)size.x)), slocation.y);

    _relative_mouse.x = elocation.x;

    return true;
  }

  return false;
}

bool Component::MouseWheel(MouseEvent *event)
{
  if (IsVisible() == false) {
    return false;
  }

  return false;
}

void Component::GetInternalComponents(std::shared_ptr<Container> current, std::vector<std::shared_ptr<Component>> *components)
{
  if (current == nullptr) {
    return;
  }

  std::vector<std::shared_ptr<Component>> v = current->GetComponents();

  for (std::vector<std::shared_ptr<Component>>::iterator i=v.begin(); i!=v.end(); i++) {
    std::shared_ptr<Container> container = std::dynamic_pointer_cast<Container>(*i);

    if (container != nullptr) {
      GetInternalComponents(container, components);
    }

    components->push_back(*i);
  }
}

std::shared_ptr<Container> Component::GetFocusCycleRootAncestor()
{
  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    return parent->GetFocusCycleRootAncestor();
  }

  return nullptr;
}

bool Component::ProcessNavigation(KeyEvent *event)
{
  if (_is_navigation_enabled == false) {
    return false;
  }

  jkeyevent_symbol_t action = event->GetSymbol();

  if (action != jkeyevent_symbol_t::CursorLeft &&
      action != jkeyevent_symbol_t::CursorRight && 
      action != jkeyevent_symbol_t::CursorUp && 
      action != jkeyevent_symbol_t::CursorDown) {
    return false;
  }

  jpoint_t<int> 
    location = GetAbsoluteLocation();
  jpoint_t<int> 
    size = GetSize();
  jrect_t<int> 
    rect = {
      location,
      size
    };

  std::shared_ptr<Component> next = GetSharedPointer();

  if (action == jkeyevent_symbol_t::CursorLeft && GetNextFocusLeft() != nullptr) {
    next = GetNextFocusLeft();
  } else if (action == jkeyevent_symbol_t::CursorRight && GetNextFocusRight() != nullptr) {
    next = GetNextFocusRight();
  } else if (action == jkeyevent_symbol_t::CursorUp && GetNextFocusUp() != nullptr) {
    next = GetNextFocusUp();
  } else if (action == jkeyevent_symbol_t::CursorDown && GetNextFocusDown() != nullptr) {
    next = GetNextFocusDown();
  } else {
    std::shared_ptr<Component>
      left = GetSharedPointer(),
      right = GetSharedPointer(),
      up = GetSharedPointer(),
      down = GetSharedPointer();

    FindNextComponentFocus(rect, left, right, up, down);
  
    if (action == jkeyevent_symbol_t::CursorLeft) {
      next = left;
    } else if (action == jkeyevent_symbol_t::CursorRight) {
      next = right;
    } else if (action == jkeyevent_symbol_t::CursorUp) {
      next = up;
    } else if (action == jkeyevent_symbol_t::CursorDown) {
      next = down;
    }

    if (_is_cyclic_focus == true && next == GetSharedPointer()) {
      std::vector<std::shared_ptr<Component>> components;
      int 
        x1 = 0,
        y1 = 0,
        x2 = 0,
        y2 = 0;

      GetInternalComponents(GetFocusCycleRootAncestor(), &components);

      for (std::vector<std::shared_ptr<Component>>::iterator i=components.begin(); i!=components.end(); i++) {
        std::shared_ptr<Component> cmp = (*i);

        if (cmp->IsFocusable() == false || cmp->IsEnabled() == false || cmp->IsVisible() == false) {
          continue;
        }

        jpoint_t<int> 
          t = cmp->GetAbsoluteLocation();
        jpoint_t<int> 
          size = cmp->GetSize();

        if (x1 > t.x) {
          x1 = t.x;
        }

        if (x2 < (t.x + size.x)) {
          x2 = t.x + size.x;
        }

        if (y1 > t.y) {
          y1 = t.y;
        }

        if (y2 < (t.y + size.y)) {
          y2 = t.y + size.y;
        }
      }

      if (action == jkeyevent_symbol_t::CursorLeft) {
        rect.point.x = x2;
      } else if (action == jkeyevent_symbol_t::CursorRight) {
        rect.point.x = x1 - rect.size.x;
      } else if (action == jkeyevent_symbol_t::CursorUp) {
        rect.point.y = y2;
      } else if (action == jkeyevent_symbol_t::CursorDown) {
        rect.point.y = y1 - rect.size.y;
      }

      FindNextComponentFocus(rect, left, right, up, down);
    
      if (action == jkeyevent_symbol_t::CursorLeft) {
        next = left;
      } else if (action == jkeyevent_symbol_t::CursorRight) {
        next = right;
      } else if (action == jkeyevent_symbol_t::CursorUp) {
        next = up;
      } else if (action == jkeyevent_symbol_t::CursorDown) {
        next = down;
      }
    }
  }

  if (next != nullptr) {
    next->RequestFocus();
  
    return true;
  }

  return false;
}

void Component::FindNextComponentFocus(jrect_t<int> rect, std::shared_ptr<Component> &left, std::shared_ptr<Component> &right, std::shared_ptr<Component> &up, std::shared_ptr<Component> &down)
{
  std::vector<std::shared_ptr<Component>> components;

  GetInternalComponents(GetFocusCycleRootAncestor(), &components);

  if (components.size() == 0 || (components.size() == 1 && components[0] == GetSharedPointer())) {
    return;
  }

  int 
    d_left = INT_MAX,
    d_right = INT_MAX,
    d_up = INT_MAX,
    d_down = INT_MAX;

  for (std::vector<std::shared_ptr<Component>>::iterator i=components.begin(); i!=components.end(); i++) {
    std::shared_ptr<Component> cmp = (*i);

    if (cmp == GetSharedPointer() || cmp->IsFocusable() == false || cmp->IsEnabled() == false || cmp->IsVisible() == false) {
      continue;
    }

    jpoint_t<int> 
      cmp_size = cmp->GetSize();
    jpoint_t<int> 
      cmp_location = cmp->GetAbsoluteLocation();
    int 
      c1x = rect.point.x + rect.size.x/2,
      c1y = rect.point.y + rect.size.y/2,
      c2x = cmp_location.x + cmp_size.x/2,
      c2y = cmp_location.y + cmp_size.y/2;

    if (cmp_location.x < rect.point.x) {
      int value = ::abs(c1y - c2y)*(rect.size.x + cmp_size.x) + (rect.point.x + rect.size.x - cmp_location.x - cmp_size.x);

      if (value < d_left) {
        left = cmp;
        d_left = value;
      }
    } 
    
    if (cmp_location.x > rect.point.x) {
      int value = ::abs(c1y - c2y)*(rect.size.x + cmp_size.x) + (cmp_location.x + cmp_size.x - rect.point.x - rect.size.x);

      if (value < d_right) {
        right = cmp;
        d_right = value;
      }
    }
    
    if (cmp_location.y < rect.point.y) {
      int value = ::abs(c1x - c2x)*(rect.size.y + cmp_size.y) + (rect.point.y + rect.size.y - cmp_location.y - cmp_size.y);

      if (value < d_up) {
        up = cmp;
        d_up = value;
      }
    }
    
    if (cmp_location.y > rect.point.y) {
      int value = ::abs(c1x - c2x)*(rect.size.y + cmp_size.y) + (cmp_location.y + cmp_size.y - rect.point.y - rect.size.y);

      if (value < d_down) {
        down = cmp;
        d_down = value;
      }
    }
  }
}

void Component::RequestFocus()
{
  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->RequestComponentFocus(GetSharedPointer());
  }
}

void Component::ReleaseFocus()
{
  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    parent->ReleaseComponentFocus(GetSharedPointer());
  }
}

bool Component::HasFocus()
{
  std::shared_ptr<Container> parent = GetParent();

  if (parent != nullptr) {
    return parent->GetFocusOwner() == GetSharedPointer();
  }

  return false;
}

bool Component::IsFocusable()
{
  return _is_focusable;
}

void Component::SetFocusable(bool b)
{
  _is_focusable = b;
}

bool Component::IsVisible()
{
  return _is_visible;
}

bool Component::IsHidden()
{
  if (IsVisible() == false) {
    return true;
  }

  std::shared_ptr<Container> cmp = GetParent();
  
  while (cmp != nullptr) {
    if (cmp->IsVisible() == false) {
      return true;
    }

    cmp = cmp->GetParent();
  }

  return false;
}

void Component::SetVisible(bool visible)
{
  if (_is_visible == visible) {
    return;
  }

  _is_visible = visible;

  if (_is_visible == false) {
    if (HasFocus() == true) {
      ReleaseFocus();
    }
  
    DispatchComponentEvent(new ComponentEvent(this, jcomponentevent_type_t::Hide));
  } else {
    DispatchComponentEvent(new ComponentEvent(this, jcomponentevent_type_t::Show));
  }
}

void Component::RegisterFocusListener(FocusListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_focus_listener_mutex);

  if (std::find(_focus_listeners.begin(), _focus_listeners.end(), listener) == _focus_listeners.end()) {
    _focus_listeners.push_back(listener);
  }
}

void Component::RemoveFocusListener(FocusListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_focus_listener_mutex);

  _focus_listeners.erase(std::remove(_focus_listeners.begin(), _focus_listeners.end(), listener), _focus_listeners.end());
}

void Component::DispatchFocusEvent(FocusEvent *event)
{
  if (event == nullptr) {
    return;
  }

  std::vector<FocusListener *> listeners;
  
  _focus_listener_mutex.lock();

  listeners = _focus_listeners;

  _focus_listener_mutex.unlock();

  for (std::vector<FocusListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    FocusListener *listener = (*i);

    if (event->GetType() == jfocusevent_type_t::Gain) {
      listener->FocusGained(event);
    } else if (event->GetType() == jfocusevent_type_t::Lost) {
      listener->FocusLost(event);
    }
  }

  /*
  for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
    if (event->GetType() == jfocusevent_type_t::Gain) {
      (*i)->FocusGained(event);
    } else if (event->GetType() == jfocusevent_type_t::Lost) {
      (*i)->FocusLost(event);
    }
  }
  */

  delete event;
}

const std::vector<FocusListener *> & Component::GetFocusListeners()
{
  return _focus_listeners;
}

void Component::RegisterComponentListener(ComponentListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_component_listener_mutex);

  if (std::find(_component_listeners.begin(), _component_listeners.end(), listener) == _component_listeners.end()) {
    _component_listeners.push_back(listener);
  }
}

void Component::RemoveComponentListener(ComponentListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_component_listener_mutex);

  _component_listeners.erase(std::remove(_component_listeners.begin(), _component_listeners.end(), listener), _component_listeners.end());
}

void Component::DispatchComponentEvent(ComponentEvent *event)
{
  if (event == nullptr) {
    return;
  }

  std::vector<ComponentListener *> listeners;
  
  _component_listener_mutex.lock();

  listeners = _component_listeners;

  _component_listener_mutex.unlock();

  for (std::vector<ComponentListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    ComponentListener *listener = (*i);

    if (event->GetType() == jcomponentevent_type_t::Hide) {
      listener->OnHide(event);
    } else if (event->GetType() == jcomponentevent_type_t::Show) {
      listener->OnShow(event);
    } else if (event->GetType() == jcomponentevent_type_t::Move) {
      listener->OnMove(event);
    } else if (event->GetType() == jcomponentevent_type_t::Size) {
      listener->OnSize(event);
    } else if (event->GetType() == jcomponentevent_type_t::Paint) {
      listener->OnPaint(event);
    } else if (event->GetType() == jcomponentevent_type_t::Enter) {
      listener->OnEnter(event);
    } else if (event->GetType() == jcomponentevent_type_t::Leave) {
      listener->OnLeave(event);
    }
  }

  delete event;
}

const std::vector<ComponentListener *> & Component::GetComponentListeners()
{
  return _component_listeners;
}

}

