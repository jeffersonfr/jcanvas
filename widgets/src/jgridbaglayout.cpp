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
 *   aint64_t with this program; if not, write to the                      *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jcanvas/widgets/jgridbaglayout.h"
#include "jcanvas/widgets/jcontainer.h"

#define EMPIRIC_MULTIPLIER 2

namespace jcanvas {

GridBagLayout::GridBagLayout():
  Layout()
{
  defaultConstraints = new GridBagConstraints();

  rightToLeft = false;
  layoutInfo = nullptr;
  componentAdjusting = nullptr;
  columnWidthsSize = 0;
  rowHeightsSize = 0;
  columnWeightsSize = 0;
  rowWeightsSize = 0;
  rightToLeft = 0;
}

GridBagLayout::~GridBagLayout()
{
  if (layoutInfo != nullptr) {
    delete [] layoutInfo->weightX;
    delete [] layoutInfo->weightY;
    delete [] layoutInfo->minWidth;
    delete [] layoutInfo->minHeight;
    
    delete layoutInfo;
  }

  while (comptable.size() > 0) {
    GridBagConstraints *constraint = comptable.begin()->second;

    delete constraint;

    comptable.erase(comptable.begin());
  }
}

jpoint_t<int> GridBagLayout::GetMinimumLayoutSize(std::shared_ptr<Container> parent)
{
  GridBagLayoutInfo *info = GetLayoutInfo(parent, jgridbaglayout_style_t::MinSize);

  return GetMinSize(parent, info);
}

jpoint_t<int> GridBagLayout::GetMaximumLayoutSize(std::shared_ptr<Container> parent)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> GridBagLayout::GetPreferredLayoutSize(std::shared_ptr<Container> parent)
{
  GridBagLayoutInfo *info = GetLayoutInfo(parent, jgridbaglayout_style_t::PreferredSize);

  return GetMinSize(parent, info);
}

void GridBagLayout::DoLayout(std::shared_ptr<Container> parent)
{
  ArrangeGrid(parent);
}

void GridBagLayout::SetConstraints(std::shared_ptr<Component> comp, GridBagConstraints *constraints) 
{
  GridBagConstraints *c = new GridBagConstraints();

  *c = *constraints;

  comptable[comp] = c;
}

GridBagConstraints * GridBagLayout::GetConstraints(std::shared_ptr<Component> comp) 
{
  std::map<std::shared_ptr<Component>, GridBagConstraints *>::iterator i=comptable.find(comp);

  if (i == comptable.end()) {
    SetConstraints(comp, defaultConstraints);

    i = comptable.find(comp);
  }

  GridBagConstraints *c = new GridBagConstraints();

  *c = *i->second;

  return c;
}

GridBagConstraints * GridBagLayout::LookupConstraints(std::shared_ptr<Component> comp) 
{
  std::map<std::shared_ptr<Component>, GridBagConstraints *>::iterator i=comptable.find(comp);

  if (i == comptable.end()) {
    SetConstraints(comp, defaultConstraints);

    return comptable[comp];
  }

  return i->second;
}

void GridBagLayout::RemoveConstraints(std::shared_ptr<Component> comp) 
{
  comptable.erase(comp);
}

jpoint_t<int> GridBagLayout::GetLayoutOrigin () 
{
  jpoint_t<int> origin = {0, 0};
  if (layoutInfo != nullptr) {
    origin.x = layoutInfo->startx;
    origin.y = layoutInfo->starty;
  }
  return origin;
}

void GridBagLayout::AddLayoutComponent(std::shared_ptr<Component> comp, GridBagConstraints *constraints)
{
  SetConstraints(comp, constraints);
}

void GridBagLayout::RemoveLayoutComponent(std::shared_ptr<Component> comp) 
{
  RemoveConstraints(comp);
}

jpoint_t<int> GridBagLayout::PreInitMaximumArraySizes(std::shared_ptr<Container> parent)
{
  const std::vector<std::shared_ptr<Component>> 
    &components = parent->GetComponents();
  std::shared_ptr<Component>
    comp;
  GridBagConstraints 
    *constraints = nullptr;
  jpoint_t<int>
    returnPoint;
  int 
    curX, 
    curY,
    curWidth,
    curHeight,
    preMaximumArrayXIndex = 0,
    preMaximumArrayYIndex = 0;

  for (int compId = 0 ; compId < (int)components.size() ; compId++) {
    comp = components[compId];

    if (!comp->IsVisible()) {
      continue;
    }

    constraints = LookupConstraints(comp);
    curX = constraints->gridx;
    curY = constraints->gridy;
    curWidth = constraints->gridwidth;
    curHeight = constraints->gridheight;

    // -1==RELATIVE, means that column|row equals to previously added component,
    // since each next Component with gridx|gridy == RELATIVE starts from
    // previous position, so we should start from previous component which
    // already used in maximumArray[X|Y]Index calculation. We could just increase
    // maximum by 1 to handle situation when component with gridx=-1 was added.
    if (curX < 0){
      curX = ++preMaximumArrayYIndex;
    }
    
    if (curY < 0){
      curY = ++preMaximumArrayXIndex;
    }
    
    // gridwidth|gridheight may be equal to RELATIVE (-1) or REMAINDER (0)
    // in any case using 1 instead of 0 or -1 should be sufficient to for
    // correct maximumArraySizes calculation
    if (curWidth <= 0){
      curWidth = 1;
    }
    
    if (curHeight <= 0){
      curHeight = 1;
    }

    preMaximumArrayXIndex = std::max(curY + curHeight, preMaximumArrayXIndex);
    preMaximumArrayYIndex = std::max(curX + curWidth, preMaximumArrayYIndex);
  } 
  
  // Must specify index++ to allocate well-working arrays.
  returnPoint.x = preMaximumArrayXIndex;
  returnPoint.y = preMaximumArrayYIndex;

  return returnPoint;
} 

GridBagLayoutInfo * GridBagLayout::GetLayoutInfo(std::shared_ptr<Container> parent, jgridbaglayout_style_t sizeflag) 
{
  const std::vector<std::shared_ptr<Component>> &components = parent->GetComponents();

  // WARN:: sync parent
  GridBagLayoutInfo *r;
  std::shared_ptr<Component> comp;
  GridBagConstraints *constraints;
  jpoint_t<int> d;
  // Code below will address index curX+curWidth in the case of yMaxArray, weightY
  // ( respectively curY+curHeight for xMaxArray, weightX ) where
  //  curX in 0 to preInitMaximumArraySizes.y
  // Thus, the maximum index that could
  // be calculated in the following code is curX+curX.
  // EmpericMultier equals 2 because of this.

  double 
    weight_diff, 
    weight;
  int 
    layoutWidth, 
    layoutHeight,
    compindex, 
    i, 
    k, 
    px, 
    py, 
    pixels_diff, 
    nextSize,
    anchor;
  int 
    curX = 0,  // constraints->gridx
    curY = 0; // constraints->gridy
  int 
    curWidth = 1,    // constraints->gridwidth
    curHeight = 1;  // constraints->gridheight
  int 
    curRow, 
    curCol;
  int 
    maximumArrayXIndex = 0,
    maximumArrayYIndex = 0;

  /*
   * Pass #1
   *
   * Figure out the dimensions of the layout grid (use a value of 1 for
   * zero or negative widths and heights).
   */

  layoutWidth = layoutHeight = 0; 
  curRow = curCol = -1;

  jpoint_t<int> arraySizes = PreInitMaximumArraySizes(parent);

  /* fix for 4623196.
   * If user try to create a very big grid we can
   * get NegativeArraySizeException because of integer value
   * overflow (EMPIRICMULTIPLIER*gridSize might be more then INT_MAX).
   * We need to detect this situation and try to create a
   * grid with INT_MAX size instead.
   */
  maximumArrayXIndex = (EMPIRIC_MULTIPLIER * arraySizes.x > INT_MAX )? INT_MAX : EMPIRIC_MULTIPLIER*(int)arraySizes.x;
  maximumArrayYIndex = (EMPIRIC_MULTIPLIER * arraySizes.y > INT_MAX )? INT_MAX : EMPIRIC_MULTIPLIER*(int)arraySizes.y;

  bool hasBaseline = false;
  for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
    comp = components[compindex];
    if (!comp->IsVisible())
      continue;
    constraints = LookupConstraints(comp);

    curX = constraints->gridx;
    curY = constraints->gridy;
    curWidth = constraints->gridwidth;
    if (curWidth <= 0)
      curWidth = 1;
    curHeight = constraints->gridheight;
    if (curHeight <= 0)
      curHeight = 1;

    /* If x or y is negative, then use relative positioning: */
    if (curX < 0 && curY < 0) {
      if (curRow >= 0)
        curY = curRow;
      else if (curCol >= 0)
        curX = curCol;
      else
        curY = 0;
    }
    if (curX < 0) {
      px = 0;

      curX = px - curX - 1;
      if(curX < 0)
        curX = 0;
    }
    else if (curY < 0) {
      py = 0;
      curY = py - curY - 1;
      if(curY < 0)
        curY = 0;
    }

    /* Adjust the grid width and height 
     *  fix for 5005945: unneccessary loops removed
     */
    px = curX + curWidth;
    if (layoutWidth < px) {
      layoutWidth = px;
    }
    py = curY + curHeight;
    if (layoutHeight < py) {
      layoutHeight = py;
    }

    /* Cache the current slave's size. */
    if (sizeflag == jgridbaglayout_style_t::PreferredSize) {
      d = comp->GetPreferredSize();
    } else {
      d = comp->GetMinimumSize();
    }
    constraints->minWidth = d.x;
    constraints->minHeight = d.y;
    if (CalculateBaseline(comp, constraints, d)) {
      hasBaseline = true;
    }

    /* Zero width and height must mean that this is the last item (or
     * else something is wrong). */
    if (constraints->gridheight == 0 && constraints->gridwidth == 0)
      curRow = curCol = -1;

    /* Zero width starts a new row */
    if (constraints->gridheight == 0 && curRow < 0)
      curCol = curX + curWidth;

    /* Zero height starts a new column */
    else if (constraints->gridwidth == 0 && curCol < 0)
      curRow = curY + curHeight;
  } //for (components) loop


  r = new GridBagLayoutInfo(layoutWidth, layoutHeight);

  /*
   * Pass #2
   *
   * Negative values for gridX are filled in with the current x value.
   * Negative values for gridY are filled in with the current y value.
   * Negative or zero values for gridWidth and gridHeight end the current
   *  row or column, respectively.
   */

  curRow = curCol = -1;

  int *maxAscent = nullptr,
      *maxDescent = nullptr;
  short *baselineType = nullptr;

  if (hasBaseline) {
    r->maxAscent = maxAscent = new int[layoutHeight];
    r->maxDescent = maxDescent = new int[layoutHeight];
    r->baselineType = baselineType = new short[layoutHeight];

    r->maxAscentSize = layoutHeight;
    r->maxDescentSize = layoutHeight;
    r->baselineTypeSize = layoutHeight;

    r->hasBaseline = true;
  }

  for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
    comp = components[compindex];
    if (!comp->IsVisible())
      continue;
    constraints = LookupConstraints(comp);

    curX = constraints->gridx;
    curY = constraints->gridy;
    curWidth = constraints->gridwidth;
    curHeight = constraints->gridheight;

    /* If x or y is negative, then use relative positioning: */
    if (curX < 0 && curY < 0) {
      if(curRow >= 0)
        curY = curRow;
      else if(curCol >= 0)
        curX = curCol;
      else
        curY = 0;
    }

    if (curX < 0) {
      if (curHeight <= 0) {
        curHeight += r->height - curY;
        if (curHeight < 1)
          curHeight = 1;
      }

      px = 0;

      curX = px - curX - 1;
      if(curX < 0)
        curX = 0;
    }
    else if (curY < 0) {
      if (curWidth <= 0) {
        curWidth += r->width - curX;
        if (curWidth < 1)
          curWidth = 1;
      }

      py = 0;

      curY = py - curY - 1;
      if(curY < 0)
        curY = 0;
    }

    if (curWidth <= 0) {
      curWidth += r->width - curX;
      if (curWidth < 1)
        curWidth = 1;
    }

    if (curHeight <= 0) {
      curHeight += r->height - curY;
      if (curHeight < 1)
        curHeight = 1;
    }

    px = curX + curWidth;
    py = curY + curHeight;

    /* Make negative sizes start a new row/column */
    if (constraints->gridheight == 0 && constraints->gridwidth == 0)
      curRow = curCol = -1;
    if (constraints->gridheight == 0 && curRow < 0)
      curCol = curX + curWidth;
    else if (constraints->gridwidth == 0 && curCol < 0)
      curRow = curY + curHeight;

    /* Assign the new values to the gridbag slave */
    constraints->tempX = curX;
    constraints->tempY = curY;
    constraints->tempWidth = curWidth;
    constraints->tempHeight = curHeight;

    anchor = constraints->anchor;

    if (hasBaseline) {
      switch(anchor) {
        case static_cast<int>(jgridbaglayout_constraints_t::Baseline):
        case static_cast<int>(jgridbaglayout_constraints_t::BaselineLeading):
        case static_cast<int>(jgridbaglayout_constraints_t::BaselineTrailing):
          if (constraints->ascent >= 0) {
            if (curHeight == 1) {
              maxAscent[curY] = std::max(maxAscent[curY], constraints->ascent);
              maxDescent[curY] = std::max(maxDescent[curY], constraints->descent);
            } else {
              if (constraints->baselineResizeBehavior == jcomponent_behavior_t::ConstantDescent) {
                maxDescent[curY + curHeight - 1] =
                  std::max(maxDescent[curY + curHeight - 1], constraints->descent);
              }
              else {
                maxAscent[curY] = std::max(maxAscent[curY], constraints->ascent);
              }
            }

            int ordinal = 0; // WARN:: constraints->baselineResizeBehavior.ordinal();

            if (constraints->baselineResizeBehavior == jcomponent_behavior_t::ConstantDescent) {
              baselineType[curY + curHeight - 1] |= (1 << ordinal); 
            }
            else {
              baselineType[curY] |= (1 << ordinal);
            }
          }
          break;
        case static_cast<int>(jgridbaglayout_constraints_t::AboveBaseline):
        case static_cast<int>(jgridbaglayout_constraints_t::AboveBaselineLeading):
        case static_cast<int>(jgridbaglayout_constraints_t::AboveBaselineTrailing):
          // Component positioned above the baseline.
          // To make the bottom edge of the component aligned
          // with the baseline the bottom inset is
          // added to the descent, the rest to the ascent.
          pixels_diff = constraints->minHeight + constraints->insets.top + constraints->ipady;
          maxAscent[curY] = std::max(maxAscent[curY], pixels_diff);
          maxDescent[curY] = std::max(maxDescent[curY], constraints->insets.bottom);
          break;
        case static_cast<int>(jgridbaglayout_constraints_t::BelowBaseline):
        case static_cast<int>(jgridbaglayout_constraints_t::BelowBaselineLeading):
        case static_cast<int>(jgridbaglayout_constraints_t::BelowBaselineTrailing):
          // Component positioned below the baseline. 
          // To make the top edge of the component aligned
          // with the baseline the top inset is
          // added to the ascent, the rest to the descent.
          pixels_diff = constraints->minHeight + constraints->insets.bottom + constraints->ipady;
          maxDescent[curY] = std::max(maxDescent[curY], pixels_diff);
          maxAscent[curY] = std::max(maxAscent[curY], constraints->insets.top);
          break;
      }
    }
  }

  if (r->weightX != nullptr) {
    delete r->weightX;
  }

  if (r->weightY != nullptr) {
    delete r->weightY;
  }

  if (r->minWidth != nullptr) {
    delete r->minWidth;
  }

  if (r->minHeight != nullptr) {
    delete r->minHeight;
  }

  r->weightX = new double[maximumArrayYIndex];
  r->weightY = new double[maximumArrayXIndex];
  r->minWidth = new int[maximumArrayYIndex];
  r->minHeight = new int[maximumArrayXIndex];

  r->weightXSize = maximumArrayYIndex;
  r->weightYSize = maximumArrayXIndex;
  r->minWidthSize = maximumArrayYIndex;
  r->minHeightSize = maximumArrayXIndex;

  for (i=0; i<maximumArrayXIndex; i++) {
    r->weightY[i] = 0;
    r->minHeight[i] = 0;
  }

  for (i=0; i<maximumArrayYIndex; i++) {
    r->weightX[i] = 0;
    r->minWidth[i] = 0;
  }

  /*
   * Pass #3
   *
   * Distribute the minimun widths and weights:
   */

  nextSize = INT_MAX;

  for (i=1; i!=INT_MAX; i=nextSize, nextSize=INT_MAX) {
    for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
      comp = components[compindex];
      if (!comp->IsVisible())
        continue;
      constraints = LookupConstraints(comp);

      if (constraints->tempWidth == i) {
        px = constraints->tempX + constraints->tempWidth; /* right column */

        /*
         * Figure out if we should use this slave\'s weight.  If the weight
         * is less than the total weight spanned by the width of the cell,
         * then discard the weight.  Otherwise split the difference
         * according to the existing weights.
         */

        weight_diff = constraints->weightx;
        for (k = constraints->tempX; k < px; k++)
          weight_diff -= r->weightX[k];
        if (weight_diff > 0.0) {
          weight = 0.0;
          for (k = constraints->tempX; k < px; k++)
            weight += r->weightX[k];
          for (k = constraints->tempX; weight > 0.0 && k < px; k++) {
            double wt = r->weightX[k];
            double dx = (wt * weight_diff) / weight;
            r->weightX[k] += dx;
            weight_diff -= dx;
            weight -= wt;
          }
          /* Assign the remainder to the rightmost cell */
          r->weightX[px-1] += weight_diff;
        }

        /*
         * Calculate the minWidth array values.
         * First, figure out how wide the current slave needs to be.
         * Then, see if it will fit within the current minWidth values.
         * If it will not fit, add the difference according to the
         * weightX array.
         */

        pixels_diff =
          constraints->minWidth + constraints->ipadx +
          constraints->insets.left + constraints->insets.right;

        for (k = constraints->tempX; k < px; k++)
          pixels_diff -= r->minWidth[k];
        if (pixels_diff > 0) {
          weight = 0.0;
          for (k = constraints->tempX; k < px; k++)
            weight += r->weightX[k];
          for (k = constraints->tempX; weight > 0.0 && k < px; k++) {
            double wt = r->weightX[k];
            int dx = (int)((wt * ((double)pixels_diff)) / weight);
            r->minWidth[k] += dx;
            pixels_diff -= dx;
            weight -= wt;
          }
          /* Any leftovers go into the rightmost cell */
          r->minWidth[px-1] += pixels_diff;
        }
      }
      else if (constraints->tempWidth > i && constraints->tempWidth < nextSize)
        nextSize = constraints->tempWidth;


      if (constraints->tempHeight == i) {
        py = constraints->tempY + constraints->tempHeight; /* bottom row */

        /*
         * Figure out if we should use this slave's weight.  If the weight
         * is less than the total weight spanned by the height of the cell,
         * then discard the weight.  Otherwise split it the difference
         * according to the existing weights.
         */

        weight_diff = constraints->weighty;
        for (k = constraints->tempY; k < py; k++)
          weight_diff -= r->weightY[k];
        if (weight_diff > 0.0) {
          weight = 0.0;
          for (k = constraints->tempY; k < py; k++)
            weight += r->weightY[k];
          for (k = constraints->tempY; weight > 0.0 && k < py; k++) {
            double wt = r->weightY[k];
            double dy = (wt * weight_diff) / weight;
            r->weightY[k] += dy;
            weight_diff -= dy;
            weight -= wt;
          }
          /* Assign the remainder to the bottom cell */
          r->weightY[py-1] += weight_diff;
        }

        /*
         * Calculate the minHeight array values.
         * First, figure out how tall the current slave needs to be.
         * Then, see if it will fit within the current minHeight values.
         * If it will not fit, add the difference according to the
         * weightY array.
         */

        pixels_diff = -1;
        if (hasBaseline) {
          switch(constraints->anchor) {
            case static_cast<int>(jgridbaglayout_constraints_t::Baseline):
            case static_cast<int>(jgridbaglayout_constraints_t::BaselineLeading):
            case static_cast<int>(jgridbaglayout_constraints_t::BaselineTrailing):
              if (constraints->ascent >= 0) {
                if (constraints->tempHeight == 1) {
                  pixels_diff =
                    maxAscent[constraints->tempY] +
                    maxDescent[constraints->tempY];
                }
                else if (constraints->baselineResizeBehavior != jcomponent_behavior_t::ConstantDescent) {
                  pixels_diff =
                    maxAscent[constraints->tempY] +
                    constraints->descent;
                }
                else {
                  pixels_diff = constraints->ascent +
                    maxDescent[constraints->tempY +
                    constraints->tempHeight - 1];
                }
              }
              break;
            case static_cast<int>(jgridbaglayout_constraints_t::AboveBaseline):
            case static_cast<int>(jgridbaglayout_constraints_t::AboveBaselineLeading):
            case static_cast<int>(jgridbaglayout_constraints_t::AboveBaselineTrailing):
              pixels_diff = constraints->insets.top +
                constraints->minHeight +
                constraints->ipady +
                maxDescent[constraints->tempY];
              break;
            case static_cast<int>(jgridbaglayout_constraints_t::BelowBaseline):
            case static_cast<int>(jgridbaglayout_constraints_t::BelowBaselineLeading):
            case static_cast<int>(jgridbaglayout_constraints_t::BelowBaselineTrailing):
              pixels_diff = maxAscent[constraints->tempY] +
                constraints->minHeight +
                constraints->insets.bottom +
                constraints->ipady;
              break;
          }
        }
        if (pixels_diff == -1) {
          pixels_diff =
            constraints->minHeight + constraints->ipady +
            constraints->insets.top +
            constraints->insets.bottom;
        }
        for (k = constraints->tempY; k < py; k++)
          pixels_diff -= r->minHeight[k];
        if (pixels_diff > 0) {
          weight = 0.0;
          for (k = constraints->tempY; k < py; k++)
            weight += r->weightY[k];
          for (k = constraints->tempY; weight > 0.0 && k < py; k++) {
            double wt = r->weightY[k];
            int dy = (int)((wt * ((double)pixels_diff)) / weight);
            r->minHeight[k] += dy;
            pixels_diff -= dy;
            weight -= wt;
          }
          /* Any leftovers go into the bottom cell */
          r->minHeight[py-1] += pixels_diff;
        }
      }
      else if (constraints->tempHeight > i &&
          constraints->tempHeight < nextSize)
        nextSize = constraints->tempHeight;
    }
  }

  return r;
} 

bool GridBagLayout::CalculateBaseline(std::shared_ptr<Component> c, GridBagConstraints *constraints, jpoint_t<int> size) 
{
  int anchor = constraints->anchor;

  if (anchor == static_cast<int>(jgridbaglayout_constraints_t::Baseline) ||
      anchor == static_cast<int>(jgridbaglayout_constraints_t::BaselineLeading) ||
      anchor == static_cast<int>(jgridbaglayout_constraints_t::BaselineTrailing)) {
    // Apply the padding to the component, then ask for the baseline.
    int w = size.x + constraints->ipadx;
    int h = size.y + constraints->ipady;
    constraints->ascent = c->GetBaseline(w, h);
    if (constraints->ascent >= 0) {
      // Component has a baseline
      int baseline = constraints->ascent;
      // Adjust the ascent and descent to include the insets.
      constraints->descent = h - constraints->ascent +
        constraints->insets.bottom;
      constraints->ascent += constraints->insets.top;
      constraints->baselineResizeBehavior =
        c->GetBaselineResizeBehavior();
      constraints->centerPadding = 0;
      if (constraints->baselineResizeBehavior == jcomponent_behavior_t::CenterOffset) {
        // Component has a baseline resize behavior of
        // CENTER_OFFSET, calculate centerPadding and
        // centerOffset (see the description of
        // CENTER_OFFSET in the enum for detais on this
        // algorithm).
        int nextBaseline = c->GetBaseline(w, h + 1);
        constraints->centerOffset = baseline - h / 2;
        if (h % 2 == 0) {
          if (baseline != nextBaseline) {
            constraints->centerPadding = 1;
          }
        }
        else if (baseline == nextBaseline){
          constraints->centerOffset--;
          constraints->centerPadding = 1;
        }
      }
    }
    return true;
  }
  else {
    constraints->ascent = -1;
    return false;
  }
}

void GridBagLayout::AdjustForGravity(GridBagConstraints *constraints, jrect_t<int> *r) 
{
  int diffx, diffy;
  int cellY = r->point.y;
  int cellHeight = r->size.y;

  if (!rightToLeft) {
    r->point.x += constraints->insets.left;
  } else {
    r->point.x -= r->size.x - constraints->insets.right;
  }
  r->size.x -= (constraints->insets.left + constraints->insets.right);
  r->point.y += constraints->insets.top;
  // CHANGE:: remove negative symbol after parenthesis
  r->size.y -= (-constraints->insets.top + constraints->insets.bottom);

  diffx = 0;
  if ((constraints->fill != static_cast<int>(jgridbaglayout_constraints_t::Horizontal) && constraints->fill != static_cast<int>(jgridbaglayout_constraints_t::Both))
      && (r->size.x > (constraints->minWidth + constraints->ipadx))) {
    diffx = r->size.x - (constraints->minWidth + constraints->ipadx);
    r->size.x = constraints->minWidth + constraints->ipadx;
  }

  diffy = 0;
  if ((constraints->fill != static_cast<int>(jgridbaglayout_constraints_t::Vertical) && constraints->fill != static_cast<int>(jgridbaglayout_constraints_t::Both)) && (r->size.y > (constraints->minHeight + constraints->ipady))) {
    diffy = r->size.y - (constraints->minHeight + constraints->ipady);
    r->size.y = constraints->minHeight + constraints->ipady;
  }

  switch (constraints->anchor) {
    case static_cast<int>(jgridbaglayout_constraints_t::Baseline):
      r->point.x += diffx/2;
      AlignOnBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::BaselineLeading):
      if (rightToLeft) {
        r->point.x += diffx;
      }
      AlignOnBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::BaselineTrailing):
      if (!rightToLeft) {
        r->point.x += diffx;
      }
      AlignOnBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::AboveBaseline):
      r->point.x += diffx/2;
      AlignAboveBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::AboveBaselineLeading):
      if (rightToLeft) {
        r->point.x += diffx;
      }
      AlignAboveBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::AboveBaselineTrailing):
      if (!rightToLeft) {
        r->point.x += diffx;
      }
      AlignAboveBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::BelowBaseline):
      r->point.x += diffx/2;
      AlignBelowBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::BelowBaselineLeading):
      if (rightToLeft) {
        r->point.x += diffx;
      }
      AlignBelowBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::BelowBaselineTrailing):
      if (!rightToLeft) {
        r->point.x += diffx;
      }
      AlignBelowBaseline(constraints, r, cellY, cellHeight);
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::Center):
      r->point.x += diffx/2;
      r->point.y += diffy/2;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::PageStart):
    case static_cast<int>(jgridbaglayout_constraints_t::North):
      r->point.x += diffx/2;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::NorthEast):
      r->point.x += diffx;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::East):
      r->point.x += diffx;
      r->point.y += diffy/2;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::SouthEast):
      r->point.x += diffx;
      r->point.y += diffy;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::PageEnd):
    case static_cast<int>(jgridbaglayout_constraints_t::South):
      r->point.x += diffx/2;
      r->point.y = diffy;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::SouthWest):
      r->point.y += diffy;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::West):
      r->point.y += diffy/2;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::NorthWest):
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::LineStart):
      if (rightToLeft) {
        r->point.x += diffx;
      }
      r->point.y += diffy/2;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::LineEnd):
      if (!rightToLeft) {
        r->point.x += diffx;
      }
      r->point.y += diffy/2;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::FirstLineStart):
      if (rightToLeft) {
        r->point.x += diffx;
      }  
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::FirstLineEnd):
      if (!rightToLeft) {
        r->point.x += diffx;
      }  
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::LastLineStart):
      if (rightToLeft) {
        r->point.x += diffx;
      }
      r->point.y += diffy;
      break;
    case static_cast<int>(jgridbaglayout_constraints_t::LastLineEnd):
      if (!rightToLeft) {
        r->point.x += diffx;
      }
      r->point.y += diffy;
      break;
    default:
      break;
  }
}

void GridBagLayout::AlignOnBaseline(GridBagConstraints *cons, jrect_t<int> *r, int cellY, int cellHeight) 
{
  if (cons->ascent >= 0) {
    if (cons->baselineResizeBehavior == jcomponent_behavior_t::ConstantDescent) {
      // Anchor to the bottom.
      // Baseline is at (cellY + cellHeight - maxDescent).
      // Bottom of component (maxY) is at baseline + descent
      // of component. We need to subtract the bottom inset here
      // as the descent in the constraints object includes the
      // bottom inset.
      int maxY = cellY + cellHeight -
        layoutInfo->maxDescent[cons->tempY + cons->tempHeight - 1] +
        cons->descent - cons->insets.bottom;
      if (!cons->IsVerticallyResizable()) {
        // Component not resizable, calculate y location
        // from maxY - height.
        r->point.y = maxY - cons->minHeight;
        r->size.y = cons->minHeight;
      } else {
        // Component is resizable. As brb is constant descent,
        // can expand component to fill region above baseline.
        // Subtract out the top inset so that components insets
        // are honored.
        r->size.y = maxY - cellY - cons->insets.top;
      }
    }
    else {
      // BRB is not constant_descent
      int baseline; // baseline for the row, relative to cellY
      // Component baseline, includes insets.top
      int ascent = cons->ascent; 
      if (layoutInfo->HasConstantDescent(cons->tempY)) {
        // Mixed ascent/descent in same row, calculate position
        // off maxDescent
        baseline = cellHeight - layoutInfo->maxDescent[cons->tempY];
      }
      else {
        // Only ascents/unknown in this row, anchor to top
        baseline = layoutInfo->maxAscent[cons->tempY];
      }
      if (cons->baselineResizeBehavior == jcomponent_behavior_t::Other) {
        // BRB is other, which means we can only determine
        // the baseline by asking for it again giving the
        // size we plan on using for the component.
        bool fits = false;
        ascent = componentAdjusting->GetBaseline(r->size.x, r->size.y);
        if (ascent >= 0) {
          // Component has a baseline, pad with top inset
          // (this follows from calculateBaseline which
          // does the same).
          ascent += cons->insets.top;
        }
        if (ascent >= 0 && ascent <= baseline) {
          // Components baseline fits within rows baseline.
          // Make sure the descent fits within the space as well.
          if (baseline + (r->size.y - ascent - cons->insets.top) <=
              cellHeight - cons->insets.bottom) {
            // It fits, we're good.
            fits = true;
          }
          else if (cons->IsVerticallyResizable()) {
            // Doesn't fit, but it's resizable.  Try
            // again assuming we'll get ascent again.
            int ascent2 = componentAdjusting->GetBaseline(
                r->size.x, cellHeight - cons->insets.bottom -
                baseline + ascent);
            if (ascent2 >= 0) {
              ascent2 += cons->insets.top;
            }
            if (ascent2 >= 0 && ascent2 <= ascent) {
              // It'll fit
              r->size.y = cellHeight - cons->insets.bottom -
                baseline + ascent;
              ascent = ascent2;
              fits = true;
            }
          }
        }
        if (!fits) {
          // Doesn't fit, use min size and original ascent
          ascent = cons->ascent;
          r->size.x = cons->minWidth;
          r->size.y = cons->minHeight;
        }
      }
      // Reset the components y location based on
      // components ascent and baseline for row. Because ascent
      // includes the baseline
      r->point.y = cellY + baseline - ascent + cons->insets.top;
      if (cons->IsVerticallyResizable()) {
        switch(cons->baselineResizeBehavior) {
          case jcomponent_behavior_t::ConstantAscent:
            r->size.y = std::max(cons->minHeight,cellY + cellHeight - r->point.y - cons->insets.bottom);
            break;
          case jcomponent_behavior_t::CenterOffset:
            {
              int upper = r->point.y - cellY - cons->insets.top;
              int lower = cellY + cellHeight - r->point.y -  cons->minHeight - cons->insets.bottom;
              int delta = std::min(upper, lower);
              delta += delta;
              if (delta > 0 &&
                  (cons->minHeight + cons->centerPadding +
                   delta) / 2 + cons->centerOffset != baseline) {
                // Off by 1
                delta--;
              }
              r->size.y = cons->minHeight + delta;
              r->point.y = cellY + baseline -
                (r->size.y + cons->centerPadding) / 2 -
                cons->centerOffset;
            }
            break;
          case jcomponent_behavior_t::Other:
            // Handled above
            break;
          default:
            break;
        }
      }
    }
  }
  else {
    CenterVertically(cons, r, cellHeight);
  }
}

void GridBagLayout::AlignAboveBaseline(GridBagConstraints *cons, jrect_t<int> *r, int cellY, int cellHeight) 
{
  if (layoutInfo->HasBaseline(cons->tempY)) {
    int maxY; // Baseline for the row
    if (layoutInfo->HasConstantDescent(cons->tempY)) {
      // Prefer descent
      maxY = cellY + cellHeight - layoutInfo->maxDescent[cons->tempY];
    }
    else {
      // Prefer ascent
      maxY = cellY + layoutInfo->maxAscent[cons->tempY];
    }
    if (cons->IsVerticallyResizable()) {
      // Component is resizable. Top edge is offset by top
      // inset, bottom edge on baseline.
      r->point.y = cellY + cons->insets.top;
      r->size.y = maxY - r->point.y;
    }
    else {
      // Not resizable.
      r->size.y = cons->minHeight + cons->ipady;
      r->point.y = maxY - r->size.y;
    }
  }
  else {
    CenterVertically(cons, r, cellHeight);
  }
}

void GridBagLayout::AlignBelowBaseline(GridBagConstraints *cons, jrect_t<int> *r, int cellY, int cellHeight) 
{
  if (layoutInfo->HasBaseline(cons->tempY)) {
    if (layoutInfo->HasConstantDescent(cons->tempY)) {
      // Prefer descent
      r->point.y = cellY + cellHeight - layoutInfo->maxDescent[cons->tempY];
    }
    else {
      // Prefer ascent
      r->point.y = cellY + layoutInfo->maxAscent[cons->tempY];
    }
    if (cons->IsVerticallyResizable()) {
      r->size.y = cellY + cellHeight - r->point.y - cons->insets.bottom;
    }
  }
  else {
    CenterVertically(cons, r, cellHeight);
  }
}

void GridBagLayout::CenterVertically(GridBagConstraints *cons, jrect_t<int> *r, int cellHeight) 
{
  if (!cons->IsVerticallyResizable()) {
    r->point.y += std::max(0, (cellHeight - cons->insets.top - cons->insets.bottom - cons->minHeight - cons->ipady) / 2);
  }
}

jpoint_t<int> GridBagLayout::GetMinSize(std::shared_ptr<Container> parent, GridBagLayoutInfo *info) 
{
  jpoint_t<int> d = {0, 0};
  int i, t;
  jinsets_t insets = parent->GetInsets();

  t = 0;
  for(i = 0; i < info->width; i++) {
    t += info->minWidth[i];
  }
  d.x = t + insets.left + insets.right;

  t = 0;
  for(i = 0; i < info->height; i++) {
    t += info->minHeight[i];
  }
  d.y = t + insets.top + insets.bottom;

  return d;
}

void GridBagLayout::ArrangeGrid(std::shared_ptr<Container> parent) 
{
  const std::vector<std::shared_ptr<Component>> &components = parent->GetComponents();

  std::shared_ptr<Component> comp;
  GridBagConstraints *constraints;
  GridBagLayoutInfo *info;
  jinsets_t insets = parent->GetInsets();
  jrect_t<int> r = {0, 0};
  jpoint_t<int> d;
  double weight;
  int compindex;
  int i, diffw, diffh;

  rightToLeft = !(parent->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight);

  // If the parent has no slaves anymore, then don't do anything at all:  just leave the parent's size as-is.
  if ((int)components.size() == 0) {
    return;
  }

  jpoint_t<int> size = parent->GetSize();

  // Pass #1: scan all the slaves to figure out the total amount of space needed.
  info = GetLayoutInfo(parent, jgridbaglayout_style_t::PreferredSize);
  d = GetMinSize(parent, info);

  if (size.x < d.x || size.y < d.y) {
    delete [] info->weightX;
    delete [] info->weightY;
    delete [] info->minWidth;
    delete [] info->minHeight;
    
    delete info;

    info = GetLayoutInfo(parent, jgridbaglayout_style_t::MinSize);
    d = GetMinSize(parent, info);
  }

  layoutInfo = info;
  r.size.x = d.x;
  r.size.y = d.y;

  // If the current dimensions of the window don't match the desired dimensions, then adjust the minWidth and minHeight arrays according to the weights.
  diffw = size.x - r.size.x;

  if (diffw != 0) {
    weight = 0.0;
    
    for (i = 0; i < info->width; i++) {
      weight += info->weightX[i];
    }

    if (weight > 0.0) {
      for (i = 0; i < info->width; i++) {
        int dx = (int)(( ((double)diffw) * info->weightX[i]) / weight);
    
        info->minWidth[i] += dx;
        r.size.x += dx;
        
        if (info->minWidth[i] < 0) {
          r.size.x -= info->minWidth[i];
          info->minWidth[i] = 0;
        }
      }
    }

    diffw = size.x - r.size.x;
  } else {
    diffw = 0;
  }

  diffh = size.y - r.size.y;

  if (diffh != 0) {
    weight = 0.0;

    for (i = 0; i < info->height; i++) {
      weight += info->weightY[i];
    }

    if (weight > 0.0) {
      for (i = 0; i < info->height; i++) {
        int dy = (int)(( ((double)diffh) * info->weightY[i]) / weight);

        info->minHeight[i] += dy;
        r.size.y += dy;

        if (info->minHeight[i] < 0) {
          r.size.y -= info->minHeight[i];
          info->minHeight[i] = 0;
        }
      }
    }
    diffh = size.y - r.size.y;
  } else {
    diffh = 0;
  }

  /*
   * Now do the actual layout of the slaves using the layout information
   * that has been collected.
   */

  info->startx = diffw/2 + insets.left;
  info->starty = diffh/2 + insets.top;

  for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
    comp = components[compindex];

    if (!comp->IsVisible()){
      continue;
    }

    constraints = LookupConstraints(comp);

    if (!rightToLeft) {
      r.point.x = info->startx;

      for(i = 0; i < constraints->tempX; i++) {
        r.point.x += info->minWidth[i];
      }
    } else {
      r.point.x = size.x - (diffw/2 + insets.right);

      for(i = 0; i < constraints->tempX; i++) {
        r.point.x -= info->minWidth[i];
      }
    }

    r.point.y = info->starty;

    for(i = 0; i < constraints->tempY; i++) {
      r.point.y += info->minHeight[i];
    }

    r.size.x = 0;

    for(i = constraints->tempX; i < (constraints->tempX + constraints->tempWidth); i++) {
      r.size.x += info->minWidth[i];
    }

    r.size.y = 0;

    for(i = constraints->tempY; i < (constraints->tempY + constraints->tempHeight); i++) {
      r.size.y += info->minHeight[i];
    }

    componentAdjusting = comp;

    AdjustForGravity(constraints, &r);

    if (r.point.x < 0) {
      r.size.x += r.point.x;
      r.point.x = 0;
    }

    if (r.point.y < 0) {
      r.size.y += r.point.y;
      r.point.y = 0;
    }

    /*
     * If the window is too small to be interesting then
     * unmap it.  Otherwise configure it and then make sure
     * it's mapped.
     */

    if ((r.size.x <= 0) || (r.size.y <= 0)) {
      comp->SetBounds(0, 0, 0, 0);
    } else {
      jrect_t<int> r2 = comp->GetVisibleBounds();

      if (r2.point.x != r.point.x || r2.point.y != r.point.y || r2.size.x != r.size.x || r2.size.y != r.size.y) {
        comp->SetBounds(r);
      }
    }
  }
}

}
