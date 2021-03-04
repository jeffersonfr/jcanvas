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
#ifndef J_ENUM_H
#define J_ENUM_H

#include <iostream>

namespace jcanvas {

  template<typename T>
    struct jenum_t {

      static_assert(std::is_enum<T>::value, "T must be a enum");

      using Type = typename std::underlying_type<T>::type;

      T value;

      operator bool()
      {
        return static_cast<Type>(value) != Type{};
      }

      operator Type()
      {
        return static_cast<Type>(value);
      }

      operator T()
      {
        return value;
      }

      template<typename ...Args>
        jenum_t And(Args ...args)
        {
          return {static_cast<T>((static_cast<Type>(value) & ... & static_cast<Type>(args)))};
        }

      template<typename ...Args>
        jenum_t Or(Args ...args)
        {
          return {static_cast<T>((static_cast<Type>(value) | ... | static_cast<Type>(args)))};
        }

      jenum_t Not()
      {
        return {static_cast<T>(~static_cast<Type>(value))};
      }

      friend std::ostream & operator<<(std::ostream &stream, jenum_t const &obj)
      {
        stream << static_cast<Type>(obj.value);

        return stream;
      }

    };

}

#endif
