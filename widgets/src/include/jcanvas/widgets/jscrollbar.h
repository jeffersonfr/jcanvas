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

#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jslider.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jactionlistener.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ScrollBar : public Container, public ActionListener {

  private:
    /** \brief */
    Slider _slider;
    /** \brief */
    Button _previous;
    /** \brief */
    Button _next;
    /** \brief */
    
    /**
     * \brief
     *
     */
    virtual void ActionPerformed(ActionEvent *event) override;
    
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
    virtual void SetValue(float value);

    /**
     * \brief
     *
     */
    virtual float GetValue();

    /**
     * \brief
     *
     */
    virtual void SetRange(jrange_t range);

    /**
     * \brief
     *
     */
    virtual jrange_t GetRange();

    /**
     * \brief
     *
     */
    virtual void SetTicks(jrange_t ticks);

    /**
     * \brief
     *
     */
    virtual jrange_t GetTicks();

    /**
     * \brief
     *
     */
    virtual void SetVertical(bool vertical);

    /**
     * \brief
     *
     */
    virtual bool IsVertical();

    /**
     * \brief
     *
     */
    virtual void RegisterAdjustmentListener(AdjustmentListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveAdjustmentListener(AdjustmentListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchAdjustmentEvent(AdjustmentEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<AdjustmentListener *> & GetAdjustmentListeners();
};

}

#endif

