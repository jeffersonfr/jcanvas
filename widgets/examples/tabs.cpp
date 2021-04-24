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
#include "jcanvas/core/japplication.h"
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jcardlayout.h"
#include "jcanvas/widgets/jgridlayout.h"
#include "jcanvas/widgets/jborderlayout.h"
#include "jcanvas/widgets/jnulllayout.h"
#include "jcanvas/widgets/jrowlayout.h"
#include "jcanvas/widgets/jroundedrectangleborder.h"
#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/core/jenum.h"

using namespace jcanvas;

// TODO::
// .. jogar button pra frente, setar o foco, mudar a cor quando for selectionado e remover isso quando perder o foco

class TabbedContainer : public Container, public ActionListener {

  struct jtab_item_t {
    std::string id;
    Button *tab;
    Component *cmp;
  };

	private:
    std::vector<jtab_item_t>
      _items;
    Container
      *_tabs,
      *_cards;
    std::shared_ptr<RoundedRectangleBorder>
      _border;
    std::string
      _current_id;

	public:
		TabbedContainer()
    {
      _border = std::make_shared<RoundedRectangleBorder>();

      _border->SetCorners(jenum_t<jrect_corner_t>{jrect_corner_t::None}.Or(jrect_corner_t::TopRight));

      _tabs = new Container();
      _cards = new Container();

      _tabs->SetLayout<RowLayout>(jmainaxis_align_t::Start, jcrossaxis_align_t::Stretch);
      _tabs->GetLayout<RowLayout>()->SetGap(0);
      _cards->SetLayout<CardLayout>();
      _cards->GetLayout<CardLayout>()->SetGap({0, 0});

      SetLayout<BorderLayout>(jpoint_t<int>{0, 0});

      Add(_tabs, jborderlayout_align_t::North);
      Add(_cards, jborderlayout_align_t::Center);
    }

		virtual ~TabbedContainer()
		{
      RemoveAll();

      _tabs->RemoveAll();

      for (auto item : _items) {
        delete item.tab;
      }

      _items.clear();

      _cards->RemoveAll();

      delete _cards;
		}

    virtual void AddTab(std::string id, Component *cmp)
    {
      Button *tab = new Button(id);

      tab->SetBackground<SolidBackground>();
      tab->SetBorder(_border);
      tab->RegisterActionListener(this);

      _tabs->Add(tab);
      _tabs->SetPreferredSize(_tabs->GetLayout()->GetPreferredLayoutSize(_tabs));
      
      if (_items.size() > 0) {
        cmp->SetVisible(false);
      } else {
        cmp->SetVisible(true);

        _current_id = id;
      }

      _cards->Add(cmp);

      _items.emplace_back(jtab_item_t {id, tab, cmp});
    }

    virtual void RemoveTab(std::string id)
    {
      jtab_item_t *current = GetCurrentItem();

      if (current->id == id) {
        if (_items.empty() == true) {
          SetCurrentItem("");
        } else {
          SetCurrentItem(_items[0].id);
        }
      }

      for (std::vector<jtab_item_t>::iterator i=_items.begin(); i!=_items.end(); i++) {
        if (i->id == id) {
          _tabs->Remove(i->tab);

          delete i->tab;

          _items.erase(i);

          break;
        }
      }
      
      _tabs->SetPreferredSize(_tabs->GetLayout()->GetPreferredLayoutSize(_tabs));
    }

    virtual void SetCurrentItem(std::string id)
    {
      jtab_item_t *current = GetCurrentItem();

      current->cmp->SetVisible(false);

      for (auto &item : _items) {
        if (item.id == id) {
          item.cmp->SetVisible(true);

          break;
        }
      }
    }

    virtual jtab_item_t * GetCurrentItem()
    {
      for (auto &item : _items) {
        if (item.id == _current_id) {
          return &item;
        }
      }

      return nullptr;
    }

		virtual void ActionPerformed(ActionEvent *event)
		{
      jtab_item_t *item = GetCurrentItem();

      item->cmp->SetVisible(false);

      Button *button = reinterpret_cast<Button *>(event->GetSource());

      SetCurrentItem(button->GetTextComponent()->GetText());
		}

};

class App : public Frame, public ActionListener {

	private:
    TabbedContainer
      _container;

	public:
		App():
			Frame({1280, 720})
		{
      SetLayout<GridLayout>(1, 1);

      _container.AddTab("Tab 01", new Button("Screen 01"));
      _container.AddTab("Tab 02", new Button("Screen 02"));
      _container.AddTab("Tab 03", new Button("Screen 03"));
      _container.AddTab("Tab 04", new Button("Screen 04"));

      Add(&_container);
    }

		virtual ~App()
		{
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<App>();

	app->SetTitle("App");

	Application::Loop();

	return 0;
}

