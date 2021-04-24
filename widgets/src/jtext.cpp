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
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jgridlayout.h"
#include "jcanvas/widgets/jrectangleborder.h"

namespace jcanvas {

GenericChar::GenericChar(TextComponent *parent, char ch)
{
  mParent = parent;
  mChar = ch;

  Update();
}

GenericChar::~GenericChar()
{
}

void GenericChar::SetSelect(bool param)
{
  mSelected = param;
}

bool GenericChar::IsSelected()
{
  return mSelected;
}

void GenericChar::Update()
{
  jtheme_t &theme = GetTheme();

  theme.bg.normal = jcolor_name_t::Transparent;
  theme.bg.focus = jcolor_name_t::Transparent;
  theme.bg.disable = jcolor_name_t::Transparent;
  // theme.bg.select = jcolor_name_t::Transparent;
  theme.bg.highlight = jcolor_name_t::Transparent;

  mExtends = theme.font.primary->GetStringExtends(std::string(1, mChar));

  jpoint_t<int> size = {(int)(mExtends.advance.x + 2), theme.font.primary->GetSize()};

  SetMinimumSize(size);
  SetMaximumSize(size);
  SetPreferredSize(size);
  SetSize(size);
  
  SetBorder(nullptr);
}

void GenericChar::Paint(jcanvas::Graphics *g)
{
  jtheme_t theme = GetTheme();

  g->SetCompositeFlags(jcomposite_flags_t::SrcOver);
  g->SetFont(theme.font.primary);

  g->SetColor(theme.bg.normal);

  if (IsSelected() == true) {
    g->SetColor(theme.bg.select);
  }

  g->FillRectangle({0, 0, GetSize()});

  if (mChar >= 0x20) { // greater than space
    g->SetColor(theme.fg.normal);

    if (IsSelected() == true) {
      g->SetColor(theme.fg.select);
    }

    g->DrawString(std::string(1, mChar), jpoint_t<int>{static_cast<int>(GetSize().x - mExtends.size.x)/2, 0});
  }
  
  g->SetCompositeFlags(jcomposite_flags_t::Src);
}

SpaceChar::SpaceChar(TextComponent *parent):
  GenericChar(parent, ' ')
{
}

SpaceChar::~SpaceChar()
{
}

EmptyChar::EmptyChar(TextComponent *parent):
  GenericChar(parent, '\0')
{
  SetBackground(nullptr);

}

EmptyChar::~EmptyChar()
{
}

NewLineChar::NewLineChar(TextComponent *parent):
  GenericChar(parent, '\0')
{
  jpoint_t<int> size = GetSize();

  size.x = 0;

  SetMinimumSize(size);
  SetMaximumSize(size);
  SetPreferredSize(size);
  SetSize(size);
}

NewLineChar::~NewLineChar()
{
}

TextLayout::TextLayout(jhorizontal_align_t halign, jvertical_align_t valign)
{
  _halign = halign;
  _valign = valign;
}

TextLayout::~TextLayout()
{
}

void TextLayout::SetWrap(bool param)
{
  _wrap = param;
}

bool TextLayout::IsWrap()
{
  return _wrap;
}

void TextLayout::SetGap(jpoint_t<int> gap)
{
  _gap = gap;
}

jpoint_t<int> TextLayout::GetGap()
{
  return _gap;
}

jhorizontal_align_t TextLayout::GetHorizontalAlign()
{
  return _halign;
}

void TextLayout::SetHorizontalAlign(jhorizontal_align_t align)
{
  _halign = align;
}

jvertical_align_t TextLayout::GetVerticalAlign()
{
  return _valign;
}

void TextLayout::SetVerticalAlign(jvertical_align_t align)
{
  _valign = align;
}

int TextLayout::MoveComponents(Container *target, int x, int y, int width, int height, int rowStart, int rowEnd, bool ltr, bool useBaseline, int *ascent, int *descent, bool newLine)
{
  switch (_halign) {
    case jhorizontal_align_t::Left:
      x += ltr ? 0 : width;
      break;
    case jhorizontal_align_t::Center:
      x += width / 2;
      break;
    case jhorizontal_align_t::Right:
      x += ltr ? width : 0;
      break;
    case jhorizontal_align_t::Justify: // TODO:: implement
      x += ltr ? 0 : width;
      break;
  }

  int maxAscent = 0, nonbaselineHeight = 0, baselineOffset = 0;

  if (useBaseline) {
    int maxDescent = 0;

    for (int i = rowStart ; i < rowEnd ; i++) {
      Component *m = target->GetComponents()[i];
      jpoint_t<int> size = m->GetSize();

      if (m->IsVisible() == true) {
        if (ascent[i] >= 0) {
          maxAscent = std::max(maxAscent, ascent[i]);
          maxDescent = std::max(maxDescent, descent[i]);
        } else {
          nonbaselineHeight = std::max(size.y, nonbaselineHeight);
        }
      }
    }

    height = std::max(maxAscent + maxDescent, nonbaselineHeight);

    baselineOffset = (height - maxAscent - maxDescent) / 2;
  }

  jinsets_t insets = target->GetInsets();
  int maxwidth = target->GetSize().x - (insets.left + insets.right + 2*_gap.x);

  if (target->IsScrollYVisible() == true) {
    jtheme_t theme = target->GetTheme();

    maxwidth = maxwidth - theme.scroll.size.x - theme.scroll.padding.x;
  }

  float step = 0.0f;

  if (newLine == false and _halign == jhorizontal_align_t::Justify) {
    int total = 0;
    int count = 0;

    for (int i=rowStart; i<rowEnd; i++) {
      Component *m = target->GetComponents()[i];
      jpoint_t<int> size = m->GetSize();

      total = total + size.x;
      count = count + 1;
    }

    if (count > 1) {
      int free = maxwidth - total;

      if (free > 0) {
        step = free/(float)(count - 1);
      }
    }
  }

  Component *first = nullptr;
  Component *last = nullptr;
  float px = x;

  for (int i = rowStart ; i < rowEnd ; i++) {
    Component *m = target->GetComponents()[i];

    if (dynamic_cast<SpaceChar *>(m) == nullptr) {
      if (first == nullptr) {
        first = m;
      }

      last = m;
    }
  }

  for (int i = rowStart ; i < rowEnd ; i++) {
    Component *m = target->GetComponents()[i];

    jpoint_t<int> size = m->GetSize();

    if (m->IsVisible() == true) {
      int cy;

      if (useBaseline && ascent[i] >= 0) {
        cy = y + baselineOffset + maxAscent - ascent[i];
      } else {
        cy = y + (height - size.y) / 2;
      }

      if (ltr) {
        if (m == last) {
          if (_halign == jhorizontal_align_t::Justify and newLine == false) {
            int dx = maxwidth - (px + m->GetSize().x);

            if (dx > 0) {
              px = px + dx;
            }
          }
        }

        m->SetLocation(static_cast<int>(px), cy);
      } else {
        jpoint_t<int> size2 = target->GetSize();

        m->SetLocation(static_cast<int>(size2.x - px - size.x), cy);
      }

      if (_halign == jhorizontal_align_t::Justify and first != nullptr and first != m) {
        continue;
      }

      first = nullptr;

      px += size.x + step;
    }
  }

  return height;
}

jpoint_t<int> TextLayout::GetMinimumLayoutSize(Container *target)
{
  jpoint_t<int> t = {0, 0};

  // WARN:: sync parent
  int nmembers = target->GetComponentCount(), maxAscent = 0, maxDescent = 0;
  bool useBaseline = false, firstVisibleComponent = true;

  for (int i = 0 ; i < nmembers ; i++) {
    Component *m = target->GetComponents()[i];

    if (m->IsVisible()) {
      jpoint_t<int> d = m->GetMinimumSize();

      t.y = std::max(t.y, d.y);

      if (firstVisibleComponent) {
        firstVisibleComponent = false;
      } else {
        t.x += _gap.x;
      }

      t.x += d.x;

      if (useBaseline) {
        int baseline = m->GetBaseline(d.x, d.y);

        if (baseline >= 0) {
          maxAscent = std::max(maxAscent, baseline);
          maxDescent = std::max(maxDescent, t.y - baseline);
        }
      }
    }
  }

  if (useBaseline) {
    t.y = std::max(maxAscent + maxDescent, t.y);
  }

  jinsets_t insets = target->GetInsets();

  t.x += insets.left + insets.right + _gap.x*2;
  t.y += insets.top + insets.bottom + _gap.y*2;

  return t;
}

jpoint_t<int> TextLayout::GetMaximumLayoutSize(Container *)
{
  jpoint_t<int> t = {INT_MAX, INT_MAX};

  return t;
}

jpoint_t<int> TextLayout::GetPreferredLayoutSize(Container *target)
{
  jpoint_t<int> t = {0, 0};

  // WARN:: sync parent
  int nmembers = target->GetComponentCount(), maxAscent = 0, maxDescent = 0;
  bool firstVisibleComponent = true, useBaseline = false;

  for (int i = 0 ; i < nmembers ; i++) {
    Component *m = target->GetComponents()[i];

    if (m->IsVisible()) {
      jpoint_t<int> d = m->GetPreferredSize();

      t.y = std::max(t.y, d.y);

      if (firstVisibleComponent) {
        firstVisibleComponent = false;
      } else {
        t.x += _gap.x;
      }

      t.x += d.x;

      if (useBaseline) {
        int baseline = m->GetBaseline(d.x, d.y);
        if (baseline >= 0) {
          maxAscent = std::max(maxAscent, baseline);
          maxDescent = std::max(maxDescent, d.y - baseline);
        }
      }
    }
  }

  if (useBaseline) {
    t.y = std::max(maxAscent + maxDescent, t.y);
  }

  jinsets_t insets = target->GetInsets();

  t.x += insets.left + insets.right + _gap.x*2;
  t.y += insets.top + insets.bottom + _gap.y*2;

  return t;
}

void TextLayout::DoLayout(Container *target)
{
  // WARN:: syn with jframe
  jinsets_t 
    insets = target->GetInsets();
  jpoint_t<int> 
    size = target->GetSize();

  if (_wrap == false) {
    size.x = INT_MAX;
  }

  int maxwidth = size.x - (insets.left + insets.right + 2*_gap.x);

  if (target->IsScrollYVisible() == true) {
    jtheme_t theme = target->GetTheme();

    maxwidth = maxwidth - theme.scroll.size.x - theme.scroll.padding.x;
  }

  int nmembers = target->GetComponentCount();
  int x = insets.left, y = insets.top + _gap.y;
  int rowh = 0, start = 0;
  int *ascent = nullptr, *descent = nullptr;
  bool ltr = (target->GetComponentOrientation() == jcomponent_orientation_t::LeftToRight), useBaseline = false;

  if (useBaseline) {
    ascent = new int[nmembers];
    descent = new int[nmembers];
  }

  int rows = 0;
  int diff = 0;
  bool newLine = false;

  for (int i = 0 ; i < nmembers ; i++) {
    Component *m = target->GetComponents()[i];

    if (m->IsVisible() == true) {
      jpoint_t<int> psize = m->GetPreferredSize();

      m->SetSize(psize.x, psize.y);

      if (useBaseline) {
        int baseline = m->GetBaseline(psize.x, psize.y);

        if (baseline >= 0) {
          ascent[i] = baseline;
          descent[i] = psize.y - baseline;
        } else {
          ascent[i] = -1;
        }
      }

      if (newLine == false and ((x == 0) || ((x + psize.x) <= maxwidth))) {
        if (x > 0) {
          x += _gap.x;
        }
        x += psize.x;
        rowh = std::max(rowh, psize.y);
      } else {
        rowh = MoveComponents(target, insets.left + _gap.x, y, maxwidth - x, rowh, start, i, ltr, useBaseline, ascent, descent, newLine);
        x = psize.x;
        y = y + _gap.y + rowh;
        rowh = psize.y;
        start = i;
        rows = rows + 1;
      }

      if (newLine == true) {
        rows = rows + 1;
        diff = diff + psize.y + _gap.y;
      }
      
      newLine = (_wrap == true and dynamic_cast<NewLineChar *>(m) != nullptr);
    }
  }

  MoveComponents(target, insets.left + _gap.x, y, maxwidth - x, rowh, start, nmembers, ltr, useBaseline, ascent, descent, true);

  if (ascent != nullptr) {
    delete ascent;
  }

  if (descent != nullptr) {
    delete descent;
  }

  // INFO:: vertical align
  if (_valign == jvertical_align_t::Top) {
    return;
  }

  int max_y {};

  for (int i = 0 ; i < nmembers ; i++) {
    jrect_t<int> rect = target->GetComponents()[i]->GetBounds();

    if (max_y < (rect.point.y + rect.size.y)) {
      max_y = rect.point.y + rect.size.y;
    }
  }

  if (_valign == jvertical_align_t::Center) {
    float offset = (size.y - max_y)/2;

    for (int i = 0 ; i < nmembers ; i++) {
      target->GetComponents()[i]->Move(0, offset);
    }
  } else if (_valign == jvertical_align_t::Bottom) {
    float offset = size.y - max_y;

    for (int i = 0 ; i < nmembers ; i++) {
      target->GetComponents()[i]->Move(0, offset);
    }
  } else if (_valign == jvertical_align_t::Justify) {
    float offset = 0.0f;

    if (rows > 1) {
      offset = (size.y - insets.top - insets.bottom - y - diff)/(rows - 1);

      if (offset < 0.0f) {
        offset = 0.0f;
      }
    }

    jpoint_t<int> pos {};
    float py = -offset;

    for (int i = 0 ; i < nmembers ; i++) {
      Component *cmp = target->GetComponents()[i];
      jpoint_t<int> cpos = cmp->GetLocation();

      if (dynamic_cast<EmptyChar *>(cmp) == nullptr) {
        if (pos.y != cpos.y) {
          pos = cpos;
          py = py + offset;
        }
      }

      target->GetComponents()[i]->Move(0, py);
    }
  } 
}

Word::Word(TextComponent *parent, std::string word)
{
  mWord = word;

  SetLayout<FlowLayout>(jflowlayout_align_t::Left, jpoint_t<int>{0, 0});

  for (auto ch : mWord) {
    Add(new GenericChar(parent, ch));
  }

  SetMinimumSize({});
  SetPreferredSize(GetLayout()->GetPreferredLayoutSize(this));
}

Word::~Word()
{
  auto cmps = GetComponents();

  RemoveAll();

  for (auto cmp : cmps) {
    delete cmp;
  }
}

Paragraph::Paragraph(TextComponent *parent, std::string text)
{
  mParent = parent;
  mText = text;

  SetScrollable(true);
  SetBackground(nullptr);
  SetLayout(nullptr);

  std::string word;

  for (auto ch : mText) {
    if (ch == '\n') {
      if (word.empty() == false) {
        Add(new Word(parent, word));

        word = "";
      }

      Add(new NewLineChar(parent));
    } else if (ch == ' ') {
      if (word.empty() == false) {
        Add(new Word(parent, word));

        word = "";
      }

      Add(new SpaceChar(parent));
    } else {
      word = word + ch;
    }
  }

  if (word.empty() == false) {
    Add(new Word(parent, word));
  }

  Add(new EmptyChar(parent));

  SetLayout<TextLayout>(jhorizontal_align_t::Left, jvertical_align_t::Top);
}

Paragraph::~Paragraph()
{
  auto cmps = GetComponents();

  RemoveAll();

  for (auto cmp : cmps) {
    delete cmp;
  }
}

const std::string & Paragraph::GetText()
{
  return mText;
}

void Paragraph::SetCurrentIndex(std::size_t index)
{
  mCurrentIndex = index;
  
  if (mCurrentIndex > GetCharsCount()) {
    mCurrentIndex = GetCharsCount();
  }

  Component *current = GetCharByIndex(mCurrentIndex);

  if (current != nullptr) {
    Container *container = dynamic_cast<Word *>(current->GetParent());

    if (container != nullptr) {
      container->ScrollToVisibleArea();
    } else {
      current->ScrollToVisibleArea();
    }
  }
}

std::size_t Paragraph::GetCurrentIndex()
{
  return mCurrentIndex;
}

std::size_t Paragraph::GetCharsCount()
{
  return mText.size();
}

Component * Paragraph::GetCharByIndex(std::size_t index)
{
  for (auto cmp : GetComponents()) {
    auto word = dynamic_cast<Word *>(cmp);

    if (index == 0) {
      if (word != nullptr) {
        return word->GetComponents()[index];
      } 

      return cmp;
    }

    if (word != nullptr) {
      if (index >= (std::size_t)word->GetComponentCount()) {
        index = index - word->GetComponentCount();
      } else {
        return word->GetComponents()[index];
      }
    } else {
      index = index - 1;
    }
  }

  return nullptr;
}

void Paragraph::SetCaretType(jcaret_type_t type)
{
  mCaretType = type;
}

jcaret_type_t Paragraph::GetCaretType()
{
  return mCaretType;
}

void Paragraph::Paint(Graphics *g)
{
  Container::Paint(g);

  if (mParent->IsEditable() == true and dynamic_cast<Component *>(mParent)->HasFocus()) {
    Component *cmp = GetCharByIndex(GetCurrentIndex());

    if (cmp == nullptr) {
      return;
    }

    jrect_t<int> rect {cmp->GetAbsoluteLocation() - GetAbsoluteLocation(), cmp->GetSize()};
    jtheme_t theme = cmp->GetTheme();

    g->SetColor(theme.fg.focus);

    if (mCaretType == jcaret_type_t::Underscore) {
      int width = rect.size.x;

      if (width < 8) {
        width = 8;
      }

      g->FillRectangle({rect.point.x, rect.point.y + rect.size.y - 2, width, 2});
    } else if (mCaretType == jcaret_type_t::Stick) {
      g->FillRectangle({rect.point.x, rect.point.y, 2, rect.size.y});
    } else if (mCaretType == jcaret_type_t::Block) {
      int width = rect.size.x;

      if (width < 8) {
        width = 8;
      }

      g->SetColor(theme.fg.highlight);
      g->FillRectangle({rect.point.x, rect.point.y, width, rect.size.y});
    }
  }
}

Text::Text(std::string text)
{
  SetInsets({4, 4, 4, 4});
  SetLayout<BorderLayout>();
  SetText(text);
  SetBackground(nullptr);
  SetBorder(std::make_shared<RectangleBorder>());
  SetFocusable(true);
}

Text::~Text()
{
  RemoveAll();

  delete mParagraph;
}

void Text::Build(const std::string &text)
{
  bool wrap = true;

  if (mParagraph != nullptr) {
    wrap = mParagraph->GetLayout<TextLayout>()->IsWrap();
  }

  auto paragraph = new Paragraph(this, text);
  auto layout = paragraph->GetLayout<TextLayout>();

  layout->SetWrap(wrap);

  if (mParagraph != nullptr) {
    paragraph->SetCaretType(mParagraph->GetCaretType());

    layout->SetHorizontalAlign(mParagraph->GetLayout<TextLayout>()->GetHorizontalAlign());
    layout->SetVerticalAlign(mParagraph->GetLayout<TextLayout>()->GetVerticalAlign());
  }
  
  paragraph->SetScrollable(IsScrollable());

  Add(paragraph, jborderlayout_align_t::Center);

  Remove(mParagraph);

  delete mParagraph;
  mParagraph = paragraph;

  mSelectionLength = 0;

  SetCaretPosition(0);

  SetPreferredSize({32, 32});
}

std::function<Text::KeyMapResult(jkeyevent_symbol_t)> Text::OnKeyMap(std::function<Text::KeyMapResult(jkeyevent_symbol_t)> callback)
{
  std::lock_guard<std::mutex> lock(mKeyMapMutex);

  auto old = mKeyMap;

  mKeyMap = callback;

  return old;
}

void Text::SetScrollableX(bool param)
{
  Component::SetScrollableX(param);

  mParagraph->SetScrollableX(param);
}

void Text::SetScrollableY(bool param)
{
  Component::SetScrollableY(param);

  mParagraph->SetScrollableY(param);
}
    
void Text::SetScrollable(bool param)
{
  Component::SetScrollable(param);

  mParagraph->SetScrollable(param);
}

void Text::SetWrap(bool param)
{
  mParagraph->GetLayout<TextLayout>()->SetWrap(param);
}

bool Text::IsWrap()
{
  return mParagraph->GetLayout<TextLayout>()->IsWrap();
}

bool Text::IsEnabled()
{
  return Container::IsEnabled();
}

void Text::SetHorizontalAlign(jhorizontal_align_t align)
{
  mParagraph->GetLayout<TextLayout>()->SetHorizontalAlign(align);
}

void Text::SetVerticalAlign(jvertical_align_t align)
{
  mParagraph->GetLayout<TextLayout>()->SetVerticalAlign(align);
}

jhorizontal_align_t Text::GetHorizontalAlign()
{
  return mParagraph->GetLayout<TextLayout>()->GetHorizontalAlign();
}

jvertical_align_t Text::GetVerticalAlign()
{
  return mParagraph->GetLayout<TextLayout>()->GetVerticalAlign();
}

void Text::SetMaxSize(std::size_t max)
{
  mMaxSize = max;
}

std::size_t Text::GetMaxSize()
{
  return mMaxSize;
}

void Text::SetText(std::string text)
{
  if (mMaxSize > 0) {
    text = GetText().substr(0, std::min(GetText().size(), mMaxSize));
  }

  Build(text);

  DispatchTextEvent(new TextEvent(this, text));
}

const std::string & Text::GetText()
{
  return mParagraph->GetText();
}

std::size_t Text::GetCharsCount()
{
  return mParagraph->GetCharsCount();
}

void Text::SetCaretPosition(std::size_t pos)
{
  auto current = GetCurrentChar();

  if (current != nullptr) {
    std::size_t start = mCaretPosition;
    std::size_t end = start + mSelectionLength;

    for (std::size_t i=start; i<end; i++) {
      dynamic_cast<GenericChar *>(GetCharByIndex(i))->SetSelect(false);
    }
  }

  mSelectionLength = 0;

  mParagraph->SetCurrentIndex(pos);
}

std::size_t Text::GetCaretPosition()
{
  return mParagraph->GetCurrentIndex();
}

void Text::NextChar()
{
  SetCaretPosition(GetCaretPosition() + 1);
}

void Text::PreviousChar()
{
  SetCaretPosition(GetCaretPosition() - 1);
}

void Text::Insert(std::string text)
{
  std::size_t caretPosition = GetCaretPosition();

  std::string currentText = GetText();

  if (mSelectionLength > 0) {
    SetText(currentText.replace(caretPosition, mSelectionLength, text));
    SetCaretPosition(caretPosition);
  } else {
    SetText(currentText.insert(caretPosition, text));
    SetCaretPosition(caretPosition + text.size());
  }
}

void Text::Append(std::string text)
{
  std::size_t caretPosition = GetCaretPosition();

  SetCaretPosition(INT_MAX);

  Insert(text);

  SetCaretPosition(caretPosition);
}

void Text::Select(std::size_t start, std::size_t end)
{
  SetCaretPosition(mCaretPosition);

  start = std::min(start, GetCharsCount());
  end = std::min(end, GetCharsCount());

  if (start > end) {
    std::swap(start, end);
  }

  SetCaretPosition(start);

  mSelectionLength = end - start;

  for (std::size_t i=start; i<end; i++) {
    dynamic_cast<GenericChar *>(GetCharByIndex(i))->SetSelect(true);
  }
}

void Text::SelectAll()
{
  SetCaretPosition(0);

  mSelectionLength = GetCharsCount();
}

void Text::SetCaretType(jcaret_type_t type)
{
  mParagraph->SetCaretType(type);
}

jcaret_type_t Text::GetCaretType()
{
  return mParagraph->GetCaretType();
}

void Text::SetEditable(bool enable)
{
  mEditable = enable;
}

bool Text::IsEditable()
{
  return mEditable;
}

std::string Text::GetSelectedText()
{
  return mParagraph->GetText().substr(mCaretPosition, mCaretPosition + mSelectionLength);
}

void Text::Backspace()
{
  if (mSelectionLength > 0) {
    Insert("");

    return;
  }

  PreviousChar();
  Delete();
}

void Text::Delete()
{
  if (mSelectionLength > 0) {
    Insert("");

    return;
  }

  auto current = GetCurrentChar();

  if (current == nullptr or dynamic_cast<EmptyChar *>(current) != nullptr) {
    return;
  }

  std::size_t pos = GetCaretPosition();

  Select(pos, pos + 1);
  Insert("");
  SetCaretPosition(pos);

  DispatchTextEvent(new TextEvent(this, GetText()));
}

Component * Text::GetCharByIndex(std::size_t index)
{
  return mParagraph->GetCharByIndex(index);
}

Component * Text::GetCurrentChar()
{
  return GetCharByIndex(GetCaretPosition());
}

void Text::RegisterTextListener(TextListener *listener)
{
  if (listener == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> guard(mTextListenersMutex);

  if (std::find(mTextListeners.begin(), mTextListeners.end(), listener) == mTextListeners.end()) {
    mTextListeners.push_back(listener);
  }
}

void Text::RemoveTextListener(TextListener *listener)
{
  if (listener == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock1(mRemoveTextListenersMutex);
  std::lock_guard<std::mutex> lock2(mTextListenersMutex);

  mTextListeners.erase(std::remove(mTextListeners.begin(), mTextListeners.end(), listener), mTextListeners.end());
}

void Text::DispatchTextEvent(TextEvent *event)
{
  if (event == nullptr) {
    return;
  }

  mTextListenersMutex.lock();

  std::vector<TextListener *> listeners = mTextListeners;

  mTextListenersMutex.unlock();

  std::lock_guard<std::mutex> lock(mRemoveTextListenersMutex);

  for (std::vector<TextListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    TextListener *listener = (*i);

    listener->TextChanged(event);
  }

  delete event;
}

const std::vector<TextListener *> & Text::GetTextListeners()
{
  return mTextListeners;
}

bool Text::KeyReleased(KeyEvent *event)
{
  if (event->GetSymbol() == jkeyevent_symbol_t::Shift) {
    mShiftPressed = false;
  }

  return true;
}

bool Text::KeyPressed(KeyEvent *event)
{
  if (event->GetSymbol() == jkeyevent_symbol_t::Shift) {
    mShiftPressed = true;
  }

  if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - event->GetTimestamp()) > std::chrono::milliseconds{200}) {
    return true;
  }

  if (IsEditable() == false) {
    return false;
  }

  if (event->GetSymbol() == jkeyevent_symbol_t::CursorLeft) {
    // TODO:: adjust the text selection to both sides
    if (mShiftPressed) {
      std::size_t pos = GetCaretPosition();

      Select(pos + mSelectionLength, pos - 1);
    } else {
      PreviousChar();
    }
  } else if (event->GetSymbol() == jkeyevent_symbol_t::CursorRight) {
    // TODO:: adjust the text selection to both sides
    if (mShiftPressed) {
      std::size_t pos = GetCaretPosition();

      Select(pos, pos + mSelectionLength + 1);
    } else {
      NextChar();
    }
  } else if (event->GetSymbol() == jkeyevent_symbol_t::CursorUp) {
  } else if (event->GetSymbol() == jkeyevent_symbol_t::CursorDown) {
  } else if (event->GetSymbol() == jkeyevent_symbol_t::Home) {
    SetCaretPosition(0);
  } else if (event->GetSymbol() == jkeyevent_symbol_t::End) {
    SetCaretPosition(INT_MAX);
  } else if (event->GetSymbol() == jkeyevent_symbol_t::Delete) {
    Delete();
  } else if (event->GetSymbol() == jkeyevent_symbol_t::Backspace) {
    Backspace();
  } else {
    jkeyevent_symbol_t symbol = event->GetSymbol();

    {
      std::lock_guard<std::mutex> lock(mKeyMapMutex);
    
      if (mKeyMap != nullptr) {
        KeyMapResult result = mKeyMap(event->GetSymbol());

        if (result.first == false) {
          return false;
        }

        symbol = result.second;
      }
    }

    int code = KeyEvent::GetCodeFromSymbol(symbol);

    if (code > 0x00 and code < 0xff) {
      if (mShiftPressed == true) {
        code = toupper(code);
      }

      Insert(std::string(1, code));
    }
  }

  Repaint();

  return true;
}

}
