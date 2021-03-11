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
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindowlistener.h"
#include "jcanvas/widgets/jframe.h"
#include "jcanvas/widgets/janimation.h"
#include "jcanvas/widgets/jmarquee.h"
#include "jcanvas/widgets/jtextfield.h"
#include "jcanvas/widgets/jtextarea.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jprogressbar.h"
#include "jcanvas/widgets/jspin.h"
#include "jcanvas/widgets/jslider.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jcheckbutton.h"
#include "jcanvas/widgets/jcheckbuttongroup.h"
#include "jcanvas/widgets/jlistbox.h"
#include "jcanvas/widgets/jflatimage.h"
#include "jcanvas/widgets/jscrollbar.h"

#include <iostream>
#include <mutex>

using namespace jcanvas;

class ImageAnimation : public Animation {

  private:
    std::vector<Image *>
      _images;
    int 
      _index = 0;

  public:
    ImageAnimation(jrect_t<int> bounds):
      Animation(std::chrono::milliseconds(0), std::chrono::milliseconds(2000))
    {
      SetBounds(bounds);

			_images.push_back(new BufferedImage("images/tux-alien.png"));
			_images.push_back(new BufferedImage("images/tux-bart.png"));
			_images.push_back(new BufferedImage("images/tux-batman.png"));
			_images.push_back(new BufferedImage("images/tux-freddy.png"));
			_images.push_back(new BufferedImage("images/tux-homer.png"));
			_images.push_back(new BufferedImage("images/tux-indiana.png"));
			_images.push_back(new BufferedImage("images/tux-ipod.png"));
			_images.push_back(new BufferedImage("images/tux-jamaican.png"));
			_images.push_back(new BufferedImage("images/tux-jason.png"));
			_images.push_back(new BufferedImage("images/tux-kenny.png"));
			_images.push_back(new BufferedImage("images/tux-mario.png"));
			_images.push_back(new BufferedImage("images/tux-neo.png"));
			_images.push_back(new BufferedImage("images/tux-potter.png"));
			_images.push_back(new BufferedImage("images/tux-raider.png"));
			_images.push_back(new BufferedImage("images/tux-rambo.png"));
			_images.push_back(new BufferedImage("images/tux-rapper.png"));
			_images.push_back(new BufferedImage("images/tux-shrek.png"));
			_images.push_back(new BufferedImage("images/tux-spiderman.png"));
			_images.push_back(new BufferedImage("images/tux-turtle.png"));
			_images.push_back(new BufferedImage("images/tux-wolverine.png"));
			_images.push_back(new BufferedImage("images/tux-zombie.png"));
    }

    virtual ~ImageAnimation()
    {
      for (auto image : _images) {
        delete image;
      }
    }

    virtual void Update(std::chrono::milliseconds tick)
    {
      _index = (_index + 1) % _images.size();
    }

    virtual void Render(Graphics *g)
    {
      g->DrawImage(_images[_index], {0, 0, GetSize()});
    }

};

class Widgets : public Frame, public ActionListener, public SelectListener, public ToggleListener {

	private:
    std::mutex _mutex;
		ImageAnimation 
			*_animation;
		Marquee 
			*_marquee;
		TextField 
			*_textfield;
		TextArea 
			*_textarea;
		ProgressBar 
			*_progress;
		Button 
			*_button1,
			*_button2,
			*_button3;
		Spin 
			*_spin;
		CheckButton 
			*_check1,
			*_check2,
			*_check3;
		CheckButton 
			*_radio1,
			*_radio2,
			*_radio3;
		CheckButtonGroup 
			*_group;
		Text 
			*_label1,
			*_label2;
		ListBox 
			*_list;
		FlatImage 
			*_staticimage;
		Slider 
			*_slider;
		ScrollBar 
			*_scroll;
		Button 
			*_imagebutton1,
			*_imagebutton2,
			*_imagebutton3;
		Button 
			*_toggle;
		jtheme_t
			_theme1,
			_theme2,
			_theme3,
			_theme4;

	public:
		Widgets():
			Frame({1280, 720})
	{
    jinsets_t
      insets = GetInsets();

    _theme1 = GetTheme();
		_theme1.bg.focus = 0xff40f040;
		
    _theme2 = GetTheme();
    _theme2.bg.focus = 0xfff02020;
		
    _theme3 = GetTheme();
    _theme3.bg.focus = 0xfff0f040;

    _theme4 = GetTheme();
		_theme4.bg.normal = 0xe0358035; // component
		_theme4.bg.normal = 0xa0755535; // window
		_theme4.bg.normal = 0xe0f08035; // item

		{
			_animation = new ImageAnimation({insets.left, insets.top, 96, 96});

      _animation->Start();
		}

    jpoint_t
      size = GetSize();

		{
			_button1 = new Button("Increase");
			_button2 = new Button("Decrease");
			_button3 = new Button("Testing a long text in a buttom component");
      
      _button1->SetBounds({insets.left, insets.top + 1*(96 + 16), 196, 96});
      _button2->SetBounds({insets.left, insets.top + 2*(96 + 16), 196, 96});
      _button3->SetBounds({insets.left, insets.top + 3*(96 + 16), 196, 96});

			_button1->SetTheme(_theme1);
			_button2->SetTheme(_theme2);
			_button3->SetTheme(_theme3);

			_button1->RegisterActionListener(this);
			_button2->RegisterActionListener(this);
		}

		{
      jrect_t
        region = _button3->GetVisibleBounds();

			_toggle = new Button("Toggle Button", nullptr);
      
      _toggle->SetBounds({insets.left, region.point.y + region.size.y + 8, 196, 48});
		}

		{
      jrect_t
        region = _toggle->GetVisibleBounds();

			_imagebutton1 = new Button("", new BufferedImage("images/alert_icon.png"));
			_imagebutton2 = new Button("", new BufferedImage("images/info_icon.png"));
			_imagebutton3 = new Button("", new BufferedImage("images/error_icon.png"));
      
      _imagebutton1->SetBounds({insets.left + 0*(48 + 18), region.point.y + 1*(region.size.y + 8), 64, 64});
      _imagebutton2->SetBounds({insets.left + 1*(48 + 18), region.point.y + 1*(region.size.y + 8), 64, 64});
      _imagebutton3->SetBounds({insets.left + 2*(48 + 18), region.point.y + 1*(region.size.y + 8), 64, 64});
		}

		{
      jrect_t
        region = _imagebutton1->GetVisibleBounds();

			_spin = new Spin();
      
      _spin->SetBounds({insets.left+0*(32+16), region.point.y + region.size.y + 8, 196, 48});

			_spin->AddTextItem("loop");
			_spin->AddTextItem("bounce");

			_spin->RegisterSelectListener(this);
		}

		{
			_marquee = new Marquee("Marquee Test", std::chrono::milliseconds(100));
      
      _marquee->SetBounds({insets.left + 196 + 16, insets.top, size.x - 2*(196 + 16) - insets.left - insets.right, 48});

      _marquee->Start();
		}

		{
      jrect_t
        region = _marquee->GetVisibleBounds();

			_progress = new ProgressBar();
			_slider = new Slider();
			_scroll = new ScrollBar();
      
      _progress->SetBounds({insets.left + 196 + 16, region.point.y + 1*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 48});
      _slider->SetBounds({insets.left + 196 + 16, region.point.y + 2*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 48});
      _scroll->SetBounds({insets.left + 196 + 16, region.point.y + 3*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 48});

			_progress->SetValue(20.0);
			_slider->SetValue(20.0);
			_scroll->SetValue(20.0);
		}

		{
      jrect_t
        region = _marquee->GetVisibleBounds();

			_textfield = new TextField();
      
      _textfield->SetBounds({insets.left + 196 + 16, region.point.y + 4*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 48});

			_textfield->Insert("Text Field");
		}

		{
      jrect_t
        region = _marquee->GetVisibleBounds();

			_textarea = new TextArea();
      
      _textarea->SetBounds({insets.left + 196 + 16, region.point.y + 5*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 96});

			_textarea->Insert("Text Area\nwriting some text ...\nbye bye");
		}

		{
      jrect_t
        region = _textarea->GetVisibleBounds();

			_label1 = new Text("Text Test 1");
      
      _label1->SetBounds({insets.left + 196 + 16, region.point.y + region.size.y + 8, (region.size.x - 16)/2, 48});
      
      region = _label1->GetVisibleBounds();

			_label2 = new Text("Text Test 2");
      
      _label2->SetBounds({region.point.x + region.size.x + 16, region.point.y, region.size.x, 48});
		}

		{
      jrect_t
        region = _label1->GetVisibleBounds();

			_check1 = new CheckButton(jcheckbox_type_t::Check, "Wrap Text");
			_check2 = new CheckButton(jcheckbox_type_t::Check, "Password");
			_check3 = new CheckButton(jcheckbox_type_t::Check, "Hide");
      
      _check1->SetBounds({region.point.x, region.point.y + 1*(region.size.y + 8), region.size.x, 48});
      _check2->SetBounds({region.point.x, region.point.y + 2*(region.size.y + 8), region.size.x, 48});
      _check3->SetBounds({region.point.x, region.point.y + 3*(region.size.y + 8), region.size.x, 48});

			_check1->SetSelected(true);

			_check1->RegisterToggleListener(this);
			_check2->RegisterToggleListener(this);
			_check3->RegisterToggleListener(this);
		}

		{
      jrect_t
        region = _label2->GetVisibleBounds();

			_radio1 = new CheckButton(jcheckbox_type_t::Radio, "Left");
			_radio2 = new CheckButton(jcheckbox_type_t::Radio, "Center");
			_radio3 = new CheckButton(jcheckbox_type_t::Radio, "Right");
      
      _radio1->SetBounds({region.point.x, region.point.y + 1*(region.size.y + 8), region.size.x, 48});
      _radio2->SetBounds({region.point.x, region.point.y + 2*(region.size.y + 8), region.size.x, 48});
      _radio3->SetBounds({region.point.x, region.point.y + 3*(region.size.y + 8), region.size.x, 48});

			_group = new CheckButtonGroup();

			_radio2->SetSelected(true);

			_group->Add(_radio1);
			_group->Add(_radio2);
			_group->Add(_radio3);

			_radio1->RegisterToggleListener(this);
			_radio2->RegisterToggleListener(this);
			_radio3->RegisterToggleListener(this);
		}

		{
      jrect_t
        region = _marquee->GetVisibleBounds();

			_staticimage = new FlatImage(new BufferedImage(JCANVAS_RESOURCES_DIR "/images/green_icon.png"));
      
      _staticimage->SetBounds({region.point.x + region.size.x + 16, region.point.y, 196, 196});
		}

		{
      jrect_t
        region = _staticimage->GetVisibleBounds();

			_list = new ListBox();
      
      _list->SetBounds({region.point.x, region.point.y + region.size.y + 8, 196, 196});

			_list->AddImageItem("Item 01", new BufferedImage(("images/tux-alien.png")));
			_list->AddTextItem("Item 02");
			_list->AddImageItem("Item 03", new BufferedImage(("images/tux-bart.png")));
			_list->AddTextItem("Item 04");
			_list->AddImageItem("Item 05", new BufferedImage(("images/tux-batman.png")));
			_list->AddTextItem("Item 06");
			_list->AddImageItem("Item 07", new BufferedImage(("images/tux-freddy.png")));
			_list->AddTextItem("Item 08");
			_list->AddImageItem("Item 09", new BufferedImage(("images/tux-homer.png")));
			_list->AddTextItem("Item 10");
			_list->AddImageItem("Item 11", new BufferedImage(("images/tux-indiana.png")));
			_list->AddTextItem("Item 12");
			_list->AddImageItem("Item 13", new BufferedImage(("images/tux-ipod.png")));
			_list->AddTextItem("Item 14");
			_list->AddImageItem("Item 15", new BufferedImage(("images/tux-jamaican.png")));
			_list->AddTextItem("Item 16");
			_list->AddImageItem("Item 17", new BufferedImage(("images/tux-jason.png")));
			_list->AddTextItem("Item 18");
			_list->AddImageItem("Item 19", new BufferedImage(("images/tux-kenny.png")));
			_list->AddTextItem("Item 20");
			_list->AddImageItem("Item 21", new BufferedImage(("images/tux-mario.png")));
			_list->AddTextItem("Item 22");
			_list->AddImageItem("Item 23", new BufferedImage(("images/tux-neo.png")));
			_list->AddTextItem("Item 24");
			_list->AddImageItem("Item 25", new BufferedImage(("images/tux-potter.png")));
			_list->AddTextItem("Item 26");
			_list->AddImageItem("Item 27", new BufferedImage(("images/tux-raider.png")));
			_list->AddTextItem("Item 28");
			_list->AddImageItem("Item 29", new BufferedImage(("images/tux-rambo.png")));
			_list->AddTextItem("Item 30");
			_list->AddImageItem("Item 31", new BufferedImage(("images/tux-rapper.png")));
			_list->AddTextItem("Item 32");
			_list->AddImageItem("Item 33", new BufferedImage(("images/tux-shrek.png")));
			_list->AddTextItem("Item 34");
			_list->AddImageItem("Item 35", new BufferedImage(("images/tux-spiderman.png")));
			_list->AddTextItem("Item 36");
			_list->AddImageItem("Item 37", new BufferedImage(("images/tux-turtle.png")));
			_list->AddTextItem("Item 38");
			_list->AddImageItem("Item 39", new BufferedImage(("images/tux-wolverine.png")));
			_list->AddTextItem("Item 40");
			_list->AddImageItem("Item 41", new BufferedImage(("images/tux-zombie.png")));
			_list->AddTextItem("Item 42");
		}

		Add(_marquee);
		Add(_progress);
		Add(_slider);
		Add(_scroll);
		Add(_textfield);
		Add(_textarea);
		Add(_button1);
		Add(_button2);
		Add(_button3);
		Add(_imagebutton1);
		Add(_imagebutton2);
		Add(_imagebutton3);
		Add(_label1);
		Add(_label2);
		Add(_check1);
		Add(_check2);
		Add(_check3);
		Add(_radio1);
		Add(_radio2);
		Add(_radio3);
		Add(_toggle);
		Add(_spin);
		Add(_staticimage);
		Add(_animation);
		Add(_list);

		_button1->RequestFocus();
	}

	virtual ~Widgets()
	{
    RemoveAll();

		_group->Remove(_radio1);
		_group->Remove(_radio2);
		_group->Remove(_radio3);

    delete _animation;

		delete _group;
		delete _marquee;
		delete _textfield;
		delete _textarea;
		delete _progress;
		delete _button1;
		delete _button2;
		delete _button3;
		delete _check1;
		delete _check2;
		delete _check3;
		delete _radio1;
		delete _radio2;
		delete _radio3;
		delete _toggle;
		delete _imagebutton1;
		delete _imagebutton2;
		delete _imagebutton3;
		delete _spin;
		delete _slider;
		delete _scroll;
		delete _staticimage;
		delete _list;
		delete _label1;
		delete _label2;
	}

	virtual void StateChanged(ToggleEvent *event)
	{
		_mutex.lock();

		if (event->GetSource() == _check1 ||
				event->GetSource() == _check2 ||
				event->GetSource() == _check3) {
			if (_check1->IsSelected() == true) {
				_textarea->SetWrap(true);
			} else {
				_textarea->SetWrap(false);
			}

			if (_check2->IsSelected() == true) {
				_textarea->SetEchoChar('*');
			} else {
				_textarea->SetEchoChar('\0');
			}

			if (_check3->IsSelected() == true) {
				_textarea->SetVisible(false);
			} else {
				_textarea->SetVisible(true);
			}
		} else if (event->GetSource() == _radio1) {
			_label1->SetHorizontalAlign(jhorizontal_align_t::Left);
			_label2->SetHorizontalAlign(jhorizontal_align_t::Left);
		} else if (event->GetSource() == _radio2) {
			_label1->SetHorizontalAlign(jhorizontal_align_t::Center);
			_label2->SetHorizontalAlign(jhorizontal_align_t::Center);
		} else if (event->GetSource() == _radio3) {
			_label1->SetHorizontalAlign(jhorizontal_align_t::Right);
			_label2->SetHorizontalAlign(jhorizontal_align_t::Right);
		}
		
    _mutex.unlock();
	}

	virtual void ItemChanged(SelectEvent *event)
	{
    _mutex.lock();

		if (_spin->GetCurrentIndex() == 0) {
			// _marquee->SetType(JMM_LOOP);
		} else if (_spin->GetCurrentIndex() == 1) {
			// _marquee->SetType(JMM_BOUNCE);
		}

    _mutex.unlock();
	}

	virtual void ActionPerformed(ActionEvent *event)
	{
    _mutex.lock();

		if (event->GetSource() == _button1) {
			_progress->SetValue(_progress->GetValue() + 10);
			_slider->SetValue(_slider->GetValue() + 10);
		} else if (event->GetSource() == _button2) {
			_progress->SetValue(_progress->GetValue() - 10);
			_slider->SetValue(_slider->GetValue() - 10);
		} else if (event->GetSource() == _button3) {
		}
    
    _mutex.unlock();
	}

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	Widgets app;

  app.Exec();

  Application::Loop();

	return 0;
}
