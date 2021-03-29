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
#ifndef J_MARQUEE_H
#define J_MARQUEE_H

#include "jcanvas/widgets/janimation.h"
#include "jcanvas/widgets/jcomponent.h"

#include <string>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Marquee : public Component, public Animation {

  private:
    /** \brief */
    std::string _text;
    /** \brief */
    float _ratio {0.1f};
    /** \brief */
    int _text_size {0};
    /** \brief */
    int _position {0};
    /** \brief */
    int _direction {-1};
    /** \brief */
    bool _is_bounce_enabled {false};

  public:
    /**
     * \brief
     *
     */
    Marquee(std::string text);
    
    /**
     * \brief
     *
     */
    virtual ~Marquee();

    /**
     * \brief
     *
     */
    virtual void SetBounceEnabled(bool enabled);
    
    /**
     * \brief
     *
     */
    virtual bool IsBounceEnabled();
    
    /**
     * \brief
     *
     */
    virtual void SetRatio(float ratio);
    
    /**
     * \brief
     *
     */
    virtual float GetRatio();
    
    /**
     * \brief
     *
     */
    virtual void SetText(std::string text);
    
    /**
     * \brief
     *
     */
    virtual std::string GetText();
    
    /**
     * \brief
     *
     */
    virtual void Update(std::chrono::milliseconds tick);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
};

}

#endif

