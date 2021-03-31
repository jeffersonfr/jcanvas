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
#ifndef J_PROGRESSBAR_H
#define J_PROGRESSBAR_H

#include "jcanvas/widgets/jslidercomponent.h"
#include "jcanvas/widgets/janimation.h"

namespace jcanvas {

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ProgressBar : public Component, public Animation {

  private:
    /** \brief */
    float _value;
    /** \brief */
    bool _is_vertical;
    /** \brief */
    bool _is_continuous;

    /**
     * \brief
     *
     */
    virtual void Update(std::chrono::milliseconds tick);

  public:
    /**
     * \brief
     *
     */
    ProgressBar();
    
    /**
     * \brief
     *
     */
    virtual ~ProgressBar();

    /**
     * \brief
     *
     */
    virtual bool IsVertical();
    
    /**
     * \brief
     *
     */
    virtual void SetVertical(bool vertical);
    
    /**
     * \brief
     *
     */
    virtual bool IsContinuous();
    
    /**
     * \brief
     *
     */
    virtual void SetContinuous(bool continuous);
    
    /**
     * \brief
     *
     */
    virtual float GetValue();
    
    /**
     * \brief
     *
     */
    virtual void SetValue(float value);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

};

}

#endif

