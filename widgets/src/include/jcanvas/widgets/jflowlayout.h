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
#ifndef J_FLOWLAYOUT_H
#define J_FLOWLAYOUT_H

#include "jcanvas/widgets/jlayout.h"
#include "jcanvas/widgets/jcontainer.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jflowlayout_align_t {
  Left,
  Center,
  Right,
  Leading,
  Trailing
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class FlowLayout : public Layout {

  private:
    /** \brief */
    jflowlayout_align_t _align;
    /** \brief */
    jflowlayout_align_t _newalign;
    /** \brief */
    int _hgap;
    /** \brief */
    int _vgap;
    /** \brief */
    bool _align_on_baseline;

  public:
    /**
     * \brief
     *
     */
    FlowLayout(jflowlayout_align_t align = jflowlayout_align_t::Center, int hgap = 8, int vgap = 8);
    
    /**
     * \brief
     *
     */
    virtual ~FlowLayout();

    /**
     * \brief
     *
     */
    virtual jflowlayout_align_t GetAlign();

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
    virtual bool GetAlignOnBaseline();
    
    /**
     * \brief
     *
     */
    virtual void SetAlign(jflowlayout_align_t align);
    
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
    virtual void SetAlignOnBaseline(bool align_on_baseline);

    /**
     * \brief
     *
     */
    virtual int MoveComponents(std::shared_ptr<Container> target, int x, int y, int width, int height, int rowStart, int rowEnd, bool ltr, bool useBaseline, int *ascent, int *descent);

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

