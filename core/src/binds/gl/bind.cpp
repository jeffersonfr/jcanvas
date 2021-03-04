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

namespace jcanvas {
  jkeyevent_modifiers_t jKeyEventModifiersNone = jkeyevent_modifiers_t::None;
  jmouseevent_button_t jMouseEventButtonNone = jmouseevent_button_t::None;
  jmouseevent_button_t jMouseEventButtonButton1 = jmouseevent_button_t::Button1;
  jmouseevent_button_t jMouseEventButtonButton2 = jmouseevent_button_t::Button2;
  jmouseevent_button_t jMouseEventButtonButton3 = jmouseevent_button_t::Button3;
	jwindow_rotation_t jWindowRotationNone = jwindow_rotation_t::None;
}

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
static Image *sg_back_buffer = nullptr;
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
static jrect_t<int> sg_previous_bounds;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case '\r':
			return jkeyevent_symbol_t::Enter; // jkeyevent_symbol_t::Return;
		//case SDLK_BACKSPACE:
		//	return jkeyevent_symbol_t::Backspace;
		case '\t':
			return jkeyevent_symbol_t::Tab;
		// case SDLK_CANCEL:
		//	return jkeyevent_symbol_t::Cancel;
		case 27:
			return jkeyevent_symbol_t::Escape;
    case ' ':
			return jkeyevent_symbol_t::Space;
		case '!':
			return jkeyevent_symbol_t::ExclamationMark;
		case '"':
			return jkeyevent_symbol_t::Quotation;
		case '#':
			return jkeyevent_symbol_t::Hash;
		case '$':
			return jkeyevent_symbol_t::Dollar;
		case '%':
			return jkeyevent_symbol_t::Percent;
		case '&':   
			return jkeyevent_symbol_t::Ampersand;
		case '\'':
			return jkeyevent_symbol_t::Aposthrophe;
		case '(':
			return jkeyevent_symbol_t::ParenthesisLeft;
		case ')':
			return jkeyevent_symbol_t::ParenthesisRight;
		case '*':
			return jkeyevent_symbol_t::Star;
		case '+':
			return jkeyevent_symbol_t::Plus;
		case ',':   
			return jkeyevent_symbol_t::Comma;
		case '-':
			return jkeyevent_symbol_t::Minus;
		case '.':  
			return jkeyevent_symbol_t::Period;
		case '/':
			return jkeyevent_symbol_t::Slash;
		case '0':     
			return jkeyevent_symbol_t::Number0;
		case '1':
			return jkeyevent_symbol_t::Number1;
		case '2':
			return jkeyevent_symbol_t::Number2;
		case '3':
			return jkeyevent_symbol_t::Number3;
		case '4':
			return jkeyevent_symbol_t::Number4;
		case '5':
			return jkeyevent_symbol_t::Number5;
		case '6':
			return jkeyevent_symbol_t::Number6;
		case '7':
			return jkeyevent_symbol_t::Number7;
		case '8':
			return jkeyevent_symbol_t::Number8;
		case '9':
			return jkeyevent_symbol_t::Number9;
    case ':':
			return jkeyevent_symbol_t::Colon;
		case ';':
			return jkeyevent_symbol_t::SemiColon;
		case '<':
			return jkeyevent_symbol_t::LessThan;
		case '=': 
			return jkeyevent_symbol_t::Equals;
		case '>':
			return jkeyevent_symbol_t::GreaterThan;
		case '?':   
			return jkeyevent_symbol_t::QuestionMark;
		case '@':
			return jkeyevent_symbol_t::At;
		case '[':
			return jkeyevent_symbol_t::SquareBracketLeft;
		case '\\':   
			return jkeyevent_symbol_t::BackSlash;
		case ']':
			return jkeyevent_symbol_t::SquareBracketRight;
		case '^':
			return jkeyevent_symbol_t::CircumflexAccent;
		case '_':    
			return jkeyevent_symbol_t::Underscore;
		case '`':
			return jkeyevent_symbol_t::GraveAccent;
		case 'a':       
			return jkeyevent_symbol_t::a;
		case 'b':
			return jkeyevent_symbol_t::b;
		case 'c':
			return jkeyevent_symbol_t::c;
		case 'd':
			return jkeyevent_symbol_t::d;
		case 'e':
			return jkeyevent_symbol_t::e;
		case 'f':
			return jkeyevent_symbol_t::f;
		case 'g':
			return jkeyevent_symbol_t::g;
		case 'h':
			return jkeyevent_symbol_t::h;
		case 'i':
			return jkeyevent_symbol_t::i;
		case 'j':
			return jkeyevent_symbol_t::j;
		case 'k':
			return jkeyevent_symbol_t::k;
		case 'l':
			return jkeyevent_symbol_t::l;
		case 'm':
			return jkeyevent_symbol_t::m;
		case 'n':
			return jkeyevent_symbol_t::n;
		case 'o':
			return jkeyevent_symbol_t::o;
		case 'p':
			return jkeyevent_symbol_t::p;
		case 'q':
			return jkeyevent_symbol_t::q;
		case 'r':
			return jkeyevent_symbol_t::r;
		case 's':
			return jkeyevent_symbol_t::s;
		case 't':
			return jkeyevent_symbol_t::t;
		case 'u':
			return jkeyevent_symbol_t::u;
		case 'v':
			return jkeyevent_symbol_t::v;
		case 'w':
			return jkeyevent_symbol_t::w;
		case 'x':
			return jkeyevent_symbol_t::x;
		case 'y':
			return jkeyevent_symbol_t::y;
		case 'z':
			return jkeyevent_symbol_t::z;
		case '{':
			return jkeyevent_symbol_t::CurlyBracketLeft;
		case '|':  
			return jkeyevent_symbol_t::VerticalBar;
		case '}':
			return jkeyevent_symbol_t::CurlyBracketRight;
		case '~':  
			return jkeyevent_symbol_t::Tilde;
		//case SDLK_DELETE:
		//	return jkeyevent_symbol_t::Delete;
		// case SDLK_PRINT:   
		//	return jkeyevent_symbol_t::Print;
		//case SDLK_PAUSE:
		//	return jkeyevent_symbol_t::Pause;
		// case SDLK_RED:
		//	return jkeyevent_symbol_t::Red;
		// case SDLK_GREEN:
		//	return jkeyevent_symbol_t::Green;
		// case SDLK_YELLOW:
		//	return jkeyevent_symbol_t::Yellow;
		// case SDLK_BLUE:
		//	return jkeyevent_symbol_t::Blue;
		//case SDLK_LSHIFT:
		//case SDLK_RSHIFT:
		//	return jkeyevent_symbol_t::Shift;
		//case SDLK_LCTRL:
		//case SDLK_RCTRL:
		//	return jkeyevent_symbol_t::Control;
		//case SDLK_LALT:
		//case SDLK_RALT:
		//	return jkeyevent_symbol_t::Alt;
		// case SDLK_ALTGR:
		//	return jkeyevent_symbol_t::AltGr;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jkeyevent_symbol_t::Meta;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jkeyevent_symbol_t::Super;
		// case SDLK_HYPER:
		//	return jkeyevent_symbol_t::Hyper;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
}

static jkeyevent_symbol_t TranslateToNativeKeySymbolSpecial(int symbol)
{
	switch (symbol) {
		case GLUT_KEY_LEFT:
			return jkeyevent_symbol_t::CursorLeft;
		case GLUT_KEY_RIGHT:
			return jkeyevent_symbol_t::CursorRight;
		case GLUT_KEY_UP:  
			return jkeyevent_symbol_t::CursorUp;
		case GLUT_KEY_DOWN:
			return jkeyevent_symbol_t::CursorDown;
		case GLUT_KEY_INSERT:  
	  	return jkeyevent_symbol_t::Insert;
	  case GLUT_KEY_HOME:     
			return jkeyevent_symbol_t::Home;
		case GLUT_KEY_END:
			return jkeyevent_symbol_t::End;
		case GLUT_KEY_PAGE_UP:
			return jkeyevent_symbol_t::PageUp;
		case GLUT_KEY_PAGE_DOWN:
			return jkeyevent_symbol_t::PageDown;
		case GLUT_KEY_F1:
			return jkeyevent_symbol_t::F1;
		case GLUT_KEY_F2:
			return jkeyevent_symbol_t::F2;
		case GLUT_KEY_F3:
			return jkeyevent_symbol_t::F3;
		case GLUT_KEY_F4:
			return jkeyevent_symbol_t::F4;
		case GLUT_KEY_F5:
			return jkeyevent_symbol_t::F5;
    case GLUT_KEY_F6:
			return jkeyevent_symbol_t::F6;
		case GLUT_KEY_F7:
			return jkeyevent_symbol_t::F7;
		case GLUT_KEY_F8:
			return jkeyevent_symbol_t::F8;
		case GLUT_KEY_F9:
			return jkeyevent_symbol_t::F9;
		case GLUT_KEY_F10: 
			return jkeyevent_symbol_t::F10;
		case GLUT_KEY_F11:
			return jkeyevent_symbol_t::F11;
		case GLUT_KEY_F12:
			return jkeyevent_symbol_t::F12;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
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
  
  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));
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

std::map<jmouseevent_button_t, bool> sg_mouse_button_state;

void OnMousePress(int button_id, int state, int x, int y)
{
  jmouseevent_button_t button = jMouseEventButtonNone;
  jmouseevent_type_t type = jmouseevent_type_t::Unknown;
  int mouse_z = 0;

  sg_mouse_x = x;
  sg_mouse_y = y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x-1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y-1);

  if (state == GLUT_DOWN) {
    type = jmouseevent_type_t::Pressed;
  } else if (state == GLUT_UP) {
    type = jmouseevent_type_t::Released;
  }

  if (button_id == GLUT_LEFT_BUTTON) {
    button = jMouseEventButtonButton1;
  } else if (button_id == GLUT_MIDDLE_BUTTON) {
    button = jMouseEventButtonButton2;
  } else if (button_id == GLUT_RIGHT_BUTTON) {
    button = jMouseEventButtonButton3;
  }

  if (type == jmouseevent_type_t::Pressed) {
    sg_mouse_button_state[button] = true;
  } else {
    sg_mouse_button_state[button] = false;
  }

  // TODO:: mouse grab
  
  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jMouseEventButtonNone, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

void OnMouseMove(int x, int y)
{
  jmouseevent_button_t button = jMouseEventButtonNone;
  jmouseevent_type_t type = jmouseevent_type_t::Unknown;
  int mouse_z = 0;

  sg_mouse_x = x;
  sg_mouse_y = y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x-1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y-1);

  type = jmouseevent_type_t::Moved;

  // TODO:: mouse grab
  
  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jMouseEventButtonNone, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

void OnKeyPressRelease(unsigned char key, int x, int y, bool released)
{
  jkeyevent_type_t type = jkeyevent_type_t::Unknown;
  jkeyevent_modifiers_t mod = jKeyEventModifiersNone;

  int m = glutGetModifiers();

  if (m == GLUT_ACTIVE_SHIFT) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
  } else if (m == GLUT_ACTIVE_CTRL) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
  } else if (GLUT_ACTIVE_ALT) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
  // } else if ((event.key.keysym.mod & ) != 0) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
  // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
  // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
  // } else if ((event.key.keysym.mod & ) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
  // } else if ((event.key.keysym.mod & ) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
  }

  type = jkeyevent_type_t::Unknown;
  
  if (released == false) {
    type = jkeyevent_type_t::Pressed;
  } else {
    type = jkeyevent_type_t::Released;
  }

  jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(key);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
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
  jkeyevent_type_t type = jkeyevent_type_t::Unknown;
  jkeyevent_modifiers_t mod = jKeyEventModifiersNone;

  int m = glutGetModifiers();

  if (m & GLUT_ACTIVE_SHIFT) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
  } else if (m & GLUT_ACTIVE_CTRL) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
  } else if (m & GLUT_ACTIVE_ALT) {
    mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
  // } else if ((event.key.keysym.mod & ) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::AltGr);
  // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
  // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
  // } else if ((event.key.keysym.mod & ) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
  // } else if ((event.key.keysym.mod & ) != 0) {
    // mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
  }

  if (released == false) {
    type = jkeyevent_type_t::Pressed;
  } else {
    type = jkeyevent_type_t::Released;
  }

  jkeyevent_symbol_t symbol = TranslateToNativeKeySymbolSpecial(key);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
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
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Opened));
      } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
      } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
        InternalPaint();
      } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Moved));
      } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        InternalPaint();

        sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Resized));
      } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
      }
    } else if(event.type == SDL_QUIT) {
      SDL_HideWindow(sg_window);

      sg_quitting = true;

      sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Closed));
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

    sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Entered));
  } else if (GLUT_LEFT) {
    // SDL_CaptureMouse(false);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(jcursor_style_t::Default);

    sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Leaved));
  }
}

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
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

WindowAdapter::~WindowAdapter()
{
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_previous_bounds = GetBounds();

    glutFullScreen();

    sg_fullscreen = true;
  } else {
    SetBounds(sg_previous_bounds);
    
    sg_fullscreen = false;
  }
}

void WindowAdapter::SetTitle(std::string title)
{
  sg_title = title;

  glutSetWindowTitle(title.c_str());
}

std::string WindowAdapter::GetTitle()
{
  return sg_title;
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
	return 1.0f;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
	if (undecorated == true) {
	} else {
	}
}

bool WindowAdapter::IsUndecorated()
{
  return false;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
  glutPositionWindow(bounds.point.x, bounds.point.y);
  glutReshapeWindow(bounds.size.x, bounds.size.y);
}

jrect_t<int> WindowAdapter::GetBounds()
{
  return {
    glutGet(GLUT_WINDOW_X),
    glutGet(GLUT_WINDOW_Y),
    glutGet(GLUT_WINDOW_WIDTH),
    glutGet(GLUT_WINDOW_HEIGHT)
  };
}
		
void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == false) {
    glutHideWindow();
  } else {
    glutShowWindow();
  }
}

bool WindowAdapter::IsVisible()
{
	return sg_visible;
}
		
void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return true;
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	return p;
}

jcursor_style_t WindowAdapter::GetCursor()
{
	int id = glutGet(GLUT_WINDOW_CURSOR);

  if (id == GLUT_CURSOR_INHERIT) {
    return jcursor_style_t::Default;
  } else if (id == GLUT_CURSOR_CROSSHAIR) {
    return jcursor_style_t::Crosshair;
  } else if (GLUT_CURSOR_RIGHT_SIDE) {
    return jcursor_style_t::East;
  } else if (GLUT_CURSOR_LEFT_SIDE) {
    return jcursor_style_t::West;
  } else if (GLUT_CURSOR_TOP_SIDE) {
    return jcursor_style_t::North;
  } else if (GLUT_CURSOR_BOTTOM_SIDE) {
    return jcursor_style_t::South;
  } else if (GLUT_CURSOR_INFO) {
    return jcursor_style_t::Hand;
  // } else if (GLUT_CURSOR_INHERIT) {
  //  return jcursor_style_t::Move;
  } else if (GLUT_CURSOR_UP_DOWN) {
    return jcursor_style_t::Vertical;
  } else if (GLUT_CURSOR_LEFT_RIGHT) {
    return jcursor_style_t::Horizontal;
  } else if (GLUT_CURSOR_TOP_LEFT_CORNER) {
    return jcursor_style_t::NorthWest;
  } else if (GLUT_CURSOR_TOP_RIGHT_CORNER) {
    return jcursor_style_t::NorthEast;
  } else if (GLUT_CURSOR_BOTTOM_RIGHT_CORNER) {
    return jcursor_style_t::SouthWest;
  } else if (GLUT_CURSOR_BOTTOM_LEFT_CORNER) {
    return jcursor_style_t::SouthEast;
  } else if (GLUT_CURSOR_TEXT) {
    return jcursor_style_t::Text;
  } else if (GLUT_CURSOR_WAIT) {
    return jcursor_style_t::Wait;
  }

  return jcursor_style_t::Default;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
  if (enabled == false) {
    glutSetCursor(GLUT_CURSOR_NONE);
  } else {
    glutSetCursor(GLUT_CURSOR_INHERIT);
  }
}

bool WindowAdapter::IsCursorEnabled()
{
	return glutGet(GLUT_WINDOW_CURSOR) != GLUT_CURSOR_NONE;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  int type = GLUT_CURSOR_INHERIT;

  if (style == jcursor_style_t::Default) {
    type = GLUT_CURSOR_INHERIT;
  } else if (style == jcursor_style_t::Crosshair) {
    type = GLUT_CURSOR_CROSSHAIR;
  } else if (style == jcursor_style_t::East) {
    type = GLUT_CURSOR_RIGHT_SIDE;
  } else if (style == jcursor_style_t::West) {
    type = GLUT_CURSOR_LEFT_SIDE;
  } else if (style == jcursor_style_t::North) {
    type = GLUT_CURSOR_TOP_SIDE;
  } else if (style == jcursor_style_t::South) {
    type = GLUT_CURSOR_BOTTOM_SIDE;
  } else if (style == jcursor_style_t::Hand) {
    type = GLUT_CURSOR_INFO;
  } else if (style == jcursor_style_t::Move) {
    type = GLUT_CURSOR_INHERIT;
  } else if (style == jcursor_style_t::Vertical) {
    type = GLUT_CURSOR_UP_DOWN;
  } else if (style == jcursor_style_t::Horizontal) {
    type = GLUT_CURSOR_LEFT_RIGHT;
  } else if (style == jcursor_style_t::NorthWest) {
    type = GLUT_CURSOR_TOP_LEFT_CORNER;
  } else if (style == jcursor_style_t::NorthEast) {
    type = GLUT_CURSOR_TOP_RIGHT_CORNER;
  } else if (style == jcursor_style_t::SouthWest) {
    type = GLUT_CURSOR_BOTTOM_RIGHT_CORNER;
  } else if (style == jcursor_style_t::SouthEast) {
    type = GLUT_CURSOR_BOTTOM_LEFT_CORNER;
  } else if (style == jcursor_style_t::Text) {
    type = GLUT_CURSOR_TEXT;
  } else if (style == jcursor_style_t::Wait) {
    type = GLUT_CURSOR_WAIT;
  }

  glutSetCursor(type);
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
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

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jWindowRotationNone;
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
