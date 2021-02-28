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
#ifndef J_LINE_H
#define J_LINE_H

#include "jcanvas/algebra/jpoint.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <type_traits>
#include <iostream>

#include <stdio.h>

namespace jcanvas {

/**
 * brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jline_t {
    jpoint_t<T> p0;
    jpoint_t<T> p1;

    template<typename U>
      operator jline_t<U>()
      {
        return {
          .p0 = p0,
            .p1 = p1,
        };
      }

    bool operator==(const jline_t<T> &param) const
    {
      return (p0 == param.p0 and p1 == param.p1);
    }

    bool operator!=(const jline_t<T> &param) const
    {
      return (p0 != param.p0 or p1 != param.p1);
    }

    jline_t<T> operator-() const
    {
      return {-p0, -p1};
    }

    template<typename U>
      jline_t<T> & operator=(const U &param)
      {
        p0 = param;
        p1 = param;

        return *this;
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator+(const U &param) const
      {
        return {p0 + param, p1 + param};
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator-(const U &param) const
      {
        return {p0 - param, p1 - param};
      }

    template<typename U> 
      jline_t<typename std::common_type<T, U>::type> operator*(const U &param) const
      {
        return {p0*param, p1*param};
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator/(const U &param) const
      {
        return {p0/param, p1/param};
      }

    template<typename U> 
      jline_t<T> & operator+=(const U &param)
      {
        return (*this = *this + param);
      }

    template<typename U>
      jline_t<T> & operator-=(const U &param)
      {
        return (*this = *this - param);
      }

    template<typename U>
      jline_t<T> & operator*=(const U &param)
      {
        return (*this = *this*param);
      }

    template<typename U> 
      jline_t<T> & operator/=(const U &param)
      {
        return (*this = *this/param);
      }

    template<typename U>
      jline_t<T> & operator=(const jpoint_t<U> &param)
      {
        p0 = param;
        p1 = param;

        return *this;
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param) const
      {
        return {p0 + param, p1 + param};
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param) const
      {
        return {p0 - param, p1 - param};
      }

    template<typename U> 
      jline_t<typename std::common_type<T, U>::type> & operator+=(const jpoint_t<U> &param)
      {
        return (*this = *this + param);
      }

    template<typename U> 
      jline_t<typename std::common_type<T, U>::type> & operator-=(const jpoint_t<U> &param)
      {
        return (*this = *this - param);
      }

    template<typename U>
      jline_t<T> & operator=(const jline_t<U> &param)
      {
        p0 = param.p0;
        p1 = param.p1;

        return *this;
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator+(const jline_t<U> &param) const
      {
        return {p0 + param.p0, p1 + param.p1};
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator-(const jline_t<U> &param) const
      {
        return {p0 - param.p0, p1 - param.p1};
      }

    template<typename U> 
      jline_t<T> & operator+=(const jline_t<U> &param)
      {
        return (*this = *this + param);
      }

    template<typename U>
      jline_t<T> & operator-=(const jline_t<U> &param)
      {
        return (*this = *this - param);
      }

    T Size() const
    {
      return p0.Distance(p1);
    }

    template<typename U>
      bool Exists(const jpoint_t<U> &point) const
      {
        return Sign(point) == 0;
      }

    jpoint_t<T> Point(float t) const
    {
      return jpoint_t<T>{(T)(p0.x + t*(p1.x - p0.x)), (T)(p0.y + t*(p1.y - p0.y))};
    }

    template<typename U>
      int Sign(const jpoint_t<U> &point) const
      {
        typename std::common_type<T, U>::type
          sign = (p1.x - p0.x)*(point.y - p0.y) - (p1.y - p0.y)*(point.x - p0.x);

        if (sign < 0) {
          return (T) - 1;
        }

        if (sign > 0) {
          return (T) + 1;
        }

        return 0;
      }

    /**
     * \brief Returns the perpendicular intersection in line u=[0..1]. To known 
     * the point in line calculate the intersection point as follows:
     *
     * x = u*(line.p1.x - line.p0.x)
     * y = u*(line.p1.y - line.p0.y)
     *
     */
    template<typename U>
      std::optional<float> PerpendicularIntersection(const jpoint_t<U> &point) const
      {
        float
          px = p1.x - p0.x,
             py = p1.y - p0.y;
        float
          den = (px*px + py*py);

        if (den == 0.0f) {
          return std::nullopt;
        }

        return ((point.x - p0.x)*px + (point.y - p0.y)*py)/den;
      }

    /**
     * \brief Returns the intersection point u and v in range [0..1]. To known 
     * the point in line calculate the intersection point as follows:
     *
     * x0 = t*(line0.p1.x - line0.p0.x)
     * y0 = t*(line0.p1.y - line0.p0.y)
     *
     * x1 = u*(line1.p1.x - line1.p0.x)
     * y1 = u*(line1.p1.y - line1.p0.y)
     *
     */
    template<typename U>
      std::optional<std::pair<float, float>> Intersection(const jline_t<U> &line) const
      {
        const float x1 = line.p0.x;
        const float y1 = line.p0.y;
        const float x2 = line.p1.x;
        const float y2 = line.p1.y;

        const float x3 = p0.x;
        const float y3 = p0.y;
        const float x4 = p1.x;
        const float y4 = p1.y;

        const float den = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

        if (den == 0) {
          return std::nullopt;
        }

        const float t1 = -((x1 - x2)*(y1 - y3) - (y1 - y2)*(x1 - x3))/den;
        const float t0 = ((x1 - x3)*(y3 - y4) - (y1 - y3)*(x3 - x4))/den;

        return std::make_pair(t1, t0);
      }

    template<typename U> 
      float Angle(const jline_t<U> &line) const
      {
        float
          m0 = (p1.y - p0.y)/(p1.x - p0.x),
             m1 = (line.p1.y - line.p0.y)/(line.p1.x - line.p0.x);
        float
          den = 1 + m0*m1;

        if (den == 0.0f) {
          return M_PI/2.0f;
        }

        return atanf((m0 - m1)/den);
      }

    template<typename U>
      friend jline_t<typename std::common_type<T, U>::type> operator+(const U &param, const jline_t<T> &thiz)
      {
        return {param + thiz.p0, param + thiz.p1};
      }

    template<typename U>
      friend jline_t<typename std::common_type<T, U>::type> operator-(const U &param, const jline_t<T> &thiz)
      {
        return {param - thiz.p0, param - thiz.p1};
      }

    template<typename U> 
      friend jline_t<typename std::common_type<T, U>::type> operator*(const U &param, const jline_t<T> &thiz)
      {
        return {param*thiz.p0, param*thiz.p1};
      }

    template<typename U>
      friend jline_t<typename std::common_type<T, U>::type> operator/(const U &param, const jline_t<T> &thiz)
      {
        return {param/thiz.p0, param/thiz.p1};
      }

    friend std::ostream & operator<<(std::ostream& out, const jline_t<T> &param)
    {
      out << std::string("(") << param.p0 << "), (" << param.p1 << ")";

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jline_t<T> &param) 
    {
      is >> param.p0.x >> param.p0.y >> param.p1.x >> param.p1.y;

      return is;
    }

  };

}

#endif

