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
#define RASPBERRY_PI

#include "../include/nativewindow.h"

#include "jcanvas/jbufferedimage.h"
#include "jcanvas/jfont.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

#ifdef RASPBERRY_PI

extern "C" {
	#include "bcm_host.h"
}

#include "GLES/gl.h"
#include "GLES/glext.h"
#include "EGL/egl.h"

#include <linux/input.h>

#include <termio.h>
#include <fcntl.h>

#else

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include <GL/glu.h>

#include <EGL/egl.h>
// #include <EGL/eglext.h>

#include <GLES2/gl2.h>

#include <xcb/xcb.h>

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#endif

#include <cairo.h>
#include <cairo-xcb.h>

namespace jcanvas {

#ifdef RASPBERRY_PI

struct cursor_params_t {
  jcanvas::Image *cursor;
  int hot_x;
  int hot_y;
};

static std::map<jcursor_style_t, struct cursor_params_t> sg_jcanvas_cursors;

static DISPMANX_DISPLAY_HANDLE_T sg_dispmana_display;
static DISPMANX_ELEMENT_HANDLE_T sg_dispman_element;
static DISPMANX_UPDATE_HANDLE_T sg_dispman_update;
static EGL_DISPMANX_WINDOW_T sg_dispman_window;
static struct cursor_params_t sg_cursor_params_cursor;

#else

static Display *sg_xcb_display;
static xcb_connection_t *sg_xcb_connection;
static xcb_screen_t *sg_xcb_screen;
static xcb_window_t sg_xcb_window;
static xcb_gcontext_t sg_xcb_context;

#endif

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static EGLDisplay sg_egl_display;
/** \brief */
static EGLConfig sg_egl_config;
/** \brief */
static EGLContext sg_egl_context;
/** \brief */
static EGLSurface sg_egl_surface;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static std::string sg_title;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;
/** \brief */
static Window *sg_jcanvas_window = nullptr;

#ifdef RASPBERRY_PI

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case 0x1c:
			return jcanvas::JKS_ENTER; // jcanvas::JKS_RETURN;
		case 0x0e:
			return jcanvas::JKS_BACKSPACE;
		case 0x0f:
			return jcanvas::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jcanvas::JKS_CANCEL;
		case 0x01:
			return jcanvas::JKS_ESCAPE;
		case 0x39:
			return jcanvas::JKS_SPACE;
		case 0x29:
			return jcanvas::JKS_APOSTROPHE;
		case 0x33:
			return jcanvas::JKS_COMMA;
		case 0x0c:
			return jcanvas::JKS_MINUS_SIGN;
		case 0x34:  
			return jcanvas::JKS_PERIOD;
		case 0x59:
			return jcanvas::JKS_SLASH;
		case 0x0b:     
			return jcanvas::JKS_0;
		case 0x02:
			return jcanvas::JKS_1;
		case 0x03:
			return jcanvas::JKS_2;
		case 0x04:
			return jcanvas::JKS_3;
		case 0x05:
			return jcanvas::JKS_4;
		case 0x06:
			return jcanvas::JKS_5;
		case 0x07:
			return jcanvas::JKS_6;
		case 0x08:
			return jcanvas::JKS_7;
		case 0x09:
			return jcanvas::JKS_8;
		case 0x0a:
			return jcanvas::JKS_9;
		case 0x35:
			return jcanvas::JKS_SEMICOLON;
		case 0x0d: 
			return jcanvas::JKS_EQUALS_SIGN;
		case 0x1b:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case 0x56:   
			return jcanvas::JKS_BACKSLASH;
		case 0x2b:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case 0x1e:       
			return jcanvas::JKS_a;
		case 0x30:
			return jcanvas::JKS_b;
		case 0x2e:
			return jcanvas::JKS_c;
		case 0x20:
			return jcanvas::JKS_d;
		case 0x12:
			return jcanvas::JKS_e;
		case 0x21:
			return jcanvas::JKS_f;
		case 0x22:
			return jcanvas::JKS_g;
		case 0x23:
			return jcanvas::JKS_h;
		case 0x17:
			return jcanvas::JKS_i;
		case 0x24:
			return jcanvas::JKS_j;
		case 0x25:
			return jcanvas::JKS_k;
		case 0x26:
			return jcanvas::JKS_l;
		case 0x32:
			return jcanvas::JKS_m;
		case 0x31:
			return jcanvas::JKS_n;
		case 0x18:
			return jcanvas::JKS_o;
		case 0x19:
			return jcanvas::JKS_p;
		case 0x10:
			return jcanvas::JKS_q;
		case 0x13:
			return jcanvas::JKS_r;
		case 0x1f:
			return jcanvas::JKS_s;
		case 0x14:
			return jcanvas::JKS_t;
		case 0x16:
			return jcanvas::JKS_u;
		case 0x2f:
			return jcanvas::JKS_v;
		case 0x11:
			return jcanvas::JKS_w;
		case 0x2d:
			return jcanvas::JKS_x;
		case 0x15:
			return jcanvas::JKS_y;
		case 0x2c:
			return jcanvas::JKS_z;
		// case SDLK_BACKQUOTE:
		//	return jcanvas::JKS_GRAVE_ACCENT;
		case 0x28:  
			return jcanvas::JKS_TILDE;
		case 0x6f:
			return jcanvas::JKS_DELETE;
		case 0x69:
			return jcanvas::JKS_CURSOR_LEFT;
		case 0x6a:
			return jcanvas::JKS_CURSOR_RIGHT;
		case 0x67:  
			return jcanvas::JKS_CURSOR_UP;
		case 0x6c:
			return jcanvas::JKS_CURSOR_DOWN;
		case 0x6e:  
			return jcanvas::JKS_INSERT;
		case 0x66:     
			return jcanvas::JKS_HOME;
		case 0x6b:
			return jcanvas::JKS_END;
		case 0x68:
			return jcanvas::JKS_PAGE_UP;
		case 0x6d:
			return jcanvas::JKS_PAGE_DOWN;
		case 0x63:   
			return jcanvas::JKS_PRINT;
		case 0x77:
			return jcanvas::JKS_PAUSE;
		// case SDLK_RED:
		//	return jcanvas::JKS_RED;
		// case SDLK_GREEN:
		//	return jcanvas::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jcanvas::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jcanvas::JKS_BLUE;
		case 0x3b:
			return jcanvas::JKS_F1;
		case 0x3c:
			return jcanvas::JKS_F2;
		case 0x3d:
			return jcanvas::JKS_F3;
		case 0x3e:
			return jcanvas::JKS_F4;
		case 0x3f:
			return jcanvas::JKS_F5;
		case 0x40:
			return jcanvas::JKS_F6;
		case 0x41:    
			return jcanvas::JKS_F7;
		case 0x42:
			return jcanvas::JKS_F8;
		case 0x43:  
			return jcanvas::JKS_F9;
		case 0x44: 
			return jcanvas::JKS_F10;
		case 0x57:
			return jcanvas::JKS_F11;
		case 0x58:
			return jcanvas::JKS_F12;
		case 0x2a: // left
		case 0x36: // right
			return jcanvas::JKS_SHIFT;
		case 0x1d: // left
		case 0x61: // right
			return jcanvas::JKS_CONTROL;
		case 0x38: // left
			return jcanvas::JKS_ALT;
		case 0x64: 
		  return jcanvas::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jcanvas::JKS_META;
		case 0x7d:
			return jcanvas::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jcanvas::JKS_HYPER;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

#else 

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(xcb_keycode_t symbol, bool capital)
{
	switch (symbol) {
    case 0x32:
    case 0x3e:
      return jcanvas::JKS_SHIFT;
    case 0x25:
    case 0x69:
      return jcanvas::JKS_CONTROL;
    case 0x40:
    case 0x6c:
		  return jcanvas::JKS_ALT;
		//case XK_Super_L:
		//	return jcanvas::JKS_LSYSTEM;
		//case XK_Super_R:
		//	return jcanvas::JKS_RSYSTEM;
		//case XK_Menu:
		//	return jcanvas::JKS_MENU;
		case 0x09:
			return jcanvas::JKS_ESCAPE;
		case 0x2f:
			return jcanvas::JKS_SEMICOLON;
		case 0x3d:
			return jcanvas::JKS_QUESTION_MARK;
		case 0x6a:
			return jcanvas::JKS_SLASH;
		case 0x15:
			return jcanvas::JKS_EQUALS_SIGN;
		case 0x14:
			return jcanvas::JKS_UNDERSCORE;
		case 0x52:
			return jcanvas::JKS_MINUS_SIGN;
		case 0x22:
			return jcanvas::JKS_CURLY_BRACKET_LEFT;
		case 0x23:
			return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case 0x3b:
			return jcanvas::JKS_LESS_THAN_SIGN;
		case 0x3c:
		case 0x5b:
			return jcanvas::JKS_GREATER_THAN_SIGN;
		case 0x30:
			return jcanvas::JKS_QUOTATION;
		case 0x33:
			return jcanvas::JKS_BACKSLASH;
		case 0x31:
			return jcanvas::JKS_TILDE;
		case 0x41:
			return jcanvas::JKS_SPACE;
		case 0x24:
		case 0x68:
			return jcanvas::JKS_ENTER;
		case 0x16:
			return jcanvas::JKS_BACKSPACE;
		case 0x17:
			return jcanvas::JKS_TAB;
		case 0x51:
			return jcanvas::JKS_PAGE_UP;
		case 0x59:
			return jcanvas::JKS_PAGE_DOWN;
		case 0x57:
			return jcanvas::JKS_END;
		case 0x4f:
			return jcanvas::JKS_HOME;
		case 0x76:
			return jcanvas::JKS_INSERT;
		case 0x77:
			return jcanvas::JKS_DELETE;
		case 0x56:
			return jcanvas::JKS_PLUS_SIGN;
		case 0x3f:
			return jcanvas::JKS_STAR;
		// case XK_Pause:
		//	return jcanvas::JKS_PAUSE;
		case 0x43:
			return jcanvas::JKS_F1;
		case 0x44:
			return jcanvas::JKS_F2;
		case 0x45:
			return jcanvas::JKS_F3;
		case 0x46:
			return jcanvas::JKS_F4;
		case 0x47:
			return jcanvas::JKS_F5;
		case 0x48:
			return jcanvas::JKS_F6;
		case 0x49:
			return jcanvas::JKS_F7;
		case 0x4a:
			return jcanvas::JKS_F8;
		case 0x4b:
			return jcanvas::JKS_F9;
		case 0x4c:
			return jcanvas::JKS_F10;
		case 0x5f:
			return jcanvas::JKS_F11;
		case 0x60:
			return jcanvas::JKS_F12;
		case 0x71:
			return jcanvas::JKS_CURSOR_LEFT;
		case 0x72:
			return jcanvas::JKS_CURSOR_RIGHT;
		case 0x6f:
			return jcanvas::JKS_CURSOR_UP;
		case 0x74:
			return jcanvas::JKS_CURSOR_DOWN;
		case 0x13:
		case 0x54:
			return jcanvas::JKS_PARENTHESIS_RIGHT;
		case 0x0a:
		// case 0x57:
			return jcanvas::JKS_EXCLAMATION_MARK;
		case 0x0b:
		// case 0x58:
			return jcanvas::JKS_AT;
		case 0x0c:
		// case 0x59:
			return jcanvas::JKS_NUMBER_SIGN;
		case 0x0d:
		// case 0x53:
			return jcanvas::JKS_DOLLAR_SIGN;
		case 0x0e:
		// case 0x54:
			return jcanvas::JKS_PERCENT_SIGN;
		case 0x0f:
		// case 0x55:
			return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		case 0x10:
		// case 0x4f:
			return jcanvas::JKS_AMPERSAND;
		case 0x11:
		// case 0x50:
			return jcanvas::JKS_STAR;
		case 0x12:
		// case 0x51:
			return jcanvas::JKS_PARENTHESIS_LEFT;
		case 0x26:
			return jcanvas::JKS_a;
		case 0x38:
			return jcanvas::JKS_b;
		case 0x36:
			return jcanvas::JKS_c;
		case 0x28:
			return jcanvas::JKS_d;
		case 0x1a:
			return jcanvas::JKS_e;
		case 0x29:
			return jcanvas::JKS_f;
		case 0x2a:
			return jcanvas::JKS_g;
		case 0x2b:
			return jcanvas::JKS_h;
		case 0x1f:
			return jcanvas::JKS_i;
		case 0x2c:
			return jcanvas::JKS_j;
		case 0x2d:
			return jcanvas::JKS_k;
		case 0x2e:
			return jcanvas::JKS_l;
		case 0x3a:
			return jcanvas::JKS_m;
		case 0x39:
			return jcanvas::JKS_n;
		case 0x20:
			return jcanvas::JKS_o;
		case 0x21:
			return jcanvas::JKS_p;
		case 0x18:
			return jcanvas::JKS_q;
		case 0x1b:
			return jcanvas::JKS_r;
		case 0x27:
			return jcanvas::JKS_s;
		case 0x1c:
			return jcanvas::JKS_t;
		case 0x1e:
			return jcanvas::JKS_u;
		case 0x37:
			return jcanvas::JKS_v;
		case 0x35:
			return jcanvas::JKS_w;
		case 0x19:
			return jcanvas::JKS_x;
		case 0x1d:
			return jcanvas::JKS_y;
		case 0x34:
			return jcanvas::JKS_z;
		// case XK_Print:
		//	return jcanvas::JKS_PRINT;
		case 0x7f:
			return jcanvas::JKS_BREAK;
		// case XK_acute:
		//	return jcanvas::JKS_ACUTE_ACCENT;
		default:
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

#endif

void Application::Init(int argc, char **argv)
{
  EGLint num_config;

#ifdef RASPBERRY_PI
  
  bcm_host_init();

  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    throw std::runtime_error("Unable to bind opengl es api");
  }

  sg_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#else 

  sg_xcb_display = XOpenDisplay(nullptr);

  if (!sg_xcb_display) {
    throw std::runtime_error("Unable to open display");
  }

  sg_xcb_connection = XGetXCBConnection(sg_xcb_display);
  // sg_xcb_connection = xcb_connect(nullptr,nullptr);

  if (xcb_connection_has_error(sg_xcb_connection)) {
		throw std::runtime_error("Unable to connect to display");
  }

  // get the first screen
  sg_xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(sg_xcb_connection)).data;

  if (!eglBindAPI(EGL_OPENGL_API)) {
    throw std::runtime_error("Unable to bind opengl api");
  }

  sg_egl_display = eglGetDisplay(sg_xcb_display);

#endif

  const EGLint attribute_list[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };

  const EGLint sg_egl_context_attribs[] = {
    EGL_NONE,
  };

  if (sg_egl_display == EGL_NO_DISPLAY) {
    throw std::runtime_error("Unable to get egl display");
  }

  if (eglInitialize(sg_egl_display, nullptr, nullptr) == EGL_FALSE) {
    throw std::runtime_error("Unable to initialize egl");
  }

  if (eglChooseConfig(sg_egl_display, attribute_list, &sg_egl_config, 1, &num_config) == EGL_FALSE) {
    throw std::runtime_error("Unable to choose egl configuration");
  }

  sg_egl_context = eglCreateContext(sg_egl_display, sg_egl_config, EGL_NO_CONTEXT, sg_egl_context_attribs); 
  
  if (sg_egl_context == EGL_NO_CONTEXT) {
    throw std::runtime_error("eglCreateContext() failed");
  }

#ifdef RASPBERRY_PI

  uint32_t sw, sh;

  if (graphics_get_display_size(0, &sw, &sh) < 0) {
    throw std::runtime_error("Unable to get screen size");
  }

  sg_screen.x = sw;
  sg_screen.y = sh;

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jcanvas::BufferedImage(JPF_ARGB, {w, h});												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jcanvas::jpoint_t<int>{0, 0});	\
																																				\
	sg_jcanvas_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = new jcanvas::BufferedImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
	delete cursors;

#else

  sg_screen.x = sg_xcb_screen->width_in_pixels;
  sg_screen.y = sg_xcb_screen->height_in_pixels;

#endif
  
  sg_quitting = false;
}

static void InternalPaint()
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

#ifdef RASPBERRY_PI

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params_cursor.cursor, jcanvas::jpoint_t<int>{sg_mouse_x, sg_mouse_y});
  }

#endif

  g->Flush();

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  GLuint texture;

  glGenTextures(1, &texture);

#ifdef RASPBERRY_PI

  static const GLfloat coords[4 * 2] = {
    1.0f,  1.0f,
    1.0f,  0.0f,
    0.0f,  1.0f,
    0.0f,  0.0f,
  };

  static const GLbyte verts[4*3] = {
    -1, -1, 0,
     1, -1, 0,
    -1,  1, 0,
     1,  1, 0
  };

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, sg_screen.x, sg_screen.y, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

  glViewport(0, 0, bounds.size.x, bounds.size.y);
  glClearColor(0, 0, 0, 0);
  glMatrixMode(GL_TEXTURE);

  glActiveTexture(texture);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_BYTE, 0, verts);

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, coords);

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture);

  glRotatef(90, 0.0f, 0.0f, 1.0f);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glRotatef(-90, 0.0f, 0.0f, 1.0f);

#else

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, bounds.size.x, bounds.size.y, GL_BGRA, GL_UNSIGNED_BYTE, data);

  glViewport(0, -bounds.size.y, bounds.size.x*2, bounds.size.y*2);
  glClearColor(0, 0, 0, 0);
  glMatrixMode(GL_TEXTURE);

  glActiveTexture(texture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-bounds.size.x, bounds.size.y);

  glTexCoord2f(0.0f, bounds.size.y);
  glVertex2f(-bounds.size.x, 0.0f);

  glTexCoord2f(bounds.size.x, bounds.size.y);
  glVertex2f(0, 0.0f);

  glTexCoord2f(bounds.size.x, 0.0f);
  glVertex2f(0, bounds.size.y);
  
  glEnd();

#endif

  if (g->IsVerticalSyncEnabled() == false) {
    glFlush();
  } else {
    glFinish();
  }

  eglSwapBuffers(sg_egl_display, sg_egl_surface);

  glDeleteTextures(1, &texture);
  
  sg_back_buffer->UnlockData();

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

#ifdef RASPBERRY_PI

  struct input_event ev;
  int mouse_x = 0, mouse_y = 0;
  uint32_t lastsg_mouse_state = 0x00;

  int 
    fdk = open("/dev/input/by-path/platform-3f980000.usb-usb-0:1.4:1.0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  int 
    fdm = open("/dev/input/mice", O_RDONLY);

  if(fdm == -1) {   
    printf("Cannot open the mouse device\n");
  }   

  fcntl(fdm, F_SETFL, O_NONBLOCK);

#else

  xcb_generic_event_t *event;

  xcb_intern_atom_cookie_t 
    cookie = xcb_intern_atom(sg_xcb_connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t
    *reply = xcb_intern_atom_reply(sg_xcb_connection, cookie, 0);

  xcb_intern_atom_cookie_t 
    cookie2 = xcb_intern_atom(sg_xcb_connection, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t 
    *reply2 = xcb_intern_atom_reply(sg_xcb_connection, cookie2, 0);

  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, (*reply).atom, 4, 32, 1, &(*reply2).atom);

#endif

	while (sg_quitting == false) {
#ifdef RASPBERRY_PI
      if (mouse_x != sg_mouse_x or mouse_y != sg_mouse_y) {
        mouse_x = sg_mouse_x;
        mouse_y = sg_mouse_y;

        if (sg_cursor_enabled == true) {
          sg_repaint.store(true);
        }
      }
#endif

    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

#ifdef RASPBERRY_PI

    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if (ev.code == 0x2a) { //LSHIFT
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if (ev.code == 0x36) { // RSHIFT
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        /*
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
        } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jcanvas::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        */
        }

        type = jcanvas::JKT_UNKNOWN;

        if (ev.value == 1 or ev.value == 2) {
          type = jcanvas::JKT_PRESSED;
        } else if (ev.value == 0) {
          type = jcanvas::JKT_RELEASED;
        }

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code);

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      }
    }

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
     
      x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
      y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;

      jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
      jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
      int mouse_z = 0;

      type = jcanvas::JMT_PRESSED;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jcanvas::JMT_MOVED;
      }

      sg_mouse_x = CLAMP(x, 0, sg_screen.x - 1);
      sg_mouse_y = CLAMP(y, 0, sg_screen.y - 1);

      if ((buttonMask & 0x01) == 0 && (lastsg_mouse_state & 0x01)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (lastsg_mouse_state & 0x02)) {
        type = jcanvas::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (lastsg_mouse_state & 0x04)) {
        type = jcanvas::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (lastsg_mouse_state & 0x01)) {
        button = jcanvas::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (lastsg_mouse_state & 0x02)) {
        button = jcanvas::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (lastsg_mouse_state & 0x04)) {
        button = jcanvas::JMB_BUTTON2;
      }

      lastsg_mouse_state = buttonMask;

      sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x + sg_cursor_params_cursor.hot_x, sg_mouse_y + sg_cursor_params_cursor.hot_y}, mouse_z));
    }

#else

    while ((event = xcb_poll_for_event(sg_xcb_connection))) {
    // while (e = xcb_wait_for_event(sg_xcb_connection)) {
      uint32_t id = event->response_type & ~0x80;

      if (id == XCB_EXPOSE) {
        InternalPaint();
      } else if (id == XCB_ENTER_NOTIFY) {
        // SetCursor(GetCursor());

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));
      } else if (id == XCB_LEAVE_NOTIFY) {
        // SetCursor(JCS_DEFAULT);

        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));
      } else if (id == XCB_FOCUS_IN) {
      } else if (id == XCB_FOCUS_OUT) {
      } else if (id == XCB_CREATE_NOTIFY) {
      } else if (id == XCB_DESTROY_NOTIFY) {
      } else if (id == XCB_UNMAP_NOTIFY) {
      } else if (id == XCB_MAP_NOTIFY) {
        sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_OPENED));
      } else if (id == XCB_RESIZE_REQUEST) {
      } else if (id == XCB_KEY_PRESS || id == XCB_KEY_RELEASE) {
        xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if ((e->state & XCB_MOD_MASK_SHIFT) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if ((e->state & XCB_MOD_MASK_CONTROL) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((e->state & XCB_MOD_MASK_LOCK) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CAPS_LOCK);
        } else if ((e->state & XCB_MOD_MASK_1) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        } else if ((e->state & XCB_MOD_MASK_2) != 0) {
        } else if ((e->state & XCB_MOD_MASK_3) != 0) {
        } else if ((e->state & XCB_MOD_MASK_4) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
        } else if ((e->state & XCB_MOD_MASK_5) != 0) {
        }

        type = jcanvas::JKT_UNKNOWN;

        if (id == XCB_KEY_PRESS) {
          type = jcanvas::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (id == XCB_KEY_RELEASE) {
          type = jcanvas::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        int shift = (e->state & XCB_MOD_MASK_SHIFT) != 0;
        int capslock = (e->state & XCB_MOD_MASK_LOCK) != 0;

        jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->detail, (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE || id == XCB_MOTION_NOTIFY) {
        xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = e->event_x;
        sg_mouse_y = e->event_y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

        if (id == XCB_MOTION_NOTIFY) {
          type = jcanvas::JMT_MOVED;
        } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE) {
          if (id == XCB_BUTTON_PRESS) {
            type = jcanvas::JMT_PRESSED;
          } else if (id == XCB_BUTTON_RELEASE) {
            type = jcanvas::JMT_RELEASED;
          }

          if (e->detail == 0x01) {
            button = jcanvas::JMB_BUTTON1;
          } else if (e->detail == 0x02) {
            button = jcanvas::JMB_BUTTON2;
          } else if (e->detail == 0x03) {
            button = jcanvas::JMB_BUTTON3;
          }
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (id == XCB_CLIENT_MESSAGE) {
        if ((*(xcb_client_message_event_t*)event).data.data32[0] == (*reply2).atom) {
          sg_quitting = true;

          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_CLOSED));
        }
      }

      free(event);

      xcb_flush(sg_xcb_connection);
    }
    
#endif
  }

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

NativeWindow::NativeWindow(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds):
	jcanvas::WindowAdapter()
{
  sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

#ifdef RASPBERRY_PI

  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;

  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.x = sg_screen.x;
  dst_rect.y = sg_screen.y;

  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.x = sg_screen.x << 16;
  src_rect.y = sg_screen.y << 16;

  sg_dispmana_display = vc_dispmanx_display_open(0);
  sg_dispman_update = vc_dispmanx_update_start(0);

  sg_dispman_element = vc_dispmanx_element_add (
		  sg_dispman_update, sg_dispmana_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

  sg_dispman_window.element = sg_dispman_element;
  sg_dispman_window.x = sg_screen.x;
  sg_dispman_window.y = sg_screen.y;

  vc_dispmanx_update_submit_sync(sg_dispman_update);

#else

  if (sg_xcb_window != 0) {
	  throw std::runtime_error("Cannot create more than one window");
  }

  uint32_t
    mask,
    values[2];

  sg_xcb_window = xcb_generate_id(sg_xcb_connection);

  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = sg_xcb_screen->black_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | 
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  xcb_create_window(
      sg_xcb_connection, XCB_COPY_FROM_PARENT, sg_xcb_window, sg_xcb_screen->root, x, y, width, height, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, sg_xcb_screen->root_visual, mask, values);

  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = sg_xcb_screen->black_pixel;
  values[1] = 0;

  sg_xcb_context = xcb_generate_id(sg_xcb_connection);

  xcb_create_gc(sg_xcb_connection, sg_xcb_context, sg_xcb_window, mask, values);

  // INFO:: change parameters after the window creation
  // const static uint32_t values[] = { XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS };
  // xcb_change_window_attributes (sg_xcb_connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(sg_xcb_connection, sg_xcb_window);
  xcb_flush(sg_xcb_connection);

#endif

  /*
  const EGLint sg_egl_config_attribs[] = {
    EGL_COLOR_BUFFER_TYPE,     EGL_RGB_BUFFER,
    EGL_BUFFER_SIZE,           32,
    EGL_RED_SIZE,              8,
    EGL_GREEN_SIZE,            8,
    EGL_BLUE_SIZE,             8,
    EGL_ALPHA_SIZE,            8,

    EGL_DEPTH_SIZE,            24,
    EGL_STENCIL_SIZE,          8,

    EGL_SAMPLE_BUFFERS,        0,
    EGL_SAMPLES,               0,

    EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE,       EGL_OPENGL_BIT,

    EGL_NONE,
  };
  */

  const EGLint sg_egl_surface_attribs[] = {
    EGL_RENDER_BUFFER, 
    EGL_BACK_BUFFER,
    EGL_NONE,
  };

#ifdef RASPBERRY_PI

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, &sg_dispman_window, sg_egl_surface_attribs);

#else

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, sg_xcb_window, sg_egl_surface_attribs);

#endif

  if (!sg_egl_surface) {
    throw std::runtime_error("eglCreateWindowSurface() failed");
  }

  if (eglMakeCurrent(sg_egl_display, sg_egl_surface, sg_egl_surface, sg_egl_context) == EGL_FALSE) {
    throw std::runtime_error("eglMakeCurrent() failed");
  }

  if (eglSwapInterval(sg_egl_display, 1) == EGL_FALSE) {
    throw std::runtime_error("eglSwapInterval() failed");
  }
}

NativeWindow::~NativeWindow()
{
  glClear(GL_COLOR_BUFFER_BIT);

  eglSwapBuffers(sg_egl_display, sg_egl_surface);
  eglMakeCurrent(sg_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(sg_egl_display, sg_egl_surface);
  eglDestroyContext(sg_egl_display, sg_egl_context);
  eglTerminate(sg_egl_display);

#ifdef RASPBERRY_PI

  vc_dispmanx_element_remove(sg_dispman_update, sg_dispman_element);
  vc_dispmanx_update_submit_sync(sg_dispman_update);
  vc_dispmanx_display_close(sg_dispmana_display);

#else

  xcb_destroy_window(sg_xcb_connection, sg_xcb_window);
  xcb_disconnect(sg_xcb_connection);

#endif
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint()
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
#ifdef RASPBERRY_PI
  
  if (sg_fullscreen == false) {
    sg_fullscreen = true;
  } else {
    sg_fullscreen = false;
  }

#else

  static jcanvas::jrect_t<int> previous_bounds = {
	  0, 0, 0, 0
  };

  if (sg_fullscreen == false) {
    previous_bounds = GetBounds();

    SetBounds(0, 0, sg_screen.x, sg_screen.y);

    sg_fullscreen = true;
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
    SetBounds(previous_bounds.point.x, previous_bounds.point.y, previous_bounds.size.x, previous_bounds.size.y);
    xcb_map_window(sg_xcb_connection, sg_xcb_window);

    sg_fullscreen = false;
  }
  
  xcb_flush(sg_xcb_connection);

#endif
}

void NativeWindow::SetTitle(std::string title)
{
	sg_title = title;
		
#ifdef RASPBERRY_PI

#else

  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str());

#endif
}

std::string NativeWindow::GetTitle()
{
	return sg_title;
}

void NativeWindow::SetOpacity(float opacity)
{
  sg_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return sg_opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return sg_undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
#ifdef RASPBERRY_PI

#else

  const uint32_t 
    values[] = {(uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height};

  xcb_configure_window(sg_xcb_connection, sg_xcb_window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);

#endif
}

jcanvas::jrect_t<int> NativeWindow::GetBounds()
{
	jcanvas::jrect_t<int> 
    t = {0, 0, 0, 0};

#ifdef RASPBERRY_PI

	t.size.x = sg_screen.x;
	t.size.y = sg_screen.y;

#else

  xcb_get_geometry_cookie_t 
    cookie = xcb_get_geometry(sg_xcb_connection, sg_xcb_window);
  xcb_get_geometry_reply_t 
    *reply = nullptr;

  if ((reply = xcb_get_geometry_reply(sg_xcb_connection, cookie, nullptr))) {
    t = {
      reply->x,
      reply->y,
      reply->width,
      reply->height
    };
  }

  free(reply);

#endif

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
  sg_resizable = resizable;
}

bool NativeWindow::IsResizable()
{
  return sg_resizable;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
#ifdef RASPBERRY_PI

  sg_mouse_x = (x < 0)?0:(x > sg_screen.x)?sg_screen.x:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.y)?sg_screen.y:y;

#else

  // XWarpPointer(_display, None, sg_xcb_window, 0, 0, size.x, size.y, x, y);
  
#endif
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t = {
		.x = 0,
		.y = 0
	};

#ifdef RASPBERRY_PI

	t.x = sg_mouse_x;
	t.y = sg_mouse_y;
#else

	// XTranslateCoordinates(_display, sg_xcb_window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

#endif

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

#ifdef RASPBERRY_PI

#else

  if (visible == true) {
    xcb_map_window(sg_xcb_connection, sg_xcb_window);
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
  }
  
  xcb_flush(sg_xcb_connection);

#endif
}

bool NativeWindow::IsVisible()
{
  return sg_visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jcanvas_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = enabled;

	// XDefineCursor(_display, sg_xcb_window, sg_cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  sg_jcanvas_cursor = style;

#ifdef RASPBERRY_PI

#else

  int type = XC_arrow;
  
  if (style == JCS_DEFAULT) {
    type = XC_arrow;
  } else if (style == JCS_CROSSHAIR) {
    type = XC_crosshair;
  } else if (style == JCS_EAST) {
    type = XC_sb_right_arrow;
  } else if (style == JCS_WEST) {
    type = XC_sb_left_arrow;
  } else if (style == JCS_NORTH) {
    type = XC_sb_up_arrow;
  } else if (style == JCS_SOUTH) {
    type = XC_sb_down_arrow;
  } else if (style == JCS_HAND) {
    type = XC_hand2;
  } else if (style == JCS_MOVE) {
    type = XC_fleur;
  } else if (style == JCS_NS) {
    type = XC_sb_v_double_arrow;
  } else if (style == JCS_WE) {
    type = XC_sb_h_double_arrow;
  } else if (style == JCS_NW_CORNER) {
    type = XC_left_ptr;
  } else if (style == JCS_NE_CORNER) {
    type = XC_right_ptr;
  } else if (style == JCS_SW_CORNER) {
    type = XC_bottom_left_corner;
  } else if (style == JCS_SE_CORNER) {
    type = XC_bottom_right_corner;
  } else if (style == JCS_TEXT) {
    type = XC_xterm;
  } else if (style == JCS_WAIT) {
    type = XC_watch;
  }

  xcb_font_t font = xcb_generate_id(sg_xcb_connection);
  xcb_cursor_t cursor = xcb_generate_id(sg_xcb_connection);
  xcb_create_glyph_cursor(sg_xcb_connection, cursor, font, font, type, type + 1, 0, 0, 0, 0, 0, 0 );

  uint32_t mask = XCB_CW_CURSOR;
  uint32_t values = cursor;

  xcb_change_window_attributes(sg_xcb_connection, sg_xcb_window, mask, &values);
  xcb_free_cursor(sg_xcb_connection, cursor);

#endif
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

#ifdef RASPBERRY_PI

  if (sg_cursor_params_cursor.cursor != nullptr) {
    delete sg_cursor_params_cursor.cursor;
    sg_cursor_params_cursor.cursor = nullptr;
  }

  sg_cursor_params_cursor.cursor = dynamic_cast<jcanvas::Image *>(shape->Clone());

  sg_cursor_params_cursor.hot_x = hotx;
  sg_cursor_params_cursor.hot_y = hoty;

#else

	/*
	jpoint_t<int> t = shape->GetSize();
	uint32_t data[t.x*t.y];
	
	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	if (data == nullptr) {
		return;
	}

	// Create the icon pixmap
	int screen = DefaultScreen(_display);
	Visual *visual = DefaultVisual(_display, screen);
	unsigned int depth = DefaultDepth(_display, screen);
	XImage *image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, t.x, t.y, 32, 0);
	::Window root_window = XRootWindow(_display, screen);

	if (image == nullptr) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(_display, RootWindow(_display, screen), t.x, t.y, depth);
	GC gc = XCreateGC(_display, pixmap, 0, nullptr);
	
	XPutImage(_display, pixmap, gc, image, 0, 0, 0, 0, t.x, t.y);

	XColor color;

	color.flags = DoRed | DoGreen | DoBlue;
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	Cursor cursor = XCreatePixmapCursor(_display, pixmap, pixmap, &color, &color, 0, 0);

	// XUndefineCursor(_display, root_window);
	XDefineCursor(_display, root_window, cursor);
	XSync(_display, root_window);
	XFreePixmap(_display, pixmap);

	delete [] data;
	*/

#endif
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
