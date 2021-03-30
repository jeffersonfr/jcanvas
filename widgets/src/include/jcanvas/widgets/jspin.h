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
#ifndef J_SPIN_H
#define J_SPIN_H

#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jactionlistener.h"
#include "jcanvas/widgets/jselectlistener.h"

#include <vector>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Spin : public Container, ActionListener {

  private:
    /** \brief */
    std::vector<SelectListener *> _select_listeners;
    /** \brief */
    std::mutex _select_listener_mutex;
    /** \brief */
    std::mutex _remove_select_listener_mutex;
    /** \brief */
    std::vector<std::string> _items;
    /** \brief */
    Text _text;
    /** \brief */
    Button _previous;
    /** \brief */
    Button _next;
    /** \brief */
    int _current_index;
    /** \brief */
    int _low_range;
    /** \brief */
    int _high_range;
    /** \brief */
    bool _is_range;
    /** \brief */
    bool _is_loop_enabled;

    /**
     * \brief
     *
     */
    void Build(std::string value);

    /**
     * \brief
     *
     */
    void ActionPerformed(ActionEvent *event);

  public:
    /**
     * \brief
     *
     */
    Spin(std::pair<int, int> range);
    
    /**
     * \brief
     *
     */
    Spin(const std::vector<std::string> &items);
    
    /**
     * \brief
     *
     */
    virtual ~Spin();

    /**
     * \brief
     *
     */
    void Previous();
    
    /**
     * \brief
     *
     */
    void Next();
    
    /**
     * \brief
     *
     */
    virtual void SetLoopEnabled(bool enabled);

    /**
     * \brief
     *
     */
    virtual bool IsLoopEnabled();

    /**
     * \brief
     *
     */
    virtual std::string GetValue();

    /**
     * \brief
     *
     */
    virtual void SetCurrentIndex(int index);

    /**
     * \brief
     *
     */
    virtual int GetCurrentIndex();

    /**
     * \brief
     *
     */
    virtual void RegisterSelectListener(SelectListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveSelectListener(SelectListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchSelectEvent(SelectEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<SelectListener *> & GetSelectListeners();

};

}

#endif

