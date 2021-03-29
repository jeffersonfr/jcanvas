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
#ifndef J_WINDOW_H
#define J_WINDOW_H

#include "jcanvas/core/jeventmanager.h"
#include "jcanvas/core/jkeylistener.h"
#include "jcanvas/core/jmouselistener.h"
#include "jcanvas/core/jwindowlistener.h"
#include "jcanvas/core/jfont.h"

#include <thread>

namespace jcanvas {

enum class jwindow_state_t {
  Normal,
  Maximize,
  Minimize,
  Fullscreen
};

enum class jwindow_rotation_t {
  None,
  Degrees90,
  Degrees180,
  Degrees270
};

enum class jcursor_style_t {
  Default,
  Hand,
  Move,
  Text,
  Wait,
  Crosshair,
  Horizontal,
  Vertical,
  East,
  West,
  North,
  South,
  NorthWest,
  NorthEast,
  SouthWest,
  SouthEast
};

class WindowAdapter;
class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Window {

  private:
    /** \brief */
    std::vector<WindowListener *> _window_listeners;
    /** \brief */
    std::thread _exec_thread;
    /** \brief */
     std::mutex _event_mutex;
    /** \brief */
     std::condition_variable _event_condition;
    /** \brief */
    std::mutex _paint_mutex;
    /** \brief */
    std::mutex _window_listener_mutex;
    /** \brief */
    std::mutex _remove_window_listener_mutex;
    /** \brief */
    EventManager _event_manager;
    /** \brief */
    WindowAdapter *_window_adapter;
    /** \brief */
    int _fps;

  public:
    /**
     * \brief
     *
     */
    Window(jpoint_t<int> size, jpoint_t<int> point = {0, 0});
    
    /**
     * \brief
     *
     */
    virtual ~Window();

    /**
     * \brief
     *
     */
    virtual EventManager & GetEventManager();
    
    /**
     * \brief Entry point to applications.
     *
     */
    virtual void ShowApp();

    /**
     * \brief
     *
     */
    virtual void Exec();

    /**
     * \brief
     *
     */
    void SetFramesPerSecond(int fps);

    /**
     * \brief
     *
     */
    int GetFramesPerSecond();
    
    /**
     * \brief
     *
     */
    virtual void ToggleFullScreen();
    
    /**
     * \brief
     *
     */
    virtual void SetOpacity(float opacity);
    
    /**
     * \brief
     *
     */
    virtual float GetOpacity();
    
    /**
     * \brief
     *
     */
    virtual void SetTitle(std::string title);

    /**
     * \brief
     *
     */
    virtual std::string GetTitle();

    /**
     * \brief
     *
     */
    virtual void SetResizable(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsResizable();

    /**
     * \brief
     *
     */
    virtual void SetUndecorated(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsUndecorated();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetPosition();

    /**
     * \brief
     *
     */
    virtual void SetPosition(jpoint_t<int> pos);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetSize();

    /**
     * \brief
     *
     */
    virtual void SetSize(jpoint_t<int> size);

    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetBounds();

    /**
     * \brief
     *
     */
    virtual void SetBounds(jrect_t<int> bounds);
    
    /**
     * \brief
     *
     */
    virtual void Repaint();
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

    /**
     * \brief
     *
     */
    virtual void SetVisible(bool visible);
    
    /**
     * \brief
     *
     */
    virtual bool IsVisible();
    
    /**
     * \brief
     *
     */
    virtual void SetCursorLocation(int x, int y);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetCursorLocation();
    
    /**
     * \brief
     *
     */
    virtual void SetCursorEnabled(bool enable);

    /**
     * \brief
     *
     */
    virtual bool IsCursorEnabled();

    /**
     * \brief
     *
     */
    virtual void SetCursor(jcursor_style_t cursor);

    /**
     * \brief
     *
     */
    virtual void SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty);

    /**
     * \brief
     *
     */
    virtual jcursor_style_t GetCursor();

    /**
     * \brief
     *
     */
    virtual void SetRotation(jwindow_rotation_t t);

    /**
     * \brief
     *
     */
    virtual jwindow_rotation_t GetRotation();

    /**
     * \brief
     *
     */
    virtual void RegisterWindowListener(WindowListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveWindowListener(WindowListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchWindowEvent(WindowEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<WindowListener *> & GetWindowListeners();

};

}

#endif 
