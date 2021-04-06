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
#include "jcanvas/widgets/jcardlayout.h"
#include "jcanvas/widgets/jcontainer.h"

namespace jcanvas {

CardLayout::CardLayout(int hgap, int vgap):
  Layout()
{
  _hgap = hgap;
  _vgap = vgap;
  _index = 0;
}

CardLayout::~CardLayout()
{
}

int CardLayout::GetHGap() 
{
  return _hgap;
}

void CardLayout::SetHGap(int hgap) 
{
  _hgap = hgap;
}

int CardLayout::GetVGap() 
{
  return _vgap;
}

void CardLayout::SetVGap(int vgap) 
{
  _vgap = vgap;
}

void CardLayout::AddLayoutComponent(std::string name, Component *comp) 
{
  // WARN:: sync
  if (!_cards.empty()) {
    comp->SetVisible(false);
  }
  
  for (int i=0; i < (int)_cards.size(); i++) {
    if (_cards[i].name == name) {
      _cards[i].component = comp;

      return;
    }
  }

  jcardlayout_map_t t;

  t.name = name;
  t.component = comp;

  _cards.push_back(t);
}

void CardLayout::RemoveLayoutComponent(Component *comp) 
{
  // WARN:: sync
  for (int i = 0; i < (int)_cards.size(); i++) {
    if (_cards[i].component == comp) { 
      // if we remove current component we should show next one
      if (comp->IsVisible() && (comp->GetParent() != nullptr)) {
        Next(comp->GetParent());
      }

      // WARN:: _cards.remove(i);

      // correct currentCard if this is necessary
      if (_index > i) {
        _index--;
      }

      break;
    }
  }
}

void CardLayout::CheckLayout(Container *parent) 
{
  if (parent == nullptr) {
    throw std::invalid_argument("Parent container must be a valid address");
  }

  if (parent->GetLayout().get() != this) {
    throw std::runtime_error("Wrong parent for CardLayout");
  }
}

void CardLayout::First(Container *parent) 
{
  // WARN:: sync
    CheckLayout(parent);

    int ncomponents = parent->GetComponentCount();

    // CHANGE:: for (int i = 0 ; i < ncomponents ; i++) {
    for (int i = 1 ; i < ncomponents ; i++) {
      Component *comp = parent->GetComponents()[i];

      if (comp->IsVisible()) {
        comp->SetVisible(false);

        break;
      }
    }

    if (ncomponents > 0) {
      _index = 0;
    
      parent->GetComponents()[0]->SetVisible(true);
      // WARN:: parent.validate();
    }
}

void CardLayout::Next(Container *parent) 
{
  // WARN:: sync
  CheckLayout(parent);

  int ncomponents = parent->GetComponentCount();
  
  for (int i = 0 ; i < ncomponents ; i++) {
    Component *comp = parent->GetComponents()[i];

    if (comp->IsVisible()) {
      comp->SetVisible(false);
      _index = (i + 1) % ncomponents;
      comp = parent->GetComponents()[_index];
      comp->SetVisible(true);
      // WARN:: parent.validate();
      return;
    }
  }

  ShowDefaultComponent(parent);
}

void CardLayout::Previous(Container *parent) 
{
  // WARN:: sync
    CheckLayout(parent);
    
    int ncomponents = parent->GetComponentCount();
    
    for (int i = 0 ; i < ncomponents ; i++) {
      Component *comp = parent->GetComponents()[i];

      if (comp->IsVisible()) {
        comp->SetVisible(false);
        _index = ((i > 0) ? i-1 : ncomponents-1);
        comp = parent->GetComponents()[_index];
        comp->SetVisible(true);
        // WARN:: parent.validate();
        return;
      }
    }

    ShowDefaultComponent(parent);
}

void CardLayout::ShowDefaultComponent(Container *parent) 
{
  if (parent->GetComponentCount() > 0) {
    _index = 0;
    parent->GetComponents()[0]->SetVisible(true);
    // WARN:: parent.validate();
  }
}

void CardLayout::Last(Container *parent) 
{
  // WARN:: sync
    CheckLayout(parent);

    int ncomponents = parent->GetComponentCount();
    
    // CHANGE:: for (int i = 0 ; i < ncomponents ; i++) {
    for (int i = 0 ; i < ncomponents-1 ; i++) {
      Component *comp = parent->GetComponents()[i];

      if (comp->IsVisible()) {
        comp->SetVisible(false);

        break;
      }
    }

    if (ncomponents > 0) {
      _index = ncomponents - 1;
      parent->GetComponents()[_index]->SetVisible(true);
      // WARN:: parent.validate();
    }
}

jpoint_t<int> CardLayout::GetMinimumLayoutSize(Container *parent)
{
  // WARN:: sync parent
  jinsets_t insets = parent->GetInsets();
  int ncomponents = parent->GetComponentCount(),
      w = 0,
      h = 0;

  for (int i = 0 ; i < ncomponents ; i++) {
    Component *comp = parent->GetComponents()[i];

    jpoint_t<int> d = comp->GetMinimumSize();

    if (d.x > w) {
      w = d.x;
    }
    if (d.y > h) {
      h = d.y;
    }
  }

  jpoint_t<int> t = {insets.left + insets.right + w + _hgap*2, insets.top + insets.bottom + h + _vgap*2};

  return t;
}

jpoint_t<int> CardLayout::GetMaximumLayoutSize(Container *parent)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> CardLayout::GetPreferredLayoutSize(Container *parent)
{
  // WARN:: sync parent
  jinsets_t insets = parent->GetInsets();
  int ncomponents = parent->GetComponentCount(),
      w = 0,
      h = 0;

  for (int i = 0 ; i < ncomponents ; i++) {
    Component *comp = parent->GetComponents()[i];

    jpoint_t<int> d = comp->GetPreferredSize();

    if (d.x > w) {
      w = d.x;
    }
    if (d.y > h) {
      h = d.y;
    }
  }

  jpoint_t<int> t = {insets.left + insets.right + w + _hgap*2, insets.top + insets.bottom + h + _vgap*2};

  return t;
}

void CardLayout::DoLayout(Container *parent)
{
  // WARN:: sync
  jinsets_t insets = parent->GetInsets();
  Component *comp = nullptr;
  int ncomponents = parent->GetComponentCount();
  bool currentFound = false;

  for (int i = 0 ; i < ncomponents ; i++) {
    jpoint_t<int> size = parent->GetSize();

    comp = parent->GetComponents()[i];

    comp->SetBounds(
        _hgap + insets.left, 
        _vgap + insets.top, 
        size.x - (_hgap*2 + insets.left + insets.right), 
        size.y - (_vgap*2 + insets.top + insets.bottom));

    if (comp->IsVisible()) {
      currentFound = true;
    }
  }

  if (!currentFound && ncomponents > 0) {
    parent->GetComponents()[0]->SetVisible(true);
  }
}

}

