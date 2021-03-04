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
#ifndef J_SCROLLBAR_H
#define J_SCROLLBAR_H

#include "jcanvas/widgets/jslidercomponent.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ScrollBar : public SliderComponent {

  private:
    /** \brief */
    std::vector<AdjustmentListener *> _adjust_listeners;
    /** \brief */
    int _index;
    /** \brief */
    int _stone_size;
    /** \brief */
    int _count_paint;
    /** \brief */
    bool _pressed;
    /** \brief */
    bool _label_visible;
    
  public:
    /**
     * \brief
     *
     */
    ScrollBar();
    
    /**
     * \brief
     *
     */
    virtual ~ScrollBar();

    /**
     * \brief
     *
     */
    virtual jscroll_orientation_t GetScrollOrientation();
    
    /**
     * \brief
     *
     */
    virtual void SetScrollOrientation(jscroll_orientation_t type);

    /**
     * \brief
     *
     */
    virtual int GetStoneSize();
    
    /**
     * \brief
     *
     */
    virtual void SetStoneSize(int size);
    
    /**
     * \brief
     *
     */
    virtual bool KeyPressed(KeyEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MousePressed(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
};

}

#endif

