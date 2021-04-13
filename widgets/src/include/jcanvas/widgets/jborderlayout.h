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
#ifndef J_BORDERLAYOUT_H
#define J_BORDERLAYOUT_H

#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/widgets/jlayout.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jborderlayout_align_t {
  Unknown,
  North,
  South,
  East,
  West,
  Center,
  PageStart,
  PageEnd,
  LineStart,
  LineEnd,
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class BorderLayout : public Layout {

  private:
    /** \brief */
    Component *north;
    /** \brief */
    Component *west;
    /** \brief */
    Component *east;
    /** \brief */
    Component *south;
    /** \brief */
    Component *center;
    /** \brief */
    Component *firstLine;
    /** \brief */
    Component *lastLine;
    /** \brief */
    Component *firstItem;
    /** \brief */
    Component *lastItem;
    /** \brief */
    jpoint_t<int> _gap;

  public:
    /**
     * \brief
     *
     */
    BorderLayout(jpoint_t<int> gap = jpoint_t<int>{8, 8});
    
    /**
     * \brief
     *
     */
    virtual ~BorderLayout();

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
    virtual void AddLayoutComponent(Component *c, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponent(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponents();

    /**
     * \brief
     *
     */
    virtual jborderlayout_align_t GetConstraints(Component *c);

    /**
     * \brief
     *
     */
    virtual Component * GetLayoutComponent(jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual Component * GetLayoutComponent(Container *target, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual Component * GetChild(jborderlayout_align_t key, bool ltr);

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
    virtual void DoLayout(Container *target);

};

}

#endif

