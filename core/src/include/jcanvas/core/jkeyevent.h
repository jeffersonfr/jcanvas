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
#ifndef J_KEYEVENT_H
#define J_KEYEVENT_H

#include "jcanvas/core/jeventobject.h"

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jkeyevent_modifiers_t {
  None = 0,
  Shift = 1 << 0,
  Control = 1 << 1,
  Alt = 1 << 2,
  AltGr = 1 << 3,
  Meta = 1 << 4,
  Super = 1 << 5,
  Hyper = 1 << 6,
  CapsLock = 1 << 7,
  NumLock = 1 << 8,
  ScrollLock = 1 << 9
};

/**
 * \brief
 *
 */
enum class jkeyevent_type_t {
  Unknown,
  Pressed,
  Released,
  Typed
};

/**
 * \brief
 *
 */
enum class jkeyevent_symbol_t {
  Unknown,
  Exit,
  Backspace,
  Tab,
  Enter,
  Cancel,
  Escape,
  Space,
  ExclamationMark,
  Quotation,
  Hash,
  Dollar,
  Percent,
  Ampersand,
  Aposthrophe,
  ParenthesisLeft,
  ParenthesisRight,
  Star,
  Sharp,
  Plus,
  Comma,
  Minus,
  Period,
  Slash,
  Number0,
  Number1,
  Number2,
  Number3,
  Number4,
  Number5,
  Number6,
  Number7,
  Number8,
  Number9,

  Colon,
  SemiColon,
  LessThan,
  Equals,
  GreaterThan, 
  QuestionMark,
  At,

  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,         
  J,       
  K,     
  L,   
  M, 
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,            
  Y,          
  Z,        

  a,      
  b,    
  c,  
  d,
  e,
  f,
  g,
  h,
  i,
  j,
  k,              
  l,            
  m,          
  n,        
  o,      
  p,    
  q,  
  r,
  s,
  t,
  u,
  v,
  w,
  x,
  y,
  z,

  CapitalCedilla,
  Cedilla,

  SquareBracketLeft,
  BackSlash,
  SquareBracketRight,
  CircumflexAccent,
  Underscore,
  AcuteAccent,
  GraveAccent,
  CurlyBracketLeft,    
  VerticalBar,  
  CurlyBracketRight,
  Tilde,

  Delete,
  Insert,
  Home,
  End,
  PageUp,
  PageDown,
  Print,
  Pause,
  Break,
  Sleep,
  Suspend,
  Hibernate,

  CursorLeft,
  CursorRight,
  CursorUp,
  CursorDown,

  CursorLeftUp,
  CursorLeftDown,  
  CursorUpRight,
  CursorDownRight,

  Red,
  Green,
  Yellow,
  Blue,

  F1, 
  F2,
  F3,
  F4,
  F5,
  F6,          
  F7,        
  F8,      
  F9,    
  F10,  
  F11,
  F12,

  Shift,
  Control,    
  Alt,  
  AltGr,
  Meta,
  Super,
  Hyper,

  Power,
  Menu,
  File,
  Info,
  Back,
  Guide,

  ChannelUp,
  ChannelDown,

  VolumeUp,
  VolumeDown,

  Play,
  Stop,
  Eject,
  Rewind,
  Record,
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class KeyEvent : public EventObject {

  private:
    /** \brief */
    int _code;
    /** \brief */
    jkeyevent_symbol_t _symbol;
    /** \brief */
    jkeyevent_type_t _type;
    /** \brief */
    jkeyevent_modifiers_t _mod;

  public:
    /**
     * \brief
     *
     */
    KeyEvent(void *source, jkeyevent_type_t type, jkeyevent_modifiers_t mod, int code, jkeyevent_symbol_t symbol);
    
    /**
     * \brief
     *
     */
    virtual ~KeyEvent();

    /**
     * \brief
     *
     */
    static int GetCodeFromSymbol(jkeyevent_symbol_t symbol);

    /**
     * \brief
     *
     */
    virtual int GetKeyCode();
    
    /**
     * \brief
     *
     */
    virtual jkeyevent_symbol_t GetSymbol();
    
    /**
     * \brief
     *
     */
    virtual jkeyevent_type_t GetType();
    
    /**
     * \brief
     *
     */
    virtual jkeyevent_modifiers_t GetModifiers();

};

}

#endif

