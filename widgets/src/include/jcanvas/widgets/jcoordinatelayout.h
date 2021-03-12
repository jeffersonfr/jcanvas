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
#ifndef J_COORDINATELAYOUT_H
#define J_COORDINATELAYOUT_H

#include "jcanvas/widgets/jlayout.h"
#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/core/jenum.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum jcoordinatelayout_orientation_t {
  None = 0,
  Horizontal = 1 << 0,
  Vertical = 1 << 1
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CoordinateLayout : public Layout {

  private:
    /** \brief */
    jcoordinatelayout_orientation_t _type;
    /** \brief */
    jpoint_t<int> _size;

  public:
    /**
     * \brief
     *
     */
    CoordinateLayout(int width = -1, int height = -1, jcoordinatelayout_orientation_t type = jenum_t<jcoordinatelayout_orientation_t>{jcoordinatelayout_orientation_t::Horizontal}.Or(jcoordinatelayout_orientation_t::Vertical));
    
    /**
     * \brief
     *
     */
    virtual ~CoordinateLayout();

    /**
     * \brief
     *
     */
    virtual void SetSize(jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual void SetType(jcoordinatelayout_orientation_t type);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetSize();
    
    /**
     * \brief
     *
     */
    virtual jcoordinatelayout_orientation_t GetType();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMinimumLayoutSize(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMaximumLayoutSize(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetPreferredLayoutSize(std::shared_ptr<Container> target);

    /**
     * \brief
     *
     */
    virtual void DoLayout(std::shared_ptr<Container> parent);

};

}

#endif

