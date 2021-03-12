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
#include "jcanvas/widgets/jflowlayout.h"

using namespace jcanvas;

class RectangleContainer : public Container {

	public:
		RectangleContainer():
			Container()
		{
		}

		virtual ~RectangleContainer()
		{
		}

		virtual void Paint(Graphics *g) 
		{
			Container::Paint(g);

      jpoint_t
        size = GetSize();

			g->SetColor({0xff, 0xff, 0x00, 0xff});
			g->DrawRectangle({0, 0, size.x, size.y});
		}
};

/**
 * \brief Ajuste o PreferredSize() caso deseje um nivel diferente de adaptacao dos layouts.
 *
 */
class Layouts : public Frame, public ActionListener {

	private:
    std::shared_ptr<Container>
      _cardcontainer;
    std::shared_ptr<Button>
      _first,
			_last,
			_previous,
			_next;

	public:
		Layouts():
			Frame({1280, 720})
		{
    }

		virtual ~Layouts()
		{
			RemoveAll();
		}

    void Init()
    {
			SetLayout<GridLayout>(2, 3);

      jtheme_t theme = GetTheme();

			theme.bg.normal = 0xff404090;
			theme.fg.normal = 0xff000000;
			
#define LAYOUT(id) \
        auto b = std::make_shared<RectangleContainer>(); \
        auto c = std::make_shared<Container>(); \
        auto l = std::make_shared<Text>(#id); \
        l->SetTheme(theme); \
        b->SetLayout<BorderLayout>(); \
        c->SetLayout<id>(); \
        b->Add(l, jborderlayout_align_t::North); \
        b->Add(c, jborderlayout_align_t::Center); \
        Add(b); \

      {
        LAYOUT(FlowLayout);

        c->Add(std::make_shared<Button>("Button 1"));
        c->Add(std::make_shared<Button>("Button 2"));
        c->Add(std::make_shared<Button>("Button 3"));
        c->Add(std::make_shared<Button>("Button 4"));
        c->Add(std::make_shared<Button>("Button 5"));
      }
			
      {
        LAYOUT(GridLayout);

        c->SetLayout<GridLayout>(3, 3);
        c->Add(std::make_shared<Button>("Button 1"));
        c->Add(std::make_shared<Button>("Button 2"));
        c->Add(std::make_shared<Button>("Button 3"));
        c->Add(std::make_shared<Button>("Button 4"));
        c->Add(std::make_shared<Button>("Button 5"));
      }

      {
        LAYOUT(BorderLayout);

        c->Add(std::make_shared<Button>("Button 1"), jborderlayout_align_t::North);
        c->Add(std::make_shared<Button>("Button 2"), jborderlayout_align_t::South);
        c->Add(std::make_shared<Button>("Button 3"), jborderlayout_align_t::West);
        c->Add(std::make_shared<Button>("Button 4"), jborderlayout_align_t::East);
        c->Add(std::make_shared<Button>("Button 5"), jborderlayout_align_t::Center);
      }

			// cardlayout
      {
        LAYOUT(CardLayout);
			
        auto buttons = std::make_shared<Container>();
        auto cards = std::make_shared<Container>();

        _cardcontainer = cards;

        buttons->SetLayout<FlowLayout>();

        _first = std::make_shared<Button>("First");
        _next = std::make_shared<Button>("Next");
        _previous = std::make_shared<Button>("Previous");
        _last = std::make_shared<Button>("Last");

        _first->RegisterActionListener(this);
        _previous->RegisterActionListener(this);
        _next->RegisterActionListener(this);
        _last->RegisterActionListener(this);

        buttons->Add(_first);
        buttons->Add(_next);
        buttons->Add(_previous);
        buttons->Add(_last);

        cards->SetLayout<CardLayout>();
        cards->Add(std::make_shared<Button>("First Screen"), "01");
        cards->Add(std::make_shared<Button>("Second Screen"), "02");
        cards->Add(std::make_shared<Button>("Third Screen"), "03");
        cards->Add(std::make_shared<Button>("Last Screen"), "04");

        buttons->SetPreferredSize(buttons->GetComponents()[0]->GetPreferredSize());

        c->SetLayout<BorderLayout>();
        c->Add(buttons, jborderlayout_align_t::East);
        c->Add(cards, jborderlayout_align_t::Center);
      }

      {
        LAYOUT(GridBagLayout);

        bool 
          shouldFill = true,
          shouldWeightX = true;

        GridBagConstraints *ct = new GridBagConstraints();

        if (shouldFill) {
          ct->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
        }

        if (shouldWeightX) {
          ct->weightx = 0.5;
        }

        ct->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
        ct->gridx = 0;
        ct->gridy = 0;
        c->Add(std::make_shared<Button>("Button 1"), ct);

        ct->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
        ct->weightx = 0.5;
        ct->gridx = 1;
        ct->gridy = 0;
        c->Add(std::make_shared<Button>("Button 2"), ct);

        ct->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
        ct->weightx = 0.5;
        ct->gridx = 2;
        ct->gridy = 0;
        c->Add(std::make_shared<Button>("Button 3"), ct);

        ct->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
        ct->ipady = 40;      // make this component tall
        ct->weightx = 0.0;
        ct->gridwidth = 3;
        ct->gridx = 0;
        ct->gridy = 1;
        c->Add(std::make_shared<Button>("Button 4"), ct);

        ct->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
        ct->ipady = 0;       // reset to default
        ct->weighty = 1.0;   // request any extra vertical space
        ct->weightx = 0.0;   // request any extra vertical space
        ct->anchor = static_cast<int>(jgridbaglayout_constraints_t::LastLineStart); // PAGE_END; //bottom of space
        ct->insets.left = 0;
        ct->insets.right = 0;
        ct->insets.top = 0;
        ct->insets.bottom = 0;
        ct->gridx = 1;       // aligned with button 2
        ct->gridwidth = 2;   // 2 columns wide
        ct->gridy = 2;       // third row
        c->Add(std::make_shared<Button>("Button 5"), ct);
      }

			// nulllayout
      {
        LAYOUT(NullLayout);

        std::vector<std::shared_ptr<Button>> buttons {
          std::make_shared<Button>("Button 1"),
          std::make_shared<Button>("Button 2"),
          std::make_shared<Button>("Button 3"),
          std::make_shared<Button>("Button 4"),
          std::make_shared<Button>("Button 5")
        };

        for (int i=0; i<(int)buttons.size(); i++) {
          buttons[i]->SetBounds({i*40 + 8, i*(48 + 8), 128, 48});

          c->Add(buttons[i]);
        }
      }
		}

		virtual void ActionPerformed(ActionEvent *event)
		{
      std::shared_ptr<CardLayout> card = std::dynamic_pointer_cast<CardLayout>(_cardcontainer->GetLayout());

			if (event->GetSource() == _first.get()) {
				card->First(_cardcontainer);
			} else if (event->GetSource() == _last.get()) {
				card->Last(_cardcontainer);
			} else if (event->GetSource() == _previous.get()) {
				card->Previous(_cardcontainer);
			} else if (event->GetSource() == _next.get()) {
				card->Next(_cardcontainer);
			}
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<Layouts>();

  app->Init();
	app->SetTitle("Layouts");

	Application::Loop();

	return 0;
}

