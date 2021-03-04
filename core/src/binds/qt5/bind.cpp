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
#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindowadapter.h"
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jenum.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <stdexcept>

#include <QDialog>
#include <QResizeEvent>
#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>
#include <QScreen>

namespace jcanvas {

/** \brief */
Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static QApplication *sg_application = nullptr;
/** \brief */
static QDialog *sg_handler = nullptr;
/** \brief */
static QByteArray sg_geometry;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static bool sg_visible = false;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;
/** \brief */
static Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 16777220:
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		case 16777219:
			return jkeyevent_symbol_t::Backspace;
		case 16777217:
			return jkeyevent_symbol_t::Tab;
		// case ALLEGRO_KEY_CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case 16777216:
			return jkeyevent_symbol_t::Escape;
		case 32:
			return jkeyevent_symbol_t::Space;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return jkeyevent_symbol_t::ExclamationMark;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return jkeyevent_symbol_t::Quotation;
		// case ALLEGRO_KEY_HASH:
		// 	return jkeyevent_symbol_t::Hash;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return jkeyevent_symbol_t::Dollar;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return jkeyevent_symbol_t::Percent;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return jkeyevent_symbol_t::Ampersand;
		case 39:
			return jkeyevent_symbol_t::Aposthrophe;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return jkeyevent_symbol_t::ParenthesisLeft;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return jkeyevent_symbol_t::ParenthesisRight;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return jkeyevent_symbol_t::Star;
		// case ALLEGRO_KEY_PLUS:
		// 	return jkeyevent_symbol_t::Plus;
		case 44:   
			return jkeyevent_symbol_t::Comma;
		case 45:
			return jkeyevent_symbol_t::Minus;
		case 46:  
		 	return jkeyevent_symbol_t::Period;
		case 47:
			return jkeyevent_symbol_t::Slash;
		case 48:     
			return jkeyevent_symbol_t::Number0;
		case 49:
			return jkeyevent_symbol_t::Number1;
		case 50:
			return jkeyevent_symbol_t::Number2;
		case 51:
			return jkeyevent_symbol_t::Number3;
		case 52:
			return jkeyevent_symbol_t::Number4;
		case 53:
			return jkeyevent_symbol_t::Number5;
		case 54:
			return jkeyevent_symbol_t::Number6;
		case 55:
			return jkeyevent_symbol_t::Number7;
		case 56:
			return jkeyevent_symbol_t::Number8;
		case 57:
			return jkeyevent_symbol_t::Number9;
		// case ALLEGRO_KEY_COLON:
		// 	return jkeyevent_symbol_t::Colon;
		case 59:
			return jkeyevent_symbol_t::SemiColon;
		// case ALLEGRO_KEY_LESS:
		// 	return jkeyevent_symbol_t::LessThan;
		case 61: 
			return jkeyevent_symbol_t::Equals;
		// case ALLEGRO_KEY_GREATER:
		// 	return jkeyevent_symbol_t::GreaterTHan;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return jkeyevent_symbol_t::QuestionMark;
		// case ALLEGRO_KEY_AT:
		//	return jkeyevent_symbol_t::At;
		case 65:
			return jkeyevent_symbol_t::a;
		case 66:
			return jkeyevent_symbol_t::b;
		case 67:
			return jkeyevent_symbol_t::c;
		case 68:
			return jkeyevent_symbol_t::d;
		case 69:
			return jkeyevent_symbol_t::e;
		case 70:
			return jkeyevent_symbol_t::f;
		case 71:
			return jkeyevent_symbol_t::g;
		case 72:
			return jkeyevent_symbol_t::h;
		case 73:
			return jkeyevent_symbol_t::i;
		case 74:
			return jkeyevent_symbol_t::j;
		case 75:
			return jkeyevent_symbol_t::k;
		case 76:
			return jkeyevent_symbol_t::l;
		case 77:
			return jkeyevent_symbol_t::m;
		case 78:
			return jkeyevent_symbol_t::n;
		case 79:
			return jkeyevent_symbol_t::o;
		case 80:
			return jkeyevent_symbol_t::p;
		case 81:
			return jkeyevent_symbol_t::q;
		case 82:
			return jkeyevent_symbol_t::r;
		case 83:
			return jkeyevent_symbol_t::s;
		case 84:
			return jkeyevent_symbol_t::t;
		case 85:
			return jkeyevent_symbol_t::u;
		case 86:
			return jkeyevent_symbol_t::v;
		case 87:
			return jkeyevent_symbol_t::w;
		case 88:
			return jkeyevent_symbol_t::x;
		case 89:
			return jkeyevent_symbol_t::y;
		case 90:
			return jkeyevent_symbol_t::z;
		case 91:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case 92:   
			return jkeyevent_symbol_t::BackSlash;
		case 93:
			return jkeyevent_symbol_t::SquareBracketRight;
		// case ALLEGRO_KEY_CARET:
		// 	return jkeyevent_symbol_t::CircumflexAccent;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return jkeyevent_symbol_t::Underscore;
		// case ALLEGRO_KEY_BACKQUOTE:
		//	return jkeyevent_symbol_t::GraveAccent;
		// case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
		//	return jkeyevent_symbol_t::CurlyBracketLeft;
		// case ALLEGRO_KEY_VERTICAL_BAR:  
		// 	return jkeyevent_symbol_t::VerticalBar;
		// case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
		// 	return jkeyevent_symbol_t::CurlyBracketRight;
		// case ALLEGRO_KEY_TILDE:  
		//	return jkeyevent_symbol_t::Tilde;
		case 16777223:
			return jkeyevent_symbol_t::Delete;
		case 16777234:
			return jkeyevent_symbol_t::CursorLeft;
		case 16777236:
			return jkeyevent_symbol_t::CursorRight;
		case 16777235:  
			return jkeyevent_symbol_t::CursorUp;
		case 16777237:
			return jkeyevent_symbol_t::CursorDown;
		case 16777222:  
			return jkeyevent_symbol_t::Insert;
		case 16777232:     
			return jkeyevent_symbol_t::Home;
		case 16777233:
			return jkeyevent_symbol_t::End;
		case 16777238:
			return jkeyevent_symbol_t::PageUp;
		case 16777239:
			return jkeyevent_symbol_t::PageDown;
		// case ALLEGRO_KEY_PRINTSCREEN:   
		//	return jkeyevent_symbol_t::Print;
		case 16777224:
			return jkeyevent_symbol_t::Pause;
		// case ALLEGRO_KEY_RED:
		// 	return jkeyevent_symbol_t::Red;
		// case ALLEGRO_KEY_GREEN:
		// 	return jkeyevent_symbol_t::Green;
		// case ALLEGRO_KEY_YELLOW:
		// 	return jkeyevent_symbol_t::Yellow;
		// case ALLEGRO_KEY_BLUE:
		// 	return jkeyevent_symbol_t::Blue;
		case 16777264:
		 	return jkeyevent_symbol_t::F1;
		case 16777265:
		 	return jkeyevent_symbol_t::F2;
		case 16777266:
			return jkeyevent_symbol_t::F3;
		case 16777267:
			return jkeyevent_symbol_t::F4;
		case 16777268:
			return jkeyevent_symbol_t::F5;
		case 16777269:
			return jkeyevent_symbol_t::F6;
		case 16777270:
		 	return jkeyevent_symbol_t::F7;
		case 16777271:
			return jkeyevent_symbol_t::F8;
		case 16777272:
			return jkeyevent_symbol_t::F9;
		case 16777273:
		 	return jkeyevent_symbol_t::F10;
		case 16777274:
			return jkeyevent_symbol_t::F11;
		case 16777275:
		 	return jkeyevent_symbol_t::F12;
    case 16777248:
		 	return jkeyevent_symbol_t::Shift;
		case 16777249:
		 	return jkeyevent_symbol_t::Control;
		case 16777251:
		 	return jkeyevent_symbol_t::Alt;
		// case ALLEGRO_KEY_ALTGR:
		//	return jkeyevent_symbol_t::AltGr;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return jkeyevent_symbol_t::Meta;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return jkeyevent_symbol_t::Super;
		// case ALLEGRO_KEY_HYPER:
		// 	return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

class QTWindowRender : public QDialog {

  protected:
    virtual bool event(QEvent *event) override
    {
      if (event->type() == QEvent::ActivationChange) {
          // printf("ActivationEvent\n");
      } else if (event->type() == QEvent::ApplicationStateChange) {
          printf("ApplicationStateEvent\n");;
      } else if (event->type() == QEvent::Close) {
          printf("CloseEvent\n");
          
          sg_quitting = true;
      } else if (event->type() == QEvent::Enter) {
          printf("EnterEvent\n");
      } else if (event->type() == QEvent::Leave) {
          printf("LeaveEvent\n");
      // } else if (event->type() == QEvent::Expose) {
      // } else if (event->type() == QEvent::UpdateRequest) {
      // } else if (event->type() == QEvent::Paint) {
      } else if (event->type() == QEvent::Move) {
          printf("MoveEvent\n");
      } else if (event->type() == QEvent::Resize) {
          printf("ResizeEvent\n");
      } else if (event->type() == QEvent::Show) {
          printf("ShowEvent\n");
      } else if (event->type() == QEvent::Hide) {
          printf("HideEvent\n");
      } else if (event->type() == QEvent::FocusIn) {
          printf("FocusInEvent\n");
      } else if (event->type() == QEvent::FocusOut) {
          printf("FocusOutEvent\n");
      } else if (
          event->type() == QEvent::KeyPress or
          event->type() == QEvent::KeyRelease) {
        QKeyEvent *e = dynamic_cast<QKeyEvent *>(event);

        /*
        std::string info;

        if (event->type() == QEvent::KeyPress) {
          info = "KeyPress";
        } else if (event->type() == QEvent::KeyRelease) {
          info = "KeyRelease";
        }

        printf("%s:: count:[%d], key:[%d], modifiers:[%x]\n", 
            info.c_str(), e->count(), e->key(), e->modifiers()); 
        */

        jkeyevent_type_t type = jkeyevent_type_t::Unknown;
        jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

        if (e->modifiers() & Qt::ShiftModifier) {
          mod = jenum_t{mod}.Or(jkeyevent_modifiers_t::Shift);
        } else if (e->modifiers() & Qt::ControlModifier) {
          mod = jenum_t{mod}.Or(jkeyevent_modifiers_t::Control);
        } else if (e->modifiers() & Qt::AltModifier) {
          mod = jenum_t{mod}.Or(jkeyevent_modifiers_t::Alt);
        } else if (e->modifiers() & Qt::MetaModifier) {
          mod = jenum_t{mod}.Or(jkeyevent_modifiers_t::Super);
        }

        if (event->type() == QEvent::KeyPress) {
          type = jkeyevent_type_t::Pressed;
        } else if (event->type() == QEvent::KeyRelease) {
          type = jkeyevent_type_t::Released;
        }

        jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->key());

        sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (
          event->type() == QEvent::MouseButtonDblClick or
          event->type() == QEvent::MouseButtonPress or
          event->type() == QEvent::MouseButtonRelease or
          event->type() == QEvent::MouseMove) {
        QMouseEvent *e = dynamic_cast<QMouseEvent *>(event);

        /*
        std::string info;

        if (event->type() == QEvent::MouseButtonDblClick) {
          info = "MouseDoubleClick";
        } else if (event->type() == QEvent::MouseButtonPress) {
          info = "MousePress";
        } else if (event->type() == QEvent::MouseButtonRelease) {
          info = "MouseRelease";
        } else if (event->type() == QEvent::MouseMove) {
          info = "MouseMove";
        } else if (event->type() == QEvent::Wheel) {
          info = "MouseWheel";
        }

        printf("%s:: button:[%x], buttons:[%x], global:[%d, %d], local:[%d, %d]\n", 
            info.c_str(), e->button(), e->buttons(), e->globalX(), e->globalY(), e->x(), e->y()); 
        */

        static jmouseevent_button_t buttons = jmouseevent_button_t::None;

        jmouseevent_button_t button = jmouseevent_button_t::None;
        jmouseevent_type_t type = jmouseevent_type_t::Moved;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        if (e->button() == Qt::LeftButton) {
          button = jmouseevent_button_t::Button1;
        } else if (e->button() == Qt::MidButton) {
          button = jmouseevent_button_t::Button2;
        } else if (e->button() == Qt::RightButton) {
          button = jmouseevent_button_t::Button3;
        }

        if (event->type() == QEvent::MouseButtonDblClick) {
          type = jmouseevent_type_t::Pressed;
          buttons = jenum_t{buttons}.Or(button);
        } else if (event->type() == QEvent::MouseButtonPress) {
          type = jmouseevent_type_t::Pressed;
          buttons = jenum_t{buttons}.And(jenum_t{button}.Not());
        } else if (event->type() == QEvent::MouseButtonRelease) {
          type = jmouseevent_type_t::Released;
          buttons = jenum_t{buttons}.And(jenum_t{button}.Not());
        }

        if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jmouseevent_button_t::None) {
          sg_handler->grabMouse();
        } else {
          sg_handler->releaseMouse();
        }

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {mouse_x, mouse_y}, mouse_z));
      } else if (event->type() == QEvent::Wheel) {
        QWheelEvent *e = dynamic_cast<QWheelEvent *>(event);

        /*
        printf("MouseWheel:: buttons:[%x], global:[%d, %d], local:[%d, %d]\n", 
            e->buttons(), e->globalX(), e->globalY(), e->x(), e->y()); 
        */

        jmouseevent_button_t button = jmouseevent_button_t::None;
        jmouseevent_type_t type = jmouseevent_type_t::Rotated;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        QPoint degrees = e->angleDelta();

        if (degrees.y() > 0) {
          mouse_z = 1;
        } else {
          mouse_z = -1;
        }

        sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {mouse_x, mouse_y}, mouse_z));
      } else if (event->type() == QEvent::WindowActivate) {
          // printf("WindowActivateEvent\n");
      } else if (event->type() == QEvent::WindowDeactivate) {
          // printf("WindowDeactivateEvent\n");
      } else if (event->type() == QEvent::WindowStateChange) {
          printf("WindowStateEvent\n");
      } else {
          return QDialog::event(event);
      }

      return true;
    }

  public:
    explicit QTWindowRender(): 
      QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint)
    {
      setWindowFlags(Qt::Window);
    }

    virtual ~QTWindowRender() 
    {
    }

    virtual void paintEvent(QPaintEvent* event)
    {
      // QDialog::paintEvent(event);

      QPainter painter(this);

      if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
        return;
      }

      // WindowAdapter 
      //   *handler = reinterpret_cast<WindowAdapter *>(user_data);
      jrect_t<int> 
        bounds = sg_jcanvas_window->GetBounds();

      if (sg_back_buffer != nullptr) {
        jpoint_t<int>
          size = sg_back_buffer->GetSize();

        if (size.x != bounds.size.x or size.y != bounds.size.y) {
          delete sg_back_buffer;
          sg_back_buffer = nullptr;
        }
      }

      if (sg_back_buffer == nullptr) {
        sg_back_buffer = new BufferedImage(jpixelformat_t::RGB32, bounds.size);
      }

      Graphics 
        *g = sg_back_buffer->GetGraphics();

      g->Reset();
      g->SetCompositeFlags(jcomposite_t::Src);

      sg_jcanvas_window->Paint(g);

      g->Flush();

      Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());
        
      uint8_t *data = sg_back_buffer->LockData();

      QImage image(data, bounds.size.x, bounds.size.y, bounds.size.x*4, QImage::Format_RGB32);
      QPixmap pixmap = QPixmap::fromImage(image);

      // painter.beginNativePainting();
      painter.drawPixmap(0, 0, pixmap);
      // painter.endNativePainting();

      sg_back_buffer->UnlockData();

      sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
    }

};

void Application::Init(int argc, char **argv)
{
  static int argc0 = 1;
  static char *argv0[2] = {
    (char *)"app", nullptr
  };

  sg_application = new QApplication(argc0, argv0);

  QList<QScreen *> screens = QGuiApplication::screens();
  QRect geometry = screens.front()->geometry();

	sg_screen.x = geometry.x();
	sg_screen.y = geometry.y();
  
  sg_quitting = false;
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  sg_handler->show();
  // sg_handler->activateWindow();

  do {
    if (sg_repaint.exchange(false) == true) {
      sg_handler->repaint();
    }

    sg_application->processEvents();

    std::this_thread::yield();
  } while (sg_quitting == false);

  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
{
	if (sg_jcanvas_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  sg_jcanvas_window = parent;

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_handler = new QTWindowRender();

  sg_handler->setVisible(true);
  sg_handler->setMouseTracking(true);
  sg_handler->setGeometry(bounds.point.x, bounds.point.y, bounds.size.x, bounds.size.y);

  sg_visible = true;
}

WindowAdapter::~WindowAdapter()
{
  QCoreApplication::quit();
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
    const QString 
      session = QString(getenv("DESKTOP_SESSION")).toLower();

	if (sg_fullscreen == false) {
    sg_fullscreen = true;
    
    sg_geometry = sg_handler->saveGeometry();

    if (session == "ubuntu") {
      sg_handler->setFixedSize({sg_screen.x, sg_screen.y});
      sg_handler->setWindowFlags(Qt::FramelessWindowHint);
      sg_handler->setWindowState(sg_handler->windowState() | Qt::WindowFullScreen);
      sg_handler->show();
      sg_handler->activateWindow();
    } else {
      sg_handler->showFullScreen();
    }
	} else {
    sg_fullscreen = false;
    
    if (session == "ubuntu") {
        sg_handler->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        sg_handler->setMinimumSize(0, 0);
        sg_handler->restoreGeometry(sg_geometry);
        sg_handler->setWindowFlags(Qt::Dialog);
        sg_handler->show();
        sg_handler->activateWindow();
    } else {
        sg_handler->showNormal();
    }
	}
}

void WindowAdapter::SetTitle(std::string title)
{
	sg_handler->setWindowTitle(title.c_str());
}

std::string WindowAdapter::GetTitle()
{
	return sg_handler->windowTitle().toStdString();
}

void WindowAdapter::SetOpacity(float opacity)
{
	sg_opacity = opacity;
}

float WindowAdapter::GetOpacity()
{
  return sg_opacity;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
  sg_undecorated = undecorated;

  if (undecorated == true) {
    sg_handler->setWindowFlags(Qt::CustomizeWindowHint);
    sg_handler->setWindowFlags(Qt::FramelessWindowHint);
  } else {
    sg_handler->setWindowFlags(Qt::WindowTitleHint);
    sg_handler->setWindowFlags(Qt::WindowMinimizeButtonHint);
    sg_handler->setWindowFlags(Qt::WindowMaximizeButtonHint);
    sg_handler->setWindowFlags(Qt::WindowCloseButtonHint);
  }
}

bool WindowAdapter::IsUndecorated()
{
  return sg_undecorated;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
  sg_handler->resize(bounds.size.x, bounds.size.y);
  sg_handler->move(bounds.point.x, bounds.point.y);
}

jrect_t<int> WindowAdapter::GetBounds()
{
  QSize 
    size = sg_handler->size();
  QPoint
    location = sg_handler->pos();

  return {
    location.x(),
    location.y(),
    size.width(),
    size.height()
  };
}

void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return false;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > sg_screen.x) {
		x = sg_screen.x;
	}

	if (y > sg_screen.y) {
		y = sg_screen.y;
	}

  QCursor cursor = sg_handler->cursor();

  cursor.setPos(QPoint(x, y));
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

  QCursor 
    cursor = sg_handler->cursor();
  QPoint
    location = cursor.pos();

  t.x = location.x();
  t.y = location.y();

	return t;
}

void WindowAdapter::SetVisible(bool visible)
{
  if (sg_visible == visible) {
    return;
  }

  sg_visible = visible;

  // sg_handler->setVisible(sg_visible);

  if (sg_visible == false) {
    sg_handler->hide();
    // sg_handler->deactivateWindow();
  } else {
    sg_handler->show();
    sg_handler->activateWindow();
  }
}

bool WindowAdapter::IsVisible()
{
  return sg_visible;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  QCursor cursor = sg_handler->cursor();

  if (enabled == false) {
    cursor.setShape(Qt::BlankCursor);
  } else {
    SetCursor(sg_jcanvas_cursor);
  }
}

bool WindowAdapter::IsCursorEnabled()
{
  return sg_handler->cursor().shape() != Qt::BlankCursor;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  Qt::CursorShape type = Qt::ArrowCursor;

  if (style == jcursor_style_t::Default) {
    type = Qt::ArrowCursor;
  } else if (style == jcursor_style_t::Crosshair) {
    type = Qt::CrossCursor;
  } else if (style == jcursor_style_t::East) {
  } else if (style == jcursor_style_t::West) {
  } else if (style == jcursor_style_t::North) {
    type = Qt::UpArrowCursor;
  } else if (style == jcursor_style_t::South) {
  } else if (style == jcursor_style_t::Hand) {
    type = Qt::OpenHandCursor;
  } else if (style == jcursor_style_t::Move) {
    type = Qt::SizeAllCursor;
  } else if (style == jcursor_style_t::Vertical) {
    type = Qt::SizeVerCursor;
  } else if (style == jcursor_style_t::Horizontal) {
    type = Qt::SizeHorCursor;
  } else if (style == jcursor_style_t::NorthWest) {
  } else if (style == jcursor_style_t::NorthEast) {
  } else if (style == jcursor_style_t::SouthWest) {
  } else if (style == jcursor_style_t::SouthEast) {
  } else if (style == jcursor_style_t::Text) {
    type = Qt::IBeamCursor;
  } else if (style == jcursor_style_t::Wait) {
    type = Qt::BusyCursor;
  }

  QCursor cursor;

  cursor.setShape(type);

  sg_handler->setCursor(cursor);

  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{

	if ((void *)shape == nullptr) {
		return;
	}

	jpoint_t<int> t = shape->GetSize();
	uint32_t data[t.x*t.y];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	if (data == nullptr) {
		return;
	}
  
  QImage image((uint8_t *)data, t.x, t.y, t.x*4, QImage::Format_RGB32);
  QPixmap pixmap = QPixmap::fromImage(image);
  QCursor cursor_default = QCursor(pixmap, hotx, hoty);

  sg_handler->setCursor(cursor_default);
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jwindow_rotation_t::None;
}

void WindowAdapter::SetIcon(Image *image)
{
  sg_jcanvas_icon = image;
}

Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
