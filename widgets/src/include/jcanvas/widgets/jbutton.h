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

#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jflatimage.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jactionlistener.h"

#include <functional>
#include <mutex>

namespace jcanvas {

class ActionListener;
class ActionEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Button : public Container {

  private:
    /** \brief */
    std::vector<ActionListener *> _action_listeners;
    /** \brief */
    std::mutex _action_listener_mutex;
    /** \brief */
    std::function<void(Button *, bool)> _onclick;
    /** \brief */
    FlatImage *_image {nullptr};
    /** \brief */
    Text *_text {nullptr};
    /** \brief */
    bool _is_pressed {false};

    void Build(std::string text, std::shared_ptr<Image> image);

  protected:
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
    virtual bool MousePressed(MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(MouseEvent *event);
 
  public:
    /**
     * \brief
     *
     */
    Button(std::string text = {}, std::shared_ptr<Image> image = {});
    
    /**
     * \brief
     *
     */
    virtual ~Button();

    /**
     * \brief
     *
     */
    std::function<void(Button *, bool)> OnClick(std::function<void(Button *, bool)> callback);

    /**
     * \brief
     *
     */
    void Click();

    /**
     * \brief
     *
     */
    virtual void SetPressed(bool param);
    
    /**
     * \brief
     *
     */
    virtual bool IsPressed();
    
    /**
     * \brief
     *
     */
    virtual void SetText(std::string text);
    
    /**
     * \brief
     *
     */
    virtual Text * GetTextComponent();

    /**
     * \brief
     *
     */
    virtual void SetImage(std::shared_ptr<Image> image);
    
    /**
     * \brief
     *
     */
    virtual FlatImage * GetImageComponent();
    
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

