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
#include "../include/nativewindow.h"

#include "jcanvas/core/jbufferedimage.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <X11/Xlib.h>

namespace jcanvas {

/** \brief */
static jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_window = 0;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static jcanvas::jrect_t<int> sg_previous_bounds;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case '\r':
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		//case SDLK_BACKSPACE:
		//	return jcanvas::JKS_BACKSPACE;
		case '\t':
			return jcanvas::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case 27:
			return jcanvas::JKS_ESCAPE;
    case ' ':
			return jcanvas::JKS_SPACE;
		case '!':
			return jcanvas::JKS_EXCLAMATION_MARK;
		case '"':
			return jcanvas::JKS_QUOTATION;
		case '#':
			return jcanvas::JKS_NUMBER_SIGN;
		case '$':
			return jcanvas::JKS_DOLLAR_SIGN;
		case '%':
			return jcanvas::JKS_PERCENT_SIGN;
		case '&':   
			return jcanvas::JKS_AMPERSAND;
		case '\'':
			return jcanvas::JKS_APOSTROPHE;
		case '(':
			return jcanvas::JKS_PARENTHESIS_LEFT;
		case ')':
			return jcanvas::JKS_PARENTHESIS_RIGHT;
		case '*':
			return jcanvas::JKS_STAR;
		case '+':
			return jcanvas::JKS_PLUS_SIGN;
		case ',':   
			return jcanvas::JKS_COMMA;
		case '-':
			return jcanvas::JKS_MINUS_SIGN;
		case '.':  
			return jcanvas::JKS_PERIOD;
		case '/':
			return jcanvas::JKS_SLASH;
		case '0':     
			return jcanvas::JKS_0;
		case '1':
			return jcanvas::JKS_1;
		case '2':
			return jcanvas::JKS_2;
		case '3':
			return jcanvas::JKS_3;
		case '4':
			return jcanvas::JKS_4;
		case '5':
			return jcanvas::JKS_5;
		case '6':
			return jcanvas::JKS_6;
		case '7':
			return jcanvas::JKS_7;
		case '8':
			return jcanvas::JKS_8;
		case '9':
			return jcanvas::JKS_9;
    case ':':
			return jcanvas::JKS_COLON;
		case ';':
			return jcanvas::JKS_SEMICOLON;
		case '<':
			return jcanvas::JKS_LESS_THAN_SIGN;
		case '=': 
			return jcanvas::JKS_EQUALS_SIGN;
		case '>':
			return jcanvas::JKS_GREATER_THAN_SIGN;
		case '?':   
			return jcanvas::JKS_QUESTION_MARK;
		case '@':
			return jcanvas::JKS_AT;
		case '[':
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case '\\':   
			return jcanvas::JKS_BACKSLASH;
		case ']':
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case '^':
			return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		case '_':    
			return jcanvas::JKS_UNDERSCORE;
		case '`':
			return jcanvas::JKS_GRAVE_ACCENT;
		case 'a':       
			return jcanvas::JKS_a;
		case 'b':
			return jcanvas::JKS_b;
		case 'c':
			return jcanvas::JKS_c;
		case 'd':
			return jcanvas::JKS_d;
		case 'e':
			return jcanvas::JKS_e;
		case 'f':
			return jcanvas::JKS_f;
		case 'g':
			return jcanvas::JKS_g;
		case 'h':
			return jcanvas::JKS_h;
		case 'i':
			return jcanvas::JKS_i;
		case 'j':
			return jcanvas::JKS_j;
		case 'k':
			return jcanvas::JKS_k;
		case 'l':
			return jcanvas::JKS_l;
		case 'm':
			return jcanvas::JKS_m;
		case 'n':
			return jcanvas::JKS_n;
		case 'o':
			return jcanvas::JKS_o;
		case 'p':
			return jcanvas::JKS_p;
		case 'q':
			return jcanvas::JKS_q;
		case 'r':
			return jcanvas::JKS_r;
		case 's':
			return jcanvas::JKS_s;
		case 't':
			return jcanvas::JKS_t;
		case 'u':
			return jcanvas::JKS_u;
		case 'v':
			return jcanvas::JKS_v;
		case 'w':
			return jcanvas::JKS_w;
		case 'x':
			return jcanvas::JKS_x;
		case 'y':
			return jcanvas::JKS_y;
		case 'z':
			return jcanvas::JKS_z;
		case '{':
			return jcanvas::JKS_CURLY_BRACKET_LEFT;
		case '|':  
			return jcanvas::JKS_VERTICAL_BAR;
		case '}':
			return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case '~':  
			return jcanvas::JKS_TILDE;
		//case SDLK_DELETE:
		//	return jcanvas::JKS_DELETE;
		// case SDLK_PRINT:   
		//	return jcanvas::JKS_PRINT;
		//case SDLK_PAUSE:
		//	return jcanvas::JKS_PAUSE;
		// case SDLK_RED:
		//	return jcanvas::JKS_RED;
		// case SDLK_GREEN:
		//	return jcanvas::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jcanvas::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jcanvas::JKS_BLUE;
		//case SDLK_LSHIFT:
		//case SDLK_RSHIFT:
		//	return jcanvas::JKS_SHIFT;
		//case SDLK_LCTRL:
		//case SDLK_RCTRL:
		//	return jcanvas::JKS_CONTROL;
		//case SDLK_LALT:
		//case SDLK_RALT:
		//	return jcanvas::JKS_ALT;
		// case SDLK_ALTGR:
		//	return jcanvas::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jcanvas::JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jcanvas::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbolSpecial(int symbol)
{
	switch (symbol) {
		case GLUT_KEY_LEFT:
			return jcanvas::JKS_CURSOR_LEFT;
		case GLUT_KEY_RIGHT:
			return jcanvas::JKS_CURSOR_RIGHT;
		case GLUT_KEY_UP:  
			return jcanvas::JKS_CURSOR_UP;
		case GLUT_KEY_DOWN:
			return jcanvas::JKS_CURSOR_DOWN;
		case GLUT_KEY_INSERT:  
	  	return jcanvas::JKS_INSERT;
	  case GLUT_KEY_HOME:     
			return jcanvas::JKS_HOME;
		case GLUT_KEY_END:
			return jcanvas::JKS_END;
		case GLUT_KEY_PAGE_UP:
			return jcanvas::JKS_PAGE_UP;
		case GLUT_KEY_PAGE_DOWN:
			return jcanvas::JKS_PAGE_DOWN;
		case GLUT_KEY_F1:
			return jcanvas::JKS_F1;
		case GLUT_KEY_F2:
			return jcanvas::JKS_F2;
		case GLUT_KEY_F3:
			return jcanvas::JKS_F3;
		case GLUT_KEY_F4:
			return jcanvas::JKS_F4;
		case GLUT_KEY_F5:
			return jcanvas::JKS_F5;
    case GLUT_KEY_F6:
			return jcanvas::JKS_F6;
		case GLUT_KEY_F7:
			return jcanvas::JKS_F7;
		case GLUT_KEY_F8:
			return jcanvas::JKS_F8;
		case GLUT_KEY_F9:
			return jcanvas::JKS_F9;
		case GLUT_KEY_F10: 
			return jcanvas::JKS_F10;
		case GLUT_KEY_F11:
			return jcanvas::JKS_F11;
		case GLUT_KEY_F12:
			return jcanvas::JKS_F12;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
	XInitThreads();

  glutInit(&argc, argv);
  // glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_MULTISAMPLE);

	sg_screen.x = glutGet(GLUT_SCREEN_WIDTH);
	sg_screen.y = glutGet(GLUT_SCREEN_HEIGHT);
  
  sg_quitting = false;
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  glutMainLoop();
  // glutMainLoopEvent(); // single loop event

  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  glutLeaveMainLoop();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

static void OnDraw()
{
	if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
		return;
	}

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

  g->Reset();
  g->SetCompositeFlags(jcanvas::JCF_SRC);

  sg_jcanvas_window->Paint(g);

  g->Flush();

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  glTexSubImage2D(
      GL_TEXTURE_2D, 0 ,0, 0, bounds.size.x, bounds.size.y, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)data);

  sg_back_buffer->UnlockData();

  glBegin( GL_QUADS );
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  glTexCoord2d(1.0, 0.0); glVertex2d(bounds.size.x, 0.0);
  glTexCoord2d(1.0, 1.0); glVertex2d(bounds.size.x, bounds.size.y);
  glTexCoord2d(0.0, 1.0); glVertex2d(0.0, bounds.size.y);
  glEnd();

  glutSwapBuffers();

  if (g->IsVerticalSyncEnabled() == false) {
    glFlush();
  } else {
    glFinish();
  }
  
  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

void OnShape(int w, int h)
{
  glTexImage2D(
      GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)nullptr);
  
  glViewport(0, 0, (GLsizei) w, (GLsizei) -h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f, (GLdouble)w, (GLdouble)h, 0.0f);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, w, h);
}

std::map<jcanvas::jmouseevent_button_t, bool> sg_mouse_button_state;

void OnMousePress(int button_id, int state, int x, int y)
{
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
  int mouse_z = 0;

  sg_mouse_x = x;
  sg_mouse_y = y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x-1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y-1);

  if (state == GLUT_DOWN) {
    type = jcanvas::JMT_PRESSED;
  } else if (state == GLUT_UP) {
    type = jcanvas::JMT_RELEASED;
  }

  if (button_id == GLUT_LEFT_BUTTON) {
    button = jcanvas::JMB_BUTTON1;
  } else if (button_id == GLUT_MIDDLE_BUTTON) {
    button = jcanvas::JMB_BUTTON2;
  } else if (button_id == GLUT_RIGHT_BUTTON) {
    button = jcanvas::JMB_BUTTON3;
  }

  if (type == jcanvas::JMT_PRESSED) {
    sg_mouse_button_state[button] = true;
  } else {
    sg_mouse_button_state[button] = false;
  }

  // TODO:: mouse grab
  
  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

void OnMouseMove(int x, int y)
{
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
  jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
  int mouse_z = 0;

  sg_mouse_x = x;
  sg_mouse_y = y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x-1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y-1);

  type = jcanvas::JMT_MOVED;

  // TODO:: mouse grab
  
  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

void OnKeyPressRelease(unsigned char key, int x, int y, bool released)
{
  jcanvas::jkeyevent_type_t type;
  jcanvas::jkeyevent_modifiers_t mod;

  mod = (jcanvas::jkeyevent_modifiers_t)(0);

  int m = glutGetModifiers();

  if (m == GLUT_ACTIVE_SHIFT) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
  } else if (m == GLUT_ACTIVE_CTRL) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
  } else if (GLUT_ACTIVE_ALT) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
  // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_SUPER);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_HYPER);
  }

  type = jcanvas::JKT_UNKNOWN;
  
  if (released == false) {
    type = jcanvas::JKT_PRESSED;
  } else {
    type = jcanvas::JKT_RELEASED;
  }

  jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(key);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

void OnKeyPress(unsigned char key, int x, int y)
{
  OnKeyPressRelease(key, x, y, false);
}

void OnKeyRelease(unsigned char key, int x, int y)
{
  OnKeyPressRelease(key, x, y, true);
}

void OnKeyPressReleaseSpecial(int key, int x, int y, bool released)
{
  jcanvas::jkeyevent_type_t type;
  jcanvas::jkeyevent_modifiers_t mod;

  mod = (jcanvas::jkeyevent_modifiers_t)(0);

  int m = glutGetModifiers();

  if (m & GLUT_ACTIVE_SHIFT) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
  } else if (m & GLUT_ACTIVE_CTRL) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
  } else if (m & GLUT_ACTIVE_ALT) {
    mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
  // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_SUPER);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_HYPER);
  }

  type = jcanvas::JKT_UNKNOWN;
  
  if (released == false) {
    type = jcanvas::JKT_PRESSED;
  } else {
    type = jcanvas::JKT_RELEASED;
  }

  jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbolSpecial(key);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

void OnKeyPressSpecial(int key, int x, int y)
{
  OnKeyPressReleaseSpecial(key, x, y, false);
}

void OnKeyReleaseSpecial(int key, int x, int y)
{
  OnKeyPressReleaseSpecial(key, x, y, true);
}

void OnTimer(int value)
{
  if (sg_repaint.exchange(false) == true) {
    glutPostRedisplay();
  }

  /*
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_WINDOWEVENT) {
      } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_OPENED));
      } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
      } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
        InternalPaint();
      } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_MOVED));
      } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        InternalPaint();

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_RESIZED));
      } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
      }
    } else if(event.type == SDL_QUIT) {
      SDL_HideWindow(sg_window);

      sg_quitting = true;

      sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
    }
  }
  */

  if (sg_quitting == false) {
    glutTimerFunc(1, OnTimer, value);
  }

  std::this_thread::yield();
}

void OnVisibility(int state)
{
  if (state == GLUT_NOT_VISIBLE) {
    sg_visible = false;
  } else if (state == GLUT_VISIBLE) {
    sg_visible = true;
  }
}

void OnEntry(int state)
{
  if (state == GLUT_ENTERED) {
    // SDL_CaptureMouse(true);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(GetCursor());

    sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
  } else if (GLUT_LEFT) {
    // SDL_CaptureMouse(false);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(JCS_DEFAULT);

    sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
  }
}

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
{
	if (sg_window > 0) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");
	
  sg_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  glutInitWindowSize(bounds.size.x, bounds.size.y);
  glutInitWindowPosition(bounds.point.x, bounds.point.y);
  
  sg_window = glutCreateWindow(nullptr);

  if (sg_window == 0) {
		throw std::runtime_error("Cannot create a window");
  }

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glutDisplayFunc(OnDraw);
  glutReshapeFunc(OnShape);
  glutKeyboardFunc(OnKeyPress);
  glutSpecialFunc(OnKeyPressSpecial);
  glutKeyboardUpFunc(OnKeyRelease);
  glutSpecialUpFunc(OnKeyReleaseSpecial);
  glutMouseFunc(OnMousePress);
  glutMotionFunc(OnMouseMove);
  glutPassiveMotionFunc(OnMouseMove);
  glutTimerFunc(100, OnTimer, 1);
  glutVisibilityFunc(OnVisibility);
  glutEntryFunc(OnEntry);
  glutIdleFunc(nullptr);

  glTexImage2D(
      GL_TEXTURE_2D, 0, 3, bounds.size.x, bounds.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glEnable(GL_TEXTURE_2D);
}

NativeWindow::~NativeWindow()
{
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint()
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_previous_bounds = GetBounds();

    glutFullScreen();

    sg_fullscreen = true;
  } else {
    SetBounds(sg_previous_bounds.point.x, sg_previous_bounds.point.y, sg_previous_bounds.size.x, sg_previous_bounds.size.y);
    
    sg_fullscreen = false;
  }
}

void NativeWindow::SetTitle(std::string title)
{
  sg_title = title;

  glutSetWindowTitle(title.c_str());
}

std::string NativeWindow::GetTitle()
{
  return sg_title;
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
	return 1.0f;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
	if (undecorated == true) {
	} else {
	}
}

bool NativeWindow::IsUndecorated()
{
  return false;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  glutPositionWindow(x, y);
  glutReshapeWindow(width, height);
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
  return {
    glutGet(GLUT_WINDOW_X),
    glutGet(GLUT_WINDOW_Y),
    glutGet(GLUT_WINDOW_WIDTH),
    glutGet(GLUT_WINDOW_HEIGHT)
  };
}
		
void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == false) {
    glutHideWindow();
  } else {
    glutShowWindow();
  }
}

bool NativeWindow::IsVisible()
{
	return sg_visible;
}
		
void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return true;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	return p;
}

jcursor_style_t NativeWindow::GetCursor()
{
	int id = glutGet(GLUT_WINDOW_CURSOR);

  if (id == GLUT_CURSOR_INHERIT) {
    return JCS_DEFAULT;
  } else if (id == GLUT_CURSOR_CROSSHAIR) {
    return JCS_CROSSHAIR;
  } else if (GLUT_CURSOR_RIGHT_SIDE) {
    return JCS_EAST;
  } else if (GLUT_CURSOR_LEFT_SIDE) {
    return JCS_WEST;
  } else if (GLUT_CURSOR_TOP_SIDE) {
    return JCS_NORTH;
  } else if (GLUT_CURSOR_BOTTOM_SIDE) {
    return JCS_SOUTH;
  } else if (GLUT_CURSOR_INFO) {
    return JCS_HAND;
  // } else if (GLUT_CURSOR_INHERIT) {
  //  return JCS_MOVE;
  } else if (GLUT_CURSOR_UP_DOWN) {
    return JCS_NS;
  } else if (GLUT_CURSOR_LEFT_RIGHT) {
    return JCS_WE;
  } else if (GLUT_CURSOR_TOP_LEFT_CORNER) {
    return JCS_NW_CORNER;
  } else if (GLUT_CURSOR_TOP_RIGHT_CORNER) {
    return JCS_NE_CORNER;
  } else if (GLUT_CURSOR_BOTTOM_RIGHT_CORNER) {
    return JCS_SW_CORNER;
  } else if (GLUT_CURSOR_BOTTOM_LEFT_CORNER) {
    return JCS_SE_CORNER;
  } else if (GLUT_CURSOR_TEXT) {
    return JCS_TEXT;
  } else if (GLUT_CURSOR_WAIT) {
    return JCS_WAIT;
  }

  return JCS_DEFAULT;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  if (enabled == false) {
    glutSetCursor(GLUT_CURSOR_NONE);
  } else {
    glutSetCursor(GLUT_CURSOR_INHERIT);
  }
}

bool NativeWindow::IsCursorEnabled()
{
	return glutGet(GLUT_WINDOW_CURSOR) != GLUT_CURSOR_NONE;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  int type = GLUT_CURSOR_INHERIT;

  if (style == JCS_DEFAULT) {
    type = GLUT_CURSOR_INHERIT;
  } else if (style == JCS_CROSSHAIR) {
    type = GLUT_CURSOR_CROSSHAIR;
  } else if (style == JCS_EAST) {
    type = GLUT_CURSOR_RIGHT_SIDE;
  } else if (style == JCS_WEST) {
    type = GLUT_CURSOR_LEFT_SIDE;
  } else if (style == JCS_NORTH) {
    type = GLUT_CURSOR_TOP_SIDE;
  } else if (style == JCS_SOUTH) {
    type = GLUT_CURSOR_BOTTOM_SIDE;
  } else if (style == JCS_HAND) {
    type = GLUT_CURSOR_INFO;
  } else if (style == JCS_MOVE) {
    type = GLUT_CURSOR_INHERIT;
  } else if (style == JCS_NS) {
    type = GLUT_CURSOR_UP_DOWN;
  } else if (style == JCS_WE) {
    type = GLUT_CURSOR_LEFT_RIGHT;
  } else if (style == JCS_NW_CORNER) {
    type = GLUT_CURSOR_TOP_LEFT_CORNER;
  } else if (style == JCS_NE_CORNER) {
    type = GLUT_CURSOR_TOP_RIGHT_CORNER;
  } else if (style == JCS_SW_CORNER) {
    type = GLUT_CURSOR_BOTTOM_RIGHT_CORNER;
  } else if (style == JCS_SE_CORNER) {
    type = GLUT_CURSOR_BOTTOM_LEFT_CORNER;
  } else if (style == JCS_TEXT) {
    type = GLUT_CURSOR_TEXT;
  } else if (style == JCS_WAIT) {
    type = GLUT_CURSOR_WAIT;
  }

  glutSetCursor(type);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

	jpoint_t<int> t = shape->GetSize();
	uint32_t data[t.x*t.y];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	if (data == nullptr) {
		return;
	}

	SDL_Surface *surface = nullptr;
	uint32_t rmask = 0x000000ff;
	uint32_t gmask = 0x0000ff00;
	uint32_t bmask = 0x00ff0000;
	uint32_t amask = 0xff000000;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#endif

	surface = SDL_CreateRGBSurfaceFrom(data, t.x, t.y, 32, t.x*4, rmask, gmask, bmask, amask);

	if (surface == nullptr) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
  */
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jcanvas::JWR_NONE;
}

void NativeWindow::SetIcon(jcanvas::Image *image)
{
  sg_jcanvas_icon = image;
}

jcanvas::Image * NativeWindow::GetIcon()
{
  return sg_jcanvas_icon;
}

}
