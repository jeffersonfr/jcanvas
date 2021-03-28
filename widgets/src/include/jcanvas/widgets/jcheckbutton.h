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
#ifndef J_CHECKButton_H
#define J_CHECKButton_H

#include "jcanvas/widgets/jbutton.h"

#include <mutex>

namespace jcanvas {

enum class jcheckbutton_type_t {
  Check,
  Radio
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CheckButton : public Button {

  private:
    /** \brief */
    std::shared_ptr<Image> _check_image;
    /** \brief */
    std::shared_ptr<Image> _radio_image;
    /** \brief */
    std::shared_ptr<Image> _checked_image;
    /** \brief */
    std::shared_ptr<Image> _unchecked_image;
    /** \brief */
    jcheckbutton_type_t _type;

  public:
    /**
     * \brief
     *
     */
    CheckButton(jcheckbutton_type_t type, std::string text);
  
    /**
     * \brief
     *
     */
    virtual ~CheckButton();

    /**
     * \brief
     *
     */
    virtual void SetType(jcheckbutton_type_t type);
    
    /**
     * \brief
     *
     */
    virtual jcheckbutton_type_t GetType();
    
};

}

#endif

