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
#ifndef J_TEXT_H
#define J_TEXT_H

#include "jcanvas/widgets/jcomponent.h"

namespace jcanvas {

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Text : public Component {

  private:
    /** \brief */
    std::string _text;
    /** \brief */
    jhorizontal_align_t _halign;
    /** \brief */
    jvertical_align_t _valign;
    /** \brief */
    bool _is_wrap;

  private:
    /**
     * \brief
     *
     */
    void UpdatePreferredSize();

  public:
    /**
     * \brief
     *
     */
    Text(std::string text = {""});
    
    /**
     * \brief
     *
     */
    virtual ~Text();

    /**
     * \brief
     *
     */
    virtual void SetText(std::string text);
    
    /**
     * \brief
     *
     */
    virtual std::string GetText();
    
    /**
     * \brief
     *
     */
    virtual void SetWrap(bool b);
    
    /**
     * \brief
     *
     */
    virtual bool IsWrap();
    
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
    virtual  jvertical_align_t GetVerticalAlign();
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

};

}

#endif

