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
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindowadapter.h"
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jenum.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

namespace jcanvas {

/** \brief */
Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jcanvas_window = nullptr;

void Application::Init(int argc, char **argv)
{
	sg_screen.x = 1280;
	sg_screen.y = 720;
  
  sg_quitting = false;
}

static void InternalPaint()
{
	if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
		return;
	}

  jrect_t<int> 
    bounds = sg_jcanvas_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jpoint_t<int>
      size = sg_back_buffer->GetSize();

    if (size.x != bounds.size.x or size.y != bounds.size.y) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new BufferedImage(jpixelformat_t::RGB32, bounds.size);
  }

  Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jcomposite_t::Src);

  sg_jcanvas_window->Paint(g);

  g->Flush();

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));

  sg_quitting = true;
 
  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	if (sg_jcanvas_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  sg_jcanvas_window = parent;
}

WindowAdapter::~WindowAdapter()
{
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
}

void WindowAdapter::SetTitle(std::string title)
{
}

std::string WindowAdapter::GetTitle()
{
  return std::string();
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
	return 1.0;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
}

bool WindowAdapter::IsUndecorated()
{
  return true;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
}

jrect_t<int> WindowAdapter::GetBounds()
{
	return {
    0,
    0,
    sg_screen.x,
    sg_screen.y
  };
}
		
void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return false;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	return {0, 0};
}

void WindowAdapter::SetVisible(bool visible)
{
}

bool WindowAdapter::IsVisible()
{
  return sg_quitting == false;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return jcursor_style_t::Default;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
}

bool WindowAdapter::IsCursorEnabled()
{
	return false;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(Image *image)
{
}

Image * WindowAdapter::GetIcon()
{
  return nullptr;
}

}
