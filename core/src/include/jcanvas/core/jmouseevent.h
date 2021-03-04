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
#ifndef J_MOUSEEVENT_H
#define J_MOUSEEVENT_H

#include "jcanvas/core/jeventobject.h"
#include "jcanvas/core/jgraphics.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jmouseevent_type_t {
  Unknown,
  Pressed,
  Released,
  Moved,
  Rotated
};

/**
 * \brief
 *
 */
enum class jmouseevent_button_t {
  None = 0,
  Button1 = 1 << 0,
  Button2 = 1 << 1,
  Button3 = 1 << 2,
  Wheel = 1 << 3
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class MouseEvent : public EventObject {

  private:
    /** \brief */
    jpoint_t<int> _location;
    /** \brief */
    jmouseevent_button_t _button;
    /** \brief */
    jmouseevent_button_t _buttons;
    /** \brief */
    jmouseevent_type_t _type;
    /** \brief */
    int _clicks;

  public:
    /**
     * \brief
     *
     */
    MouseEvent(void *source, jmouseevent_type_t type, jmouseevent_button_t button, jmouseevent_button_t buttons, jpoint_t<int> location, int clicks);
    
    /**
     * \brief
     *
     */
    virtual ~MouseEvent();

    /**
     * \brief
     *
     */
    virtual jmouseevent_type_t GetType();

    /**
     * \brief
     *
     */
    virtual int GetClicks();
    
    /**
     * \brief
     *
     */
    virtual jmouseevent_button_t GetButton();

    /**
     * \brief
     *
     */
    virtual jmouseevent_button_t GetButtons();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetLocation();
    
};

}

#endif

