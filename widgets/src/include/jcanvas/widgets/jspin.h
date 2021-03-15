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
#ifndef J_SPIN_H
#define J_SPIN_H

#include "jcanvas/widgets/jitemcomponent.h"
#include "jcanvas/widgets/jslidercomponent.h"
#include "jcanvas/widgets/jselectlistener.h"

#include <vector>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Spin : public Component, public ItemComponent {

  private:
    /** \brief */
    jscroll_orientation_t _type;

  private:
    void NextItem();
    void PreviousItem();

  public:
    /**
     * \brief
     *
     */
    Spin();
    
    /**
     * \brief
     *
     */
    virtual ~Spin();

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
    virtual void AddEmptyItem();
    
    /**
     * \brief
     *
     */
    virtual void AddTextItem(std::string text);
    
    /**
     * \brief
     *
     */
    virtual void AddImageItem(std::string text, std::shared_ptr<Image> image);
    
    /**
     * \brief
     *
     */
    virtual void AddCheckedItem(std::string text, bool checked);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
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

};

}

#endif

