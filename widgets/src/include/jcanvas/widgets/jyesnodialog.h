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
#ifndef J_YESNODIALOG_H
#define J_YESNODIALOG_H

#include "jcanvas/widgets/jdialog.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jflowlayout.h"
#include "jcanvas/widgets/jactionlistener.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class YesNoDialog : public Dialog, public ActionListener {

  private:
    /** \brief */
    std::shared_ptr<Text> _label;
    /** \brief */
    std::shared_ptr<Button> _yes;
    /** \brief */
    std::shared_ptr<Button> _no;
    /** \brief */
    std::shared_ptr<Container> _buttons_container;

  public:
    /**
     * \brief
     *
     */
    YesNoDialog(std::shared_ptr<Container> parent, std::string title, std::string msg);
    
    /**
     * \brief
     *
     */
    virtual ~YesNoDialog();

    /**
     * \brief
     *
     */
    virtual void Init();

    /**
     * \brief
     *
     */
    virtual void SetHorizontalAlign(jhorizontal_align_t align);

    /**
     * \brief
     *
     */
    virtual jhorizontal_align_t GetHorizontalAlign();

    /**
     * \brief
     *
     */
    virtual void SetVerticalAlign(jvertical_align_t align);

    /**
     * \brief
     *
     */
    virtual jvertical_align_t GetVerticalAlign();
    
    /**
     * \brief
     *
     */
    virtual void ActionPerformed(ActionEvent *event);

    /**
     * \brief
     *
     */
    virtual std::string GetResponse();

};

}

#endif 
