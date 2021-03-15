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
#ifndef J_BUFFEREDIMAGE_H
#define J_BUFFEREDIMAGE_H

#include "jcanvas/core/jimage.h"

#include <mutex>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class BufferedImage : public virtual Image {

  private:
    void ConstructFromStream(std::istream &stream);

  protected:
    /** \brief */
    Graphics *_graphics;
    /** \brief */
    std::recursive_mutex _mutex;
    /** \brief */
    cairo_surface_t *_cairo_surface;

  public:
    /**
     * \brief
     *
     */
    BufferedImage(jpixelformat_t pixelformat, jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    BufferedImage(cairo_surface_t *surface);

    /**
     * \brief
     *
     */
    BufferedImage(std::string file);

    /**
     * \brief
     *
     */
    BufferedImage(std::istream &stream);

    /**
     * \brief
     *
     */
    virtual ~BufferedImage();

    /**
     * \brief
     *
     */
    virtual Graphics * GetGraphics();

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Flip(jflip_t mode);
    
    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Shear(jpoint_t<float> size);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Rotate(double radians, bool resize = true);
    
    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Scale(jpoint_t<int> size);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Crop(jrect_t<int> rect);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Blend(double alpha);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Colorize(jcolor_t<float> color);

    /**
     * \brief
     *
     */
    virtual void GetRGBArray(uint32_t *rgb, jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual uint8_t * LockData();
  
    /**
     * \brief
     *
     */
    virtual void UnlockData();
  
    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> Clone();

};

}

#endif 
