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
#ifndef J_CALENDARDIALOG_H
#define J_CALENDARDIALOG_H

#include "jcanvas/widgets/jdialog.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/widgets/jspin.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jselectlistener.h"
#include "jcanvas/widgets/jactionlistener.h"

namespace jcanvas {

/**
 * \brief
 *
 */
struct jcalendar_warnning_t {
  jtheme_t theme;
  int day;
  int month;
  int year;
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CalendarDialog : public Dialog, public ActionListener, public SelectListener {

  private:
    /** \brief */
    std::vector<jcalendar_warnning_t> _warnning_days;
    /** \brief */
    std::vector<Button *> _buttons;
    /** \brief */
    jtheme_t _week_day_theme;
    /** \brief */
    jtheme_t _selected_theme;
    /** \brief */
    Text *_ldom;
    /** \brief */
    Text *_lseg;
    /** \brief */
    Text *_lter;
    /** \brief */
    Text *_lqua;
    /** \brief */
    Text *_lqui;
    /** \brief */
    Text *_lsex;
    /** \brief */
    Text *_lsab;
    /** \brief */
    Spin *_syear;
    /** \brief */
    Spin *_smonth;
    /** \brief */
    std::string _text;
    /** \brief */
    int _state;
    /** \brief */
    int _select_day;
    /** \brief */
    int _select_month;
    /** \brief */
    int _select_year;
    /** \brief */
    bool started;
    /** \brief */
    bool _show_text;
    /** \brief */
    bool _response;

  private:
    /**
     * \brief
     *
     */
    void BuildCalendar();

  public:
    /**
     * \brief
     *
     */
    CalendarDialog(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual ~CalendarDialog();

    /**
     * \brief
     *
     */
    virtual void SetDay(int d);
    
    /**
     * \brief
     *
     */
    virtual void SetMonth(int m);
    
    /**
     * \brief
     *
     */
    virtual void SetYear(int y);

    /**
     * \brief
     *
     */
    virtual int GetDay();
    
    /**
     * \brief
     *
     */
    virtual int GetMonth();
    
    /**
     * \brief
     *
     */
    virtual int GetYear();

    /**
     * \brief
     *
     */
    virtual void AddWarnning(jtheme_t theme, int day, int month, int year);
    
    /**
     * \brief
     *
     */
    virtual void RemoveWarnning(jcalendar_warnning_t t);
    
    /**
     * \brief
     *
     */
    virtual void RemoveAll();

    /**
     * \brief
     *
     */
    virtual void ActionPerformed(ActionEvent *event);
    
    /**
     * \brief
     *
     */
    virtual void ItemChanged(SelectEvent *event);
    
};

}

#endif 
