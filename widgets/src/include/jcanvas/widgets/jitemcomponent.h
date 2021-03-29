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
#ifndef  J_ITEMCOMPONENT_H
#define J_ITEMCOMPONENT_H

#include "jcanvas/widgets/jcomponent.h"
#include "jcanvas/widgets/jselectlistener.h"
#include "jcanvas/core/jimage.h"

#include <mutex>

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jitem_type_t {
  Empty,
  Text,
  Image,
  Check
};

class Item {

  private:
    /** \brief */
    std::vector<Item *> _childs;
    /** \brief */
    std::string _value;
    /** \brief */
    std::shared_ptr<Image> _image;
    /** \brief */
    jitem_type_t _type;
    /** \brief */
    jhorizontal_align_t _halign;
    /** \brief */
    jvertical_align_t _valign;
    /** \brief */
    bool _is_checked;
    /** \brief */
    bool _is_visible;
    /** \brief */
    bool _is_enabled;

  public:
    /**
     * \brief
     *
     */
    Item();

    /**
     * \brief
     *
     */
    Item(std::string value);

    /**
     * \brief
     *
     */
    Item(std::string value, std::shared_ptr<Image> image);

    /**
     * \brief
     *
     */
    Item(std::string value, bool checked);

    /**
     * \brief
     *
     */
    virtual ~Item();

    /**
     * \brief Clone object.
     *
     */
    virtual Item * Clone();
    
    /**
     * \brief
     *
     */
    virtual const std::vector<Item *> & GetChilds();

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
    virtual void SetEnabled(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsEnabled();

    /**
     * \brief
     *
     */
    virtual void SetVisible(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsVisible();

    /**
     * \brief
     *
     */
    virtual bool IsSelected();

    /**
     * \brief
     *
     */
    virtual void SetSelected(bool b);

    /**
     * \brief
     *
     */
    virtual void AddChild(Item *item);

    /**
     * \brief
     *
     */
    virtual void AddChild(Item *item, int index);

    /**
     * \brief
     *
     */
    virtual void AddChilds(std::vector<Item *> &items);

    /**
     * \brief
     *
     */
    virtual void RemoveChild(int index);

    /**
     * \brief
     *
     */
    virtual void RemoveChild(Item *item);

    /**
     * \brief
     *
     */
    virtual int GetChildsSize();

    /**
     * \brief
     *
     */
    virtual std::string GetValue();

    /**
     * \brief
     *
     */
    virtual std::shared_ptr<Image> GetImage();

    /**
     * \brief
     *
     */
    virtual jitem_type_t GetType();

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ItemComponent {

  protected:
    /** \brief */
    std::vector<SelectListener *> _select_listeners;
    /** \brief */
    std::mutex _itemcomponent_mutex;
    /** \brief */
    std::mutex _select_listener_mutex;
    /** \brief */
    std::mutex _remove_select_listener_mutex;
    /** \brief */
    std::vector<Item *> _items;
    /** \brief */
    std::vector<Item *> _internal;
    /** \brief */
    std::string _text;
    /** \brief */
    int _index;
    /** \brief */
    int _item_size;
    /** \brief */
    int _item_gap;
    /** \brief */
    bool _loop;

  protected:
    void AddInternalItem(Item *item);

  public:
    /**
     * \brief
     *
     */
    ItemComponent();

    /**
     * \brief
     *
     */
    virtual ~ItemComponent();

    /**
     * \brief
     *
     */
    virtual int GetItemSize();

    /**
     * \brief
     *
     */
    virtual int GetItemGap();

    /**
     * \brief
     *
     */
    virtual void SetItemSize(int size);

    /**
     * \brief
     *
     */
    virtual void SetItemGap(int gap);

    /**
     * \brief
     *
     */
    virtual int GetItemsCount();

    /**
     * \brief
     *
     */
    virtual void SetLoop(bool loop);

    /**
     * \brief
     *
     */
    virtual void SetCurrentIndex(int i);

    /**
     * \brief
     *
     */
    virtual void AddEmptyItem();
    
    /**
     * \brief
     *
     */
    virtual void AddTextItem(std::string text);
    
    /**
     * \brief
     *
     */
    virtual void AddImageItem(std::string text, std::shared_ptr<Image> image);
    
    /**
     * \brief
     *
     */
    virtual void AddCheckedItem(std::string text, bool checked);
    
    /**
     * \brief
     *
     */
    virtual void AddItem(Item *item);

    /**
     * \brief
     *
     */
    virtual void AddItem(Item *item, int index);

    /**
     * \brief
     *
     */
    virtual void AddItems(std::vector<Item *> &items);

    /**
     * \brief
     *
     */
    virtual void RemoveItem(Item *item);
    
    /**
     * \brief
     *
     */
    virtual void RemoveItem(int index);
    
    /**
     * \brief
     *
     */
    virtual void RemoveItems();

    /**
     * \brief
     *
     */
    virtual Item * GetItem(int index);
  
    /**
     * \brief
     *
     */
    virtual const std::vector<Item *> & GetItems();
  
    /**
     * \brief
     *
     */
    virtual Item * GetCurrentItem();

    /**
     * \brief
     *
     */
    virtual int GetCurrentIndex();

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

