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
#ifndef J_DIALOG_H
#define J_DIALOG_H

#include "jcanvas/widgets/jcontainer.h"

#include <mutex>
#include <condition_variable>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Dialog : public Container {

  protected:
    /** \brief */
    Component *_focus_owner;
    /** \brief */
    jtheme_t _dialog_theme;
    /** \brief */
    std::mutex _modal_mutex;
    /** \brief */
    std::condition_variable _modal_condition;
    /** \brief */
    std::string _title;
    /** \brief */
    bool _is_modal;

  public:
    /**
     * \brief
     *
     */
    Dialog(Container *parent, jrect_t<int> bounds = {0, 0, 0, 0});
    
    /**
     * \brief
     *
     */
    Dialog(std::string title, Container *parent, jrect_t<int> bounds = {0, 0, 0, 0});
    
    /**
     * \brief
     *
     */
    virtual ~Dialog();

    /**
     * \brief
     *
     */
    virtual void SetTitle(std::string title);

    /**
     * \brief
     *
     */
    virtual std::string GetTitle();

    /**
     * \brief
     *
     */
    virtual bool IsModal();

    /**
     * \brief
     *
     */
    virtual void Exec(bool modal = false);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

    /**
     * \brief
     *
     */
    Component * GetFocusOwner();

    /**
     * \brief
     *
     */
    void RequestComponentFocus(Component *c);
    /**
     * \brief
     *
     */
    void ReleaseComponentFocus(Component *c);
    /**
     * \brief
     *
     */
    Container * GetFocusCycleRootAncestor();

    /**
     * \brief
     *
     */
    virtual void Close();

    /**
     * \brief
     *
     */
    virtual bool KeyPressed(KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool KeyReleased(KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool KeyTyped(KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool MousePressed(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(MouseEvent *event);
    
};

}

#endif

