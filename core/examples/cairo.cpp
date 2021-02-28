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

#include <thread>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo.h>
#include <cairo-xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace jcanvas;

int running = 1;

int cairo_check_event(cairo_surface_t *sfc, int block)
{
   char keybuf[8];
   KeySym key;
   XEvent e;

   for (;;) {
      if (block || XPending(cairo_xlib_surface_get_display(sfc))) {
         XNextEvent(cairo_xlib_surface_get_display(sfc), &e);
      } else  {
         return 0;
      }

      switch (e.type) {
         case KeyPress:
            XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
            return key;
         default:
           break;
      }
   }
}

static void fullscreen(Display* dpy, Window win)
{
  Atom atoms[2] = { 
    XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False), None 
  };

  XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char*) atoms, 1);
}

cairo_surface_t *cairo_create_x11_surface(int *x, int *y)
{
   Display *dsp;
   Drawable da;
   Screen *scr;
   int screen;
   cairo_surface_t *sfc;

   if ((dsp = XOpenDisplay(NULL)) == NULL)
      exit(1);

   screen = DefaultScreen(dsp);
   scr = DefaultScreenOfDisplay(dsp);

   if (!*x || !*y) {
      *x = WidthOfScreen(scr), *y = HeightOfScreen(scr);
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
      fullscreen (dsp, da);
   } else {
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
   }

   XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
   XMapWindow(dsp, da);

   sfc = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), *x, *y);

   cairo_xlib_surface_set_size(sfc, *x, *y);

   return sfc;
}

void cairo_close_x11_surface(cairo_surface_t *sfc)
{
   Display *dsp = cairo_xlib_surface_get_display(sfc);

   cairo_surface_destroy(sfc);
   XCloseDisplay(dsp);
}

void Framerate(int fps)
{
  static auto begin = std::chrono::steady_clock::now();
  static int index = 0;

  std::chrono::time_point<std::chrono::steady_clock> timestamp = begin + std::chrono::milliseconds(index++*(1000/fps));
  std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
  std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - current);

  if (diff.count() < 0) {
    return;
  }

  std::this_thread::sleep_for(diff);
}

int main(int argc, char **argv)
{
  int x = 0, y = 0;

  cairo_surface_t *sfc = cairo_create_x11_surface(&x, &y);
  cairo_t *ctx = cairo_create(sfc);

  while (running == 1) {
    Graphics g(sfc);

    cairo_push_group(ctx);

    int ca = random()%80 + 80;
    int cr = random()%255;
    int cg = random()%255;
    int cb = random()%255;

    g.SetColor(ca << 24 | cr << 16 | cg << 8 | cb);
    g.FillRectangle({(int)(random()%(x - 200)), (int)(random()%(y - 200)), 200, 200});

    cairo_pop_group_to_source(ctx);
    cairo_paint(ctx);
    cairo_surface_flush(sfc);
      
    switch (cairo_check_event(sfc, 0)) {
      case 65307: // escape
        running = 0;
        break;
    }

    Framerate(25);
  }

  cairo_destroy(ctx);
  cairo_close_x11_surface(sfc);

  return 0;
}

