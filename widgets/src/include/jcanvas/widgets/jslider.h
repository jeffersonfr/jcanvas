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
#ifndef J_SLIDER_H
#define J_SLIDER_H

#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/widgets/jadjustmentlistener.h"

namespace jcanvas {

struct jrange_t {
  float min;
  float max;
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Slider : public Component {

  private:
    /** \brief */
    std::vector<AdjustmentListener *> _adjustment_listeners;
    /** \brief */
    std::mutex _adjustment_listener_mutex;
    /** \brief */
    std::mutex _remove_adjustment_listener_mutex;
    /** \brief */
    std::shared_ptr<Image> _stone_image;
    /** \brief */
    jrange_t _range;
    /** \brief */
    jrange_t _ticks;
    /** \brief */
    float _value;
    /** \brief */
    bool _is_pressed;
    /** \brief */
    bool _is_vertical;
    /** \brief */
    bool _is_metric_visible;

    /**
     * \brief
     *
     */
    virtual bool KeyPressed(KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool MousePressed(MouseEvent *event) override;
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(MouseEvent *event) override;
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(MouseEvent *event) override;
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(MouseEvent *event) override;
    
  public:
    /**
     * \brief
     *
     */
    Slider();
    
    /**
     * \brief
     *
     */
    virtual ~Slider();

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
    void SetStoneImage(std::shared_ptr<Image> image);

    /**
     * \brief
     *
     */
    std::shared_ptr<Image> GetStoneImage();

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
    virtual void SetMetricVisible(bool visible);

    /**
     * \brief
     *
     */
    virtual bool IsMetricVisible();

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
    virtual void Paint(Graphics *g) override;

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

