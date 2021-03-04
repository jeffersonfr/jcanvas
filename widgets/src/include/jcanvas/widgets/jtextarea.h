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
#ifndef J_TEXTAREA_H
#define J_TEXTAREA_H

#include "jcanvas/widgets/jtextcomponent.h"

#include <vector>

namespace jcanvas {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TextArea : public TextComponent {

  private:
    /** \brief */
    std::vector<std::string> _lines;
    /** \brief */
    int _rows_gap;
    /** \brief */
    int _current_row;
    /** \brief */
    int _rows_string;
    /** \brief */
    bool _is_wrap;

    /**
     * \brief
     *
     */
    virtual void IncrementLines(int lines);
    
    /**
     * \brief
     *
     */
    virtual void DecrementLines(int lines);
    
    /**
     * \brief
     *
     */
    virtual void InitRowsString();

  public:
    /**
     * \brief
     *
     */
    TextArea();
    
    /**
     * \brief
     *
     */
    virtual ~TextArea();

    /**
     * \brief
     *
     */
    virtual int GetRowsGap();
    
    /**
     * \brief
     *
     */
    virtual void SetRowsGap(int gap);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetScrollDimension();
    
    /**
     * \brief
     *
     */
    virtual std::string GetLineAt(int row);

    /**
     * \brief
     *
     */
    virtual const std::vector<std::string> & GetLines();

    /**
     * \brief
     *
     */
    virtual int GetRows();

    /*
     * \brief
     *
     */
    virtual void SetCurrentRow(int line);

    /*
     * \brief
     *
     */
    virtual int GetCurrentRow();

    /**
     * \brief
     *
     */
    virtual void SetWrap(bool b);
    
    /**
     * \brief
     *
     */
    virtual bool IsWrap();

    /**
     * \brief
     *
     */
    virtual void SetEchoChar(char echo_char);

    /**
     * \brief
     *
     */
    virtual void SetText(std::string text);
    
    /**
     * \brief
     *
     */
    virtual void Insert(std::string text);

    /**
     * \brief
     *
     */
    virtual void Delete();

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

};

}

#endif

