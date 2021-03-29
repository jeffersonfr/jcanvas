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
#ifndef J_ROUNDEDRECTANGLEBORDER_H
#define J_ROUNDEDRECTANGLEBORDER_H

#include "jcanvas/widgets/jrectangleborder.h"
#include "jcanvas/core/jgraphics.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class RoundedRectangleBorder : public RectangleBorder {

  private:
    /** \brief */
    jrect_corner_t _corners {static_cast<jrect_corner_t>(0xff)};
    /** \brief */
    jpoint_t<int> _corner_size {16};

  public:
    /**
     * \brief
     *
     */
    RoundedRectangleBorder(std::size_t size = {1}, jcolor_t<float> color = jcolor_t {0xff808080});

    /**
     * \brief
     *
     */
    virtual ~RoundedRectangleBorder();

    /**
     * \brief
     *
     */
    void SetCornerSize(jpoint_t<int> size);

    /**
     * \brief
     *
     */
    jpoint_t<int> GetCornerSize();

    /**
     * \brief
     *
     */
    void SetCorners(jrect_corner_t corners);

    /**
     * \brief
     *
     */
    jrect_corner_t GetCorners();

    /**
     * \brief
     *
     */
    virtual void Paint(Component *cmp, Graphics *g);
    
};

}

#endif

