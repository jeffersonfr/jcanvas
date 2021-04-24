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
#ifndef J_POINT_H
#define J_POINT_H

#include "jvector.h"

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
  struct jpoint_t {
    T x;
    T y;

    template<typename U> 
      operator jpoint_t<U>()
      {
        return {
          .x = (U)x,
          .y = (U)y
        };
      }

    template<typename U> 
      operator jvector_t<2, U>()
      {
        return {
          .data = {
            (U)x,
            (U)y
          }
        };
      }

    bool operator==(const jpoint_t<T> &param) const
    {
      return (x == param.x and y == param.y);
    }

    bool operator!=(const jpoint_t<T> &param) const
    {
      return (x != param.x or y != param.y);
    }

    jpoint_t<T> operator-() const
    {
      return {-x, -y};
    }

    template<typename U> 
      jpoint_t<T> & operator=(const U &param)
      {
        x = (T)param;
        y = (T)param;

        return *this;
      }

    template<typename U> 
      jpoint_t<T> & operator=(const jvector_t<2, U> &param)
      {
        x = (T)param[0];
        y = (T)param[1];

        return *this;
      }

    template<typename U> 
      jpoint_t<typename std::common_type<T, U>::type> operator+(const U &param) const
      {
        return {(x + param), (y + param)};
      }

    template<typename U> 
      jpoint_t<typename std::common_type<T, U>::type> operator-(const U &param) const
      {
        return {(x - param), (y - param)};
      }

    template<typename U> 
      jpoint_t<typename std::common_type<T, U>::type> operator*(const U &param) const
      {
        return {(x * param), (y * param)};
      }

    template<typename U> 
      jpoint_t<typename std::common_type<T, U>::type> operator/(const U &param) const
      {
        return {(x / param), (y / param)};
      }

    template<typename U> 
      jpoint_t<T> & operator+=(const U &param)
      {
        return (*this = *this + param);
      }

    template<typename U>
      jpoint_t<T> & operator-=(const U param)
      {
        return (*this = *this - param);
      }

    template<typename U>
      jpoint_t<T> & operator*=(const U &param)
      {
        return (*this = *this * param);
      }

    template<typename U>
      jpoint_t<T> & operator/=(const U &param)
      {
        return (*this = *this / param);
      }

    template<typename U>
      jpoint_t<T> & operator=(const jpoint_t<U> &param)
      {
        x = (T)param.x;
        y = (T)param.y;

        return *this;
      }

    template<typename U>
      jpoint_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param) const
      {
        return {(x + param.x), (y + param.y)};
      }

    template<typename U> 
      jpoint_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param) const
      {
        return {(x - param.x), (y - param.y)};
      }

    template<typename U>
      jpoint_t<typename std::common_type<T, U>::type> operator*(const jpoint_t<U> &param) const
      {
        return {(x*param.x), (y*param.y)};
      }

    template<typename U>
      jpoint_t<typename std::common_type<T, U>::type> operator/(const jpoint_t<U> &param) const
      {
        return {(x/param.x), (y/param.y)};
      }

    template<typename U>
      jpoint_t<T> & operator+=(const jpoint_t<U> &param)
      {
        return (*this = *this + param);
      }

    template<typename U> 
      jpoint_t<T> & operator-=(const jpoint_t<U> &param)
      {
        return (*this = *this - param);
      }

    template<typename U> 
      jpoint_t<T> & operator*=(const jpoint_t<U> &param)
      {
        return (*this = *this*param);
      }

    template<typename U>
      jpoint_t<T> & operator/=(const jpoint_t<U> &param)
      {
        return (*this = *this/param);
      }

    template<typename U> 
      float Distance(const jpoint_t<U> &param) const
      {
        typename std::common_type<T, U>::type
          px = (x - param.x),
             py = (y - param.y);

        return std::sqrt(px*px + py*py);
      }

    template<typename U>
      typename std::common_type<T, U>::type Scalar(const jpoint_t<U> &param) const
      {
        return (x*param.x) + (y*param.y);
      }

    float Norm() const
    {
      return x*x + y*y;
    }

    float EuclidianNorm() const
    {
      return std::sqrt(Norm());
    }

    template<typename U, typename = typename std::enable_if<std::is_floating_point<U>::value, T>::type> 
      jpoint_t<U> Normalize() const
      {
        return jpoint_t<U>(*this)/EuclidianNorm();
      }

    jpoint_t<T> Rotate(float angle) const
    {
      float
        c = cosf(angle),
          s = sinf(angle);

      return {x*c - y*s, x*s + y*c};
    }

    float Angle() const
    {
      float
        angle = fabs(atanf(y/(float)x));

      if (x > 0) {
        if (y > 0) {
        } else {
          angle = 2*M_PI - angle;
        }
      } else {
        if (y > 0) {
          angle = M_PI - angle;
        } else {
          angle = M_PI + angle;
        }
      }

      return angle;
    }

    T Min() const
    {
      return std::min<T>(x, y);
    }

    T Max() const
    {
      return std::max<T>(x, y);
    }

    template<typename U> 
      friend jpoint_t<typename std::common_type<T, U>::type> operator+(const U &param, const jpoint_t<T> &thiz)
      {
        return {(param + thiz.x), (param + thiz.y)};
      }

    template<typename U> 
      friend jpoint_t<typename std::common_type<T, U>::type> operator-(const U &param, const jpoint_t<T> &thiz)
      {
        return {(param - thiz.x), (param - thiz.y)};
      }

    template<typename U> 
      friend jpoint_t<typename std::common_type<T, U>::type> operator*(const U &param, const jpoint_t<T> &thiz)
      {
        return {(param*thiz.x), (param*thiz.y)};
      }

    template<typename U>
      friend jpoint_t<typename std::common_type<T, U>::type> operator/(const U &param, const jpoint_t<T> &thiz)
      {
        return {(param/thiz.x), (param/thiz.y)};
      }

    friend std::ostream & operator<<(std::ostream& out, const jpoint_t<T> &param)
    {
      out << param.x << ", " << param.y;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jpoint_t<T> &param) 
    {
      is >> param.x >> param.y;

      return is;
    }

  };

}

#endif

