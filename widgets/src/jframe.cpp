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
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/jnulllayout.h"
#include "jcanvas/core/japplication.h"

namespace jcanvas {

Frame::Frame(jpoint_t<int> size, jpoint_t<int> point):
  Window(size, point)
{
  _focus_owner = nullptr;
  _icon = nullptr;

  jtheme_t
    &theme = GetTheme();

  theme.border.type = jtheme_border_t::style::RaisedGradient;
  theme.font.primary = &Font::Big;

  SetInsets({8, 8, 8, 8});
  SetTitle("Main");
  SetLayout<BorderLayout>();
  SetBackgroundVisible(true);
  SetUndecorated(false);
}

Frame::~Frame()
{
}

jrect_t<int> Frame::GetVisibleBounds()
{
	jpoint_t<int> 
    scroll = GetScrollLocation();
  jpoint_t<int> 
    size = Window::GetSize();

	return {{-scroll.x, -scroll.y}, size};
}

jpoint_t<int> Frame::GetPosition()
{
  return Window::GetPosition();
}
    
void Frame::SetPosition(jpoint_t<int> pos)
{
  Window::SetPosition(pos);
}
    
jpoint_t<int> Frame::GetSize()
{
  return Window::GetSize();
}
    
void Frame::SetSize(jpoint_t<int> size)
{
  return Window::SetSize(size);
}
    
jrect_t<int> Frame::GetBounds()
{
  return Window::GetBounds();
}
    
void Frame::SetBounds(jrect_t<int> bounds)
{
  Window::SetBounds(bounds);
}
    
void Frame::SetIcon(Image *image)
{
  _icon = image;
}

Image * Frame::GetIcon()
{
  return _icon;
}

Component * Frame::GetFocusOwner()
{
  return _focus_owner;
}

void Frame::RequestComponentFocus(Component *c)
{
  if (_focus_owner != nullptr && _focus_owner != c) {
    _focus_owner->ReleaseFocus();
  }

  _focus_owner = c;

  Repaint();

  dynamic_cast<Component *>(c)->DispatchFocusEvent(new jcanvas::FocusEvent(c, jcanvas::jfocusevent_type_t::Gain));
}

void Frame::ReleaseComponentFocus(Component *c)
{
  if (_focus_owner == nullptr or _focus_owner != c) {
    return;
  }

  _focus_owner = nullptr;

  Repaint();

  dynamic_cast<Component *>(c)->DispatchFocusEvent(new jcanvas::FocusEvent(c, jcanvas::jfocusevent_type_t::Lost));
}

Container * Frame::GetFocusCycleRootAncestor()
{
  return this;
}

void Frame::Repaint(Component *cmp)
{
  Window::Repaint();
}

void Frame::PaintBackground(Graphics *g)
{
  Container::PaintBackground(g);
}

void Frame::PaintGlassPane(Graphics *g)
{
  Container::PaintGlassPane(g);

  jtheme_t
    theme = GetTheme();
  jpoint_t
    size = Window::GetSize();
  jinsets_t
    insets = GetInsets();

  g->SetFont(theme.font.primary);
  g->SetColor(jcolorname::White);
}

void Frame::Paint(Graphics *g)
{
	g->Clear();

  Container::Paint(g);
}

bool Frame::KeyPressed(jcanvas::KeyEvent *event)
{
  if (Container::KeyPressed(event) == true) {
    return true;
  }

  return false;
}

bool Frame::KeyReleased(jcanvas::KeyEvent *event)
{
  if (Container::KeyReleased(event) == true) {
    return true;
  }

  return false;
}

bool Frame::KeyTyped(jcanvas::KeyEvent *event)
{
  if (Container::KeyTyped(event) == true) {
    return true;
  }

  return false;
}

bool Frame::MousePressed(jcanvas::MouseEvent *event)
{
  if (Container::MousePressed(event) == true) {
    return true;
  }

  return false;
}

bool Frame::MouseReleased(jcanvas::MouseEvent *event)
{
  if (Container::MouseReleased(event) == true) {
    return true;
  }

  return false;
}

bool Frame::MouseMoved(jcanvas::MouseEvent *event)
{
  if (Container::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool Frame::MouseWheel(jcanvas::MouseEvent *event)
{
  if (Container::MouseWheel(event) == true) {
    return true;
  }

  return false;
}

}

