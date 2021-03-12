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
    std::shared_ptr<Component> north;
    /** \brief */
    std::shared_ptr<Component> west;
    /** \brief */
    std::shared_ptr<Component> east;
    /** \brief */
    std::shared_ptr<Component> south;
    /** \brief */
    std::shared_ptr<Component> center;
    /** \brief */
    std::shared_ptr<Component> firstLine;
    /** \brief */
    std::shared_ptr<Component> lastLine;
    /** \brief */
    std::shared_ptr<Component> firstItem;
    /** \brief */
    std::shared_ptr<Component> lastItem;
    /** \brief */
    int _hgap;
    /** \brief */
    int _vgap;

  public:
    /**
     * \brief
     *
     */
    BorderLayout(int hgap = 10, int vgap = 10);
    
    /**
     * \brief
     *
     */
    virtual ~BorderLayout();

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
    virtual void SetVgap(int vgap);

    /**
     * \brief
     *
     */
    virtual void AddLayoutComponent(std::shared_ptr<Component> c, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponent(std::shared_ptr<Component> c);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponents();

    /**
     * \brief
     *
     */
    virtual jborderlayout_align_t GetConstraints(std::shared_ptr<Component> c);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Component> GetLayoutComponent(jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Component> GetLayoutComponent(std::shared_ptr<Container> target, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Component> GetChild(jborderlayout_align_t key, bool ltr);

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
    virtual void DoLayout(std::shared_ptr<Container> target);

};

}

#endif

