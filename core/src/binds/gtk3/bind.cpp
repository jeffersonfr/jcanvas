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
#include <stdexcept>

#include <gtk/gtk.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkkeysyms-compat.h>

namespace jcanvas {

/** \brief */
jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static GtkApplication *sg_handler = nullptr;
/** \brief */
static GtkWidget *sg_window = nullptr;
/** \brief */
static GtkWidget *sg_frame = nullptr;
/** \brief */
static GtkWidget *sg_widget = nullptr;
/** \brief */
static jcanvas::jrect_t<int> sg_visible_bounds;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sgsg_jcanvas_cursor_enabled = true;
/** \brief */
static bool sg_visible = false;
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
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static jcanvas::jkeyevent_symbol_t TranslateToNativeKeySymbol(guint symbol)
{
	switch (symbol) {
		case GDK_Return:
		case GDK_KP_Enter:
			return jcanvas::JKS_ENTER;
		case GDK_BackSpace:
			return jcanvas::JKS_BACKSPACE;
		case GDK_Tab:
		case GDK_KP_Tab:
			return jcanvas::JKS_TAB;
		//case DIKS_RETURN:
		//	return jcanvas::JKS_RETURN;
		case GDK_Cancel:
			return jcanvas::JKS_CANCEL;
		case GDK_Escape:
			return jcanvas::JKS_ESCAPE;
		case GDK_space:
		case GDK_KP_Space:
			return jcanvas::JKS_SPACE;
		case GDK_exclam:
			return jcanvas::JKS_EXCLAMATION_MARK;
		case GDK_quotedbl:
			return jcanvas::JKS_QUOTATION;
		case GDK_numbersign:
			return jcanvas::JKS_NUMBER_SIGN;
		case GDK_dollar:
		case GDK_currency:
			return jcanvas::JKS_DOLLAR_SIGN;
		case GDK_percent:
			return jcanvas::JKS_PERCENT_SIGN;
		case GDK_ampersand:
			return jcanvas::JKS_AMPERSAND;
		case GDK_apostrophe:
		// case GDK_quoteright:
			return jcanvas::JKS_APOSTROPHE;
		case GDK_parenleft:
			return jcanvas::JKS_PARENTHESIS_LEFT;
		case GDK_parenright:
			return jcanvas::JKS_PARENTHESIS_RIGHT;
		case GDK_asterisk:
		case GDK_KP_Multiply:
			return jcanvas::JKS_STAR;
		case GDK_plus:
		case GDK_KP_Add:
			return jcanvas::JKS_PLUS_SIGN;
		case GDK_minus:
		case GDK_hyphen:
		case GDK_KP_Subtract:
			return jcanvas::JKS_MINUS_SIGN;
		case GDK_period:
		case GDK_KP_Decimal:
			return jcanvas::JKS_PERIOD;
		case GDK_slash:
		case GDK_KP_Divide:
			return jcanvas::JKS_SLASH;
		case GDK_0:
		case GDK_KP_0:
			return jcanvas::JKS_0;
		case GDK_1:
		case GDK_KP_1:
			return jcanvas::JKS_1;
		case GDK_2:
		case GDK_KP_2:
			return jcanvas::JKS_2;
		case GDK_3:
		case GDK_KP_3:
			return jcanvas::JKS_3;
		case GDK_4:
		case GDK_KP_4:
			return jcanvas::JKS_4;
		case GDK_5:
		case GDK_KP_5:
			return jcanvas::JKS_5;
		case GDK_6:
		case GDK_KP_6:
			return jcanvas::JKS_6;
		case GDK_7:
		case GDK_KP_7:
			return jcanvas::JKS_7;
		case GDK_8:
		case GDK_KP_8:
			return jcanvas::JKS_8;
		case GDK_9:
		case GDK_KP_9:
			return jcanvas::JKS_9;
		case GDK_colon:
			return jcanvas::JKS_COLON;
		case GDK_semicolon:
			return jcanvas::JKS_SEMICOLON;
		case GDK_comma:
			return jcanvas::JKS_COMMA;
		case GDK_equal:
		case GDK_KP_Equal:
			return jcanvas::JKS_EQUALS_SIGN;
		case GDK_less:
			return jcanvas::JKS_LESS_THAN_SIGN;
		case GDK_greater:
			return jcanvas::JKS_GREATER_THAN_SIGN;
		case GDK_question:
			return jcanvas::JKS_QUESTION_MARK;
		case GDK_at:
			return jcanvas::JKS_AT;
		case GDK_bracketleft:
			return jcanvas::JKS_SQUARE_BRACKET_LEFT;
		case GDK_backslash:
			return jcanvas::JKS_BACKSLASH;
		case GDK_bracketright:
			return jcanvas::JKS_SQUARE_BRACKET_RIGHT;
		case GDK_asciicircum:
			return jcanvas::JKS_CIRCUMFLEX_ACCENT;
		case GDK_underscore:
			return jcanvas::JKS_UNDERSCORE;
		case GDK_acute:
			return jcanvas::JKS_ACUTE_ACCENT;
		case GDK_grave:
		// case GDK_quoteleft:
			return jcanvas::JKS_GRAVE_ACCENT;
		case GDK_a:       
			return jcanvas::JKS_a;
		case GDK_b:
			return jcanvas::JKS_b;
		case GDK_c:
			return jcanvas::JKS_c;
		case GDK_d:
			return jcanvas::JKS_d;
		case GDK_e:
			return jcanvas::JKS_e;
		case GDK_f:
			return jcanvas::JKS_f;
		case GDK_g:
			return jcanvas::JKS_g;
		case GDK_h:
			return jcanvas::JKS_h;
		case GDK_i:
			return jcanvas::JKS_i;
		case GDK_j:
			return jcanvas::JKS_j;
		case GDK_k:
			return jcanvas::JKS_k;
		case GDK_l:
			return jcanvas::JKS_l;
		case GDK_m:
			return jcanvas::JKS_m;
		case GDK_n:
			return jcanvas::JKS_n;
		case GDK_o:
			return jcanvas::JKS_o;
		case GDK_p:
			return jcanvas::JKS_p;
		case GDK_q:
			return jcanvas::JKS_q;
		case GDK_r:
			return jcanvas::JKS_r;
		case GDK_s:
			return jcanvas::JKS_s;
		case GDK_t:
			return jcanvas::JKS_t;
		case GDK_u:
			return jcanvas::JKS_u;
		case GDK_v:
			return jcanvas::JKS_v;
		case GDK_w:
			return jcanvas::JKS_w;
		case GDK_x:
			return jcanvas::JKS_x;
		case GDK_y:
			return jcanvas::JKS_y;
		case GDK_z:
			return jcanvas::JKS_z;
		// case GDK_Cedilla:
		//	return jcanvas::JKS_CAPITAL_CEDILlA;
		case GDK_cedilla:
			return jcanvas::JKS_SMALL_CEDILLA;
		case GDK_braceleft:
			return jcanvas::JKS_CURLY_BRACKET_LEFT;
		case GDK_bar:
		case GDK_brokenbar:
			return jcanvas::JKS_VERTICAL_BAR;
		case GDK_braceright:
			return jcanvas::JKS_CURLY_BRACKET_RIGHT;
		case GDK_asciitilde:
			return jcanvas::JKS_TILDE;
		case GDK_Delete:
		case GDK_KP_Delete:
			return jcanvas::JKS_DELETE;
		case GDK_Left:
		case GDK_KP_Left:
			return jcanvas::JKS_CURSOR_LEFT;
		case GDK_Right:
		case GDK_KP_Right:
			return jcanvas::JKS_CURSOR_RIGHT;
		case GDK_Up:
		case GDK_KP_Up:
			return jcanvas::JKS_CURSOR_UP;
		case GDK_Down:
		case GDK_KP_Down:
			return jcanvas::JKS_CURSOR_DOWN;
		case GDK_Break:
			return jcanvas::JKS_BREAK;
		case GDK_Insert:
		case GDK_KP_Insert:
			return jcanvas::JKS_INSERT;
		case GDK_Home:
		case GDK_KP_Home:
			return jcanvas::JKS_HOME;
		case GDK_End:
		case GDK_KP_End:
			return jcanvas::JKS_END;
		case GDK_Page_Up:
		case GDK_KP_Page_Up:
			return jcanvas::JKS_PAGE_UP;
		case GDK_Page_Down:
		case GDK_KP_Page_Down:
			return jcanvas::JKS_PAGE_DOWN;
		case GDK_Print:
			return jcanvas::JKS_PRINT;
		case GDK_Pause:
			return jcanvas::JKS_PAUSE;
		case GDK_Red:
			return jcanvas::JKS_RED;
		case GDK_Green:
			return jcanvas::JKS_GREEN;
		case GDK_Yellow:
			return jcanvas::JKS_YELLOW;
		case GDK_Blue:
			return jcanvas::JKS_BLUE;
		case GDK_F1:
			return jcanvas::JKS_F1;
		case GDK_F2:
			return jcanvas::JKS_F2;
		case GDK_F3:
			return jcanvas::JKS_F3;
		case GDK_F4:
			return jcanvas::JKS_F4;
		case GDK_F5:
			return jcanvas::JKS_F5;
		case GDK_F6:
			return jcanvas::JKS_F6;
		case GDK_F7:
			return jcanvas::JKS_F7;
		case GDK_F8:
			return jcanvas::JKS_F8;
		case GDK_F9:
			return jcanvas::JKS_F9;
		case GDK_F10:
			return jcanvas::JKS_F10;
		case GDK_F11:
			return jcanvas::JKS_F11;
		case GDK_F12:
			return jcanvas::JKS_F12;
		case GDK_Shift_L:
		case GDK_Shift_R:
			return jcanvas::JKS_SHIFT;
		case GDK_Control_L:
		case GDK_Control_R:
			return jcanvas::JKS_CONTROL;
		case GDK_Alt_L:
		case GDK_Alt_R:
			return jcanvas::JKS_ALT;
		case GDK_Meta_L:
		case GDK_Meta_R:
			return jcanvas::JKS_META;
		case GDK_Super_L:
		case GDK_Super_R:
			return jcanvas::JKS_SUPER;
		case GDK_Hyper_L:
		case GDK_Hyper_R:
			return jcanvas::JKS_HYPER;
		case GDK_Sleep:
			return jcanvas::JKS_SLEEP;
		case GDK_Suspend:
			return jcanvas::JKS_SUSPEND;
		case GDK_Hibernate:
			return jcanvas::JKS_HIBERNATE;
		default: 
			break;
	}

	return jcanvas::JKS_UNKNOWN;
}

static gboolean OnDraw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	if (sg_jcanvas_window == nullptr || sg_jcanvas_window->IsVisible() == false) {
		return FALSE;
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

  cairo_surface_t *cairo_surface = g->GetCairoSurface();

  // cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));

  return TRUE;
}

static gboolean OnKeyPressEvent(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  jcanvas::jkeyevent_type_t type;
	jcanvas::jkeyevent_modifiers_t mod;

	mod = jcanvas::JKM_NONE;

	if (event->state & GDK_SHIFT_MASK) {
		mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
	} else if (event->state & GDK_CONTROL_MASK) {
		mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
	} else if (event->state & GDK_MOD1_MASK) {
		mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
	} else if (event->state & GDK_SUPER_MASK) {
		mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
	} else if (event->state & GDK_HYPER_MASK) {
		mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_HYPER);
	} else if (event->state & GDK_META_MASK) {
		mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_META);
	}
	
	type = (jcanvas::jkeyevent_type_t)(0);

	if (event->type == GDK_KEY_PRESS) {
		type = jcanvas::JKT_PRESSED;
	} else if (event->type == GDK_KEY_RELEASE	) {
		type = jcanvas::JKT_RELEASED;
	}

	jcanvas::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event->keyval);

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));

	return FALSE;
}

static gboolean OnMouseMoveEvent(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
	jcanvas::jmouseevent_type_t type = jcanvas::JMT_MOVED;

	int mouse_x = event->x;
	int mouse_y = event->y;
	int mouse_z = 0;
	
	// handle (x,y) motion
	gdk_event_request_motions(event); // handles is_hint events

  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, jcanvas::JMB_NONE, {mouse_x, mouse_y}, mouse_z));

  return TRUE;
}

static gboolean OnMousePressEvent(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  static jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;

  jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
	jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;

	int mouse_x = event->x; // event->x_root;
	int mouse_y = event->y; // event->y_root;
	int mouse_z = 0;
	
	if (event->button == 1) {
		button = jcanvas::JMB_BUTTON1;
	} else if (event->button == 2) {
		button = jcanvas::JMB_BUTTON3;
	} else if (event->button == 3) {
		button = jcanvas::JMB_BUTTON2;
	}

	if (event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS) {
		type = jcanvas::JMT_PRESSED;
    buttons = (jcanvas::jmouseevent_button_t)(buttons | button);
  } else { // if (event->type == GDK_BUTTON_RELEASE) {
		type = jcanvas::JMT_RELEASED;
    buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
  }

  if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
    gtk_grab_add(sg_window);
  } else {
    gtk_grab_remove(sg_window);
  }
  
  sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {mouse_x, mouse_y}, mouse_z));

  return TRUE;
}

static void OnClose(void)
{
	// gtk_window_close((GtkWindow *)sg_window);
}

static gboolean OnConfigureEvent(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data)
{
  gtk_window_get_position((GtkWindow *)sg_window, &sg_visible_bounds.point.x, &sg_visible_bounds.point.y);
  gtk_window_get_size((GtkWindow *)sg_window, &sg_visible_bounds.size.x, &sg_visible_bounds.size.y);

  gtk_widget_queue_draw(sg_widget);

  return TRUE;
}

static void ConfigureApplication(GtkApplication *app, gpointer user_data)
{
  sg_window = gtk_application_window_new(app);

  gtk_window_set_title(GTK_WINDOW(sg_window), "");
  gtk_window_set_default_size(GTK_WINDOW(sg_window), sg_visible_bounds.size.x, sg_visible_bounds.size.y);

  sg_frame = gtk_frame_new(nullptr);

  gtk_container_add(GTK_CONTAINER(sg_window), sg_frame);

  sg_widget = gtk_drawing_area_new();
  
  gtk_container_add(GTK_CONTAINER(sg_frame), sg_widget);

	g_signal_connect(G_OBJECT(sg_widget),"configure-event", G_CALLBACK (OnConfigureEvent), nullptr);
  g_signal_connect(sg_window, "destroy", G_CALLBACK(OnClose), nullptr);
  g_signal_connect(sg_widget, "draw", G_CALLBACK(OnDraw), nullptr);
	g_signal_connect(G_OBJECT(sg_window), "key_press_event", G_CALLBACK(OnKeyPressEvent), nullptr);
	g_signal_connect(G_OBJECT(sg_window), "key_release_event", G_CALLBACK(OnKeyPressEvent), nullptr);
	g_signal_connect(G_OBJECT(sg_window), "motion_notify_event", G_CALLBACK(OnMouseMoveEvent), nullptr);
	g_signal_connect(G_OBJECT(sg_window), "button_press_event", G_CALLBACK(OnMousePressEvent), nullptr);
	g_signal_connect(G_OBJECT(sg_window), "button_release_event", G_CALLBACK(OnMousePressEvent), nullptr);

  gtk_widget_set_events(
      sg_widget, gtk_widget_get_events(sg_widget) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
  
  gtk_widget_show_now(sg_window);
  gtk_widget_show_all(sg_window);
  
  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_OPENED));
}

void Application::Init(int argc, char **argv)
{
  gtk_init(&argc, &argv);

	GdkScreen *screen = gdk_screen_get_default();
  GdkDisplay *display = gdk_screen_get_display(screen);
  GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
  GdkRectangle geometry;

  gdk_monitor_get_geometry(monitor, &geometry);

	sg_screen.x = geometry.x;
	sg_screen.y = geometry.y;

  sg_quitting = false;
}

static void InternalPaint()
{
  while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      gtk_widget_queue_draw(GTK_WIDGET(sg_widget));
    }
  }
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  std::thread thread = std::thread(InternalPaint);

 	g_application_run(G_APPLICATION(sg_handler), 0, nullptr);

  sg_quitting = true;

  thread.join();
  
  sg_jcanvas_window->SetVisible(false);
}

jpoint_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  g_application_release(G_APPLICATION(sg_handler));
  g_application_quit(G_APPLICATION(sg_handler));

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
{
	if (sg_window != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

	sg_window = nullptr;
  sg_jcanvas_window = parent;
  sg_visible_bounds = bounds;

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_handler = gtk_application_new("jlibcpp.gtk", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(sg_handler, "activate", G_CALLBACK(ConfigureApplication), nullptr);

  sg_visible = true;
}

WindowAdapter::~WindowAdapter()
{
  // g_signalsg_handler_disconnect(sg_window, "destroy");
  // g_signalsg_handler_disconnect(sg_widget, "draw");
	// g_signalsg_handler_disconnect(G_OBJECT(sg_widget),"configure-event");
	// g_signalsg_handler_disconnect(G_OBJECT(sg_window), "key_press_event");
	// g_signalsg_handler_disconnect(G_OBJECT(sg_window), "key_release_event");
	// g_signalsg_handler_disconnect(G_OBJECT(sg_window), "motion_notify_event");
	// g_signalsg_handler_disconnect(G_OBJECT(sg_window), "button_press_event");
	// g_signalsg_handler_disconnect(G_OBJECT(sg_window), "button_release_event");

  g_object_unref(sg_widget);
  g_object_unref(sg_frame);
  g_object_unref(sg_window);
  g_object_unref(sg_handler);

  gtk_window_close((GtkWindow *)sg_window);
  // gtk_main_quit();
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
  // gtk_window_unfullscreen (GtkWindow *window);
  // gtk_window_fullscreen_on_monitor (GtkWindow *window, GdkScreen *screen, gint monitor);
	if (sg_fullscreen == false) {
    sg_fullscreen = true;
    
		gtk_window_fullscreen((GtkWindow *)sg_window);
	} else {
    sg_fullscreen = false;

		gtk_window_unfullscreen((GtkWindow *)sg_window);
	}

  gtk_widget_queue_draw(sg_widget);
}

void WindowAdapter::SetTitle(std::string title)
{
  if (sg_window != nullptr) {
	  gtk_window_set_title(GTK_WINDOW(sg_window), title.c_str());
  }
}

std::string WindowAdapter::GetTitle()
{
	return gtk_window_get_title(GTK_WINDOW(sg_window));
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
  if (sg_window != nullptr) {
	  gtk_window_set_decorated(GTK_WINDOW(sg_window), undecorated == false);
  }
}

bool WindowAdapter::IsUndecorated()
{
  return gtk_window_get_decorated(GTK_WINDOW(sg_window));
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
  gtk_window_move(GTK_WINDOW(sg_window), bounds.point.x, bounds.point.y);
  gtk_window_resize(GTK_WINDOW(sg_window), bounds.size.x, bounds.size.y);
	gtk_widget_set_size_request(sg_window, bounds.size.x, bounds.size.y);
}

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
  return sg_visible_bounds;
}

void WindowAdapter::SetResizable(bool resizable)
{
  gtk_window_set_resizable((GtkWindow *)sg_window, resizable);
}

bool WindowAdapter::IsResizable()
{
  return gtk_window_get_resizable((GtkWindow *)sg_window);
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

  // TODO::
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

  // TODO::

	return t;
}

void WindowAdapter::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == true) {
    gtk_widget_show(sg_window);
    // gtk_widget_show_all(sg_window);
  } else {
    gtk_widget_hide(sg_window);
  }
}

bool WindowAdapter::IsVisible()
{
  return sg_visible; 

  // INFO:: first calls return false ...
  // return (bool)gtk_widget_issg_visible(sg_window);
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
	sgsg_jcanvas_cursor_enabled = enabled;
}

bool WindowAdapter::IsCursorEnabled()
{
  return sgsg_jcanvas_cursor_enabled;
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  sg_jcanvas_cursor = style;
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

  */
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

// t.x = gtk_widget_get_allocated_width(sg_widget);
// t.y = gtk_widget_get_allocated_height(sg_widget);

}
