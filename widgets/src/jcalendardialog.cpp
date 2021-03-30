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
#include "jcanvas/widgets/jcalendardialog.h"

#include <ctime>

namespace jcanvas {

CalendarDialog::CalendarDialog(Container *parent):
  Dialog("Calendar", parent)
{
  std::time_t 
    t1 = std::time(0);
  std::tm 
    *tm1 = std::localtime(&t1);

  _select_day = tm1->tm_mday - 1;
  _select_month = tm1->tm_mon;
  _select_year = tm1->tm_year;

  jinsets_t 
    insets = GetInsets();
  char 
    tmp[255];
  int
    ds = 32,
    dx = ds + 2,
    dy = ds + 2;

  _syear = new Spin();
  
  _syear->SetBounds({insets.left, insets.top + 0*dy, 7*dx - 2, ds});

  for (int i=1900; i<2199; i++) {
    sprintf(tmp, "%d", i);

    _syear->AddTextItem(tmp);
  }

  _syear->SetLoop(true);
  _syear->RegisterSelectListener(this);

  _smonth = new Spin();
  
  _smonth->SetBounds({insets.left, insets.top + 1*dy, 7*dx - 2, ds});

  _smonth->AddTextItem("Janeiro");
  _smonth->AddTextItem("Fevereiro");
  _smonth->AddTextItem("Marco");
  _smonth->AddTextItem("Abril");
  _smonth->AddTextItem("Maio");
  _smonth->AddTextItem("Junho");
  _smonth->AddTextItem("Julho");
  _smonth->AddTextItem("Agosto");
  _smonth->AddTextItem("Setembro");
  _smonth->AddTextItem("Outubro");
  _smonth->AddTextItem("Novembro");
  _smonth->AddTextItem("Dezembro");

  _smonth->SetLoop(true);
  _smonth->RegisterSelectListener(this);

  _ldom = new Text("D");
  _lseg = new Text("S");
  _lter = new Text("T");
  _lqua = new Text("Q");
  _lqui = new Text("Q");
  _lsex = new Text("S");
  _lsab = new Text("S");

  _ldom->SetBounds({insets.left + 0*dx, insets.top + 2*dy, ds, ds});
  _lseg->SetBounds({insets.left + 1*dx, insets.top + 2*dy, ds, ds});
  _lter->SetBounds({insets.left + 2*dx, insets.top + 2*dy, ds, ds});
  _lqua->SetBounds({insets.left + 3*dx, insets.top + 2*dy, ds, ds});
  _lqui->SetBounds({insets.left + 4*dx, insets.top + 2*dy, ds, ds});
  _lsex->SetBounds({insets.left + 5*dx, insets.top + 2*dy, ds, ds});
  _lsab->SetBounds({insets.left + 6*dx, insets.top + 2*dy, ds, ds});

  _ldom->SetEditable(false);
  _lseg->SetEditable(false);
  _lter->SetEditable(false);
  _lqua->SetEditable(false);
  _lqui->SetEditable(false);
  _lsex->SetEditable(false);
  _lsab->SetEditable(false);

  _ldom->SetFocusable(false);
  _lseg->SetFocusable(false);
  _lter->SetFocusable(false);
  _lqua->SetFocusable(false);
  _lqui->SetFocusable(false);
  _lsex->SetFocusable(false);
  _lsab->SetFocusable(false);

  _ldom->SetHorizontalAlign(jhorizontal_align_t::Center);
  _lseg->SetHorizontalAlign(jhorizontal_align_t::Center);
  _lter->SetHorizontalAlign(jhorizontal_align_t::Center);
  _lqua->SetHorizontalAlign(jhorizontal_align_t::Center);
  _lqui->SetHorizontalAlign(jhorizontal_align_t::Center);
  _lsex->SetHorizontalAlign(jhorizontal_align_t::Center);
  _lsab->SetHorizontalAlign(jhorizontal_align_t::Center);

  _week_day_theme.bg.normal = jcolor_t {0xff808080};
  _selected_theme.bg.normal = jcolor_t {0xff408040};

  _ldom->SetTheme(_week_day_theme);
  _lseg->SetTheme(_week_day_theme);
  _lter->SetTheme(_week_day_theme);
  _lqua->SetTheme(_week_day_theme);
  _lqui->SetTheme(_week_day_theme);
  _lsex->SetTheme(_week_day_theme);
  _lsab->SetTheme(_week_day_theme);

  Add(_smonth);
  Add(_syear);
  Add(_ldom);
  Add(_lseg);
  Add(_lter);
  Add(_lqua);
  Add(_lqui);
  Add(_lsex);
  Add(_lsab);

  _smonth->SetCurrentIndex(_select_month);
  _syear->SetCurrentIndex(_select_year);

  BuildCalendar();
}

CalendarDialog::~CalendarDialog() 
{
}

void CalendarDialog::SetDay(int d)
{
  if (d < 1) {
    d = 1;
  }

  if (d > 31) {
    d = 31;
  }

  _select_day = d - 1;
}

void CalendarDialog::SetMonth(int m)
{
  if (m < 1) {
    m = 1;
  }

  if (m > 12) {
    m = 12;
  }

  _select_month = m - 1;
}

void CalendarDialog::SetYear(int y)
{
  if (y < 1900) {
    y = 1900;
  }

  if (y > 2199) {
    y = 2199;
  }

  _select_year = y - 1900;
}

int CalendarDialog::GetDay()
{
  return _select_day;
}

int CalendarDialog::GetMonth()
{
  return _select_month;
}

int CalendarDialog::GetYear()
{
  return _select_year;
}

void CalendarDialog::AddWarnning(jtheme_t theme, int day, int month, int year)
{
  jcalendar_warnning_t t;

  t.theme = theme;
  t.day = day;
  t.month = month;
  t.year = year;

  _warnning_days.push_back(t);

  BuildCalendar();
}

void CalendarDialog::RemoveWarnning(jcalendar_warnning_t t)
{
  for (std::vector<jcalendar_warnning_t >::iterator i=_warnning_days.begin(); i!=_warnning_days.end(); i++) {
    if (t.day == (*i).day && t.month == (*i).month && t.year == (*i).year) {
      _warnning_days.erase(i);

      break;
    }
  }
}

void CalendarDialog::RemoveAll()
{
  _warnning_days.clear();
}

void CalendarDialog::BuildCalendar()
{
  int 
    day_count = 0,
    first_day = 0;
  char 
    tmp[255];

  std::time_t 
    t1 = std::time(nullptr);
  std::tm 
    *tm1 = std::localtime(&t1);

  int
    dd = tm1->tm_mday,
    mm = tm1->tm_mon,
    yy = tm1->tm_year + 1900;

  tm1->tm_mday = 1;
  tm1->tm_mon = _smonth->GetCurrentIndex();
  tm1->tm_year = _syear->GetCurrentIndex() + 1900;

  std::mktime(tm1);

  first_day = tm1->tm_wday - 1;

  if (first_day < 0) {
    first_day = first_day + 7;
  }

  switch (tm1->tm_mon + 1) {
    case 1: day_count = 31; break;
    case 2: 
        if ((((tm1->tm_year + 1900)%4) == 0 && ((tm1->tm_year + 1900)%100) != 0) || ((tm1->tm_year + 1900)%400) == 0) {
          day_count = 29;
        } else {
          day_count = 28;
        }

        break;
    case 3: day_count = 31; break;
    case 4: day_count = 30; break;
    case 5: day_count = 31; break;
    case 6: day_count = 30; break;
    case 7: day_count = 31; break;
    case 8: day_count = 31; break;
    case 9: day_count = 30; break;
    case 10: day_count = 31; break;
    case 11: day_count = 30; break;
    case 12: day_count = 31; break;
  }

  while (_buttons.size() > 0) {
    auto i = _buttons.begin();

    Remove(*i);

    _buttons.erase(i);
  }

  jinsets_t 
    insets = GetInsets();
  int 
    k = 3;

  for (int i=0; i<day_count; i++) {
    int
      ds = 36,
      dx = ds + 2,
      dy = ds + 2;

    sprintf(tmp, "%d", (i+1));

    Button
      *button = new Button(tmp);
    
    button->SetBounds({insets.left + dx*first_day, insets.top + dy*k + 16, ds, ds});

    first_day = ((first_day + 1)%7);

    if (first_day == 0) {
      k++;
    }

    _buttons.push_back(button);

    if (dd == (i + 1) && tm1->tm_mon == mm && tm1->tm_year == yy) {
      button->SetTheme(_selected_theme);
    }

    for (std::vector<jcalendar_warnning_t>::iterator it=_warnning_days.begin(); it!=_warnning_days.end(); it++) {
      if ((i + 1) == (*it).day && (tm1->tm_mon + 1) == (*it).month && (tm1->tm_year) == (*it).year) {
        button->SetTheme(it->theme);
      }
    }

    button->RegisterActionListener(this);

    Add(button);
  }

  if (GetFocusOwner() == nullptr) {
    _buttons[tm1->tm_mday - 1]->RequestFocus();
  }

  Pack(true);
  Repaint();
}

void CalendarDialog::ActionPerformed(ActionEvent *event)
{
  Button *button = reinterpret_cast<Button *>(event->GetSource());

  if (button->IsPressed() == false) {
    return;
  }

  // TODO:: send a generic action event com a data atual selectionada
  // TODO:: deixar o button como checked (toggle)

  // DispatchActionEvent(new ActionEvent(this));
}

void CalendarDialog::ItemChanged(SelectEvent *event)
{
  Spin
    *spin = reinterpret_cast<Spin *>(event->GetSource());

  if (event->GetType() == jselectevent_type_t::Left) {
    if (spin == _smonth and _smonth->GetCurrentIndex() == (12 - 1)) {
      _syear->SetCurrentIndex(_syear->GetCurrentIndex() - 1);
    }
  } else if (event->GetType() == jselectevent_type_t::Right) {
    if (spin == _smonth and _smonth->GetCurrentIndex() == (1 - 1)) {
      _syear->SetCurrentIndex(_syear->GetCurrentIndex() + 1);
    }
  }

  BuildCalendar();
}

}
