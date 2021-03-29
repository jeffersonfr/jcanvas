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
#ifndef J_COLUMNLAYOUT_H
#define J_COLUMNLAYOUT_H

#include "jcanvas/widgets/jlayout.h"
#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/core/jenum.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jmainaxis_align_t {
  Start,
  Center,
  End,
  SpaceAround,
  SpaceBetween,
  SpaceEvenly
};

/**
 * \brief
 *
 */
enum class jcrossaxis_align_t {
  Start,
  End,
  Stretch
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ColumnLayout : public Layout {

  protected:
    /** \brief */
    jmainaxis_align_t _main_align;
    /** \brief */
    jcrossaxis_align_t _cross_align;
    /** \brief */
    int _gap {4};

  public:
    /**
     * \brief
     *
     */
    ColumnLayout(jmainaxis_align_t main = jmainaxis_align_t::Start, jcrossaxis_align_t cross = jcrossaxis_align_t::Stretch);
    
    /**
     * \brief
     *
     */
    virtual ~ColumnLayout();

    /**
     * \brief
     *
     */
    virtual void SetGap(int gap);
    
    /**
     * \brief
     *
     */
    virtual int GetGap();

    /**
     * \brief
     *
     */
    virtual void SetMainAxisAlign(jmainaxis_align_t align);

    /**
     * \brief
     *
     */
    virtual jmainaxis_align_t GetMainAxisAlign();

    /**
     * \brief
     *
     */
    virtual void SetCrossAxisAlign(jcrossaxis_align_t align);

    /**
     * \brief
     *
     */
    virtual jcrossaxis_align_t GetCrossAxisAlign();

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
    virtual jpoint_t<int> GetPreferredLayoutSize(Container *target);

    /**
     * \brief
     *
     */
    virtual void DoLayout(Container *parent);

};

}

#endif

