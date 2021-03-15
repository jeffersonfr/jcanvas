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
#ifndef J_WINDOWADAPTER_H
#define J_WINDOWADAPTER_H

#include "jcanvas/core/jwindow.h"

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class WindowAdapter {

  public:
    /**
     * \brief
     *
     */
    WindowAdapter(Window *parent, jrect_t<int> bounds);
    
    /**
     * \brief
     *
     */
    virtual ~WindowAdapter();

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
    virtual void ToggleFullScreen();
    
    /**
     * \brief
     *
     */
    virtual void SetOpacity(float opacity);
    
    /**
     * \brief
     *
     */
    virtual float GetOpacity();
    
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
    virtual void SetResizable(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsResizable();

    /**
     * \brief
     *
     */
    virtual void SetUndecorated(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsUndecorated();

    /**
     * \brief
     *
     */
    virtual void SetBounds(jrect_t<int> bounds);
    
    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetBounds();

    /**
     * \brief
     *
     */
    virtual void Repaint();
    
    /**
     * \brief
     *
     */
    virtual void SetVisible(bool visible);
    
    /**
     * \brief
     *
     */
    virtual bool IsVisible();
    
    /**
     * \brief
     *
     */
    virtual void SetCursorLocation(int x, int y);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetCursorLocation();
    
    /**
     * \brief
     *
     */
    virtual void SetCursorEnabled(bool enable);

    /**
     * \brief
     *
     */
    virtual bool IsCursorEnabled();

    /**
     * \brief
     *
     */
    virtual void SetCursor(jcursor_style_t cursor);

    /**
     * \brief
     *
     */
    virtual void SetCursor(std::shared_ptr<Image> shape, int hotx, int hoty);

    /**
     * \brief
     *
     */
    virtual jcursor_style_t GetCursor();

    /**
     * \brief
     *
     */
    virtual void SetRotation(jwindow_rotation_t t);

    /**
     * \brief
     *
     */
    virtual jwindow_rotation_t GetRotation();

};

}

#endif 
