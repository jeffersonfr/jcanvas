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
#ifndef J_RECT_H
#define J_RECT_H

#include "jcanvas/algebra/jline.h"
#include "jcanvas/algebra/jcircle.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <type_traits>
#include <iostream>

#include <stdio.h>

namespace jcanvas {

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jrect_t {
    jpoint_t<T> point;
    jpoint_t<T> size;

    template<typename U> 
      operator jrect_t<U>()
      {
        return {
          .point = point,
            .size = size
        };
      }

    bool operator==(const jrect_t<T> &param) const
    {
      return (point == param.point and size == param.size);
    }

    bool operator!=(const jrect_t<T> &param) const
    {
      return (point != param.point or size != param.size);
    }

    jrect_t<T> operator-() const
    {
      return {-point, -size};
    }

    jrect_t<T> & operator=(T param)
    {
      size = param;

      return *this;
    }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> operator+(const U &param) const
      {
        return {point + param, size + param};
      }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> operator-(const U &param) const
      {
        return {point - param, size - param};
      }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> operator*(const U &param) const
      {
        return {point*param, size*param};
      }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> operator/(const U &param) const
      {
        return {point/param, size/param};
      }

    template<typename U> 
      jrect_t<T> & operator+=(const U &param)
      {
        return (*this = *this + param);
      }

    template<typename U> 
      jrect_t<T> & operator-=(const U &param)
      {
        return (*this = *this - param);
      }

    template<typename U> 
      jrect_t<T> & operator*=(const U &param)
      {
        return (*this = *this*param);
      }

    template<typename U> 
      jrect_t<T> & operator/=(const U &param)
      {
        return (*this = *this/param);
      }

    template<typename U> 
      jrect_t<T> & operator=(const jrect_t<U> &param)
      {
        point = param.point;
        size = param.size;

        return *this;
      }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> operator+(const jrect_t<U> &param) const
      {
        int
          x0 = std::min(point.x, param.point.x),
             y0 = std::min(point.y, param.point.y);
        int
          w0 = std::max(point.x + size.x, param.point.x + param.size.x),
             h0 = std::max(point.y + size.y, param.point.y + param.size.y);

        return {{x0, y0}, {w0, h0}};
      }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> operator-(const jrect_t<U> &param) const
      {
        return {point - param.point, size - param.size};
      }

    template<typename U> 
      jrect_t<T> & operator+=(const jrect_t<U> &param)
      {
        return (*this = *this + param);
      }

    template<typename U> 
      jrect_t<T> & operator-=(const jrect_t<U> &param)
      {
        return (*this = *this + param);
      }

    bool operator>(const jrect_t<T> &param) const
    {
      return size > param.size;
    }

    bool operator>=(const jrect_t<T> &param) const
    {
      return size >= param.size;
    }

    bool operator<(const jrect_t<T> &param) const
    {
      return size < param.size;
    }

    bool operator<=(const jrect_t<T> &param) const
    {
      return size <= param.size;
    }

    template<typename U> 
      bool Intersects(const jpoint_t<U> &param) const
      {
        if (param.x > point.x and param.x < (point.x + size.x) and param.y > point.y and param.y < (point.y + size.y)) {
          return true;
        }

        return false;
      }

    template<typename U> 
      bool Intersects(const jline_t<U> &param) const
      {
        std::pair<float, float>
          i0 = param.Intersection({{point.x, point.y}, {point.x + size.x, point.y}}),
             i1 = param.Intersection({{point.x + size.x, point.y}, {point.x + size.x, point.y + size.y}}),
             i2 = param.Intersection({{point.x + size.x, point.y + size.y}, {point.x, point.y + size.y}}),
             i3 = param.Intersection({{point.x, point.y + size.y}, {point.x, point.y}});

        if (i0.first >= 0.0f and i0.first <= 1.0f or
            i1.first >= 0.0f and i1.first <= 1.0f or
            i2.first >= 0.0f and i2.first <= 1.0f or
            i3.first >= 0.0f and i3.first <= 1.0f) {
          return true;
        }

        return false;
      }

    template<typename U> 
      bool Intersects(const jcircle_t<U> &param) const
      {
        std::optional<std::pair<jpoint_t<float>, jpoint_t<float>>>
          i0 = param.Intersection({{point.x, point.y}, {point.x + size.x, point.y}}),
             i1 = param.Intersection({{point.x + size.x, point.y}, {point.x + size.x, point.y + size.y}}),
             i2 = param.Intersection({{point.x + size.x, point.y + size.y}, {point.x, point.y + size.y}}),
             i3 = param.Intersection({{point.x, point.y + size.y}, {point.x, point.y}});

        if (i0 != std::nullopt and i1 != std::nullopt and i2 != std::nullopt and i3 != std::nullopt) {
          return true;
        }

        return false;
      }

    template<typename U> 
      bool Contains(const jrect_t<U> &param) const
      {
        return (param.point.x >= point.x) and (param.point.y >= point.y) and ((param.point.x + param.size.x) <= size.x) and ((param.point.y + param.size.y) <= size.y);
      }

    template<typename U> 
      bool Intersects(const jrect_t<U> &param) const
      {
        return (((point.x > (param.point.x + param.size.x)) or ((point.x + size.x) < param.point.x) or (point.y > (param.point.y + param.size.y)) or ((point.y + size.y) < param.point.y)) == 0);
      }

    template<typename U> 
      jrect_t<typename std::common_type<T, U>::type> Intersection(const jrect_t<U> &param) const
      {
        typename std::common_type<T, U>::type
          left = std::max(point.x, param.point.x),
               top = std::max(point.y, param.point.y),
               right = std::min(point.x + size.x, param.point.x + param.size.x),
               bottom = std::min(point.y + size.y, param.point.y + param.size.y);

        if (right > left and bottom > top) {
          return {{left, top}, {right - left, bottom - top}};
        }

        return {0, 0, 0, 0};
      }

    template<typename U> 
      friend jrect_t<typename std::common_type<T, U>::type> operator+(const U &param, const jrect_t<T> &thiz)
      {
        return {param + thiz.point, param + thiz.size};
      }

    template<typename U> 
      friend jrect_t<typename std::common_type<T, U>::type> operator-(const U &param, const jrect_t<T> &thiz)
      {
        return {param - thiz.point, param - thiz.size};
      }

    template<typename U> 
      friend jrect_t<typename std::common_type<T, U>::type> operator*(const U &param, const jrect_t<T> &thiz)
      {
        return {param*thiz.point, param*thiz.size};
      }

    template<typename U> 
      friend jrect_t<typename std::common_type<T, U>::type> operator/(const U &param, const jrect_t<T> &thiz)
      {
        return {param/thiz.point, param/thiz.size};
      }

    friend std::ostream & operator<<(std::ostream& out, const jrect_t<T> &param)
    {
      out << param.point << ", " << param.size;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jrect_t<T> &param) 
    {
      is >> param.point.x >> param.point.y >> param.size.x >> param.y;

      return is;
    }

  };

}

#endif

