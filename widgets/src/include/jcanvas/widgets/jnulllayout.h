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
#ifndef J_NULLLAYOUT_H
#define J_NULLLAYOUT_H

#include "jcanvas/widgets/jlayout.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class NullLayout : public Layout {

  private:

  public:
    /**
     * \brief
     *
     */
    NullLayout();
    
    /**
     * \brief
     *
     */
    virtual ~NullLayout();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMinimumLayoutSize(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMaximumLayoutSize(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetPreferredLayoutSize(std::shared_ptr<Container> parent);

    /**
     * \brief
     *
     */
    virtual void DoLayout(std::shared_ptr<Container> c);

};

}

#endif

