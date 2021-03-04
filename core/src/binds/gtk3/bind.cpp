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
Image *sg_back_buffer = nullptr;
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
static jrect_t<int> sg_visible_bounds;
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
static jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = jcursor_style_t::Default;

static jkeyevent_symbol_t TranslateToNativeKeySymbol(guint symbol)
{
	switch (symbol) {
		case GDK_Return:
		case GDK_KP_Enter:
			return jkeyevent_symbol_t::Enter;
		case GDK_BackSpace:
			return jkeyevent_symbol_t::Backspace;
		case GDK_Tab:
		case GDK_KP_Tab:
			return jkeyevent_symbol_t::Tab;
		//case DIKS_RETURN:
		//	return jkeyevent_symbol_t::Return;
		case GDK_Cancel:
			return jkeyevent_symbol_t::Cancel;
		case GDK_Escape:
			return jkeyevent_symbol_t::Escape;
		case GDK_space:
		case GDK_KP_Space:
			return jkeyevent_symbol_t::Space;
		case GDK_exclam:
			return jkeyevent_symbol_t::ExclamationMark;
		case GDK_quotedbl:
			return jkeyevent_symbol_t::Quotation;
		case GDK_numbersign:
			return jkeyevent_symbol_t::Hash;
		case GDK_dollar:
		case GDK_currency:
			return jkeyevent_symbol_t::Dollar;
		case GDK_percent:
			return jkeyevent_symbol_t::Percent;
		case GDK_ampersand:
			return jkeyevent_symbol_t::Ampersand;
		case GDK_apostrophe:
		// case GDK_quoteright:
			return jkeyevent_symbol_t::Aposthrophe;
		case GDK_parenleft:
			return jkeyevent_symbol_t::ParenthesisLeft;
		case GDK_parenright:
			return jkeyevent_symbol_t::ParenthesisRight;
		case GDK_asterisk:
		case GDK_KP_Multiply:
			return jkeyevent_symbol_t::Star;
		case GDK_plus:
		case GDK_KP_Add:
			return jkeyevent_symbol_t::Plus;
		case GDK_minus:
		case GDK_hyphen:
		case GDK_KP_Subtract:
			return jkeyevent_symbol_t::Minus;
		case GDK_period:
		case GDK_KP_Decimal:
			return jkeyevent_symbol_t::Period;
		case GDK_slash:
		case GDK_KP_Divide:
			return jkeyevent_symbol_t::Slash;
		case GDK_0:
		case GDK_KP_0:
			return jkeyevent_symbol_t::Number0;
		case GDK_1:
		case GDK_KP_1:
			return jkeyevent_symbol_t::Number1;
		case GDK_2:
		case GDK_KP_2:
			return jkeyevent_symbol_t::Number2;
		case GDK_3:
		case GDK_KP_3:
			return jkeyevent_symbol_t::Number3;
		case GDK_4:
		case GDK_KP_4:
			return jkeyevent_symbol_t::Number4;
		case GDK_5:
		case GDK_KP_5:
			return jkeyevent_symbol_t::Number5;
		case GDK_6:
		case GDK_KP_6:
			return jkeyevent_symbol_t::Number6;
		case GDK_7:
		case GDK_KP_7:
			return jkeyevent_symbol_t::Number7;
		case GDK_8:
		case GDK_KP_8:
			return jkeyevent_symbol_t::Number8;
		case GDK_9:
		case GDK_KP_9:
			return jkeyevent_symbol_t::Number9;
		case GDK_colon:
			return jkeyevent_symbol_t::Colon;
		case GDK_semicolon:
			return jkeyevent_symbol_t::SemiColon;
		case GDK_comma:
			return jkeyevent_symbol_t::Comma;
		case GDK_equal:
		case GDK_KP_Equal:
			return jkeyevent_symbol_t::Equals;
		case GDK_less:
			return jkeyevent_symbol_t::LessThan;
		case GDK_greater:
			return jkeyevent_symbol_t::GreaterThan;
		case GDK_question:
			return jkeyevent_symbol_t::QuestionMark;
		case GDK_at:
			return jkeyevent_symbol_t::At;
		case GDK_bracketleft:
			return jkeyevent_symbol_t::SquareBracketLeft;
		case GDK_backslash:
			return jkeyevent_symbol_t::BackSlash;
		case GDK_bracketright:
			return jkeyevent_symbol_t::SquareBracketRight;
		case GDK_asciicircum:
			return jkeyevent_symbol_t::CircumflexAccent;
		case GDK_underscore:
			return jkeyevent_symbol_t::Underscore;
		case GDK_acute:
			return jkeyevent_symbol_t::AcuteAccent;
		case GDK_grave:
		// case GDK_quoteleft:
			return jkeyevent_symbol_t::GraveAccent;
		case GDK_a:       
			return jkeyevent_symbol_t::a;
		case GDK_b:
			return jkeyevent_symbol_t::b;
		case GDK_c:
			return jkeyevent_symbol_t::c;
		case GDK_d:
			return jkeyevent_symbol_t::d;
		case GDK_e:
			return jkeyevent_symbol_t::e;
		case GDK_f:
			return jkeyevent_symbol_t::f;
		case GDK_g:
			return jkeyevent_symbol_t::g;
		case GDK_h:
			return jkeyevent_symbol_t::h;
		case GDK_i:
			return jkeyevent_symbol_t::i;
		case GDK_j:
			return jkeyevent_symbol_t::j;
		case GDK_k:
			return jkeyevent_symbol_t::k;
		case GDK_l:
			return jkeyevent_symbol_t::l;
		case GDK_m:
			return jkeyevent_symbol_t::m;
		case GDK_n:
			return jkeyevent_symbol_t::n;
		case GDK_o:
			return jkeyevent_symbol_t::o;
		case GDK_p:
			return jkeyevent_symbol_t::p;
		case GDK_q:
			return jkeyevent_symbol_t::q;
		case GDK_r:
			return jkeyevent_symbol_t::r;
		case GDK_s:
			return jkeyevent_symbol_t::s;
		case GDK_t:
			return jkeyevent_symbol_t::t;
		case GDK_u:
			return jkeyevent_symbol_t::u;
		case GDK_v:
			return jkeyevent_symbol_t::v;
		case GDK_w:
			return jkeyevent_symbol_t::w;
		case GDK_x:
			return jkeyevent_symbol_t::x;
		case GDK_y:
			return jkeyevent_symbol_t::y;
		case GDK_z:
			return jkeyevent_symbol_t::z;
		// case GDK_Cedilla:
		//	return jkeyevent_symbol_t::CapitalCedilla;
		case GDK_cedilla:
			return jkeyevent_symbol_t::Cedilla;
		case GDK_braceleft:
			return jkeyevent_symbol_t::CurlyBracketLeft;
		case GDK_bar:
		case GDK_brokenbar:
			return jkeyevent_symbol_t::VerticalBar;
		case GDK_braceright:
			return jkeyevent_symbol_t::CurlyBracketRight;
		case GDK_asciitilde:
			return jkeyevent_symbol_t::Tilde;
		case GDK_Delete:
		case GDK_KP_Delete:
			return jkeyevent_symbol_t::Delete;
		case GDK_Left:
		case GDK_KP_Left:
			return jkeyevent_symbol_t::CursorLeft;
		case GDK_Right:
		case GDK_KP_Right:
			return jkeyevent_symbol_t::CursorRight;
		case GDK_Up:
		case GDK_KP_Up:
			return jkeyevent_symbol_t::CursorUp;
		case GDK_Down:
		case GDK_KP_Down:
			return jkeyevent_symbol_t::CursorDown;
		case GDK_Break:
			return jkeyevent_symbol_t::Break;
		case GDK_Insert:
		case GDK_KP_Insert:
			return jkeyevent_symbol_t::Insert;
		case GDK_Home:
		case GDK_KP_Home:
			return jkeyevent_symbol_t::Home;
		case GDK_End:
		case GDK_KP_End:
			return jkeyevent_symbol_t::End;
		case GDK_Page_Up:
		case GDK_KP_Page_Up:
			return jkeyevent_symbol_t::PageUp;
		case GDK_Page_Down:
		case GDK_KP_Page_Down:
			return jkeyevent_symbol_t::PageDown;
		case GDK_Print:
			return jkeyevent_symbol_t::Print;
		case GDK_Pause:
			return jkeyevent_symbol_t::Pause;
		case GDK_Red:
			return jkeyevent_symbol_t::Red;
		case GDK_Green:
			return jkeyevent_symbol_t::Green;
		case GDK_Yellow:
			return jkeyevent_symbol_t::Yellow;
		case GDK_Blue:
			return jkeyevent_symbol_t::Blue;
		case GDK_F1:
			return jkeyevent_symbol_t::F1;
		case GDK_F2:
			return jkeyevent_symbol_t::F2;
		case GDK_F3:
			return jkeyevent_symbol_t::F3;
		case GDK_F4:
			return jkeyevent_symbol_t::F4;
		case GDK_F5:
			return jkeyevent_symbol_t::F5;
		case GDK_F6:
			return jkeyevent_symbol_t::F6;
		case GDK_F7:
			return jkeyevent_symbol_t::F7;
		case GDK_F8:
			return jkeyevent_symbol_t::F8;
		case GDK_F9:
			return jkeyevent_symbol_t::F9;
		case GDK_F10:
			return jkeyevent_symbol_t::F10;
		case GDK_F11:
			return jkeyevent_symbol_t::F11;
		case GDK_F12:
			return jkeyevent_symbol_t::F12;
		case GDK_Shift_L:
		case GDK_Shift_R:
			return jkeyevent_symbol_t::Shift;
		case GDK_Control_L:
		case GDK_Control_R:
			return jkeyevent_symbol_t::Control;
		case GDK_Alt_L:
		case GDK_Alt_R:
			return jkeyevent_symbol_t::Alt;
		case GDK_Meta_L:
		case GDK_Meta_R:
			return jkeyevent_symbol_t::Meta;
		case GDK_Super_L:
		case GDK_Super_R:
			return jkeyevent_symbol_t::Super;
		case GDK_Hyper_L:
		case GDK_Hyper_R:
			return jkeyevent_symbol_t::Hyper;
		case GDK_Sleep:
			return jkeyevent_symbol_t::Sleep;
		case GDK_Suspend:
			return jkeyevent_symbol_t::Suspend;
		case GDK_Hibernate:
			return jkeyevent_symbol_t::Hibernate;
		default: 
			break;
	}

	return jkeyevent_symbol_t::Unknown;
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

  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  g->Reset();
  g->SetCompositeFlags(jcomposite_t::Src);

  sg_jcanvas_window->Paint(g);

  g->Flush();

  cairo_surface_t *cairo_surface = g->GetCairoSurface();

  // cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);

  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Painted));

  return TRUE;
}

static gboolean OnKeyPressEvent(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  jkeyevent_type_t type = jkeyevent_type_t::Unknown;
	jkeyevent_modifiers_t mod = jkeyevent_modifiers_t::None;

	if (event->state & GDK_SHIFT_MASK) {
		mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Shift);
	} else if (event->state & GDK_CONTROL_MASK) {
		mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Control);
	} else if (event->state & GDK_MOD1_MASK) {
		mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Alt);
	} else if (event->state & GDK_SUPER_MASK) {
		mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Super);
	} else if (event->state & GDK_HYPER_MASK) {
		mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Hyper);
	} else if (event->state & GDK_META_MASK) {
		mod = jenum_t<jkeyevent_modifiers_t>{mod}.Or(jkeyevent_modifiers_t::Meta);
	}
	
	if (event->type == GDK_KEY_PRESS) {
		type = jkeyevent_type_t::Pressed;
	} else if (event->type == GDK_KEY_RELEASE	) {
		type = jkeyevent_type_t::Released;
	}

	jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event->keyval);

  sg_jcanvas_window->GetEventManager().PostEvent(new KeyEvent(sg_jcanvas_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));

	return FALSE;
}

static gboolean OnMouseMoveEvent(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  jmouseevent_button_t button = jmouseevent_button_t::None;
	jmouseevent_type_t type = jmouseevent_type_t::Moved;

	int mouse_x = event->x;
	int mouse_y = event->y;
	int mouse_z = 0;
	
	// handle (x,y) motion
	gdk_event_request_motions(event); // handles is_hint events

  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, jmouseevent_button_t::None, {mouse_x, mouse_y}, mouse_z));

  return TRUE;
}

static gboolean OnMousePressEvent(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  static jmouseevent_button_t buttons = jmouseevent_button_t::None;

  jmouseevent_button_t button = jmouseevent_button_t::None;
	jmouseevent_type_t type = jmouseevent_type_t::Unknown;

	int mouse_x = event->x; // event->x_root;
	int mouse_y = event->y; // event->y_root;
	int mouse_z = 0;
	
	if (event->button == 1) {
		button = jmouseevent_button_t::Button1;
	} else if (event->button == 2) {
		button = jmouseevent_button_t::Button3;
	} else if (event->button == 3) {
		button = jmouseevent_button_t::Button2;
	}

	if (event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS) {
		type = jmouseevent_type_t::Pressed;
    buttons = jenum_t<jmouseevent_button_t>{buttons}.Or(button);
  } else { // if (event->type == GDK_BUTTON_RELEASE) {
		type = jmouseevent_type_t::Released;
    buttons = jenum_t<jmouseevent_button_t>{buttons}.And(jenum_t<jmouseevent_button_t>{button}.Not());
  }

  if (sg_jcanvas_window->GetEventManager().IsAutoGrab() == true && buttons != jmouseevent_button_t::None) {
    gtk_grab_add(sg_window);
  } else {
    gtk_grab_remove(sg_window);
  }
  
  sg_jcanvas_window->GetEventManager().PostEvent(new MouseEvent(sg_jcanvas_window, type, button, buttons, {mouse_x, mouse_y}, mouse_z));

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
  
  sg_jcanvas_window->DispatchWindowEvent(new WindowEvent(sg_jcanvas_window, jwindowevent_type_t::Opened));
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

WindowAdapter::WindowAdapter(Window *parent, jrect_t<int> bounds)
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

jrect_t<int> WindowAdapter::GetBounds()
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

// t.x = gtk_widget_get_allocated_width(sg_widget);
// t.y = gtk_widget_get_allocated_height(sg_widget);

}
