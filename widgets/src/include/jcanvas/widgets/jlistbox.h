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
#ifndef J_LISTBOX_H
#define J_LISTBOX_H

#include "jcanvas/widgets/jitemcomponent.h"
#include "jcanvas/widgets/jactionlistener.h"
#include "jcanvas/widgets/jselectlistener.h"
#include "jcanvas/widgets/jbutton.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jlistbox_selection_t {
  None,
  Single,
  Multiple
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ListBox : public Container, public ActionListener {

  private:
    /** \brief */
    std::vector<SelectListener *> _select_listeners;
    /** \brief */
    std::mutex _itemcomponent_mutex;
    /** \brief */
    std::mutex _select_listener_mutex;
    /** \brief */
    std::mutex _remove_select_listener_mutex;
    /** \brief */
    jlistbox_selection_t _selection_type;

    /**
     * \brief
     *
     */
    virtual void ActionPerformed(ActionEvent *event) override;

  public:
    /**
     * \brief
     *
     */
    ListBox();
    
    /**
     * \brief
     *
     */
    virtual ~ListBox();

    /**
     * \brief
     *
     */
    template <typename T, typename ...Args>
    void AddItem(Args ...args)
    {
      static_assert(std::is_base_of<Button, T>::value, "T must be a button");

      Button *button = new T {std::forward<Args>(args)...};

      button->OnClick(nullptr);
      button->RegisterActionListener(this);

      if (_selection_type == jlistbox_selection_t::None) {
        button->SetFocusable(false);
      }

      FlatImage *image = button->GetImageComponent();

      if (image != nullptr) {
        image->SetPreferredSize({});
      }

      Container::Add(button);
    }

    /**
     * \brief
     *
     */
    virtual void RemoveItem(Button *item);

    /**
     * \brief
     *
     */
    virtual void RemoveItemByIndex(int index);

    /**
     * \brief
     *
     */
    virtual void SelectItem(Button *item);

    /**
     * \brief
     *
     */
    virtual void SelectItemByIndex(int index);

    /**
     * \brief
     *
     */
    virtual void UnselectItem(Button *item);

    /**
     * \brief
     *
     */
    virtual void UnselectItemByIndex(int index);

    /**
     * \brief
     *
     */
    virtual void UnselectAll();

    /**
     * \brief
     *
     */
    virtual void SetSelectionType(jlistbox_selection_t type);
    
    /**
     * \brief
     *
     */
    virtual jlistbox_selection_t GetSelectionType();
    
    /**
     * \brief
     *
     */
    virtual void SelectIndexes(std::vector<int> itemsitem);
    
    /**
     * \brief
     *
     */
    virtual std::vector<int> GetSelectedIndexes();

    /**
     * \brief
     *
     */
    virtual std::vector<Button *> GetSelectedItems();

    /**
     * \brief
     *
     */
    virtual Button * GetItemByIndex(int index);
    
    /**
     * \brief
     *
     */
    virtual void RegisterSelectListener(SelectListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveSelectListener(SelectListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchSelectEvent(SelectEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<SelectListener *> & GetSelectListeners();

};

}

#endif 
