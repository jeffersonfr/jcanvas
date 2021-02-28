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
jcanvas::Image *sg_back_buffer = nullptr;
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
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 16777220:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case 16777219:
			return jcanvas::JKS_BACKSPACE;
		case 16777217:
			return jcanvas::JKS_TAB;
		// case ALLEGRO_KEY_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case 16777216:
			return jcanvas::JKS_ESCAPE;
		case 32:
			return jcanvas::JKS_SPACE;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return jcanvas::JKS_EXCLAMATION_MARK;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return jcanvas::JKS_QUOTATION;
		// case ALLEGRO_KEY_HASH:
		// 	return jcanvas::JKS_NUMBER_SIGN;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return jcanvas::JKS_DOLLAR_SIGN;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return jcanvas::JKS_PERCENT_SIGN;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return jcanvas::JKS_AMPERSAND;
		case 39:
			return jcanvas::JKS_APOSTROPHE;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return jcanvas::JKS_PARENTHESIS_LEFT;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return jcanvas::JKS_PARENTHESIS_RIGHT;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return jcanvas::JKS_STAR;
		// case ALLEGRO_KEY_PLUS:
		// 	return jcanvas::JKS_PLUS_SIGN;
		case 44:   
			return jcanvas::JKS_COMMA;
		case 45:
			return jcanvas::JKS_MINUS_SIGN;
		case 46:  
		 	return jcanvas::JKS_PERIOD;
		case 47:
			return jcanvas::JKS_SLASH;
		case 48:     
			return jcanvas::JKS_0;
		case 49:
			return jcanvas::JKS_1;
		case 50:
			return jcanvas::JKS_2;
		case 51:
			return jcanvas::JKS_3;
		case 52:
			return jcanvas::JKS_4;
		case 53:
			return jcanvas::JKS_5;
		case 54:
			return jcanvas::JKS_6;
		case 55:
			return jcanvas::JKS_7;
		case 56:
			return jcanvas::JKS_8;
		case 57:
			return jcanvas::JKS_9;
		// case ALLEGRO_KEY_COLON:
		// 	return jcanvas::JKS_COLON;
		case 59:
			return jcanvas::JKS_SEMICOLON;
		// case ALLEGRO_KEY_LESS:
		// 	return jcanvas::JKS_LESS_THAN_SIGN;
		case 61: 
			return jcanvas::JKS_EQUALS_SIGN;
		// case ALLEGRO_KEY_GREATER:
		// 	return jcanvas::JKS_GREATER_THAN_SIGN;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return jcanvas::JKS_QUESTION_MARK;
		// case ALLEGRO_KEY_AT:
		//	return jcanvas::JKS_AT;
		case 65:
			return jcanvas::JKS_a;
		case 66:
			return jcanvas::JKS_b;
		case 67:
			return jcanvas::JKS_c;
		case 68:
			return jcanvas::JKS_d;
		case 69:
			return jcanvas::JKS_e;
		case 70:
			return jcanvas::JKS_f;
		case 71:
			return jcanvas::JKS_g;
		case 72:
			return jcanvas::JKS_h;
		case 73:
			return jcanvas::JKS_i;
		case 74:
			return jcanvas::JKS_j;
		case 75:
			return jcanvas::JKS_k;
		case 76:
			return jcanvas::JKS_l;
		case 77:
			return jcanvas::JKS_m;
		case 78:
			return jcanvas::JKS_n;
		case 79:
			return jcanvas::JKS_o;
		case 80:
			return jcanvas::JKS_p;
		case 81:
			return jcanvas::JKS_q;
		case 82:
			return jcanvas::JKS_r;
		case 83:
			return jcanvas::JKS_s;
		case 84:
			return jcanvas::JKS_t;
		case 85:
			return jcanvas::JKS_u;
		case 86:
			return jcanvas::JKS_v;
		case 87:
			return jcanvas::JKS_w;
		case 88:
			return jcanvas::JKS_x;
		case 89:
			return jcanvas::JKS_y;
		case 90:
			return jcanvas::JKS_z;
		case 91:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case 92:   
			return jcanvas::JKS_BACKSLASH;
		case 93:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		// case ALLEGRO_KEY_CARET:
		// 	return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return jcanvas::JKS_UNDERSCORE;
		// case ALLEGRO_KEY_BACKQUOTE:
		//	return jcanvas::JKS_GRAVE_ACCENT;
		// case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
		//	return jcanvas::JKS_CURLY_BRACKET_LEFT;
		// case ALLEGRO_KEY_VERTICAL_BAR:  
		// 	return jcanvas::JKS_VERTICAL_BAR;
		// case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
		// 	return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		// case ALLEGRO_KEY_TILDE:  
		//	return jcanvas::JKS_TILDE;
		case 16777223:
			return jcanvas::JKS_DELETE;
		case 16777234:
			return jcanvas::JKS_CURSOR_LEFT;
		case 16777236:
			return jcanvas::JKS_CURSOR_RIGHT;
		case 16777235:  
			return jcanvas::JKS_CURSOR_UP;
		case 16777237:
			return jcanvas::JKS_CURSOR_DOWN;
		case 16777222:  
			return jcanvas::JKS_INSERT;
		case 16777232:     
			return jcanvas::JKS_HOME;
		case 16777233:
			return jcanvas::JKS_END;
		case 16777238:
			return jcanvas::JKS_PAGE_UP;
		case 16777239:
			return jcanvas::JKS_PAGE_DOWN;
		// case ALLEGRO_KEY_PRINTSCREEN:   
		//	return jcanvas::JKS_PRINT;
		case 16777224:
			return jcanvas::JKS_PAUSE;
		// case ALLEGRO_KEY_RED:
		// 	return jcanvas::JKS_RED;
		// case ALLEGRO_KEY_GREEN:
		// 	return jcanvas::JKS_GREEN;
		// case ALLEGRO_KEY_YELLOW:
		// 	return jcanvas::JKS_YELLOW;
		// case ALLEGRO_KEY_BLUE:
		// 	return jcanvas::JKS_BLUE;
		case 16777264:
		 	return jcanvas::JKS_F1;
		case 16777265:
		 	return jcanvas::JKS_F2;
		case 16777266:
			return jcanvas::JKS_F3;
		case 16777267:
			return jcanvas::JKS_F4;
		case 16777268:
			return jcanvas::JKS_F5;
		case 16777269:
			return jcanvas::JKS_F6;
		case 16777270:
		 	return jcanvas::JKS_F7;
		case 16777271:
			return jcanvas::JKS_F8;
		case 16777272:
			return jcanvas::JKS_F9;
		case 16777273:
		 	return jcanvas::JKS_F10;
		case 16777274:
			return jcanvas::JKS_F11;
		case 16777275:
		 	return jcanvas::JKS_F12;
    case 16777248:
		 	return jcanvas::JKS_SHIFT;
		case 16777249:
		 	return jcanvas::JKS_CONTROL;
		case 16777251:
		 	return jcanvas::JKS_ALT;
		// case ALLEGRO_KEY_ALTGR:
		//	return jcanvas::JKS_ALTGR;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return jcanvas::JKS_META;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return jcanvas::JKS_SUPER;
		// case ALLEGRO_KEY_HYPER:
		// 	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
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

        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = jcanvas::JKM_NONE;

        if (e->modifiers() & Qt::ShiftModifier) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if (e->modifiers() & Qt::ControlModifier) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if (e->modifiers() & Qt::AltModifier) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if (e->modifiers() & Qt::MetaModifier) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
        }

        type = (jcanvas::jkeyevent_type_t)(0);

        if (event->type() == QEvent::KeyPress) {
          type = jcanvas::JKT_PRESSED;
        } else if (event->type() == QEvent::KeyRelease) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->key());

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
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

        static jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_MOVED;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        if (e->button() == Qt::LeftButton) {
          button = jcanvas::JMB_BUTTON1;
        } else if (e->button() == Qt::MidButton) {
          button = jcanvas::JMB_BUTTON2;
        } else if (e->button() == Qt::RightButton) {
          button = jcanvas::JMB_BUTTON3;
        }

        if (event->type() == QEvent::MouseButtonDblClick) {
          type = jcanvas::JMT_PRESSED;
          buttons = (jcanvas::jmouseevent_button_t)(buttons | button);
        } else if (event->type() == QEvent::MouseButtonPress) {
          type = jcanvas::JMT_PRESSED;
          buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
        } else if (event->type() == QEvent::MouseButtonRelease) {
          type = jcanvas::JMT_RELEASED;
          buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
        }

        if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
          sg_handler->grabMouse();
        } else {
          sg_handler->releaseMouse();
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {mouse_x, mouse_y}, mouse_z));
      } else if (event->type() == QEvent::Wheel) {
        QWheelEvent *e = dynamic_cast<QWheelEvent *>(event);

        /*
        printf("MouseWheel:: buttons:[%x], global:[%d, %d], local:[%d, %d]\n", 
            e->buttons(), e->globalX(), e->globalY(), e->x(), e->y()); 
        */

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_ROTATED;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        QPoint degrees = e->angleDelta();

        if (degrees.y() > 0) {
          mouse_z = 1;
        } else {
          mouse_z = -1;
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {mouse_x, mouse_y}, mouse_z));
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
        jcanvas::jpoint_t<int>
          size = sg_back_buffer->GetSize();

        if (size.x != bounds.size.x or size.y != bounds.size.y) {
          delete sg_back_buffer;
          sg_back_buffer = nullptr;
        }
      }

      if (sg_back_buffer == nullptr) {
        sg_back_buffer = new jcanvas::BufferedImage(jcanvas::JPF_RGB32, bounds.size);
      }

      jcanvas::Graphics 
        *g = sg_back_buffer->GetGraphics();

      Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());
        
      g->Reset();
      g->SetCompositeFlags(jcanvas::JCF_SRC);

      sg_jcanvas_window->Paint(g);

      g->Flush();

      uint8_t *data = sg_back_buffer->LockData();

      QImage image(data, bounds.size.x, bounds.size.y, bounds.size.x*4, QImage::Format_RGB32);
      QPixmap pixmap = QPixmap::fromImage(image);

      // painter.beginNativePainting();
      painter.drawPixmap(0, 0, pixmap);
      // painter.endNativePainting();

      sg_back_buffer->UnlockData();

      sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
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

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
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

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
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

  if (style == JCS_DEFAULT) {
    type = Qt::ArrowCursor;
  } else if (style == JCS_CROSSHAIR) {
    type = Qt::CrossCursor;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
    type = Qt::UpArrowCursor;
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = Qt::OpenHandCursor;
  } else if (style == JCS_MOVE) {
    type = Qt::SizeAllCursor;
  } else if (style == JCS_NS) {
    type = Qt::SizeVerCursor;
  } else if (style == JCS_WE) {
    type = Qt::SizeHorCursor;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = Qt::IBeamCursor;
  } else if (style == JCS_WAIT) {
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
	return jcanvas::JWR_NONE;
}

void WindowAdapter::SetIcon(jcanvas::Image *image)
{
  sg_jcanvas_icon = image;
}

jcanvas::Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
