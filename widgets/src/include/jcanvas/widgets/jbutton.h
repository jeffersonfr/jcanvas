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
#ifndef J_BUTTON_H
#define J_BUTTON_H

#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/widgets/jactionlistener.h"

#include <mutex>

namespace jcanvas {

class ActionListener;
class ActionEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Button : public Component {

  protected:
    /** \brief */
    std::vector<ActionListener *> _action_listeners;
    /** \brief */
    std::mutex _action_listener_mutex;
    /** \brief */
    std::shared_ptr<Image> _image;
    /** \brief */
    jhorizontal_align_t _halign;
    /** \brief */
    jvertical_align_t _valign;
    /** \brief */
    std::string _text;
    /** \brief */
    bool _is_pressed;

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
    Button(std::string label = {}, std::shared_ptr<Image> image = {});
    
    /**
     * \brief
     *
     */
    virtual ~Button();

    /**
     * \brief
     *
     */
    virtual void SetText(std::string text);
    
    /**
     * \brief
     *
     */
    virtual void SetImage(std::shared_ptr<Image> image);
    
    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> GetImage();
    
    /**
     * \brief
     *
     */
    virtual std::string GetText();

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
    virtual void Paint(Graphics *g);
    
    /**
     * \brief
     *
     */
    virtual bool KeyPressed(KeyEvent *event);

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

    /**
     * \brief
     *
     */
    virtual void RegisterActionListener(ActionListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveActionListener(ActionListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchActionEvent(ActionEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<ActionListener *> & GetActionListeners();

};

}

#endif

