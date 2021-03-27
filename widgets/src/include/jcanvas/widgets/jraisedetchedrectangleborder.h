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
#ifndef J_RAISEDETCHEDRECTANGLEBORDER_H
#define J_RAISEDETCHEDRECTANGLEBORDER_H

#include "jcanvas/widgets/jrectangleborder.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class RaisedEtchedRectangleBorder : public RectangleBorder {

  public:
    /**
     * \brief
     *
     */
    RaisedEtchedRectangleBorder(std::size_t size = {1}, jcolor_t<float> color = jcolor_t {0xff808080});

    /**
     * \brief
     *
     */
    virtual ~RaisedEtchedRectangleBorder();

    /**
     * \brief
     *
     */
    virtual void Paint(Component *cmp, Graphics *g);
    
};

}

#endif

