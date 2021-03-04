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
#ifndef J_EVENTMANAGER_H
#define J_EVENTMANAGER_H

#include "jcanvas/core/jkeyevent.h"
#include "jcanvas/core/jmouseevent.h"
#include "jcanvas/core/jenum.h"

#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace jcanvas {

class Window;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class EventManager {

  friend class Window;

  private:
    /** \brief */
    std::map<std::string, int> _event_count;
    /** \brief */
    std::vector<EventObject *> _events;
    /** \brief */
    std::map<jkeyevent_symbol_t, bool> _key_button_map;
    /** \brief */
    std::map<jmouseevent_button_t, bool> _mouse_button_map;
    /** \brief */
    std::thread _thread;
    /** \brief */
    std::mutex _mutex;
    /** \brief */
    std::condition_variable _condition;
    /** \brief */
    Window *_window;
    /** \brief */
    size_t _click_delay;
    /** \brief */
    bool _alive;
    /** \brief */
    bool _autograb;

  protected:
    /**
     * \brief
     *
     */
    EventManager(Window *window);
    
    /**
     * \brief
     *
     */
    virtual void ProcessEvents();

  public:
    /**
     * \brief
     *
     */
    virtual ~EventManager();

    /**
     * \brief
     *
     */
    virtual void SetClickDelay(size_t ms);
    
    /**
     * \brief
     *
     */
    virtual size_t GetClickDelay();
    
    /**
     * \brief
     *
     */
    virtual void SetAutoGrab(bool grab);
    
    /**
     * \brief
     *
     */
    virtual bool IsAutoGrab();
    
    /**
     * \brief
     *
     */
    virtual void PostEvent(EventObject *event);

    /**
     * \brief
     *
     */
    virtual bool IsKeyDown(jkeyevent_symbol_t key);

    /**
     * \brief
     *
     */
    virtual bool IsButtonDown(jmouseevent_button_t button);

    /**
     * \brief
     *
     */
    virtual const std::vector<EventObject *> & GetEvents();

};

}

#endif 
