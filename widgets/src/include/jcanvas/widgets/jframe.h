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
#ifndef J_FRAME_H
#define J_FRAME_H

#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jgridlayout.h"
#include "jcanvas/core/jimage.h"
#include "jcanvas/core/jwindow.h"

#include <thread>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Frame : public Window, public Container {

  private:
    /** \brief */
    Component *_focus_owner;
    /** \brief */
    std::shared_ptr<Image> _icon;

  public:
    /**
     * \brief
     *
     */
    Frame(jpoint_t<int> size, jpoint_t<int> point = {0, 0});
    
    /**
     * \brief
     *
     */
    virtual ~Frame();

    /**
     * \brief
     *
     */
    virtual void SetIcon(std::shared_ptr<Image> image);

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> GetIcon();

    /**
     * \brief
     *
     */
    Component * GetFocusOwner();
    
    /**
     * \brief
     *
     */
    void RequestComponentFocus(Component *cmp);
    
    /**
     * \brief
     *
     */
    void ReleaseComponentFocus(Component *cmp);
    
    /**
     * \brief
     *
     */
    Container * GetFocusCycleRootAncestor();

    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetVisibleBounds();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetPosition();

    /**
     * \brief
     *
     */
    virtual void SetPosition(jpoint_t<int> pos);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetSize();

    /**
     * \brief
     *
     */
    virtual void SetSize(jpoint_t<int> size);

    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetBounds();

    /**
     * \brief
     *
     */
    virtual void SetBounds(jrect_t<int> bounds);
    
    /**
     * \brief
     *
     */
    virtual void Repaint(Component *cmp = nullptr);
    
    /**
     * \brief
     *
     */
    virtual void PaintGlassPane(Graphics *g);

    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g) override;

    /**
     * \brief
     *
     */
    virtual bool KeyPressed(KeyEvent *event) override;

    /**
     * \brief
     *
     */
    virtual bool KeyReleased(KeyEvent *event) override;

    /**
     * \brief
     *
     */
    virtual bool KeyTyped(KeyEvent *event) override;

    /**
     * \brief
     *
     */
    virtual bool MousePressed(MouseEvent *event) override;

    /**
     * \brief
     *
     */
    virtual bool MouseReleased(MouseEvent *event) override;

    /**
     * \brief
     *
     */
    virtual bool MouseMoved(MouseEvent *event) override;

    /**
     * \brief
     *
     */
    virtual bool MouseWheel(MouseEvent *event) override;

};

}

#endif 
