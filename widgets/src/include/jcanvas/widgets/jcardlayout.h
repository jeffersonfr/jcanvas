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
#ifndef J_CARDLAYOUT_H
#define J_CARDLAYOUT_H

#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/widgets/jlayout.h"

#include <vector>

namespace jcanvas {

struct jcardlayout_map_t {
  std::string name;
  Component *component;
};

class Container;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CardLayout : public Layout {

  private:
    /** \brief */
    std::vector<struct jcardlayout_map_t> _cards;
    /** \brief */
    jpoint_t<int> _gap;
    /** \brief */
    int _index;

  public:
    /**
     * \brief
     *
     */
    CardLayout(jpoint_t<int> gap = jpoint_t<int>{8, 8});
    
    /**
     * \brief
     *
     */
    virtual ~CardLayout();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetGap();
    
    /**
     * \brief
     *
     */
    virtual void SetGap(jpoint_t<int> gap);

    /**
     * \brief
     *
     */
    virtual void AddLayoutComponent(std::string name, Component *comp);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponent(Component *comp);

    /**
     * \brief
     *
     */
    virtual void CheckLayout(Container *parent);
  
    /**
     * \brief
     *
     */
    virtual void First(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual void Next(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual void Previous(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual void Last(Container *parent);

    /**
     * \brief
     *
     */
    virtual void ShowDefaultComponent(Container *parent);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMinimumLayoutSize(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMaximumLayoutSize(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetPreferredLayoutSize(Container *parent);

    /**
     * \brief
     *
     */
    virtual void DoLayout(Container *parent);

};

}

#endif

