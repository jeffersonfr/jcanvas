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
 a***************************************************************************/
#ifndef J_VECTOR_H
#define J_VECTOR_H

#include <iostream>
#include <fstream>
#include <complex>
#include <algorithm>
#include <optional>
#include <random>
#include <iomanip>
#include <stdexcept>

namespace jcanvas {

/* // -fconcepts -std=c++20
template<typename T>
  concept ComplexConcept = requires { 
    std::complex<T>{}; 
  };  

template<typename T>
  concept VectorConcept = ComplexConcept<T> || std::is_arithmetic<T>::value;

template<size_t N, VectorConcept T = float> 
  struct jvector_t {
*/

template<class T> 
  struct is_complex : std::false_type {
  };

template<class T> 
  struct is_complex<std::complex<T>> : std::true_type {
  };

template<size_t N, typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value || is_complex<T>::value, T>::type>
  struct jvector_t {
    typedef T value_type;

    T data[N];

    static jvector_t<N, T> Random(double lo = 0.0, double hi = 1.0)
    {
      jvector_t<N, T> v;

      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_real_distribution<> distribution(lo, hi);

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)distribution(gen);
      }

      return v;
    }

    template<typename U> operator jvector_t<N, U>() const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (U)data[i];
      }

      return v;
    }

    template<typename U> jvector_t<N, U> Real() const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (U)std::real(data[i]);
      }

      return v;
    }

    template<typename U> jvector_t<N, U> Imag() const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (U)std::real(data[i]);
      }

      return v;
    }

    T & operator[](size_t n)
    {
      if (n >= N) {
        throw std::out_of_range("Element index is out of bounds");
      }
      
      return data[n];
    }

    const T & operator[](size_t n) const
    {
      if (n >= N) {
        throw std::out_of_range("Element index is out of bounds");
      }
      
      return data[n];
    }

    bool operator==(const T &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(const T &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param) {
          return true;
        }
      }

      return false;
    }

    bool operator==(const jvector_t<N, T> &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param.data[i]) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(const jvector_t<N, T> &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param.data[i]) {
          return true;
        }
      }

      return false;
    }

    jvector_t<N, T> operator-() const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = -data[i];
      }

      return v;
    }
 
    template<typename U> jvector_t<N, T> & operator=(const U &param)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)param;
      }

      return *this;
    }

    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator+(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] + param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator-(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] - param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator*(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]*param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator/(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]/param);
      }

      return v;
    }

    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator%(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]%param);
      }

      return v;
    }

    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator|(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] | param);
      }

      return v;
    }

    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator&(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] & param);
      }

      return v;
    }

    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator^(const U &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] ^ param);
      }

      return v;
    }

    template<typename U> jvector_t<N, T> & operator=(const jvector_t<N, U> &param)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)param.data[i];
      }

      return *this;
    }

    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator+(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] + param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator-(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] - param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator*(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]*param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator/(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]/param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator%(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]%param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator|(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] | param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator&(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]&param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, typename std::common_type<T, U>::type> operator^(const jvector_t<N, U> &param) const
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i]^param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> & operator+=(const jvector_t<N, U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jvector_t<N, T> & operator-=(const jvector_t<N, U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jvector_t<N, T> & operator*=(const jvector_t<N, U> &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jvector_t<N, T> & operator/=(const jvector_t<N, U> &param)
    {
      return (*this = *this/param);
    }
    
    template<typename U> jvector_t<N, T> & operator%=(const jvector_t<N, U> &param)
    {
      return (*this = *this%param);
    }
    
    template<typename U> jvector_t<N, T> & operator|=(const jvector_t<N, U> &param)
    {
      return (*this = *this | param);
    }
    
    template<typename U> jvector_t<N, T> & operator&=(const jvector_t<N, U> &param)
    {
      return (*this = *this & param);
    }
    
    template<typename U> jvector_t<N, T> & operator^=(const jvector_t<N, U> &param)
    {
      return (*this = *this ^ param);
    }
    
    template<typename U> typename std::common_type<T, U>::type Scalar(const jvector_t<N, U> &param) const
    {
      typename std::common_type<T, U>::type n = 0;

      for (size_t i=0; i<N; i++) {
        n = n + (data[i]*param.data[i]);
      }

      return n;
    }
    
    template<size_t M> jvector_t<M, T> SubVector(size_t n, size_t m) const
    {
      if (n >= N or m > N) {
        throw std::out_of_range("Sub vector indexes are out of bounds");
      }

      jvector_t<M, T> v;

      for (size_t i=n; i<m; i++) {
        v.data[i - n] = data[i];
      }

      return v;
    }

    jvector_t<N, T> Pow(double e)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::pow(data[i], e);
      }

      return v;
    }

    jvector_t<N, T> Sqrt()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::sqrt(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Abs()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::abs(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Sin()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::sin(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Cos()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::cos(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Tan()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::tan(data[i]);
      }

      return v;
    }

    jvector_t<N, T> SinH()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::sinh(data[i]);
      }

      return v;
    }

    jvector_t<N, T> CosH()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::cosh(data[i]);
      }

      return v;
    }

    jvector_t<N, T> TanH()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::tanh(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Exp()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::exp(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Log()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::log(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Log2()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::log2(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Log10()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::log10(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Sort()
    {
      jvector_t<N, T> v = *this;

      std::sort(v.data, v.data + N);

      return v;
    }

    jvector_t<N, T> Reverse(size_t lo = 0, size_t hi = N - 1)
    {
      if (lo < 0 or hi < 0 or lo >= N or hi >= N or lo > hi) {
        throw std::out_of_range("Range index is out of bounds");
      }

      jvector_t<N, T> v = *this;
      size_t length = hi - lo;

      for (size_t i=0; i<=length/2; i++) {
        std::swap(v.data[lo + i], v.data[hi - i]);
      }

      return v;
    }

    jvector_t<N, T> MoveLeft(size_t n)
    {
      jvector_t<N, T> v = *this;

      n = n%N;

      for (size_t i=0; i<N - n; i++) {
        v.data[i] = data[n + i];
      }

      for (size_t i=0; i<n; i++) {
        v.data[N - n + i] = data[i];
      }

      return v;
    }

    jvector_t<N, T> MoveRight(size_t n)
    {
      jvector_t<N, T> v = *this;

      n = n%N;

      for (size_t i=0; i<n; i++) {
        v.data[i] = data[N - n + i];
      }

      for (size_t i=0; i<N - n; i++) {
        v.data[n + i] = data[i];
      }

      return *this;
    }

    constexpr size_t Size() noexcept
    {
      return N;
    }

    T Sum() const
    {
      T sum = 0;

      for (size_t i=0; i<N; i++) {
        sum = sum + data[i];
      }

      return sum;
    }

    T Mul() const
    {
      T mul = 1;

      for (size_t i=0; i<N; i++) {
        mul = mul*data[i];
      }

      return mul;
    }

    T Mul(jvector_t<N, T> &param) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = data[i]*param.data[i];
      }

      return v;
    }

    T Div(jvector_t<N, T> &param) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = data[i]/param.data[i];
      }

      return v;
    }

    T Norm1() const
    {
      jvector_t<N, T> v = *this;

      return v.Abs().Sum();
    }

    T Norm() const
    {
      T norm {0};

      for (size_t i=0; i<N; i++) {
        norm = norm + data[i]*data[i];
      }

      return norm;
    }

    double EuclidianNorm() const
    {
      return std::sqrt(Norm());
    }

    T NormInf() const
    {
      jvector_t<N, T> v = *this;

      return v.Abs().Min();
    }

    jvector_t<N, double> Normalize() const
    {
      return jvector_t<N, double>(*this)/EuclidianNorm();
    }
 
    jvector_t<N, double> Equalize(T lo, T hi) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] - lo)/(hi - lo);
      }

      return v;
    }
 
    jvector_t<N, double> Clip(T lo, T hi) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        T e = data[i];

        v.data[i] = (e < lo)?lo:(e > hi)?hi:e;
      }

      return v;
    }
 
    std::optional<T &> Find(const T &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] == param) {
          return data[i];
        }

        return std::nullopt;
      }

      return *std::min_element(data, data + N);
    }

    T Min() const
    {
      return *std::min_element(data, data + N);
    }

    T Max() const
    {
      return *std::max_element(data, data + N);
    }

    jvector_t<N, T> Conjugate()
    {
      static_assert(is_complex<T>::value, "T must be std::complex<U>");

      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = std::conj(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Component(const jvector_t<N, T> &param)
    {
      return Scalar(param)/param.EuclidianNorm();
    }

    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator+(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param + thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator-(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param - thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator*(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param*thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator/(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param/thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator%(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param%thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator|(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param & thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator&(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param | thiz.data[i]);
      }

      return v;
    }
    
    template<typename U> friend jvector_t<N, typename std::common_type<T, U>::type> operator^(const U &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, typename std::common_type<T, U>::type> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (param ^ thiz.data[i]);
      }

      return v;
    }
    
    friend std::ostream & operator<<(std::ostream& out, const jvector_t<N, T> &param)
    {
      std::ios_base::fmtflags flags = out.flags();

      out.precision(2);
      out.setf(std::ios_base::fixed);

      out << "|";

      for (size_t i=0; i<N; i++) {
        out << std::setw(5) << /* std::showpos <<*/ param.data[i];

        if (i != (N - 1)) {
          out << ", ";
        }
      }

      out << "|";

      out.flags(flags);

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jvector_t<N, T> &param) 
    {
      for (size_t i=0; i<N; i++) {
        is >> param(i);
      }

      return is;
    }

  };

}

#endif
