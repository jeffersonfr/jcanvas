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
#ifndef J_EVENTOBJECT_H
#define J_EVENTOBJECT_H

#include <chrono>

namespace jcanvas {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class EventObject {

  private:
    std::chrono::time_point<std::chrono::steady_clock> _timestamp;
    /** \brief */
    void *_source;
    /** \brief */
    bool _is_consumed;

  public:
    /**
     * \brief
     * 
     */
    EventObject(void *source);
    
    /**
     * \brief
     * 
     */
    virtual ~EventObject();

    /**
     * \brief
     * 
     */
    virtual void * GetSource();

    /**
     * \brief
     * 
     */
    virtual void Consume();
    
    /**
     * \brief
     * 
     */
    virtual bool IsConsumed();

    /**
     * \brief
     * 
     */
    void Reset();

    /**
     * \brief
     * 
     */
    std::chrono::time_point<std::chrono::steady_clock> GetTimestamp();

};

}

#endif
