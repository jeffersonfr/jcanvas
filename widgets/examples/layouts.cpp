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
		RectangleContainer(int x, int y, int w, int h):
			Container({x, y, w, h})
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
class Main : public Frame, public ActionListener {

	private:
		std::vector<Container *> 
      _b,
			_c;
		std::vector<Button *> 
      _buttons;
    std::shared_ptr<Layout> 
      _main,
			_flow,
			_grid,
			_border,
			_card,
			_null,
			_gridbag;
		Button *_first,
			*_last,
			*_previous,
			*_next;
		jtheme_t
      _theme;

	public:
		Main():
			Frame({1280, 720})
		{
      _main = std::make_shared<GridLayout>(2, 3);
			_flow = std::make_shared<FlowLayout>();
			_grid = std::make_shared<GridLayout>(3, 3);
			_border = std::make_shared<BorderLayout>();
			_card = std::make_shared<BorderLayout>();
			_gridbag = std::make_shared<GridBagLayout>();
			_null = std::make_shared<NullLayout>();

			SetLayout(_main);

			for (int i=0; i<6; i++) {
				// _b.push_back(new Container(0, 0, 0, 0));
				_b.push_back(new RectangleContainer(0, 0, 0, 0));
				_c.push_back(new Container());

        auto border_layout = std::make_shared<BorderLayout>();

				_b[i]->SetLayout(border_layout);
				_b[i]->Add(_c[i], jborderlayout_align_t::Center);

				Add(_b[i]);
			}

			_c[0]->SetLayout(_flow);
			_c[1]->SetLayout(_grid);
			_c[2]->SetLayout(_border);
			_c[3]->SetLayout(_card);
			_c[4]->SetLayout(_gridbag);
			_c[5]->SetLayout(_null);

			// flowlayout
			_c[0]->Add(new Button("Button 1"));
			_c[0]->Add(new Button("Button 2"));
			_c[0]->Add(new Button("Button 3"));
			_c[0]->Add(new Button("Button 4"));
			_c[0]->Add(new Button("Button 5"));
			
			// gridlayout
			_c[1]->Add(new Button("Button 1"));
			_c[1]->Add(new Button("Button 2"));
			_c[1]->Add(new Button("Button 3"));
			_c[1]->Add(new Button("Button 4"));
			_c[1]->Add(new Button("Button 5"));

			// gridlayout
			_c[2]->Add(new Button("Button 1"), jborderlayout_align_t::North);
			_c[2]->Add(new Button("Button 2"), jborderlayout_align_t::South);
			_c[2]->Add(new Button("Button 3"), jborderlayout_align_t::West);
			_c[2]->Add(new Button("Button 4"), jborderlayout_align_t::East);
			_c[2]->Add(new Button("Button 5"), jborderlayout_align_t::Center);

			// cardlayout
			_c.push_back(new Container());
			_c.push_back(new Container());

      auto flow_layout = std::make_shared<FlowLayout>();

			_c[6]->SetLayout(flow_layout);

			_first = new Button("First");
			_next = new Button("Next");
			_previous = new Button("Previous");
			_last = new Button("Last");

			_c[6]->Add(_first);
			_c[6]->Add(_next);
			_c[6]->Add(_previous);
			_c[6]->Add(_last);

			_first->RegisterActionListener(this);
			_previous->RegisterActionListener(this);
			_next->RegisterActionListener(this);
			_last->RegisterActionListener(this);

      auto card_layout = std::make_shared<CardLayout>();

			_c[7]->SetLayout(card_layout);
			_c[7]->Add(new Button("First Screen"), "01");
			_c[7]->Add(new Button("Second Screen"), "02");
			_c[7]->Add(new Button("Third Screen"), "03");
			_c[7]->Add(new Button("Last Screen"), "04");

      _c[6]->SetPreferredSize(_c[6]->GetComponents()[0]->GetPreferredSize());

			_b[3]->Add(_c[6], jborderlayout_align_t::East);
			_b[3]->Add(_c[7], jborderlayout_align_t::Center);

			// gridbaglayout
			bool shouldFill = true,
					 shouldWeightX = true;

			GridBagConstraints *c = new GridBagConstraints();

			if (shouldFill) {
				c->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
			}

			if (shouldWeightX) {
				c->weightx = 0.5;
			}

			c->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
			c->gridx = 0;
			c->gridy = 0;
			_c[4]->Add(new Button("Button 1"), c);

			c->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
			c->weightx = 0.5;
			c->gridx = 1;
			c->gridy = 0;
			_c[4]->Add(new Button("Button 2"), c);

			c->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
			c->weightx = 0.5;
			c->gridx = 2;
			c->gridy = 0;
			_c[4]->Add(new Button("Button 3"), c);

			c->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
			c->ipady = 40;      // make this component tall
			c->weightx = 0.0;
			c->gridwidth = 3;
			c->gridx = 0;
			c->gridy = 1;
			_c[4]->Add(new Button("Button 4"), c);

			c->fill = static_cast<int>(jgridbaglayout_constraints_t::Horizontal);
			c->ipady = 0;       // reset to default
			c->weighty = 1.0;   // request any extra vertical space
			c->weightx = 0.0;   // request any extra vertical space
			c->anchor = static_cast<int>(jgridbaglayout_constraints_t::LastLineStart); // PAGE_END; //bottom of space
			c->insets.left = 0;
			c->insets.right = 0;
			c->insets.top = 0;
			c->insets.bottom = 0;
			c->gridx = 1;       // aligned with button 2
			c->gridwidth = 2;   // 2 columns wide
			c->gridy = 2;       // third row
			_c[4]->Add(new Button("Button 5"), c);

			// nulllayout
      Button *b01 = new Button("Button 1");
      Button *b02 = new Button("Button 2");
      Button *b03 = new Button("Button 3");
      Button *b04 = new Button("Button 4");
      Button *b05 = new Button("Button 5");

      b01->SetBounds({0*40 + 8, 0*(48 + 8), 128, 48});
      b02->SetBounds({1*40 + 8, 1*(48 + 8), 128, 48});
      b03->SetBounds({2*40 + 8, 2*(48 + 8), 128, 48});
      b04->SetBounds({3*40 + 8, 3*(48 + 8), 128, 48});
      b05->SetBounds({4*40 + 8, 4*(48 + 8), 128, 48});

			_c[5]->Add(b01);
			_c[5]->Add(b02);
			_c[5]->Add(b03);
			_c[5]->Add(b04);
			_c[5]->Add(b05);
			
			// adicionando a legenda nos containers
			Text *l1 = new Text("FlowLayout"),
				*l2 = new Text("GridLayout"),
				*l3 = new Text("BorderLayout"),
				*l4 = new Text("CardLayout"),
				*l5 = new Text("GridBagLayout"),
				*l6 = new Text("NullLayout");
			
			_theme.bg.normal = 0xff404040;
			
			l1->SetTheme(_theme);
			l2->SetTheme(_theme);
			l3->SetTheme(_theme);
			l4->SetTheme(_theme);
			l5->SetTheme(_theme);
			l6->SetTheme(_theme);
			
			_b[0]->Add(l1, jborderlayout_align_t::North);
			_b[1]->Add(l2, jborderlayout_align_t::North);
			_b[2]->Add(l3, jborderlayout_align_t::North);
			_b[3]->Add(l4, jborderlayout_align_t::North);
			_b[4]->Add(l5, jborderlayout_align_t::North);
			_b[5]->Add(l6, jborderlayout_align_t::North);
		}

		virtual ~Main()
		{
			RemoveAll();

			for (int i=0; i<6; i++) {
				Container *bcontainer = _b[i];
				Container *ccontainer = _c[i];

				ccontainer->RemoveAll();
				bcontainer->RemoveAll();

				for (std::vector<Component *>::const_iterator iccomponent=ccontainer->GetComponents().begin(); iccomponent!=ccontainer->GetComponents().end(); iccomponent++) {
					delete (*iccomponent);
				}

				for (std::vector<Component *>::const_iterator ibcomponent=bcontainer->GetComponents().begin(); ibcomponent!=bcontainer->GetComponents().end(); ibcomponent++) {
					delete (*ibcomponent);
				}

				delete ccontainer;
				delete bcontainer;
			}
		}

		virtual void ActionPerformed(ActionEvent *event)
		{
      std::shared_ptr<CardLayout> card = std::dynamic_pointer_cast<CardLayout>(_c[7]->GetLayout());

			if (event->GetSource() == _first) {
				card->First(_c[7]);
			} else if (event->GetSource() == _last) {
				card->Last(_c[7]);
			} else if (event->GetSource() == _previous) {
				card->Previous(_c[7]);
			} else if (event->GetSource() == _next) {
				card->Next(_c[7]);
			}
		}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Main app;

	app.SetTitle("Layouts");

	Application::Loop();

	return 0;
}

