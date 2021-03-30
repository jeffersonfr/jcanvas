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
#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/widgets/jnulllayout.h"
#include "jcanvas/widgets/jraisedgradientrectangleborder.h"
#include "jcanvas/core/japplication.h"

namespace jcanvas {

Frame::Frame(jpoint_t<int> size, jpoint_t<int> point):
  Window(size, point)
{
  jtheme_t &theme = GetTheme();

  theme.font.primary = Font::Big;

  _focus_owner = nullptr;
  _icon = nullptr;

  _animation_thread = std::thread(
      [this]() {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        while (IsVisible() == true) {
          std::unique_lock lock(_animation_mutex);

          if (_animations.size() == 0) {
            _animation_condition.wait(lock, 
                [this]() {
                  return IsVisible() == false or _animations.size() > 0;
                });
          
            start = std::chrono::steady_clock::now(); // INFO:: to avoid huge time ticks
          }

          if (IsVisible() == false) {
            break;
          }

          std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
          
          for (auto animation : _animations) {
            animation->Update(std::chrono::duration_cast<std::chrono::milliseconds>(now - start));
          }

          start = now;

          _animations.erase(std::remove_if(_animations.begin(), _animations.end(), 
              [](Animation *animation) {
                return animation->IsStarted() == false;
              }), _animations.end());

          lock.unlock();

          std::this_thread::sleep_until(now + std::chrono::milliseconds{15});
        }
      });

  SetInsets({8, 8, 8, 8});
  SetTitle("Main");
  SetLayout<BorderLayout>();
  SetBackground(std::make_shared<SolidBackground>());
  SetBorder(std::make_shared<RaisedGradientRectangleBorder>(4));
  SetUndecorated(false);
}

Frame::~Frame()
{
  SetVisible(false);

  _animation_condition.notify_all();
  _animation_thread.join();
}

void Frame::RegisterAnimation(Animation *animation)
{
  std::lock_guard<std::mutex> lock(_animation_mutex);

  std::vector<Animation *>::iterator i = std::find(_animations.begin(), _animations.end(), animation);

  if (i == _animations.end()) {
    _animations.push_back(animation);

    _animation_condition.notify_one();
  }
}

void Frame::UnregisterAnimation(Animation *animation)
{
  std::lock_guard<std::mutex> lock(_animation_mutex);

  std::vector<Animation *>::iterator i = std::find(_animations.begin(), _animations.end(), animation);

  if (i != _animations.end()) {
    _animations.erase(i);
  }
}

void Frame::SetVisible(bool visible)
{
  Window::SetVisible(visible);
}

bool Frame::IsVisible()
{
  return Window::IsVisible();
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
    
void Frame::SetIcon(std::shared_ptr<Image> image)
{
  _icon = image;
}

std::shared_ptr<Image> Frame::GetIcon()
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

  c->DispatchFocusEvent(new jcanvas::FocusEvent(c, jcanvas::jfocusevent_type_t::Gain));
}

void Frame::ReleaseComponentFocus(Component *c)
{
  if (_focus_owner == nullptr or _focus_owner != c) {
    return;
  }

  _focus_owner = nullptr;

  Repaint();

  c->DispatchFocusEvent(new jcanvas::FocusEvent(c, jcanvas::jfocusevent_type_t::Lost));
}

Container * Frame::GetFocusCycleRootAncestor()
{
  return this;
}

void Frame::Repaint(Component *cmp)
{
  Window::Repaint();
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
  g->SetColor(jcolor_name_t::White);
}

void Frame::Paint(Graphics *g)
{
  DoLayout();

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

