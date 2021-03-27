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
#ifndef J_IMAGEBACKGROUND_H
#define J_IMAGEBACKGROUND_H

#include "jcanvas/widgets/jsolidbackground.h"
#include "jcanvas/core/jimage.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ImageBackground : public SolidBackground {

  private:
    /** \bried */
    std::shared_ptr<Image> _image;
    /** \bried */
    jrect_align_t _align {jrect_align_t::Center};

  public:
    /**
     * \brief
     *
     */
    ImageBackground(std::shared_ptr<Image> image = {});

    /**
     * \brief
     *
     */
    virtual ~ImageBackground();

    /**
     * \brief
     *
     */
    virtual void SetImage(std::shared_ptr<Image> image);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> GetImage();

    /**
     * \brief
     *
     */
    void SetAlign(jrect_align_t align);

    /**
     * \brief
     *
     */
    jrect_align_t GetAlign();

    /**
     * \brief
     *
     */
    virtual void Paint(Component *cmp, Graphics *g);
    
};

}

#endif

