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
#include "jcanvas/core/jeventobject.h"

namespace jcanvas {

EventObject::EventObject(void *source)
{
  _timestamp = std::chrono::steady_clock::now();
  _source = source;
  _is_consumed = false;
}

EventObject::~EventObject()
{
}

void * EventObject::GetSource()
{
  return _source;
}

void EventObject::Consume()
{
  _is_consumed = true;
}

bool EventObject::IsConsumed()
{
  return _is_consumed;
}

void EventObject::Reset()
{
  _is_consumed = false;
}

std::chrono::time_point<std::chrono::steady_clock> EventObject::GetTimestamp()
{
  return _timestamp;
}

}

