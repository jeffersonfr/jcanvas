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
#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jcardlayout.h"
#include "jcanvas/widgets/jdialog.h"

#include <algorithm>

namespace jcanvas {

Container::Container(jrect_t<int> bounds):
  Component(bounds)
{
  jtheme_t &theme = GetTheme();

  theme.border.type = jtheme_border_t::style::Empty;
  theme.border.size = jpoint_t<int>{0, 0};

  _layout = std::make_shared<BorderLayout>();

  _is_focus_cycle_root = false;
  _orientation = jcomponent_orientation_t::LeftToRight;
  _is_enabled = true;
  _is_visible = true;
  _optimized_paint = false;

  _insets = {
    .left = 0,
    .top = 0,
    .right = 0,
    .bottom = 0
  };

  SetBackgroundVisible(false);
}

Container::~Container()
{
  _layout = nullptr;

  RemoveAll();
}

bool Container::MoveScrollTowards(Component *next, jkeyevent_symbol_t symbol)
{
  if (IsScrollable()) {
    Component
      *current = GetFocusOwner();
    jpoint_t 
      slocation = GetScrollLocation();
    jpoint_t<int> 
      size = GetSize(),
      scroll_dimension = GetScrollDimension();
    int 
      x = slocation.x,
      y = slocation.y,
      w = size.x,
      h = size.y;
    bool 
      edge = false,
      currentLarge = false,
      scrollOutOfBounds = false;

    if (symbol == jkeyevent_symbol_t::CursorUp) {
        y = slocation.y - _scroll_major_increment;
        // edge = (position == 0);
        currentLarge = (scroll_dimension.y > size.y);
        scrollOutOfBounds = y < 0;
        if(scrollOutOfBounds){
          y = 0;
        }
    } else if (symbol == jkeyevent_symbol_t::CursorDown) {
        y = slocation.y + _scroll_major_increment;
        // edge = (position == f.getFocusCount() - 1);
        currentLarge = (scroll_dimension.y > size.y);
        scrollOutOfBounds = y > (scroll_dimension.y - size.y);
        if(scrollOutOfBounds){
          y = scroll_dimension.y - size.y;
        }
    } else if (symbol == jkeyevent_symbol_t::CursorRight) {
        x = slocation.x + _scroll_major_increment;
        // edge = (position == f.getFocusCount() - 1);
        currentLarge = (scroll_dimension.x > size.x);
        scrollOutOfBounds = x > (scroll_dimension.x - size.x);
        if(scrollOutOfBounds){
          x = scroll_dimension.x - size.x;
        }
    } else if (symbol == jkeyevent_symbol_t::CursorLeft) {
        x = slocation.x - _scroll_major_increment;
        // edge = (position == 0);
        currentLarge = (scroll_dimension.x > size.x);
        scrollOutOfBounds = x < 0;
        if(scrollOutOfBounds){
          x = 0;
        }
    }
    
    //if the Form doesn't contain a focusable Component simply move the viewport by pixels
    if (next == nullptr || next == this) {
      ScrollToVisibleArea({x, y, w, h}, this);

      return false;
    }

    jpoint_t 
      al = GetAbsoluteLocation(),
      nl = next->GetAbsoluteLocation();
    jpoint_t<int>
      ns = next->GetSize();
    jrect_t<int>
      region1 {al.x, al.y, ns.x, ns.y},
      region2 {al.x + slocation.x, al.y + slocation.y, w, h};
    bool 
      nextIntersects = GetBounds().Contains(next->GetBounds()) == true && region1.Intersects(jrect_t<int>{al.x + x, al.y + y, w, h});

    if ((nextIntersects && !currentLarge && !edge) || region2.Contains(jrect_t<int>{nl.x, nl.y, ns.x, ns.y})) {
      //scrollComponentToVisible(next);
    } else {
      if (!scrollOutOfBounds) {
        jpoint_t 
          cl = current->GetAbsoluteLocation();
        jpoint_t<int>
          cs = current->GetSize();
        jrect_t<int>
          region3 {cl.x, cl.y, cs.x, cs.y};

        ScrollToVisibleArea({x, y, w, h}, this);

        // if after moving the scroll the current focus is out of the view port and the next focus is in the view port move the focus
        if (nextIntersects == false || region3.Intersects(jrect_t<int>{al.x + x, al.y + y, w, h}) != 0) {
          return false;
        }
      } else {
        //scrollComponentToVisible(next);
      }
    }
  }

  return true;
}

void Container::InternalAddDialog(Dialog *dialog)
{
  _dialogs_mutex.lock();

  if (std::find(_dialogs.begin(), _dialogs.end(), dialog) != _dialogs.end()) {
    _dialogs_mutex.unlock();

    throw std::runtime_error("Dialog is already added");
  }

  _dialogs.push_back(dialog);
  
  _dialogs_mutex.unlock();

  Repaint();
}

void Container::InternalRemoveDialog(Dialog *dialog)
{
  _dialogs_mutex.lock();

  _dialogs.erase(std::remove(_dialogs.begin(), _dialogs.end(), dialog), _dialogs.end());
  
  _dialogs_mutex.unlock();
  
  Repaint();
}

jpoint_t<int> Container::GetScrollDimension()
{
  jtheme_t
    theme = GetTheme();
  int 
    p1x = 0,
    p2x = 0,
    p1y = 0,
    p2y = 0;

  jpoint_t<int>
    size = GetSize();

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    Component
      *cmp = (*i);
    jpoint_t 
      cl = cmp->GetLocation();
    jpoint_t<int> 
      cs = cmp->GetSize();

    if (p1x > cl.x) {
      p1x = cl.x;
    }

    if (p2x < (cl.x + cs.x)) {
      p2x = cl.x + cs.x;
    }
    
    if (p1y > cl.y) {
      p1y = cl.y;
    }

    if (p2y < (cl.y + cs.y)) {
      p2y = cl.y + cs.y;
    }
  }
  
  if (p1x < 0) {
    if (p2x < size.x) {
      p2x = size.x;
    }
  }

  if (p1y < 0) {
    if (p2y < size.y) {
      p2y = size.y;
    }
  }

  jpoint_t<int>
    scroll_dimension;

  scroll_dimension.x = p2x - p1x;
  scroll_dimension.y = p2y - p1y;

  if ((scroll_dimension.x > size.x)) {
    scroll_dimension.y = scroll_dimension.y + theme.scroll.size.y + theme.scroll.padding.y;

    if ((scroll_dimension.y > size.y)) {
      scroll_dimension.x = scroll_dimension.x + theme.scroll.size.x + theme.scroll.padding.x;
    }
  } else if ((scroll_dimension.y > size.y)) {
    scroll_dimension.x = scroll_dimension.x + theme.scroll.size.x + theme.scroll.padding.x;
  
    if ((scroll_dimension.x > size.x)) {
      scroll_dimension.y = scroll_dimension.y + theme.scroll.size.y + theme.scroll.padding.y;
    }
  }

  return scroll_dimension;
}

Component * Container::GetTargetComponent(Container *target, int x, int y, int *dx, int *dy)
{
  jpoint_t slocation = GetScrollLocation();
  int scrollx = (IsScrollableX() == true)?slocation.x:0,
      scrolly = (IsScrollableY() == true)?slocation.y:0;

  if ((void *)dx != nullptr) {
    *dx = x;
  }

  if ((void *)dy != nullptr) {
    *dy = y;
  }

  for (std::vector<Component *>::const_reverse_iterator i=target->GetComponents().rbegin(); i!=target->GetComponents().rend(); i++) {
    Component *c = (*i);
  
    if (c->IsVisible() == true) {
      if (c->GetBounds().Intersects(jpoint_t<int>{x + scrollx, y + scrolly}) == true) {
        jpoint_t cl = c->GetLocation();

        if ((void *)dx != nullptr) {
          *dx = x - cl.x;
        }

        if ((void *)dy != nullptr) {
          *dy = y - cl.y;
        }

        return c;
      }
    }
  }

  return target;
}

void Container::SetOptimizedPaint(bool b)
{
  _optimized_paint = b;
}

void Container::DoLayout()
{
  if (_layout != nullptr) {
    std::lock_guard<std::recursive_mutex> lock(_container_mutex);

    SetIgnoreRepaint(true);

    _layout->DoLayout(this);

    for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
      Container *container = dynamic_cast<Container *>(*i);
      
      if (container != nullptr) {
        container->DoLayout();
      }
    }
    
    SetIgnoreRepaint(false);
  }
}

void Container::Pack(bool fit)
{
  Component
    *c;
  jinsets_t<int> 
    insets = GetInsets();
  int 
    min_x = insets.left,
    min_y = insets.top,
    max_w = 0,
    max_h = 0;

  _container_mutex.lock();

  DoLayout();

  if (fit == true) {
    for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
      c = (*i);

      jpoint_t cl = c->GetLocation();

      if (cl.x < min_x) {
        min_x = cl.x;
      }

      if (cl.y < min_y) {
        min_y = cl.y;
      }
    }

    min_x = insets.left - min_x;
    min_y = insets.top - min_y;

    for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
      c = (*i);

      jpoint_t cl = c->GetLocation();

      c->SetLocation(cl.x + min_x, cl.y + min_y);
    }
  }

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    c = (*i);

    jpoint_t cl = c->GetLocation();
    jpoint_t<int> cs = c->GetSize();

    if (max_w < (cl.x + cs.x)) {
      max_w = cl.x + cs.x;
    }

    if (max_h < (cl.y + cs.y)) {
      max_h = cl.y + cs.y;
    }
  }

  _container_mutex.unlock();

  SetSize(max_w + insets.right, max_h + insets.bottom);
}

jinsets_t<int> Container::GetInsets()
{
  return _insets;
}

void Container::SetInsets(jinsets_t<int> insets)
{
  _insets = insets;
  
  SetPreferredSize(GetSize());
}

void Container::PaintGlassPane(Graphics *g)
{
}

void Container::Paint(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  std::lock_guard<std::recursive_mutex> guard(_container_mutex);

  jpoint_t 
    slocation = GetScrollLocation();
  jrect_t 
    clip = g->GetClip();

  Component::Paint(g);

  if (IsBackgroundVisible() == true) {
    PaintBackground(g);
  }

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    Component *c = (*i);

    if (c->IsVisible() == true) {
      // TODO:: considerar o scroll de um component
      jrect_t 
        bounds = c->GetBounds();

      bounds.point = bounds.point - slocation;

      if (bounds.size.x > 0 && bounds.size.y > 0) {
        g->Translate(bounds.point);
        g->ClipRect({0, 0, bounds.size});
  
        if (c->IsBackgroundVisible() == true) {
          c->PaintBackground(g);
        }

        c->Paint(g);
        
        if (c->IsScrollVisible() == true) {
          c->PaintScrollbars(g);
        }

        c->PaintBorders(g);
        
        g->Translate(-bounds.point);
        g->SetClip(clip);
      }
    }
  }
        
  if (IsScrollVisible() == true) {
    PaintScrollbars(g);
  }
    
  PaintBorders(g);

  // INFO:: paint dialogs
  _dialogs_mutex.lock();

  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    Dialog *c = (*i);

    if (c->IsVisible() == true) {
      jpoint_t 
        cl = c->GetLocation();
      jpoint_t<int> 
        cs = c->GetSize();
      int 
        cx = cl.x - slocation.x,
        cy = cl.y - slocation.y,
        cw = cs.x,
        ch = cs.y;

      if (cw > 0 && ch > 0) {
        g->Translate({cx, cy});
        g->ClipRect({0, 0, cw, ch});
  
        c->PaintBackground(g);
        c->Paint(g);
        c->PaintScrollbars(g);
        c->PaintBorders(g);
        
        g->Translate({-cx, -cy});
        g->SetClip(clip);
      }
    }
  }
  
  _dialogs_mutex.unlock();

  PaintGlassPane(g);
}

void Container::Repaint(Component *cmp)
{
  if (IsIgnoreRepaint() == true || IsVisible() == false) {
    return;
  }

  Container *parent = GetParent();

  if (parent != nullptr) {
    parent->Repaint((cmp == nullptr)?this:cmp);
  }

  Component::DispatchComponentEvent(new ComponentEvent(this, jcomponentevent_type_t::Paint));
}

void Container::Add(Component *c, int index)
{
  if (index < 0 || index > GetComponentCount()) {
    throw std::out_of_range("Index out of range");
  }

  if (c == nullptr) {
    throw std::invalid_argument("Component must have a valid pointer");
  }

  if (c == this) {
    throw std::runtime_error("Adding own container");
  }

  if (dynamic_cast<Dialog *>(c) != nullptr) {
    throw std::invalid_argument("Unable to add dialogs to container");
  }

  _container_mutex.lock();

  if (std::find(_components.begin(), _components.end(), c) == _components.end()) {
    _components.insert(_components.begin()+index, c);

    Container *container = dynamic_cast<Container *>(c);
    
    if (container != nullptr) {
      Component *focus = container->GetFocusOwner();

      c->SetParent(this);

      if (focus != nullptr) {
        RequestComponentFocus(focus);
      }
    } else {
      c->SetParent(this);
    }

    DispatchContainerEvent(new ContainerEvent(c, jcontainerevent_type_t::Add));
  }

  SetPreferredSize(GetSize());
  
  _container_mutex.unlock();

  DoLayout();
}

void Container::Add(Component *c)
{
  Add(c, GetComponentCount());
}

void Container::Add(Component *c, GridBagConstraints *constraints)
{
  if (_layout != nullptr) {
    std::shared_ptr<GridBagLayout> layout = std::dynamic_pointer_cast<GridBagLayout>(_layout);

    if (layout != nullptr) {
      layout->AddLayoutComponent(c, constraints);
    }
  }
 
  Add(c, GetComponentCount());

  DispatchContainerEvent(new ContainerEvent(c, jcontainerevent_type_t::Add));
}

void Container::Add(Component *c, std::string id)
{
  if (_layout != nullptr) {
    std::shared_ptr<CardLayout> layout = std::dynamic_pointer_cast<CardLayout>(_layout);

    if (layout != nullptr) {
      layout->AddLayoutComponent(id, c);
    }
  }

  Add(c, GetComponentCount());

  DispatchContainerEvent(new ContainerEvent(c, jcontainerevent_type_t::Add));
}

void Container::Add(Component *c, jborderlayout_align_t align)
{
  if (_layout != nullptr) {
    std::shared_ptr<BorderLayout> layout = std::dynamic_pointer_cast<BorderLayout>(_layout);

    if (layout != nullptr) {
      layout->AddLayoutComponent(c, align);
    }
  }
  
  Add(c, GetComponentCount());

  DispatchContainerEvent(new ContainerEvent(c, jcontainerevent_type_t::Add));
}

void Container::Remove(Component *c)
{
  if (c == nullptr) {
    return;
  }

  // INFO:: se o componente em foco pertencer ao container remover o foco
  Container *container = dynamic_cast<Container *>(c);

  if (container != nullptr) {
    Component *focus = GetFocusOwner();

    if (focus != nullptr) {
      Container *parent = focus->GetParent();

      while (parent != nullptr) {
        if (parent == container) {
          focus->ReleaseFocus();

          break;
        }

        if (parent->GetParent() == nullptr) {
          break;
        }

        parent = parent->GetParent();
      }
    }
  } else {
    c->ReleaseFocus();
  }

  if (_layout != nullptr) {
    std::shared_ptr<BorderLayout> layout = std::dynamic_pointer_cast<BorderLayout>(_layout);
    
    if (layout != nullptr) {
      layout->RemoveLayoutComponent(c);
    }
  }

  _container_mutex.lock();

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    if (c == (*i)) {
      c->SetParent(nullptr);

      i = _components.erase(i) - 1;

      if (i == _components.end()) {
        break;
      }

      DispatchContainerEvent(new ContainerEvent(c, jcontainerevent_type_t::Remove));
    }
  }
  
  SetPreferredSize(GetSize());

  _container_mutex.unlock();

  DoLayout();
}

void Container::RemoveAll()
{
  Component *focus = GetFocusOwner();

  if (focus != nullptr) {
    Container *parent = focus->GetParent();

    while (parent != nullptr) {
      if (parent == this) {
        focus->ReleaseFocus();

        break;
      }

      if (parent->GetParent() == nullptr) {
        break;
      }

      parent = parent->GetParent();
    }
  }

   _container_mutex.lock();

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    Component *c = (*i);

    c->SetParent(nullptr);

    DispatchContainerEvent(new ContainerEvent(c, jcontainerevent_type_t::Remove));
  }

  _components.clear();

  SetPreferredSize(GetSize());

  _container_mutex.unlock();

  DoLayout();
}

bool Container::Contains(Component *cmp)
{
  std::vector<Component *> components;

  GetInternalComponents(this, components);

  for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
    if (cmp == (*i)) {
      return true;
    }
  }

  return false;
}

int Container::GetComponentCount()
{
   std::lock_guard<std::recursive_mutex> guard(_container_mutex);

  return _components.size();
}

const std::vector<Component *> & Container::GetComponents()
{
  return _components;
}

Component * Container::GetComponentAt(int x, int y)
{
  return GetTargetComponent(this, x, y, nullptr, nullptr);
}

Component * Container::GetFocusOwner()
{
  Container *parent = GetParent();

  if (parent != nullptr) {
    return parent->GetFocusOwner();
  }

  return nullptr;
}

void Container::RequestComponentFocus(Component *c)
{
  if (c == nullptr or c->IsFocusable() == false) {
    return;
  }

  Container *parent = GetParent();

  if (parent != nullptr) {
    parent->RequestComponentFocus(c);
  }
}

void Container::ReleaseComponentFocus(Component *c)
{
  if (c == nullptr or c->IsFocusable() == false) {
    return;
  }

  Container *parent = GetParent();

  if (parent != nullptr) {
    parent->ReleaseComponentFocus(c);
  }
}

bool Container::KeyPressed(KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    if ((*i)->KeyPressed(event) == true) {
      return true;
    }
  }

  Component *current = GetFocusOwner();

  if (current != nullptr && current != this) {
    if (current->KeyPressed(event) == true) {
      return true;
    }
    
    if (current->ProcessNavigation(event) == true) {
      return true;
    }
  }

  return false;
}

bool Container::KeyReleased(KeyEvent *event)
{
  if (Component::KeyReleased(event) == true) {
    return true;
  }

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    if ((*i)->KeyReleased(event) == true) {
      return true;
    }
  }

  Component *current = GetFocusOwner();

  if (current != nullptr && current != this) {
    if (current->KeyReleased(event) == true) {
      return true;
    }
  }

  return false;
}

bool Container::KeyTyped(KeyEvent *event)
{
  if (Component::KeyTyped(event) == true) {
    return true;
  }

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    if ((*i)->KeyTyped(event) == true) {
      return true;
    }
  }

  return false;
}

bool Container::MousePressed(MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  jpoint_t 
    elocation = event->GetLocation();

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    Dialog
      *dialog = (*i);
    jpoint_t
      dlocation = dialog->GetLocation();
    MouseEvent 
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), {elocation.x - dlocation.x, elocation.y - dlocation.y}, event->GetClicks());

    if ((*i)->MousePressed(&evt) == true) {
      return true;
    }
  }

  int
    dx,
    dy;

  Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

  if (c != nullptr && c != this) {
    jpoint_t 
      slocation = GetScrollLocation();
    MouseEvent 
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), {dx + slocation.x, dy + slocation.y}, event->GetClicks());

    return c->MousePressed(&evt);
  }

  return false;
}

bool Container::MouseReleased(MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    if ((*i)->MouseReleased(event) == true) {
      return true;
    }
  }

  jpoint_t
    elocation = event->GetLocation();
  jpoint_t 
    slocation = GetScrollLocation();
  Component
    *focus = GetFocusOwner();

  if (focus != nullptr and focus != this) {
    MouseEvent
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), elocation - focus->GetLocation(), event->GetClicks());

    return focus->MouseReleased(&evt);
  }

  int 
    dx,
    dy;

  Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

  if (c != nullptr && c != this) {
    MouseEvent 
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), {dx + slocation.x, dy + slocation.y}, event->GetClicks());

    return c->MouseReleased(&evt);
  }

  return false;
}

bool Container::MouseMoved(MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    if ((*i)->MouseMoved(event) == true) {
      return true;
    }
  }

  jpoint_t
    elocation = event->GetLocation();
  jpoint_t 
    slocation = GetScrollLocation();
  Component
    *focus = GetFocusOwner();

  if (focus != nullptr and focus != this) {
    MouseEvent 
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), elocation - focus->GetLocation(), event->GetClicks());

    return focus->MouseMoved(&evt);
  }

  int dx, dy;

  Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

  if (c != nullptr && c != this) {
    MouseEvent 
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), {dx + slocation.x, dy + slocation.y}, event->GetClicks());

    return c->MouseMoved(&evt);
  }

  return false;
}

bool Container::MouseWheel(MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  // INFO:: process dialogs first
  for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
    if ((*i)->MouseWheel(event) == true) {
      return true;
    }
  }

  jpoint_t
    elocation = event->GetLocation();
  Component
    *focus = GetFocusOwner();

  if (focus != nullptr and focus != this) {
    MouseEvent
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), elocation - focus->GetLocation(), event->GetClicks());

    return focus->MouseWheel(&evt);
  }

  int dx, dy;

  Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

  if (c != nullptr && c != this) {
    jpoint_t 
      slocation = GetScrollLocation();
    MouseEvent 
      evt(event->GetSource(), event->GetType(), event->GetButton(), event->GetButtons(), {dx + slocation.x, dy + slocation.y}, event->GetClicks());

    return c->MouseWheel(&evt);
  }

  return false;
}

void Container::RaiseComponentToTop(Component *c)
{
  bool b = false;

   std::lock_guard<std::recursive_mutex> guard(_container_mutex);

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    if (c == (*i)) {
      i = _components.erase(i) - 1;

      if (i == _components.end()) {
        break;
      }

      b = true;
    }
  }
  
  if (b == true) {
    _components.push_back(c);
  }
}

void Container::LowerComponentToBottom(Component *c)
{
  bool b = false;

   std::lock_guard<std::recursive_mutex> guard(_container_mutex);

  for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    if (c == (*i)) {
      i = _components.erase(i) - 1;

      if (i == _components.end()) {
        break;
      }

      b = true;
    }
  }
  
  if (b == true) {
    _components.insert(_components.begin(), c);
  }
}

void Container::PutComponentATop(Component *c, Component *c1)
{
   std::lock_guard<std::recursive_mutex> guard(_container_mutex);

  std::vector<Component *>::iterator 
    i = std::find(_components.begin(), _components.end(), c1);

  if (i == _components.end()) {
    return;
  }

  _components.insert(i + 1, c);
}

void Container::PutComponentBelow(Component *c, Component *c1)
{
   std::lock_guard<std::recursive_mutex> guard(_container_mutex);

  std::vector<Component *>::iterator 
    i = std::find(_components.begin(), _components.end(), c1);

  if (i == _components.end()) {
    return;
  }

  _components.insert(i, c);
}

void Container::RegisterContainerListener(ContainerListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_container_listener_mutex);

  if (std::find(_container_listeners.begin(), _container_listeners.end(), listener) == _container_listeners.end()) {
    _container_listeners.push_back(listener);
  }
}

void Container::RemoveContainerListener(ContainerListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_container_listener_mutex);

  _container_listeners.erase(std::remove(_container_listeners.begin(), _container_listeners.end(), listener), _container_listeners.end());
}

void Container::DispatchContainerEvent(ContainerEvent *event)
{
  if (event == nullptr) {
    return;
  }

  std::vector<ContainerListener *> listeners;
  
  _container_listener_mutex.lock();

  listeners = _container_listeners;

  _container_listener_mutex.unlock();

  for (std::vector<ContainerListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    ContainerListener *listener = (*i);

    if (event->GetType() == jcontainerevent_type_t::Add) {
      listener->ComponentAdded(event);
    } else if (event->GetType() == jcontainerevent_type_t::Add) {
      listener->ComponentRemoved(event);
    }
  }

  delete event;
}

const std::vector<ContainerListener *> & Container::GetContainerListeners()
{
  return _container_listeners;
}

}
