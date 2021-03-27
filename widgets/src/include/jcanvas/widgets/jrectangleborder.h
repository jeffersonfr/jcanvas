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
#ifndef J_RECTANGLEBORDER_H
#define J_RECTANGLEBORDER_H

#include "jcanvas/widgets/jborder.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class RectangleBorder : public Border {

  private:
    /** \brief */
    jcolor_t<float> _color;
    /** \brief */
    std::size_t _size;

  public:
    /**
     * \brief
     *
     */
    RectangleBorder(std::size_t size = {1}, jcolor_t<float> color = jcolor_t {0xff808080});

    /**
     * \brief
     *
     */
    virtual ~RectangleBorder();

    /**
     * \brief
     *
     */
    virtual void SetSize(std::size_t size);

    /**
     * \brief
     *
     */
    virtual std::size_t GetSize();

    /**
     * \brief
     *
     */
    virtual void SetColor(jcolor_t<float> color);

    /**
     * \brief
     *
     */
    virtual jcolor_t<float> GetColor();

    /**
     * \brief
     *
     */
    virtual void Paint(Component *cmp, Graphics *g);
    
};

}

#endif

