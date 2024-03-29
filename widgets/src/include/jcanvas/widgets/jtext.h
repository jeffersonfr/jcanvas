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
#ifndef J_TEXT_H
#define J_TEXT_H

#include "jcanvas/widgets/jcontainer.h"
#include "jcanvas/widgets/jflowlayout.h"
#include "jcanvas/widgets/jtextlistener.h"
#include "jcanvas/widgets/jnulllayout.h"

#include "/usr/local/include/jmixin/jstring.h"

namespace jcanvas {

enum class jcaret_type_t {
  None,
  Underscore,
  Stick,
  Block
};

class TextComponent {

  public:
    /**
     * \brief
     *
     */
    virtual ~TextComponent()
    {
    }

    /**
     * \brief
     *
     */
    virtual bool IsEditable() = 0;
    
    /**
     * \brief
     *
     */
    virtual bool IsEnabled() = 0;
    
};

class GenericChar : public Component {

  public:
    /**
     * \brief
     *
     */
    GenericChar(TextComponent *parent, char ch);

    /**
     * \brief
     *
     */
    virtual ~GenericChar();

    /**
     * \brief
     *
     */
    void SetSelect(bool param);

    /**
     * \brief
     *
     */
    bool IsSelected();

    /**
     * \brief
     *
     */
    void Update();

    /**
     * \brief
     *
     */
    void Paint(jcanvas::Graphics *g);

  private:
    /** \brief */
    TextComponent *mParent {nullptr};
    /** \brief */
    jfont_extends_t mExtends;
    /** \brief */
    char mChar {'\0'};
    /** \brief */
    bool mSelected {false};

};

class SpaceChar : public GenericChar {

  public:
    /**
     * \brief
     *
     */
    SpaceChar(TextComponent *parent);

    /**
     * \brief
     *
     */
    virtual ~SpaceChar();

};


class EmptyChar : public GenericChar {

  public:
    /**
     * \brief
     *
     */
    EmptyChar(TextComponent *parent);

    /**
     * \brief
     *
     */
    virtual ~EmptyChar();

};

class NewLineChar : public GenericChar {

  public:
    /**
     * \brief
     *
     */
    NewLineChar(TextComponent *parent);

    /**
     * \brief
     *
     */
    virtual ~NewLineChar();

};

class TextLayout : public Layout {

  private:
    /** \brief */
    jhorizontal_align_t _halign;
    /** \brief */
    jvertical_align_t _valign;
    /** \brief */
    jpoint_t<int> _gap {0, 2};
    /** \brief */
    bool _wrap {true};

  public:
    /**
     * \brief
     *
     */
    TextLayout(jhorizontal_align_t halign = jhorizontal_align_t::Justify, jvertical_align_t valign = jvertical_align_t::Top);
    
    /**
     * \brief
     *
     */
    virtual ~TextLayout();

    /**
     * \brief
     *
     */
    void SetWrap(bool param);

    /**
     * \brief
     *
     */
    bool IsWrap();

    /**
     * \brief
     *
     */
    void SetGap(jpoint_t<int> gap);

    /**
     * \brief
     *
     */
    jpoint_t<int> GetGap();

    /**
     * \brief
     *
     */
    jhorizontal_align_t GetHorizontalAlign();

    /**
     * \brief
     *
     */
    void SetHorizontalAlign(jhorizontal_align_t align);
    
    /**
     * \brief
     *
     */
    jvertical_align_t GetVerticalAlign();

    /**
     * \brief
     *
     */
    void SetVerticalAlign(jvertical_align_t align);
    
    /**
     * \brief
     *
     */
    int MoveComponents(Container *target, int x, int y, int width, int height, int rowStart, int rowEnd, bool ltr, bool useBaseline, int *ascent, int *descent, bool newLine);

    /**
     * \brief
     *
     */
    jpoint_t<int> GetMinimumLayoutSize(Container *target);
    
    /**
     * \brief
     *
     */
    jpoint_t<int> GetMaximumLayoutSize(Container *target);
    
    /**
     * \brief
     *
     */
    jpoint_t<int> GetPreferredLayoutSize(Container *target);

    /**
     * \brief
     *
     */
    void DoLayout(Container *target);

};

class Word : public Container {

  public:
    /**
     * \brief
     *
     */
    Word(TextComponent *parent, std::string word);

    /**
     * \brief
     *
     */
    virtual ~Word();

  private:
    /** \brief */
    std::string mWord;

};

class Paragraph : public Container {

  public:
    /**
     * \brief
     *
     */
    Paragraph(TextComponent *parent, std::string text);

    /**
     * \brief
     *
     */
    virtual ~Paragraph();

    /**
     * \brief
     *
     */
    const std::string & GetText();

    /**
     * \brief
     *
     */
    void SetCurrentIndex(std::size_t index);

    /**
     * \brief
     *
     */
    std::size_t GetCurrentIndex();

    /**
     * \brief
     *
     */
    std::size_t GetCharsCount();

    /**
     * \brief
     *
     */
    Component * GetCharByIndex(std::size_t index);

    /**
     * \brief
     *
     */
    void SetCaretType(jcaret_type_t type);

    /**
     * \brief
     *
     */
    jcaret_type_t GetCaretType();

    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g) override;

  private:
    /** \brief */
    TextComponent *mParent {nullptr};
    /** \brief */
    std::string mText;
    /** \brief */
    std::size_t mCurrentIndex {0};
    /** \brief */
    jcaret_type_t mCaretType {jcaret_type_t::Underscore};

};

class Text : public Container, public TextComponent {

	public:
    using KeyMapResult = std::pair<bool, jkeyevent_symbol_t>;

    /**
     * \brief
     *
     */
		Text(std::string text = std::string(""));

    /**
     * \brief
     *
     */
    virtual ~Text();

    /**
     * \brief
     *
     */
    virtual std::function<KeyMapResult(jkeyevent_symbol_t)> OnKeyMap(std::function<KeyMapResult(jkeyevent_symbol_t)> callback);

    /**
     * \brief
     *
     */
    virtual void SetScrollableX(bool param);
    
    /**
     * \brief
     *
     */
    virtual void SetScrollableY(bool param);
    
    /**
     * \brief
     *
     */
    virtual void SetScrollable(bool param) override;

    /**
     * \brief
     *
     */
    void SetWrap(bool param);

    /**
     * \brief
     *
     */
    bool IsWrap();

    /**
     * \brief
     *
     */
    bool IsEnabled();

    /**
     * \brief
     *
     */
    void SetHorizontalAlign(jhorizontal_align_t align);

    /**
     * \brief
     *
     */
    void SetVerticalAlign(jvertical_align_t align);

    /**
     * \brief
     *
     */
    jhorizontal_align_t GetHorizontalAlign();

    /**
     * \brief
     *
     */
    jvertical_align_t GetVerticalAlign();

    /**
     * \brief
     *
     */
    void SetMaxSize(std::size_t max);

    /**
     * \brief
     *
     */
    std::size_t GetMaxSize();

    /**
     * \brief
     *
     */
    void SetText(std::string text);

    /**
     * \brief
     *
     */
    const std::string & GetText();

    /**
     * \brief
     *
     */
    std::size_t GetCharsCount();

    /**
     * \brief
     *
     */
    void SetCaretPosition(std::size_t pos);

    /**
     * \brief
     *
     */
    std::size_t GetCaretPosition();

    /**
     * \brief
     *
     */
    void NextChar();

    /**
     * \brief
     *
     */
    void PreviousChar();

    /**
     * \brief
     *
     */
    void Insert(std::string text);

    /**
     * \brief
     *
     */
    void Append(std::string text);

    /**
     * \brief
     *
     */
    void Select(std::size_t start, std::size_t end);

    /**
     * \brief
     *
     */
    void SelectAll();

    /**
     * \brief
     *
     */
    void SetCaretType(jcaret_type_t type);

    /**
     * \brief
     *
     */
    jcaret_type_t GetCaretType();

    /**
     * \brief
     *
     */
    void SetEditable(bool enable);

    /**
     * \brief
     *
     */
    bool IsEditable();

    /**
     * \brief
     *
     */
    std::string GetSelectedText();

    /**
     * \brief
     *
     */
    void Backspace();

    /**
     * \brief
     *
     */
    void Delete();

    /**
     * \brief
     *
     */
    Component * GetCharByIndex(std::size_t index);

    /**
     * \brief
     *
     */
    Component * GetCurrentChar();

    /**
     * \brief
     *
     */
    void RegisterTextListener(TextListener *listener);

    /**
     * \brief
     *
     */
    void RemoveTextListener(TextListener *listener);

    /**
     * \brief
     *
     */
    void DispatchTextEvent(TextEvent *event);

    /**
     * \brief
     *
     */
    const std::vector<TextListener *> & GetTextListeners();

    /**
     * \brief
     *
     */
    bool KeyReleased(KeyEvent *event) override;

    /**
     * \brief
     *
     */
    bool KeyPressed(KeyEvent *event) override;

  private:
    /** \brief */
    std::function<KeyMapResult(jkeyevent_symbol_t)> mKeyMap;
    /** \brief */
    std::mutex mKeyMapMutex;
    /** \brief */
    std::vector<TextListener *> mTextListeners;
    /** \brief */
    std::mutex mTextListenersMutex;
    /** \brief */
    std::mutex mRemoveTextListenersMutex;
    /** \brief */
    Paragraph *mParagraph {nullptr};
    /** \brief */
    std::size_t mMaxSize {0};
    /** \brief */
    std::size_t mCaretPosition {0};
    /** \brief */
    std::size_t mSelectionLength {0};
    /** \brief */
    bool mEditable {true};
    /** \brief */
    bool mShiftPressed {false};

    /**
     * \brief
     *
     */
    void Build(const std::string &text);

};

}

#endif
