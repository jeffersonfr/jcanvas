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
#ifndef J_CHECKBUTTONGROUP_H
#define J_CHECKBUTTONGROUP_H

#include "jcanvas/widgets/jcheckbutton.h"
#include "jcanvas/widgets/jtogglelistener.h"

#include <vector>
#include <mutex>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CheckButtonGroup : public ToggleListener {

  private:
    /** \brief */
    std::vector<std::shared_ptr<CheckButton>> _buttons;
    /** \brief */
    std::recursive_mutex _group_mutex;

  protected:
    /**
     * \brief
     *
     */
    virtual void StateChanged(ToggleEvent *event);

  public:
    /**
     * \brief
     *
     */
    CheckButtonGroup();
    
    /**
     * \brief
     *
     */
    virtual ~CheckButtonGroup();

    /**
     * \brief
     *
     */
    virtual void Add(std::shared_ptr<CheckButton> button);
    
    /**
     * \brief
     *
     */
    virtual void Remove(std::shared_ptr<CheckButton> button);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<CheckButton> GetSelected();

};

}

#endif

