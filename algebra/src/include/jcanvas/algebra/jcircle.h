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
#ifndef J_CIRCLE_H
#define J_CIRCLE_H

#include "jcanvas/algebra/jline.h"

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
  struct jcircle_t {
    jpoint_t<T> center;
    T radius;

    template<typename U>
      operator jcircle_t<U>()
      {
        return {
          .center = center,
            .radius = radius
        };
      }

    jcircle_t<T> operator-() const
    {
      return {-center, radius};
    }

    bool operator==(const jcircle_t<T> &param) const
    {
      return (center == param.center and radius == param.radius);
    }

    bool operator!=(const jline_t<T> &param) const
    {
      return (center != param.center or radius != param.radius);
    }

    template<typename U> 
      jcircle_t<T> & operator=(const U &param)
      {
        radius = (T)param;

        return *this;
      }

    template<typename U> 
      jline_t<typename std::common_type<T, U>::type> operator+(const U &param) const
      {
        return {center, radius + param};
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator-(const U &param) const
      {
        return {center, radius - param};
      }

    template<typename U> 
      jline_t<typename std::common_type<T, U>::type> operator*(const U &param) const
      {
        return {center, radius*param};
      }

    template<typename U>
      jline_t<typename std::common_type<T, U>::type> operator/(const U &param) const
      {
        return {center, radius/param};
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
      jcircle_t<T> & operator=(const jpoint_t<U> &param)
      {
        center = param;

        return *this;
      }

    template<typename U> 
      jcircle_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param) const
      {
        return {center + param, radius};
      }

    template<typename U> 
      jcircle_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param) const
      {
        return {center - param, radius};
      }

    template<typename U>
      jcircle_t<typename std::common_type<T, U>::type> & operator+=(const jpoint_t<U> &param)
      {
        return (*this = *this + param);
      }

    template<typename U> 
      jcircle_t<typename std::common_type<T, U>::type> & operator-=(const jpoint_t<U> &param)
      {
        return (*this = *this - param);
      }

    template<typename U> 
      jcircle_t<T> & operator=(const jcircle_t<U> &param)
      {
        center = param.center;
        radius = param.radius;

        return *this;
      }

    T Size() const
    {
      return 2*M_PI*radius;
    }

    template<typename U>
      bool Inside(const jpoint_t<U> &point) const
      {
        return point.Distance(center) <= radius;
      }

    float Angle(const jpoint_t<T> &param) const
    {
      return (param - center).Angle();
    }

    /**
     * \brief Returns the intersection circle and line.
     *
     */
    template<typename U>
      std::optional<std::pair<jpoint_t<float>, jpoint_t<float>>> Intersection(const jline_t<U> &line) const
      {
        float 
          dx = line.p1.x - line.p0.x, 
             dy = line.p1.y - line.p0.y, 
             A = dx*dx + dy*dy,
             B = 2*(dx*(line.p0.x - center.x) + dy*(line.p0.y - center.y)),
             C = (line.p0.x - center.x)*(line.p0.x - center.x) + (line.p0.y - center.y)*(line.p0.y - center.y) - radius*radius,
             det = B*B - 4*A*C;

        if ((A <= 0.0000001) || (det < 0)) { // no real solutions
          return std::nullopt;
        }

        float
          t0 = (float)((-B + sqrtf(det))/(2*A)),
             t1 = (float)((-B - sqrtf(det))/(2*A));

        return std::make_pair(jpoint_t<float>{line.p0.x + t0*dx, line.p0.y + t0*dy}, jpoint_t<float>{line.p0.x + t1*dx, line.p0.y + t1*dy});
      }

    template<typename U> 
      friend jcircle_t<typename std::common_type<T, U>::type> operator+(const T &param, const jcircle_t<T> &thiz)
      {
        return {thiz.center, (param + thiz.radius)};
      }

    template<typename U>
      friend jcircle_t<typename std::common_type<T, U>::type> operator-(const T &param, const jcircle_t<T> &thiz)
      {
        return {thiz.center, (param - thiz.radius)};
      }

    template<typename U>
      friend jcircle_t<typename std::common_type<T, U>::type> operator*(const T &param, const jcircle_t<T> &thiz)
      {
        return {thiz.center, (param*thiz.radius)};
      }

    template<typename U> 
      friend jcircle_t<typename std::common_type<T, U>::type> operator/(const T &param, const jcircle_t<T> &thiz)
      {
        return {thiz.center, (param/thiz.radius)};
      }

    template<typename U> 
      friend jcircle_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<T> &param, const jcircle_t<T> &thiz)
      {
        return {thiz.center + param, thiz.radius};
      }

    template<typename U> 
      friend jcircle_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<T> &param, const jcircle_t<T> &thiz)
      {
        return {thiz.center - param, thiz.radius};
      }

    friend std::ostream & operator<<(std::ostream& out, const jcircle_t<T> &param)
    {
      out << std::string("(") << param.center << "), " << param.radius;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jcircle_t<T> &param) 
    {
      is >> param.center.x >> param.center.y >> param.radius;

      return is;
    }

  };

}

#endif

