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
 *   aint64_t with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef J_GRIDBAGLAYOUT_H
#define J_GRIDBAGLAYOUT_H

#include "jcanvas/widgets/jlayout.h"
#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/core/jenum.h"

#include <map>

namespace jcanvas {

enum class jgridbaglayout_constraints_t {
  // Specifies that this component is the next-to-last component in its 
  // column or row (<code>gridwidth</code>, <code>gridheight</code>), 
  // or that this component be placed next to the previously added 
  // component (<code>gridx</code>, <code>gridy</code>). 
  Relative = -1,
  // Specifies that this component is the 
  // last component in its column or row. 
  Remainder = 0,
  // Do not resize the component. 
  None = 0,
  // Resize the component both horizontally and vertically. 
  Both = 1,
  // Resize the component horizontally but not vertically. 
  Horizontal = 2,
  // Resize the component vertically but not horizontally. 
  Vertical = 3,
  // Put the component in the center of its display area.
  Center = 10,
  // Put the component at the top of its display area, centered horizontally. 
  North = 11,
  // Put the component at the top-right corner of its display area. 
  NorthEast = 12,
  // Put the component on the right side of its display area, centered vertically.
  East = 13,
  // Put the component at the bottom-right corner of its display area. 
  SouthEast = 14,
  // Put the component at the bottom of its display area, centered horizontally. 
  South = 15,
  // Put the component at the bottom-left corner of its display area. 
  SouthWest = 16,
  // Put the component on the left side of its display area, centered vertically.
  West = 17,
  // Put the component at the top-left corner of its display area. 
  NorthWest = 18,
  // Place the component centered aint64_t the edge of its display area
  // associated with the start of a page for the current
  // <code>ComponentOrienation</code>.  Equal to NORTH for horizontal orientations. 
  PageStart = 19,
  // Place the component centered aint64_t the edge of its display area  
  // associated with the end of a page for the current
  // <code>ComponentOrienation</code>.  Equal to SOUTH for horizontal orientations.
  PageEnd = 20,
  // Place the component centered aint64_t the edge of its display area where 
  // lines of text would normally begin for the current 
  // <code>ComponentOrienation</code>.  Equal to WEST for horizontal,
  // left-to-right orientations and EAST for horizontal, right-to-left orientations.
  LineStart = 21,
  // Place the component centered aint64_t the edge of its display area where 
  // lines of text would normally end for the current 
  // <code>ComponentOrienation</code>.  Equal to EAST for horizontal,
  // left-to-right orientations and WEST for horizontal, right-to-left orientations.
  LineEnd = 22,
  // Place the component in the corner of its display area where 
  // the first line of text on a page would normally begin for the current 
  // <code>ComponentOrienation</code>.  Equal to NORTHWEST for horizontal,
  // left-to-right orientations and NORTHEAST for horizontal, right-to-left orientations.
  FirstLineStart = 23,
  // Place the component in the corner of its display area where 
  // the first line of text on a page would normally end for the current 
  // <code>ComponentOrienation</code>.  Equal to NORTHEAST for horizontal,
  // left-to-right orientations and NORTHWEST for horizontal, right-to-left 
  // orientations.
  FirstLineEnd = 24,
  // Place the component in the corner of its display area where 
  // the last line of text on a page would normally start for the current 
  // <code>ComponentOrienation</code>.  Equal to SOUTHWEST for horizontal,
  // left-to-right orientations and SOUTHEAST for horizontal, right-to-left 
  // orientations.
  LastLineStart = 25,
  // Place the component in the corner of its display area where 
  // the last line of text on a page would normally end for the current 
  // <code>ComponentOrienation</code>.  Equal to SOUTHEAST for horizontal,
  // left-to-right orientations and SOUTHWEST for horizontal, right-to-left 
  // orientations.
  LastLineEnd = 26,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally centered and
  // vertically aligned aint64_t the baseline of the prevailing row.
  // If the component does not have a baseline it will be vertically centered.
  Baseline = 0x100,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally placed aint64_t the
  // leading edge.  For components with a left-to-right orientation,
  // the leading edge is the left edge.  Vertically the component is
  // aligned aint64_t the baseline of the prevailing row.  If the
  // component does not have a baseline it will be vertically centered.
  BaselineLeading = 0x200,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally placed aint64_t the
  // trailing edge.  For components with a left-to-right
  // orientation, the trailing edge is the right edge.  Vertically
  // the component is aligned aint64_t the baseline of the prevailing
  // row.  If the component does not have a baseline it will be
  // vertically centered.
  BaselineTrailing = 0x300,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally centered.  Vertically
  // the component is positioned so that its bottom edge touches
  // the baseline of the starting row.  If the starting row does not
  // have a baseline it will be vertically centered.
  AboveBaseline = 0x400,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally placed aint64_t the
  // leading edge.  For components with a left-to-right orientation,
  // the leading edge is the left edge.  Vertically the component is
  // positioned so that its bottom edge touches the baseline of the
  // starting row.  If the starting row does not have a baseline it
  // will be vertically centered.
  AboveBaselineLeading = 0x500,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally placed aint64_t the
  // trailing edge.  For components with a left-to-right
  // orientation, the trailing edge is the right edge.  Vertically
  // the component is positioned so that its bottom edge touches
  // the baseline of the starting row.  If the starting row does not
  // have a baseline it will be vertically centered.
  AboveBaselineTrailing = 0x600,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally centered.  Vertically
  // the component is positioned so that its top edge touches the
  // baseline of the starting row.  If the starting row does not
  // have a baseline it will be vertically centered.
  BelowBaseline = 0x700,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally placed aint64_t the
  // leading edge.  For components with a left-to-right orientation,
  // the leading edge is the left edge.  Vertically the component is
  // positioned so that its top edge touches the baseline of the
  // starting row.  If the starting row does not have a baseline it
  // will be vertically centered.
  BelowBaselineLeading = 0x800,
  // Possible value for the <code>anchor</code> field.  Specifies
  // that the component should be horizontally placed aint64_t the
  // trailing edge.  For components with a left-to-right
  // orientation, the trailing edge is the right edge.  Vertically
  // the component is positioned so that its top edge touches the
  // baseline of the starting row.  If the starting row does not
  // have a baseline it will be vertically centered.
  BelowBaselineTrailing = 0x900
};

enum class jgridbaglayout_style_t {
  // The smallest grid that can be laid out by the grid bag layout.
  MinSize = 1,
  // The preferred grid size that can be laid out by the grid bag layout.
  PreferredSize = 2,
  // This field is no int64_ter used to reserve arrays and keeped for backward
  // compatibility. Previously, this was
  // the maximum number of grid positions (both horizontal and 
  // vertical) that could be laid out by the grid bag layout.
  // Current implementation doesn't impose any limits on the size of a grid. 
  MaxGridSize = 512
};

class Container;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GridBagConstraints {

  private:

  public:
    /** \brief */
    jinsets_t<int> insets;
    /** \brief */
    double weightx;
    /** \brief */
    double weighty;
    /** \brief */
    int anchor;
    /** \brief */
    int fill;
    /** \brief */
    int ipadx;
    /** \brief */
    int ipady;
    /** \brief */
    int tempX;
    /** \brief */
    int tempY;
    /** \brief */
    int tempWidth;
    /** \brief */
    int tempHeight;
    /** \brief */
    int minWidth;
    /** \brief */
    int minHeight;
    /** \brief */
    int ascent;
    /** \brief */
    int descent;
    /** \brief */
    int centerPadding;
    /** \brief */
    int centerOffset;
    /** \brief */
    int gridx;
    /** \brief */
    int gridy;
    /** \brief */
    int gridwidth;
    /** \brief */
    int gridheight;
    /** \brief */
    jcomponent_behavior_t baselineResizeBehavior;

  public:
    GridBagConstraints() 
    {
      this->gridx = jenum_t{jgridbaglayout_constraints_t::Relative};
      this->gridy = jenum_t{jgridbaglayout_constraints_t::Relative};
      this->gridwidth = 1;
      this->gridheight = 1;

      this->weightx = 0;
      this->weighty = 0;
      this->anchor = jenum_t{jgridbaglayout_constraints_t::Center};
      this->fill = jenum_t{jgridbaglayout_constraints_t::None};

      this->ipadx = 0;
      this->ipady = 0;

      this->insets.left = 0;
      this->insets.right = 0;
      this->insets.top = 0;
      this->insets.bottom = 0;

      this->tempX = 0;
      this->tempY = 0;
      this->tempWidth = 0;
      this->tempHeight = 0;
      this->minWidth = 0;
      this->minHeight = 0;
      this->ascent = 0;
      this->descent = 0;
      this->centerPadding = 0;
      this->centerOffset = 0;
      this->baselineResizeBehavior = jcomponent_behavior_t::Other;
    }

    GridBagConstraints(int gridx, int gridy, int gridwidth, int gridheight, double weightx, double weighty, int anchor, int fill, jinsets_t<int> insets, int ipadx, int ipady) 
    {
      this->gridx = gridx;
      this->gridy = gridy;
      this->gridwidth = gridwidth;
      this->gridheight = gridheight;
      this->fill = fill;
      this->ipadx = ipadx;
      this->ipady = ipady;
      this->insets = insets;
      this->anchor  = anchor;
      this->weightx = weightx;
      this->weighty = weighty;

      this->tempX = 0;
      this->tempY = 0;
      this->tempWidth = 0;
      this->tempHeight = 0;
      this->minWidth = 0;
      this->minHeight = 0;
      this->ascent = 0;
      this->descent = 0;
      this->centerPadding = 0;
      this->centerOffset = 0;
      this->baselineResizeBehavior = jcomponent_behavior_t::Other;
    }

    virtual ~GridBagConstraints()
    {
    }

    virtual bool IsVerticallyResizable() 
    {
      jgridbaglayout_constraints_t param = static_cast<jgridbaglayout_constraints_t>(fill);

      return (param == jgridbaglayout_constraints_t::Both || param == jgridbaglayout_constraints_t::Vertical);
    }
};

class GridBagLayoutInfo {

  public:
    /** \brief */
    int width; 
    /** \brief */
    int height;            // number of  cells: horizontal and vertical
    /** \brief */
    int startx; 
    /** \brief */
    int starty;            // starting point for layout
    /** \brief */
    int *minWidth;        // largest minWidth in each column
    /** \brief */
    int *minHeight;        // largest minHeight in each row
    /** \brief */
    int *maxAscent;        // Max ascent (baseline)
    /** \brief */
    int *maxDescent;      // Max descent (height - baseline)
    /** \brief */
    double *weightX;      // largest weight in each column
    /** \brief */
    double *weightY;      // largest weight in each row
    /** \brief */
    bool hasBaseline;      // Whether or not baseline layout has been requested and one of the components has a valid baseline
    // These are only valid if hasBaseline is true and are indexed by row
    /** \brief */
    short *baselineType;  // The type of baseline for a particular row.  A mix of the BaselineResizeBehavior constants (1 << ordinal())
    /** \brief */
    int minWidthSize;
    /** \brief */
    int minHeightSize;
    /** \brief */
    int maxAscentSize;
    /** \brief */
    int maxDescentSize;
    /** \brief */
    int weightXSize;
    /** \brief */
    int weightYSize;
    /** \brief */
    int baselineTypeSize;

  public:
    GridBagLayoutInfo(int width, int height) 
    {
      this->width = width;
      this->height = height;
      this->startx = 0;
      this->starty = 0;  
      this->minWidth = nullptr;
      this->minHeight = nullptr;  
      this->maxAscent = nullptr;  
      this->maxDescent = nullptr;
      this->weightX = nullptr;
      this->weightY = nullptr;
      this->hasBaseline = false;
      this->baselineType = nullptr;
      this->minWidthSize = 0;
      this->minHeightSize = 0;
      this->maxAscentSize = 0;
      this->maxDescentSize = 0;
      this->weightXSize = 0;
      this->weightYSize = 0;
      this->baselineTypeSize = 0;
    }

    virtual ~GridBagLayoutInfo() 
    {
      if (minWidth != nullptr) {
        // delete minWidth;
      }

      if (minHeight != nullptr) {
        // delete minHeight;
      }

      if (maxAscent != nullptr) {
        delete maxAscent;
      }

      if (maxDescent != nullptr) {
        delete maxDescent;
      }

      if (weightX != nullptr) {
        // delete weightX;
      }

      if (weightY != nullptr) {
        // delete weightY;
      }

      if (baselineType != nullptr) {
        delete baselineType;
      }
    }

    virtual bool HasConstantDescent(int row) 
    {
      int ordinal = 0; // TODO:: Component.BaselineResizeBehavior.CONSTANT_DESCENT.ordinal();

      return ((baselineType[row] & (1 << ordinal)) != 0);
    }

    virtual bool HasBaseline(int row) 
    {
      return (hasBaseline && baselineType[row] != 0);
    }
};

class GridBagLayout : public Layout {

  private:
    std::map<Component *, GridBagConstraints *> comptable;
    GridBagConstraints *defaultConstraints;
    GridBagLayoutInfo *layoutInfo;
    Component *componentAdjusting;
    int columnWidthsSize,
        rowHeightsSize,
        columnWeightsSize,
        rowWeightsSize;
    bool rightToLeft;

  public:
    /**
     * \brief
     *
     */
    GridBagLayout();
    
    /**
     * \brief
     *
     */
    virtual ~GridBagLayout();

    /**
     * \brief
     *
     */
    virtual void SetConstraints(Component *comp, GridBagConstraints *constraints);
    
    /**
     * \brief
     *
     */
    virtual GridBagConstraints * GetConstraints(Component *comp);
    
    /**
     * \brief
     *
     */
    virtual GridBagConstraints * LookupConstraints(Component *comp);
    
    /**
     * \brief
     *
     */
    virtual void RemoveConstraints(Component *comp);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetLayoutOrigin();
    
    /**
     * \brief
     *
     */
    virtual void AddLayoutComponent(Component *comp, GridBagConstraints *constraints);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponent(Component *comp);
    
    /**
     * \brief
     *
     */
    virtual GridBagLayoutInfo * GetLayoutInfo(Container *parent, jgridbaglayout_style_t sizeflag);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> PreInitMaximumArraySizes(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual bool CalculateBaseline(Component *c, GridBagConstraints *constraints, jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual void AdjustForGravity(GridBagConstraints *constraints, jrect_t<int> *r);
    
    /**
     * \brief
     *
     */
    virtual void AlignOnBaseline(GridBagConstraints *cons, jrect_t<int> *r, int cellY, int cellHeight);
    
    /**
     * \brief
     *
     */
    virtual void AlignAboveBaseline(GridBagConstraints *cons, jrect_t<int> *r, int cellY, int cellHeight);
    
    /**
     * \brief
     *
     */
    virtual void AlignBelowBaseline(GridBagConstraints *cons, jrect_t<int> *r, int cellY, int cellHeight);
    
    /**
     * \brief
     *
     */
    virtual void CenterVertically(GridBagConstraints *cons, jrect_t<int> *r, int cellHeight);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMinSize(Container *parent, GridBagLayoutInfo *info);
    
    /**
     * \brief
     *
     */
    virtual void ArrangeGrid(Container *parent);

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

