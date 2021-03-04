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
#ifndef J_INSETS_H
#define J_INSETS_H

#include "jcanvas/algebra/jrect.h"

namespace jcanvas {

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jinsets_t {
    T left;
    T top;
    T right;
    T bottom;
 
    template<typename U>
      operator jinsets_t<U>()
      {
        return {
          .left = (U)left,
            .top = (U)top,
            .right = (U)right,
            .bottom = (U)bottom
        };
      }

    jinsets_t<T> & operator=(const T &param)
    {
      left = param;
      top = param;
      right = param;
      bottom = param;

      return *this;
    }

    bool operator==(const jinsets_t<T> &param) const
    {
      return (left == param.left and top == param.top and right == param.right and bottom == param.bottom);
    }

    bool operator!=(const jinsets_t<T> &param) const
    {
      return (left != param.left or top != param.top or right != param.right or bottom != param.bottom);
    }

    template<typename U>
      jrect_t<typename std::common_type<T, U>::type> bounds(jrect_t<U> const &region)
      {
        return {region.point.x + left, region.point.y + top, region.size.x - left - right, region.size.y - top - bottom};
      }

    friend std::ostream & operator<<(std::ostream& out, const jinsets_t<T> &param)
    {
      out << param.left << ", " << param.top << ", " << param.right << ", " << param.bottom;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jinsets_t<T> &param) 
    {
      is >> param.left >> param.top >> param.right >> param.bottom;

      return is;
    }

  };

}

#endif

