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
#ifndef J_COMPONENT_H
#define J_COMPONENT_H

#include "jcanvas/widgets/jtheme.h"
#include "jcanvas/widgets/jbackground.h"
#include "jcanvas/widgets/jborder.h"
#include "jcanvas/widgets/jcomponentlistener.h"
#include "jcanvas/widgets/jfocuslistener.h"
#include "jcanvas/core/jgraphics.h"
#include "jcanvas/core/jkeymap.h"
#include "jcanvas/core/jkeylistener.h"
#include "jcanvas/core/jmouselistener.h"
#include "jcanvas/algebra/jinsets.h"

#include <vector>
#include <mutex>
#include <memory>

namespace jcanvas {

enum class jcomponent_alignment_t {
  Top,
  Center,
  Bottom,
  Left,
  Right
};

enum class jcomponent_orientation_t {
  LeftToRight,
  RightToLeft,
  UpToBottom,
  BottomToUp,
};

// component baseline resize behavior
enum class jcomponent_behavior_t {
  // Indicates the baseline remains fixed relative to the y-origin.  That is, <code>getBaseline</code> returns
  // the same value regardless of the height or width.  For example, a <code>JLabel</code> containing non-empty 
  // text with a vertical alignment of <code>TOP</code> should have a baseline type of <code>CONSTANT_ASCENT</code>.
  ConstantAscent,
  // Indicates the baseline remains fixed relative to the height and does not change as the width is varied.  That is, 
  // for any height H the difference between H and <code>getBaseline(w, H)</code> is the same.  For example, a <code>
  // JLabel</code> containing non-empty text with a vertical alignment of <code>BOTTOM</code> should have a baseline 
  // type of <code>CONSTANT_DESCENT</code>.
  ConstantDescent,
  // Indicates the baseline remains a fixed distance from the center of the component.  That is, for any height H the
  // difference between <code>getBaseline(w, H)</code> and <code>H / 2</code> is the same (plus or minus one depending 
  // upon rounding error). <p> Because of possible rounding errors it is recommended you ask for the baseline with two 
  // consecutive heights and use the return value to determine if you need to pad calculations by 1.  The following shows 
  // how to calculate the baseline for any height:
  // <pre>
  //    jrect_t<int> preferredSize = component->GetPreferredSize();
  //    int baseline = GetBaseline(preferredSize.x, preferredSize.y);
  //    int nextBaseline = GetBaseline(preferredSize.x, preferredSize.y + 1);
  //     // Amount to add to height when calculating where baseline lands for a particular height:
  //     int padding = 0;
  //     // Where the baseline is relative to the mid point
  //     int baselineOffset = baseline - height / 2;
  //     if (preferredSize.y % 2 == 0 && baseline != nextBaseline) {
  //       padding = 1;
  //     } else if (preferredSize.y % 2 == 1 && baseline == nextBaseline) {
  //       baselineOffset--;
  //       padding = 1;
  //     }
  //     // The following calculates where the baseline lands for the height z:
  //     int calculatedBaseline = (z + padding) / 2 + baselineOffset;
  //   </pre>
  CenterOffset,
  // Indicates the baseline resize behavior can not be expressed using any of the other constants.  
  // This may also indicate the baseline varies with the width of the component.  This is also returned
  // by components that do not have a baseline.
  Other
};

class Container;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Component : public std::enable_shared_from_this<Component>, public KeyListener, public MouseListener {

  friend class Container;
  friend class Frame;

  private:
    /** \brief */
    std::vector<FocusListener *> _focus_listeners;
    /** \brief */
    std::vector<ComponentListener *> _component_listeners;
    /** \brief */
    std::mutex _focus_listener_mutex;
    /** \brief */
    std::mutex _remove_focus_listener_mutex;
    /** \brief */
    std::mutex _component_listener_mutex;
    /** \brief */
    std::mutex _remove_component_listener_mutex;
    /** \brief */
    std::mutex _data_listener_mutex;
    /** \brief */
    std::shared_ptr<Background> _background;
    /** \brief */
    std::shared_ptr<Border> _border;
    /** \brief */
    Container *_parent;
    /** \brief */
    Component *_left;
    /** \brief */
    Component *_right;
    /** \brief */
    Component *_up;
    /** \brief */
    Component *_down;
    /** \brief */
    KeyMap *_keymap;
    /** \brief */
    jtheme_t _theme;
    /** \brief */
    std::string _name;
    /** \brief */
    jpoint_t<int> _location;
    /** \brief */
    jpoint_t<int> _scroll_location;
    /** \brief */
    int _scroll_minor_increment;
    /** \brief */
    int _scroll_major_increment;
    /** \brief */
    jpoint_t<int> _size;
    /** \brief */
    jpoint_t<int> _preferred_size;
    /** \brief */
    jpoint_t<int> _minimum_size;
    /** \brief */
    jpoint_t<int> _maximum_size;
    /** \brief */
    jcomponent_alignment_t _alignment_x;
    /** \brief */
    jcomponent_alignment_t _alignment_y;
    /** \brief */
    jcomponent_orientation_t _orientation;
    /** \brief */
    int _gradient_level;
    /** \brief */
    jpoint_t<int> _relative_mouse;
    /** \brief */
    int _component_state;
    /** \brief */
    bool _is_visible;
    /** \brief */
    bool _is_cyclic_focus;
    /** \brief */
    bool _is_navigation_enabled;
    /** \brief */
    bool _is_ignore_repaint;
    /** \brief */
    bool _is_background_visible;
    /** \brief */
    bool _is_focusable;
    /** \brief */
    bool _is_enabled;
    /** \brief */
    bool _is_focus_cycle_root;
    /** \brief */
    bool _is_scrollable_x;
    /** \brief */
    bool _is_scrollable_y;
    /** \brief */
    bool _is_smooth_scroll;
    /** \brief */
    bool _is_opaque;

  protected:
    /**
     * \brief
     *
     */
    virtual void GetInternalComponents(Container *current, std::vector<Component *> &components);

    /**
     * \brief
     *
     */
    virtual bool ProcessNavigation(KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual void FindNextComponentFocus(jrect_t<int> rect, Component *&left, Component *&right, Component *&up, Component *&down);

    /**
     * \brief Makes sure the component is visible in the scroll if this container is scrollable.
     * 
     * @param x
     * @param y
     * @param width
     * @param height
     * @param coordinateSpace the component according to whose coordinates rect is defined. Rect's 
     *     x/y are relative to that component (they are not absolute).
     *
     */
    virtual void ScrollToVisibleArea(jrect_t<int> rect, Component *coordinateSpace);

  public:
    /**
     * \brief
     *
     */
    Component(jrect_t<int> bounds = {0, 0, 0, 0});
    
    /**
     * \brief
     *
     */
    virtual ~Component();

    /**
     * \brief
     *
     */
    virtual void ScrollToVisibleArea();

    /**
     * \brief
     *
     */
    void SetBackground(std::shared_ptr<Background> background);

    /**
     * \brief
     *
     */
    template<typename T, typename ...Args>
    void SetBackground(Args ...args) 
    {
      SetBackground(std::make_shared<T>(std::forward<Args>(args)...));
    }
    
    /**
     * \brief
     *
     */
    template <typename T = Background>
    std::shared_ptr<T> GetBackground()
    {
      return std::dynamic_pointer_cast<T>(_background);
    }

    /**
     * \brief
     *
     */
    void SetBorder(std::shared_ptr<Border> border);

    /**
     * \brief
     *
     */
    template<typename T, typename ...Args>
    void SetBorder(Args ...args) 
    {
      SetBorder(std::make_shared<T>(std::forward<Args>(args)...));
    }
    
    /**
     * \brief
     *
     */
    template <typename T = Border>
    std::shared_ptr<T> GetBorder()
    {
      return std::dynamic_pointer_cast<T>(_border);
    }

    /**
     * \brief
     *
     */
    virtual void SetName(std::string name);
    
    /**
     * \brief
     *
     */
    virtual std::string GetName();

    /**
     * \brief
     *
     */
    virtual Container * GetParent();
    
    /**
     * \brief
     *
     */
    virtual Container * GetTopLevelAncestor();

    /**
     * \brief
     *
     */
    virtual int GetBaseline(int width, int height);
    
    /**
     * \brief
     *
     */
    virtual jcomponent_behavior_t GetBaselineResizeBehavior();

    /**
     * \brief
     *
     */
    virtual jcomponent_alignment_t GetAlignmentX();
    
    /**
     * \brief
     *
     */
    virtual jcomponent_alignment_t GetAlignmentY();
    
    /**
     * \brief
     *
     */
    virtual void SetAlignmentX(jcomponent_alignment_t align);
    
    /**
     * \brief
     *
     */
    virtual void SetAlignmentY(jcomponent_alignment_t align);
    
    /**
     * \brief
     *
     */
    virtual void SetComponentOrientation(jcomponent_orientation_t orientation);
    
    /**
     * \brief
     *
     */
    virtual jcomponent_orientation_t GetComponentOrientation();

    /**
     * \brief
     *
     */
    virtual bool IsScrollableX();
    
    /**
     * \brief
     *
     */
    virtual bool IsScrollableY();
    
    /**
     * \brief
     *
     */
    virtual void SetScrollableX(bool scrollable);
    
    /**
     * \brief
     *
     */
    virtual void SetScrollableY(bool scrollable);
    
    /**
     * \brief
     *
     */
    virtual void SetScrollable(bool scrollable);
    
    /**
     * \brief
     *
     */
    virtual void SetSmoothScrolling(bool smooth);

    /**
     * \brief
     *
     */
    virtual bool IsCyclicFocus();

    /**
     * \brief
     *
     */
    virtual void SetCyclicFocus(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsSmoothScrolling();

    /**
     * \brief
     *
     */
    virtual bool IsScrollable();
    
    /**
     * \brief
     *
     */
    virtual bool IsScrollXVisible();
    
    /**
     * \brief
     *
     */
    virtual bool IsScrollYVisible();
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetAbsoluteLocation();
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetScrollLocation();
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetScrollDimension();
    
    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetBounds();
    
    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetVisibleBounds();
    
    /**
     * \brief
     *
     */
    virtual void SetScrollLocation(int x, int y);
    
    /**
     * \brief
     *
     */
    virtual void SetScrollLocation(jpoint_t<int> t);
    
    /**
     * \brief
     *
     */
    virtual int GetMinorScrollIncrement();
    
    /**
     * \brief
     *
     */
    virtual int GetMajorScrollIncrement();
    
    /**
     * \brief
     *
     */
    virtual void SetMinorScrollIncrement(int increment);
    
    /**
     * \brief
     *
     */
    virtual void SetMajorScrollIncrement(int increment);
    
    /**
     * \brief
     *
     */
    virtual jtheme_t & GetTheme();

    /**
     * \brief
     *
     */
    virtual void SetTheme(jtheme_t theme);

    /**
     * \brief
     *
     */
    virtual void SetKeyMap(KeyMap *keymap);

    /**
     * \brief
     *
     */
    virtual KeyMap * GetKeyMap();

    /**
     * \brief
     *
     */
    virtual bool IsIgnoreRepaint();
    
    /**
     * \brief
     *
     */
    virtual void SetIgnoreRepaint(bool b);
    
    /**
     * \brief
     *
     */
    virtual void SetOpaque(bool opaque);
    
    /**
     * \brief
     *
     */
    virtual bool IsOpaque();
    
    /**
     * \brief
     *
     */
    virtual void SetParent(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual bool IsEnabled();
    
    /**
     * \brief
     *
     */
    virtual void SetEnabled(bool b);
    
    /**
     * \brief
     *
     */
    virtual void SetNavigationEnabled(bool b);

    /**
     * \brief
     *
     */
    virtual bool IsNavigationEnabled();

    /**
     * \brief
     *
     */
    virtual Component * GetNextFocusLeft();
    
    /**
     * \brief
     *
     */
    virtual Component * GetNextFocusRight();
    
    /**
     * \brief
     *
     */
    virtual Component * GetNextFocusUp();
    
    /**
     * \brief
     *
     */
    virtual Component * GetNextFocusDown();
    
    /**
     * \brief
     *
     */
    virtual void SetNextFocusLeft(Component *cmp);
    
    /**
     * \brief
     *
     */
    virtual void SetNextFocusRight(Component *cmp);
    
    /**
     * \brief
     *
     */
    virtual void SetNextFocusUp(Component *cmp);
    
    /**
     * \brief
     *
     */
    virtual void SetNextFocusDown(Component *cmp);
    
    /**
     * \brief
     *
     */
    virtual void SetNextComponentFocus(Component *left, Component *right, Component *up, Component *down);

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
    
    /**
     * \brief
     *
     */
    virtual void RequestFocus();
    
    /**
     * \brief
     *
     */
    virtual void ReleaseFocus();
    
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
    virtual bool IsHidden();
    
    /**
     * \brief
     *
     */
    virtual bool HasFocus();

    /**
     * \brief
     *
     */
    virtual void SetMinimumSize(jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual void SetMaximumSize(jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual void SetPreferredSize(jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMinimumSize();
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetMaximumSize();
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetPreferredSize();

    /**
     * \brief
     *
     */
    virtual void Move(int x, int y);
    
    /**
     * \brief
     *
     */
    virtual void Move(jpoint_t<int> location);
    
    /**
     * \brief
     *
     */
    virtual void SetBounds(int x, int y, int width, int height);
    
    /**
     * \brief
     *
     */
    virtual void SetBounds(jpoint_t<int> location, jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual void SetBounds(jrect_t<int> region);
    
    /**
     * \brief
     *
     */
    virtual void SetLocation(int x, int y);
    
    /**
     * \brief
     *
     */
    virtual void SetLocation(jpoint_t<int> point);
    
    /**
     * \brief
     *
     */
    virtual void SetSize(int width, int height);
    
    /**
     * \brief
     *
     */
    virtual void SetSize(jpoint_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetLocation();
    
    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetSize();
    
    /**
     * \brief
     *
     */
    virtual void SetGradientLevel(int level);
    
    /**
     * \brief
     *
     */
    virtual int GetGradientLevel();

    /**
     * \brief
     *
     */
    virtual void RaiseToTop();
    
    /**
     * \brief
     *
     */
    virtual void LowerToBottom();
    
    /**
     * \brief
     *
     */
    virtual void PutAtop(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void PutBelow(Component *c);

    /**
     * \brief
     *
     */
    virtual void SetFocusable(bool b);
    
    /**
     * \brief
     *
     */
    virtual bool IsFocusable();

    /**
     * \brief
     *
     */
    virtual Container * GetFocusCycleRootAncestor();

    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
    /**
     * \brief
     *
     */
    virtual void PaintScrollbars(Graphics *g);

    /**
     * \brief
     *
     */
    virtual void Repaint(Component *cmp = nullptr);

    /**
     * \brief
     *
     */
    virtual void RegisterFocusListener(FocusListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveFocusListener(FocusListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchFocusEvent(FocusEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<FocusListener *> & GetFocusListeners();

    /**
     * \brief
     *
     */
    virtual void RegisterComponentListener(ComponentListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveComponentListener(ComponentListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchComponentEvent(ComponentEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<ComponentListener *> & GetComponentListeners();

};

}

#endif

