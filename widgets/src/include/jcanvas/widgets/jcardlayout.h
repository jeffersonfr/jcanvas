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
  std::shared_ptr<Component> component;
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
    std::vector<struct jcardlayout_map_t> vector;
    /** \brief */
    int _hgap;
    /** \brief */
    int _vgap;
    /** \brief */
    int _current_card;

  public:
    /**
     * \brief
     *
     */
    CardLayout(int hgap = 10, int vgap = 10);
    
    /**
     * \brief
     *
     */
    virtual ~CardLayout();

    /**
     * \brief
     *
     */
    virtual int GetHGap();
    
    /**
     * \brief
     *
     */
    virtual int GetVGap();
    
    /**
     * \brief
     *
     */
    virtual void SetHGap(int hgap);
    
    /**
     * \brief
     *
     */
    virtual void SetVGap(int vgap);

    /**
     * \brief
     *
     */
    virtual void AddLayoutComponent(std::string name, std::shared_ptr<Component> comp);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponent(std::shared_ptr<Component> comp);

    /**
     * \brief
     *
     */
    virtual void CheckLayout(std::shared_ptr<Container> parent);
  
    /**
     * \brief
     *
     */
    virtual void First(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual void Next(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual void Previous(std::shared_ptr<Container> parent);
    
    /**
     * \brief
     *
     */
    virtual void Last(std::shared_ptr<Container> parent);

    /**
     * \brief
     *
     */
    virtual void ShowDefaultComponent(std::shared_ptr<Container> parent);

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
    virtual void DoLayout(std::shared_ptr<Container> parent);

};

}

#endif

