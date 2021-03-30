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

class ImageAnimation : public Component, public Animation {

  private:
    std::vector<std::shared_ptr<Image>>
      _images;
    std::chrono::milliseconds
      _current;
    int 
      _index = 0;

  public:
    ImageAnimation(jrect_t<int> bounds):
      Animation()
    {
      SetBounds(bounds);

			_images.push_back(std::make_shared<BufferedImage>("images/tux-alien.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-bart.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-batman.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-freddy.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-homer.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-indiana.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-ipod.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-jamaican.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-jason.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-kenny.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-mario.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-neo.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-potter.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-raider.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-rambo.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-rapper.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-shrek.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-spiderman.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-turtle.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-wolverine.png"));
			_images.push_back(std::make_shared<BufferedImage>("images/tux-zombie.png"));
    }

    virtual ~ImageAnimation()
    {
    }

    virtual void Update(std::chrono::milliseconds tick)
    {
      _current = _current + tick;

      if (_current < std::chrono::seconds{1}) {
        return;
      }

      if (_current > std::chrono::seconds{2}) {
        _current = std::chrono::seconds{2};
      }

      _current = _current - std::chrono::seconds{1};
      _index = (_index + 1) % _images.size();
    }

    virtual void Paint(Graphics *g)
    {
      g->DrawImage(_images[_index], {0, 0, GetSize()});
    }

};

class App : public Frame, public ActionListener, public SelectListener {

	private:
    std::mutex
      _mutex;
    ImageAnimation
			*_animation;
		Marquee
			*_marquee;
		Text
			*_textfield;
		Text
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
			_group;
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

	public:
		App():
			Frame({1280, 720})
    {
    }

    virtual ~App()
    {
      UnregisterAnimation(_animation);
      UnregisterAnimation(_marquee);

      _group.Remove(_radio1);
      _group.Remove(_radio2);
      _group.Remove(_radio3);

      RemoveAll();
      
      delete _marquee;
      delete _progress;
      delete _slider;
      delete _scroll;
      delete _textfield;
      delete _textarea;
      delete _button1;
      delete _button2;
      delete _button3;
      delete _imagebutton1;
      delete _imagebutton2;
      delete _imagebutton3;
      delete _label1;
      delete _label2;
      delete _check1;
      delete _check2;
      delete _check3;
      delete _radio1;
      delete _radio2;
      delete _radio3;
      delete _toggle;
      delete _spin;
      delete _staticimage;
      delete _animation;
      delete _list;
    }

    void Init()
    {
      jinsets_t
        insets = GetInsets();

      {
        _animation = new ImageAnimation(jrect_t<int>{insets.left, insets.top, 96, 96});

        RegisterAnimation(_animation);
      }

      jpoint_t
        size = GetSize();

      {
        _button1 = new Button("Increase", std::make_shared<BufferedImage>("images/alert_icon.png"));
        _button2 = new Button("Decrease");
        _button3 = new Button("Testing a long text in a button component");
        
        _button1->SetBounds({insets.left, insets.top + 1*(96 + 16), 196, 96});
        _button2->SetBounds({insets.left, insets.top + 2*(96 + 16), 196, 96});
        _button3->SetBounds({insets.left, insets.top + 3*(96 + 16), 196, 96});

        _button1->GetTheme().fg.focus = jcolor_t {0xffff0000};
        _button2->GetTheme().fg.focus = jcolor_t {0xff00ff00};
        _button3->GetTheme().fg.focus = jcolor_t {0xff0000ff};

        _button1->RegisterActionListener(this);
        _button2->RegisterActionListener(this);
      }

      {
        jrect_t
          region = _button3->GetVisibleBounds();

        _toggle = new Button("Toggle Button", nullptr);
        
        _toggle->OnClick(
            [oldTheme=_toggle->GetTheme()](Button *thiz, bool down) {
              if (down == true) {
                thiz->SetPressed(!thiz->IsPressed());
              }

              if (thiz->IsPressed() == false) {
                thiz->GetTheme().bg.normal = oldTheme.bg.normal;
              } else {
                thiz->GetTheme().bg.normal = jcolor_t {0xffff0000};
              }
            });

        _toggle->SetBounds({insets.left, region.point.y + region.size.y + 8, 196, 48});
      }

      {
        jrect_t
          region = _toggle->GetVisibleBounds();

        _imagebutton1 = new Button("", std::make_shared<BufferedImage>("images/alert_icon.png"));
        _imagebutton2 = new Button("", std::make_shared<BufferedImage>("images/info_icon.png"));
        _imagebutton3 = new Button("", std::make_shared<BufferedImage>("images/error_icon.png"));
        
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
        _marquee = new Marquee("Marquee Test");
        
        _marquee->SetBounds({insets.left + 196 + 16, insets.top, size.x - 2*(196 + 16) - insets.left - insets.right, 48});

        RegisterAnimation(_marquee);
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

        _textfield = new Text("Text Field");
        
        _textfield->SetWrap(false);
        _textfield->SetBounds({insets.left + 196 + 16, region.point.y + 4*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 48});
      }

      {
        jrect_t
          region = _marquee->GetVisibleBounds();

        _textarea = new Text("Text Area\nwriting some text ...\nbye bye");
        
        _textarea->SetBounds({insets.left + 196 + 16, region.point.y + 5*(region.size.y + 8), size.x - 2*(196 + 16) - insets.left - insets.right, 96});
      }

      {
        jrect_t
          region = _textarea->GetVisibleBounds();

        _label1 = new Text("Text Test 1");
        
        _label1->SetEditable(false);
        _label1->SetFocusable(false);
        _label1->SetBounds({insets.left + 196 + 16, region.point.y + region.size.y + 8, (region.size.x - 16)/2, 48});
        
        region = _label1->GetVisibleBounds();

        _label2 = new Text("Text Test 2");
        
        _label2->SetEditable(false);
        _label2->SetFocusable(false);
        _label2->SetBounds({region.point.x + region.size.x + 16, region.point.y, region.size.x, 48});
      }

      {
        jrect_t
          region = _label1->GetVisibleBounds();

        _check1 = new CheckButton(jcheckbutton_type_t::Check, "Wrap Text");
        _check2 = new CheckButton(jcheckbutton_type_t::Check, "Password");
        _check3 = new CheckButton(jcheckbutton_type_t::Check, "Hide");
        
        _check1->SetBounds({region.point.x, region.point.y + 1*(region.size.y + 8), region.size.x, 48});
        _check2->SetBounds({region.point.x, region.point.y + 2*(region.size.y + 8), region.size.x, 48});
        _check3->SetBounds({region.point.x, region.point.y + 3*(region.size.y + 8), region.size.x, 48});

        _check1->Click();

        _check1->RegisterActionListener(this);
        _check2->RegisterActionListener(this);
        _check3->RegisterActionListener(this);
      }

      {
        jrect_t
          region = _label2->GetVisibleBounds();

        _radio1 = new CheckButton(jcheckbutton_type_t::Radio, "Left");
        _radio2 = new CheckButton(jcheckbutton_type_t::Radio, "Center");
        _radio3 = new CheckButton(jcheckbutton_type_t::Radio, "Right");
        
        _radio1->SetBounds({region.point.x, region.point.y + 1*(region.size.y + 8), region.size.x, 48});
        _radio2->SetBounds({region.point.x, region.point.y + 2*(region.size.y + 8), region.size.x, 48});
        _radio3->SetBounds({region.point.x, region.point.y + 3*(region.size.y + 8), region.size.x, 48});

        _group.Add(_radio1);
        _group.Add(_radio2);
        _group.Add(_radio3);

        _radio1->Click();

        _radio1->RegisterActionListener(this);
        _radio2->RegisterActionListener(this);
        _radio3->RegisterActionListener(this);
      }

      {
        jrect_t
          region = _marquee->GetVisibleBounds();

        _staticimage = new FlatImage(std::make_shared<BufferedImage>(JCANVAS_RESOURCES_DIR "/images/green_icon.png"));
        
        _staticimage->SetBounds({region.point.x + region.size.x + 16, region.point.y, 196, 196});
      }

      {
        jrect_t
          region = _staticimage->GetVisibleBounds();

        _list = new ListBox();
        
        _list->SetBounds({region.point.x, region.point.y + region.size.y + 8, 196, 196});

        _list->AddImageItem("Item 01", std::make_shared<BufferedImage>(("images/tux-alien.png")));
        _list->AddTextItem("Item 02");
        _list->AddImageItem("Item 03", std::make_shared<BufferedImage>(("images/tux-bart.png")));
        _list->AddTextItem("Item 04");
        _list->AddImageItem("Item 05", std::make_shared<BufferedImage>(("images/tux-batman.png")));
        _list->AddTextItem("Item 06");
        _list->AddImageItem("Item 07", std::make_shared<BufferedImage>(("images/tux-freddy.png")));
        _list->AddTextItem("Item 08");
        _list->AddImageItem("Item 09", std::make_shared<BufferedImage>(("images/tux-homer.png")));
        _list->AddTextItem("Item 10");
        _list->AddImageItem("Item 11", std::make_shared<BufferedImage>(("images/tux-indiana.png")));
        _list->AddTextItem("Item 12");
        _list->AddImageItem("Item 13", std::make_shared<BufferedImage>(("images/tux-ipod.png")));
        _list->AddTextItem("Item 14");
        _list->AddImageItem("Item 15", std::make_shared<BufferedImage>(("images/tux-jamaican.png")));
        _list->AddTextItem("Item 16");
        _list->AddImageItem("Item 17", std::make_shared<BufferedImage>(("images/tux-jason.png")));
        _list->AddTextItem("Item 18");
        _list->AddImageItem("Item 19", std::make_shared<BufferedImage>(("images/tux-kenny.png")));
        _list->AddTextItem("Item 20");
        _list->AddImageItem("Item 21", std::make_shared<BufferedImage>(("images/tux-mario.png")));
        _list->AddTextItem("Item 22");
        _list->AddImageItem("Item 23", std::make_shared<BufferedImage>(("images/tux-neo.png")));
        _list->AddTextItem("Item 24");
        _list->AddImageItem("Item 25", std::make_shared<BufferedImage>(("images/tux-potter.png")));
        _list->AddTextItem("Item 26");
        _list->AddImageItem("Item 27", std::make_shared<BufferedImage>(("images/tux-raider.png")));
        _list->AddTextItem("Item 28");
        _list->AddImageItem("Item 29", std::make_shared<BufferedImage>(("images/tux-rambo.png")));
        _list->AddTextItem("Item 30");
        _list->AddImageItem("Item 31", std::make_shared<BufferedImage>(("images/tux-rapper.png")));
        _list->AddTextItem("Item 32");
        _list->AddImageItem("Item 33", std::make_shared<BufferedImage>(("images/tux-shrek.png")));
        _list->AddTextItem("Item 34");
        _list->AddImageItem("Item 35", std::make_shared<BufferedImage>(("images/tux-spiderman.png")));
        _list->AddTextItem("Item 36");
        _list->AddImageItem("Item 37", std::make_shared<BufferedImage>(("images/tux-turtle.png")));
        _list->AddTextItem("Item 38");
        _list->AddImageItem("Item 39", std::make_shared<BufferedImage>(("images/tux-wolverine.png")));
        _list->AddTextItem("Item 40");
        _list->AddImageItem("Item 41", std::make_shared<BufferedImage>(("images/tux-zombie.png")));
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

    /*
    virtual void StateChanged(ToggleEvent *event)
    {
      _mutex.lock();

      
      _mutex.unlock();
    }
    */

    virtual void ItemChanged(SelectEvent *event)
    {
      _marquee->SetBounceEnabled(_spin->GetCurrentIndex());
    }

    virtual void ActionPerformed(ActionEvent *event)
    {
      Button *button = reinterpret_cast<Button *>(event->GetSource());

      if (button->IsPressed() == false) {
        return;
      }

      _mutex.lock();

      if (event->GetSource() == _button1) {
        _progress->SetValue(_progress->GetValue() + 10);
        _slider->SetValue(_slider->GetValue() + 10);
      } else if (event->GetSource() == _button2) {
        _progress->SetValue(_progress->GetValue() - 10);
        _slider->SetValue(_slider->GetValue() - 10);
      } else if (event->GetSource() == _button3) {
        // do nothing;
      } else if (event->GetSource() == _check1) {
        _textarea->SetWrap(_check1->IsPressed());
      } else if (event->GetSource() == _check2) {
        _textarea->SetEchoChar((_check2->IsPressed()?'*':'\0'));
      } else if (event->GetSource() == _check3) {
        _textarea->SetVisible(!_check3->IsPressed());
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

};

int main(int argc, char **argv)
{
	Application::Init(argc, argv);

	auto app = std::make_shared<App>();

  app->Init();
  app->Exec();

  Application::Loop();

	return 0;
}
