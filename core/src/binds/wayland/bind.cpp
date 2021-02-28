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
#include "../wayland/bind.h"

#include "jcanvas/core/jbufferedimage.h"
#include "jcanvas/core/jwindowadapter.h"
#include "jcanvas/core/japplication.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>

namespace jcanvas {

/** \brief */
static struct display _display;
/** \brief */
static ShmSurface *sg_surface = nullptr;
/** \brief */
static struct input _input;
/** \brief */
static bool _input_inited;
/** \brief */
static jcanvas::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jcanvas::jpoint_t<int> sg_screen = {0, 0};
/** \brief */
static jcanvas::jrect_t<int> sg_previous_bounds;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcanvas::Image *sg_jcanvas_icon = nullptr;
/** \brief */
static Window *sg_jcanvas_window = nullptr;
/** \brief */
static jcursor_style_t sg_jcanvas_cursor = JCS_DEFAULT;

static struct name_keyCode name_keyCode_map[] = {
  {"Return",      jcanvas::JKS_ENTER},
  {"KP_Enter",    jcanvas::JKS_ENTER},
  {"BackSpace",   jcanvas::JKS_BACKSPACE},
  {"Tab",         jcanvas::JKS_TAB},
  {"Cancel",      jcanvas::JKS_CANCEL},
  // {"Clear",       jcanvas::JKS_CLEAR},
  {"Shift_L",     jcanvas::JKS_SHIFT},
  {"Shift_R",     jcanvas::JKS_SHIFT},
  {"Control_L",   jcanvas::JKS_CONTROL},
  {"Control_R",   jcanvas::JKS_CONTROL},
  {"Alt_L",       jcanvas::JKS_ALT},
  {"Alt_R",       jcanvas::JKS_ALT},
  {"Pause",       jcanvas::JKS_PAUSE},
  // {"Caps_Lock",   jcanvas::JKS_CAPS_LOCK},
  {"Escape",      jcanvas::JKS_ESCAPE},
  {"Prior",       jcanvas::JKS_PAGE_UP},
  {"KP_Prior",    jcanvas::JKS_PAGE_UP},
  {"Next",        jcanvas::JKS_PAGE_DOWN},
  {"KP_Next",     jcanvas::JKS_PAGE_DOWN},
  {"End",         jcanvas::JKS_END},
  {"KP_End",      jcanvas::JKS_END},
  {"Home",        jcanvas::JKS_HOME},
  {"KP_Home",     jcanvas::JKS_HOME},
  {"Left",        jcanvas::JKS_CURSOR_LEFT},
  {"KP_Left",     jcanvas::JKS_CURSOR_LEFT},
  {"Up",          jcanvas::JKS_CURSOR_UP},
  {"KP_Up",       jcanvas::JKS_CURSOR_UP},
  {"Right",       jcanvas::JKS_CURSOR_RIGHT},
  {"KP_Right",    jcanvas::JKS_CURSOR_RIGHT},
  {"Down",        jcanvas::JKS_CURSOR_DOWN},
  {"KP_Down",     jcanvas::JKS_CURSOR_DOWN},
  {"comma",       jcanvas::JKS_COMMA},
  {"minus",       jcanvas::JKS_MINUS_SIGN},
  {"KP_Subtract", jcanvas::JKS_MINUS_SIGN},
  {"period",      jcanvas::JKS_PERIOD},
  {"KP_Decimal",  jcanvas::JKS_PERIOD},
  {"slash",       jcanvas::JKS_SLASH},
  {"space",       jcanvas::JKS_SPACE},
  {"0",           jcanvas::JKS_0},
  {"KP_0",        jcanvas::JKS_0},
  {"1",           jcanvas::JKS_1},
  {"KP_1",        jcanvas::JKS_1},
  {"2",           jcanvas::JKS_2},
  {"KP_2",        jcanvas::JKS_2},
  {"3",           jcanvas::JKS_3},
  {"KP_3",        jcanvas::JKS_3},
  {"4",           jcanvas::JKS_4},
  {"KP_4",        jcanvas::JKS_4},
  {"5",           jcanvas::JKS_5},
  {"KP_5",        jcanvas::JKS_5},
  {"6",           jcanvas::JKS_6},
  {"KP_6",        jcanvas::JKS_6},
  {"7",           jcanvas::JKS_7},
  {"KP_7",        jcanvas::JKS_7},
  {"8",           jcanvas::JKS_8},
  {"KP_8",        jcanvas::JKS_8},
  {"9",           jcanvas::JKS_9},
  {"KP_9",        jcanvas::JKS_9},
  {"semicolon",   jcanvas::JKS_SEMICOLON},
  {"equal",       jcanvas::JKS_EQUALS_SIGN},
  {"a",           jcanvas::JKS_a},
  {"A",           jcanvas::JKS_a},
  {"b",           jcanvas::JKS_b},
  {"B",           jcanvas::JKS_b},
  {"c",           jcanvas::JKS_c},
  {"C",           jcanvas::JKS_c},
  {"d",           jcanvas::JKS_d},
  {"D",           jcanvas::JKS_d},
  {"e",           jcanvas::JKS_e},
  {"E",           jcanvas::JKS_e},
  {"f",           jcanvas::JKS_f},
  {"F",           jcanvas::JKS_f},
  {"g",           jcanvas::JKS_g},
  {"G",           jcanvas::JKS_g},
  {"h",           jcanvas::JKS_h},
  {"H",           jcanvas::JKS_h},
  {"i",           jcanvas::JKS_i},
  {"I",           jcanvas::JKS_i},
  {"j",           jcanvas::JKS_j},
  {"J",           jcanvas::JKS_j},
  {"k",           jcanvas::JKS_k},
  {"K",           jcanvas::JKS_k},
  {"l",           jcanvas::JKS_l},
  {"L",           jcanvas::JKS_l},
  {"m",           jcanvas::JKS_m},
  {"M",           jcanvas::JKS_m},
  {"n",           jcanvas::JKS_n},
  {"N",           jcanvas::JKS_n},
  {"o",           jcanvas::JKS_o},
  {"O",           jcanvas::JKS_o},
  {"p",           jcanvas::JKS_p},
  {"P",           jcanvas::JKS_p},
  {"q",           jcanvas::JKS_q},
  {"Q",           jcanvas::JKS_q},
  {"r",           jcanvas::JKS_r},
  {"R",           jcanvas::JKS_r},
  {"s",           jcanvas::JKS_s},
  {"S",           jcanvas::JKS_s},
  {"t",           jcanvas::JKS_t},
  {"T",           jcanvas::JKS_t},
  {"u",           jcanvas::JKS_u},
  {"U",           jcanvas::JKS_u},
  {"v",           jcanvas::JKS_v},
  {"V",           jcanvas::JKS_v},
  {"w",           jcanvas::JKS_w},
  {"W",           jcanvas::JKS_w},
  {"x",           jcanvas::JKS_x},
  {"X",           jcanvas::JKS_x},
  {"y",           jcanvas::JKS_y},
  {"Y",           jcanvas::JKS_y},
  {"z",           jcanvas::JKS_z},
  {"Z",           jcanvas::JKS_z},
  {"bracketleft", jcanvas::JKS_SQUARE_BRACKET_LEFT},
  {"backslash",   jcanvas::JKS_BACKSLASH},
  {"bracketright",jcanvas::JKS_SQUARE_BRACKET_RIGHT},
  {"asterisk",    jcanvas::JKS_STAR},
  {"KP_Multiply", jcanvas::JKS_STAR},
  {"plus",        jcanvas::JKS_PLUS_SIGN},
  {"KP_Add",      jcanvas::JKS_PLUS_SIGN},
  {"KP_Divide",   jcanvas::JKS_SLASH},
  {"Delete",      jcanvas::JKS_DELETE},
  {"KP_Delete",   jcanvas::JKS_DELETE},
  // {"Num_Lock",    jcanvas::JKS_NUM_LOCK},
  // {"Scroll_Lock", jcanvas::JKS_SCROLL_LOCK},
  {"F1",          jcanvas::JKS_F1},
  {"F2",          jcanvas::JKS_F2},
  {"F3",          jcanvas::JKS_F3},
  {"F4",          jcanvas::JKS_F4},
  {"F5",          jcanvas::JKS_F5},
  {"F6",          jcanvas::JKS_F6},
  {"F7",          jcanvas::JKS_F7},
  {"F8",          jcanvas::JKS_F8},
  {"F9",          jcanvas::JKS_F9},
  {"F10",         jcanvas::JKS_F10},
  {"F11",         jcanvas::JKS_F11},
  {"F12",         jcanvas::JKS_F12},
  // {"F13",         jcanvas::JKS_F13},
  // {"F14",         jcanvas::JKS_F14},
  // {"F15",         jcanvas::JKS_F15},
  // {"F16",         jcanvas::JKS_F16},
  // {"F17",         jcanvas::JKS_F17},
  // {"F18",         jcanvas::JKS_F18},
  // {"F19",         jcanvas::JKS_F19},
  // {"F20",         jcanvas::JKS_F20},
  // {"F21",         jcanvas::JKS_F21},
  // {"F22",         jcanvas::JKS_F22},
  // {"F23",         jcanvas::JKS_F23},
  // {"F24",         jcanvas::JKS_F24},
  {"Insert",      jcanvas::JKS_INSERT},
  // {"Help",        jcanvas::JKS_HELP},
  {"grave",       jcanvas::JKS_GRAVE_ACCENT},
  {"apostrophe",  jcanvas::JKS_APOSTROPHE},
  {"bar",         jcanvas::JKS_VERTICAL_BAR},
  {"ampersand",   jcanvas::JKS_AMPERSAND},
  {"quotedbl",    jcanvas::JKS_QUOTATION},
  {"less",        jcanvas::JKS_LESS_THAN_SIGN},
  {"greater",     jcanvas::JKS_GREATER_THAN_SIGN},
  // {"braceleft",   jcanvas::JKS_BRACELEFT},
  // {"braceright",  jcanvas::JKS_BRACERIGHT},
  {"at",          jcanvas::JKS_AT},
  {"colon",       jcanvas::JKS_COLON},
  {"asciicircum", jcanvas::JKS_CIRCUMFLEX_ACCENT},
  {"dollar",      jcanvas::JKS_DOLLAR_SIGN},
  {"exclam",      jcanvas::JKS_EXCLAMATION_MARK},
  {"parenleft",   jcanvas::JKS_PARENTHESIS_LEFT},
  {"numbersign",  jcanvas::JKS_NUMBER_SIGN},
  {"parenright",  jcanvas::JKS_PARENTHESIS_RIGHT},
  {"underscore",  jcanvas::JKS_UNDERSCORE},
  {"percent",     jcanvas::JKS_5},
  {"asciitilde",  jcanvas::JKS_APOSTROPHE},
  {"question",    jcanvas::JKS_SLASH},
  {nullptr, jcanvas::JKS_UNKNOWN}
};

static void display_handle_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t w, int32_t h, int32_t subpixel, const char *make, const char *model, int32_t transform) 
{
  printf("display_handle_geometry:: %dx%d+%d+%d, subpixel: %d, model: %s, transform: %s\n", x, y, w, h, subpixel, make, model);
}

static void display_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t w, int32_t h, int32_t refresh) 
{
  _display.width = w;
  _display.height = h;
}

static const struct wl_output_listener output_listener = {
  display_handle_geometry,
  display_handle_mode
};

bool init_input(struct input* input);

static void registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) 
{
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    _display.compositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, WL_VERSION);
  } else if (strcmp(interface, wl_shell_interface.name) == 0) {
    _display.shell = (wl_shell*)wl_registry_bind(registry, id, &wl_shell_interface, WL_VERSION);
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    _display.output = (wl_output*)wl_registry_bind(registry, id, &wl_output_interface, WL_VERSION);
    wl_output_add_listener(_display.output, &output_listener, NULL);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    _display.shm = (wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, WL_VERSION);
  } else if (strcmp(interface, wl_subcompositor_interface.name) == 0) {
    _display.subcompositor = (wl_subcompositor*)wl_registry_bind(registry, id, &wl_subcompositor_interface, WL_VERSION);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    _input.seat = (wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 2);
    _input.display = _display.display;

    if (!init_input(&_input)) {
      fprintf(stderr, "Failed to initialize input\n");
      abort();
    } else {
      _input_inited = true;
    }
  }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id) 
{
}

static const struct wl_registry_listener registry_listener = {
  registry_handler,
  registry_remover
};

bool wayland_init() 
{
  _display.display = wl_display_connect(NULL);

  if (_display.display == NULL) {
    return false;
  }

  _display.registry = wl_display_get_registry(_display.display);

  if (_display.registry == NULL) {
    return false;
  }

  wl_registry_add_listener(_display.registry, &registry_listener, NULL);

  wl_display_dispatch(_display.display);
  wl_display_roundtrip(_display.display);

  return true;
}

bool is_input_inited() 
{
  return _input_inited;
}

static void unwatch_fd(struct input* input, int fd) {
  epoll_ctl(input->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void cleanup_event(struct input* input) {
  unwatch_fd(input, input->repeat_timer_fd);
  unwatch_fd(input, wl_display_get_fd(input->display));

  close(input->epoll_fd);
}

struct input * get_input()
{
  return &_input;
}

Event* next_event() 
{
  struct input* input = get_input();
  Event* event = NULL;
  
  pthread_mutex_lock(&input->lock);
  
  if (!wl_list_empty(&input->event_list)) {
    wl_list_for_each_reverse(event, &input->event_list, link) {
      wl_list_remove(&event->link);
  
      break;
    }
  }

  pthread_mutex_unlock(&input->lock);

  return event;
}

void drain_events()
{
  Event* event;

  while((event = next_event()) != NULL) {
    free(event);
  }
}

void destroy_input(struct input* input)
{
  cleanup_event(input);
  close(input->repeat_timer_fd);

  drain_events();

  if (input->touch != NULL) {
    wl_touch_release(input->touch);
  }

  if (input->pointer != NULL) {
    wl_pointer_release(input->pointer);
  }

  if (input->keyboard != NULL) {
    wl_keyboard_release(input->keyboard);
  }
}

void wayland_cleanup() 
{
  if (_display.compositor != NULL) {
    wl_compositor_destroy(_display.compositor);
  }

  if (_display.subcompositor != NULL) {
    wl_subcompositor_destroy(_display.subcompositor);
  }

  if (_display.shell != NULL) {
    wl_shell_destroy(_display.shell);
  }

  if (_display.shm != NULL) {
    wl_shm_destroy(_display.shm);
  }

  if (_display.output != NULL) {
    wl_output_destroy(_display.output);
  }

  destroy_input(&_input);

  if (_display.display != NULL) {
    wl_display_disconnect(_display.display);
  }
}

int32_t get_display_height() 
{
  return _display.height;
}

int32_t get_display_width() 
{
  return _display.width;
}

void display_flush()
{
  wl_display_dispatch_pending(_display.display);
  wl_display_flush(_display.display);
}

struct display * get_display() 
{ 
  return &_display; 
}

static void shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial) 
{
  wl_shell_surface_pong(shell_surface, serial);
}

static void shell_surface_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) 
{
}

static struct wl_shell_surface_listener shell_surface_listener = {
  .ping = shell_surface_ping,
  .configure = shell_surface_configure
};

static struct wl_surface* make_surface() {
  struct display* display = get_display();

  return wl_compositor_create_surface(display->compositor);
}

static struct wl_shell_surface* make_shell_surface(struct wl_surface* surface)
{
  struct display* display = get_display();
  struct wl_shell_surface* shell_surface = wl_shell_get_shell_surface(display->shell, surface);

  if (shell_surface == NULL) {
    return NULL;
  }

  wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, NULL);
  wl_shell_surface_set_toplevel(shell_surface);

  return shell_surface;
}

static struct wl_shm_pool* make_shm_pool(int32_t width, int32_t height, int32_t pixel_depth, void** addr) 
{
  struct wl_shm_pool  *pool;
  char tmp[] = "/tmp/wayland_mmap_XXXXXX";
  int fd;

  int32_t stride = width * pixel_depth;
  int32_t size = stride * height;

  fd = mkostemp(tmp, O_CLOEXEC); //, O_RDWR | O_CREAT | O_TRUNC);
  
  if (fd < 0) {
    return NULL;
  }

  if (ftruncate(fd, (size_t)size) < 0) {
    return NULL;
  }

  void* map = mmap(NULL, (size_t)size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  if (map == MAP_FAILED) {
    close(fd);
    return NULL;
  }

  *addr = map;

  pool = wl_shm_create_pool(get_display()->shm, fd, size);

  return pool;
}

static struct wl_buffer* make_buffer(struct wl_shm_pool *pool, int32_t width, int32_t height, uint32_t format, int32_t pixel_depth) 
{
  return wl_shm_pool_create_buffer(pool, 0, width, height, width * pixel_depth, format);
}

ShmSurface* CreateShmScreenSurface(int64_t id, int32_t x, int32_t y, int32_t width, int32_t height, int32_t pixel_depth) 
{
  struct wl_surface*          surface;
  struct wl_shell_surface*    shell_surface;

  struct wl_shm_pool*         pool;
  struct wl_buffer*           buffer;
  void*                       content;

  surface = make_surface();
  
  if (surface == NULL) {
    return NULL;
  }

  shell_surface = make_shell_surface(surface);
  
  if (shell_surface == NULL) {
    wl_surface_destroy(surface);
  
    return NULL;
  }

  pool = make_shm_pool(get_display_width(), get_display_height(), pixel_depth, &content);
  
  if (pool == NULL) {
    wl_surface_destroy(surface);
    wl_shell_surface_destroy(shell_surface);
  
    return NULL;
  }

  buffer = make_buffer(pool, width, height, WL_SHM_FORMAT_XRGB8888, pixel_depth);
 
  if (buffer == NULL) {
    wl_shm_pool_destroy(pool);
    wl_surface_destroy(surface);
    wl_shell_surface_destroy(shell_surface);
  
    return NULL;
  }

  struct shm_surface* ssf = (struct shm_surface*)malloc(sizeof(struct shm_surface));
  
  if (ssf == NULL) {
    wl_buffer_destroy(buffer);
    wl_shm_pool_destroy(pool);
    wl_surface_destroy(surface);
    wl_shell_surface_destroy(shell_surface);
  
    return NULL;
  }

  memset((void*)ssf, 0, sizeof(struct shm_surface));

  ssf->surface = surface;
  ssf->shell_surface = shell_surface;
  ssf->buffer = buffer;
  ssf->pool = pool;

  ssf->content = content;
  ssf->id = id;
  ssf->x = x;
  ssf->y = y;
  ssf->width = width;
  ssf->height = height;
  ssf->pixel_depth = pixel_depth;
  ssf->format = WL_SHM_FORMAT_XRGB8888;

  wl_surface_set_user_data(surface, ssf);

  wl_surface_attach(surface, buffer, 0, 0);
  wl_surface_commit(surface);

  return ssf;
}

void DestroyShmScreenSurface(ShmSurface* surf) 
{
  struct input* input = get_input();
  
  input->activeSurface = NULL;

  wl_buffer_destroy(surf->buffer);
  wl_shm_pool_destroy(surf->pool);
  wl_surface_destroy(surf->surface);
  wl_shell_surface_destroy(surf->shell_surface);

  free(surf);
}

void UnmapShmScreenSurface(ShmSurface* surf) 
{
  wl_surface_attach(surf->surface, NULL, 0, 0);
  wl_surface_commit(surf->surface);
  wl_buffer_destroy(surf->buffer);
  surf->buffer = NULL;
  surf->width = 0;
  surf->height = 0;
}

bool RemapShmScreenSurface(ShmSurface* surface, int32_t width, int32_t height) 
{
  struct wl_buffer* buffer = make_buffer(surface->pool, width, height, WL_SHM_FORMAT_XRGB8888, surface->pixel_depth);
  
  if (buffer == NULL) {
    return false;
  }
 
  surface->width = width;
  surface->height = height;

  wl_surface_attach(surface->surface, buffer, 0, 0);
  wl_surface_damage(surface->surface, 0, 0, width, height);
  wl_surface_commit(surface->surface);
  surface->buffer = buffer;
  
  return true;
}

void ResizeShmScreenSurface(ShmSurface* surf, int32_t width, int32_t height) 
{
  UnmapShmScreenSurface(surf);
  RemapShmScreenSurface(surf, width, height);
}

uint32_t find_keycode_by_name(const char* name)
{
  struct name_keyCode* map = name_keyCode_map;
  
  for (; map->name != NULL; map ++) {
    if (strcmp(map->name, name) == 0) {
      return (uint32_t)map->code;
    }
  }
  
  return 0;
}

static void append_event(Event* event)
{
  struct input* input = get_input();
  
  event->modifiers = input->modifiers;
  
  pthread_mutex_lock(&input->lock);
  
  wl_list_insert(&input->event_list, &event->link);
  
  pthread_mutex_unlock(&input->lock);
}

void new_mouse_event(enum MouseAction action, long id, int32_t x, int32_t y, uint32_t button, uint32_t click_count)
{
  Event* event = (Event*)malloc(sizeof(Event));

  if (event != NULL) {
    event->kind = MOUSE_EVENT;
    event->id = id;
    event->e.m.action = action;
    event->e.m.button = button;
    event->e.m.x = x;
    event->e.m.y = y;
    event->e.m.click_count = click_count;
  
    append_event(event);
  }
}

void new_key_event(enum KeyAction action, long id, uint32_t keyCode, char keyChar)
{
  Event* event = (Event*)malloc(sizeof(Event));

  if (event != NULL) {
    event->kind = KEY_EVENT;
    event->id = id;
    event->e.k.action = action;
    event->e.k.keyCode = keyCode;
    event->e.k.keyChar = keyChar;
    append_event(event);
  }
}

void new_display_flush_event(ShmSurface* surface)
{
  Event* event = (Event*)malloc(sizeof(Event));

  if (event != NULL) {
    event->kind = DISPLAY_EVENT;
    event->e.d.surface = surface;
    clock_gettime(CLOCK_REALTIME, &event->e.d.timestamp);
  
    append_event(event);
  }
}

void new_surface_event(enum SurfaceAction action, ShmSurface* surface, int32_t width, int32_t height) {
  Event* event = (Event*)malloc(sizeof(Event));
  
  if (event != NULL) {
    event->kind = SURFACE_EVENT;
    event->e.s.action = action;
    event->e.s.surface = surface;
    event->e.s.width = width;
    event->e.s.height = height;
  
    append_event(event);
  }
}

long timestamp_diff(struct timespec* ts1, struct timespec* ts2)
{
  return (ts1->tv_sec * 1000 + ts1->tv_nsec / 1000000) - (ts2->tv_sec * 1000 + ts2->tv_nsec / 1000000);
}

static void wayland_event_handler()
{
  struct input* input = get_input();

  wl_display_dispatch(input->display);
}

static void repeat_timer_handler()
{
  struct input* input = get_input();
  uint64_t exp;

  if (read(input->repeat_timer_fd, &exp, sizeof(exp)) == 0LL) {
    ;
  }

  uint32_t keycode = input->repeat_keycode;
  char ch = input->repeat_char;

  // Nothing to repeat
  if (keycode == 0) {
    return;
  }

  ShmSurface* activeSurface = input->activeSurface;
  
  if (activeSurface == NULL) {
    return;
  }

  // Generate key repeat
  new_key_event(KEY_PRESS, activeSurface->id, keycode, ch);
  
  if (ch != '\0') {
    new_key_event(KEY_TYPE, activeSurface->id, keycode, ch);
  }
}

static void watch_fd(struct input* input, int fd, uint32_t events, EventHandler fn)
{
  struct epoll_event ep;

  ep.events = events;
  ep.data.ptr = (void *)fn;

  epoll_ctl(input->epoll_fd, EPOLL_CTL_ADD, fd, &ep);
}

bool init_event(struct input* input)
{
  input->epoll_fd = epoll_create(1);

  if (input->epoll_fd == -1) {
    return false;
  }

  // Register Wayland event handler
  watch_fd(input, wl_display_get_fd(input->display), EPOLLIN, wayland_event_handler);
  // Register keyboard repeat timer
  watch_fd(input, input->repeat_timer_fd, EPOLLIN, repeat_timer_handler);

  return true;
}

static void handle_display_event(Event* evt)
{
  ShmSurface* surface= evt->e.d.surface;
  
  // A pending flush event, that occurs earlier than most recent flush, should be executed.
  if (timestamp_diff(&evt->e.d.timestamp, &surface->last_update) >= 0) {
    clock_gettime(CLOCK_REALTIME, &surface->last_update);
    wl_surface_attach(surface->surface, surface->buffer, 0, 0);
    wl_surface_commit(surface->surface);
  
    display_flush();
  }
}

static void handle_surface_event(Event* evt)
{
  ShmSurface* surface= evt->e.s.surface;

  switch(evt->e.s.action) {
    case SURFACE_DISPOSE:
      printf("DISPOSE\n");
      DestroyShmScreenSurface(surface);
  
      break;
    case SURFACE_UNMAP:
      printf("UN-MAP\n");
      UnmapShmScreenSurface(surface);
      
      break;
    case SURFACE_MAP:
      printf("MAP\n");
      RemapShmScreenSurface(surface, surface->width, surface->height);
      
      break;
  }
}

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size) 
{
  struct input* input = (struct input*)data;
  struct xkb_keymap *keymap;
  struct xkb_state *state;
  char *map_str;

  if (data == NULL) {
    close(fd);
 
    return;
  }

  if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
    close(fd);
    
    return;
  }

  map_str = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  
  if (map_str == MAP_FAILED) {
    close(fd);
    
    return;
  }

  keymap = xkb_keymap_new_from_string(input->xkb_context, map_str, XKB_KEYMAP_FORMAT_TEXT_V1, (xkb_keymap_compile_flags)0);
  
  munmap(map_str, size);
  close(fd);

  if (!keymap) {
    return;
  }

  state = xkb_state_new(keymap);
  
  if (!state) {
    xkb_keymap_unref(keymap);
  
    return;
  }

  xkb_keymap_unref(input->xkb.keymap);
  xkb_state_unref(input->xkb.state);
  
  input->xkb.keymap = keymap;
  input->xkb.state = state;

  input->xkb.control_mask = (xkb_mod_mask_t)(1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Control"));
  input->xkb.alt_mask = (xkb_mod_mask_t)(1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Mod1"));
  input->xkb.shift_mask = (xkb_mod_mask_t)(1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Shift"));
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
{
  if (surface != NULL) {
    ShmSurface* activeSurface = (ShmSurface*)wl_surface_get_user_data(surface);
    struct input* input = get_input();
    input->activeSurface = activeSurface;
  }
}

static void stop_keyboard_repeat(struct input* input)
{
  struct itimerspec   its;

  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  
  timerfd_settime(input->repeat_timer_fd, 0, &its, NULL);
  
  input->repeat_char = '\0';
  input->repeat_keycode = 0;
}

static void start_keyboard_repeat(struct input* input, char ch, uint32_t keycode)
{
  struct itimerspec its;

  input->repeat_char = ch;
  input->repeat_keycode = keycode;

  its.it_interval.tv_sec = input->repeat_rate_sec;
  its.it_interval.tv_nsec = input->repeat_rate_nsec;
  its.it_value.tv_sec = input->repeat_delay_sec;
  its.it_value.tv_nsec = input->repeat_delay_nsec;
  
  timerfd_settime(input->repeat_timer_fd, 0, &its, NULL);
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
{
  struct input* input = (struct input*)data;

  stop_keyboard_repeat(input);
  
  input->activeSurface = NULL;
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state_w)
{
  struct input*              input = (struct input*)data;
  enum wl_keyboard_key_state state = (wl_keyboard_key_state)state_w;
  uint32_t                   code;
  int                        num_syms;
  const xkb_keysym_t         *syms;
  xkb_keysym_t               sym;

  code = key + 8;
 
  if (!input->xkb.state) {
    return;
  }

  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  num_syms = xkb_state_key_get_syms(input->xkb.state, code, &syms);
  
  if (num_syms == 1) {
    sym = syms[0];
  } else {
    return;
  }

  char buffer[128];
  char ch = '\0';

  // The key has valid utf8 character
  if (xkb_keysym_to_utf8(sym, buffer, sizeof(buffer)) > 0) {
    ch = buffer[0];

    if (ch == '\r') {
      ch = '\n';
    }
  }

  uint32_t mapped_keycode = code;
  
  if (xkb_keysym_get_name(sym, buffer, sizeof(buffer)) > 0) {
    mapped_keycode = find_keycode_by_name(buffer);
  
    if (mapped_keycode == 0) {
      return;
    }
  } else {
    return;
  }

  if (state == WL_KEYBOARD_KEY_STATE_RELEASED) { // && key == input->repeat_key) {
    stop_keyboard_repeat(input);
    new_key_event(KEY_RELEASE, activeSurface->id, mapped_keycode, ch);
  } else if (state == WL_KEYBOARD_KEY_STATE_PRESSED && xkb_keymap_key_repeats(input->xkb.keymap, code)) {
    input->repeat_key = key;

    new_key_event(KEY_PRESS, activeSurface->id, mapped_keycode, ch);
 
    if (ch != '\0') {
      new_key_event(KEY_TYPE, activeSurface->id, mapped_keycode, ch);
    }

    start_keyboard_repeat(input, ch, mapped_keycode);
  }
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
  struct input* input = (struct input*)data;
  xkb_mod_mask_t mask;

  // If we're not using a keymap, then we don't handle PC-style modifiers
  if (!input->xkb.keymap) {
    return;
  }

  xkb_state_update_mask(input->xkb.state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
  
  mask = xkb_state_serialize_mods(input->xkb.state, (xkb_state_component)(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));
  input->modifiers = 0;
  
  if (mask & input->xkb.control_mask) {
    input->modifiers |= MOD_CONTROL_MASK;
  }

  if (mask & input->xkb.alt_mask) {
    input->modifiers |= MOD_ALT_MASK;
  }

  if (mask & input->xkb.shift_mask) {
    input->modifiers |= MOD_SHIFT_MASK;
  }
}

static void keyboard_handle_repeat_info(void *data, struct wl_keyboard *keyboard, int32_t rate, int32_t delay) {

  struct input* input = (struct input*)data;

  if (rate == 0) return;

  if (rate == 1) {
    input->repeat_rate_sec = 1;
    input->repeat_rate_nsec = 0;
  } else {
    input->repeat_rate_sec = 0;
    input->repeat_rate_nsec = 1000000000 / rate;
  }

  input->repeat_delay_sec = delay / 1000;
  input->repeat_delay_nsec = (delay % 1000) * 1000 * 1000;
}

static const struct wl_keyboard_listener keyboard_listener = {
  keyboard_handle_keymap,
  keyboard_handle_enter,
  keyboard_handle_leave,
  keyboard_handle_key,
  keyboard_handle_modifiers,
  keyboard_handle_repeat_info
};

static void pointer_handle_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t sx_w, wl_fixed_t sy_w)
{
  if (surface != NULL) {
    int x = wl_fixed_to_int(sx_w);
    int y = wl_fixed_to_int(sy_w);
    ShmSurface* activeSurface = (ShmSurface*)wl_surface_get_user_data(surface);
    struct input* input = get_input();
    
    input->activeSurface = activeSurface;
    input->x = x;
    input->y = y;
    
    if (activeSurface != NULL) {
      new_mouse_event(MOUSE_ENTER, activeSurface->id, x, y, 0, 0);
    }
  }
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface)
{
  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;

  input->activeSurface = NULL;
  input->x = -1;
  input->y = -1;
  
  if (activeSurface != NULL) {
    new_mouse_event(MOUSE_LEAVE, activeSurface->id, 0, 0, 0, 0);
  }
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx_w, wl_fixed_t sy_w)
{
  int x = wl_fixed_to_int(sx_w);
  int y = wl_fixed_to_int(sy_w);
  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  input->x = x;
  input->y = y;

  if (input->button == 0) {
    new_mouse_event(MOUSE_MOVE, activeSurface->id, x, y, 0, 0);
  } else {
    new_mouse_event(MOUSE_DRAG, activeSurface->id, x, y, input->button, 0);
  }
}

static void pointer_handle_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state_w) 
{
  enum MouseAction action = (state_w == WL_POINTER_BUTTON_STATE_PRESSED) ? MOUSE_BUTTON_PRESS : MOUSE_BUTTON_RELEASE;

  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  new_mouse_event(action, activeSurface->id, input->x, input->y, button, 0);
  
  if (action == MOUSE_BUTTON_RELEASE) {
    uint32_t click_interval = time - input->last_click_time;

    input->button = 0;
    
    if (click_interval <= MOUSE_CLICK_INTERVAL) {
      input->click_count ++;
    } else {
      input->click_count = 1;
    }
    
    input->last_click_time = time;
    
    new_mouse_event(MOUSE_CLICK, activeSurface->id, input->x, input->y, button, input->click_count);
  } else {
    input->button = button;
  }
}

static void pointer_handle_axis(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  int delta = wl_fixed_to_int(value);
  
  new_mouse_event(MOUSE_WHEEL, activeSurface->id, input->x, input->y, (delta < 0 ) ? 4 : 0, 0);
}

static const struct wl_pointer_listener pointer_listener = {
  .enter  = pointer_handle_enter,
  .leave  = pointer_handle_leave,
  .motion = pointer_handle_motion,
  .button = pointer_handle_button,
  .axis   = pointer_handle_axis
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
  struct input *input1 = (struct input *)data;

  if ((caps & WL_SEAT_CAPABILITY_POINTER) && !input1->pointer) {
    input1->pointer = wl_seat_get_pointer(seat);
    wl_pointer_set_user_data(input1->pointer, input1);
    wl_pointer_add_listener(input1->pointer, &pointer_listener, input1);
  } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && input1->pointer) {
    wl_pointer_release(input1->pointer);
    input1->pointer = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !input1->keyboard) {
    input1->keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_set_user_data(input1->keyboard, input1);
    wl_keyboard_add_listener(input1->keyboard, &keyboard_listener, input1);
  } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && input1->keyboard) {
    wl_keyboard_release(input1->keyboard);
    input1->keyboard = NULL;
  }
}

static void seat_handle_name(void *data, struct wl_seat *seat, const char *name)
{
}

static const struct wl_seat_listener seat_listener = {
  seat_handle_capabilities,
  seat_handle_name
};

bool init_input(struct input* input1)
{
  if (input1->seat == NULL) {
    return false;
  }

  input1->xkb_context = xkb_context_new((xkb_context_flags)0);
  
  if (input1->xkb_context == NULL) {
    return false;
  }

  // Default key repeat settings
  // Initial delay 250 milliseconds
  input1->repeat_delay_sec = 0;
  input1->repeat_delay_nsec = 1000 * 1000 * 250;
  
  // Repeat every 100 milliseconds
  input1->repeat_rate_sec = 0;
  input1->repeat_rate_nsec = 1000 * 1000 * 100;

  // Initial repeat timer
  input1->repeat_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  
  if (input1->repeat_timer_fd == -1) {
    return false;
  }

  // Initial event pump
  if (!init_event(input1)) {
    return false;
  }

  // Initial pending event list and lock
  pthread_mutex_init(&input1->lock, NULL);
  wl_list_init(&input1->event_list);

  wl_seat_add_listener(input1->seat, &seat_listener, input1);
  wl_seat_set_user_data(input1->seat, input1);

  return true;
}

int main()
{

  return 0;
}

void Application::Init(int argc, char **argv)
{
  if (wayland_init() == false) {
		throw std::runtime_error("Unable to init wayland");
  }

  struct display *wdisplay = get_display();

  if (wdisplay == NULL) {
		throw std::runtime_error("Unable to get display for wayland");
  }

  int 
    w = get_display_width(),
    h = get_display_height();

	sg_screen.x = w;
	sg_screen.y = h;
  
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

  g->Flush();
  
  Application::FrameRate(sg_jcanvas_window->GetFramesPerSecond());

  uint32_t *src = (uint32_t *)sg_back_buffer->LockData();
  uint32_t *dst = (uint32_t *)sg_surface->content;

  wl_surface_damage(sg_surface->surface, 0, 0, sg_surface->width, sg_surface->height);

  for (int i=0; i<bounds.size.x*bounds.size.y; i++) {
    dst[i] = src[i];
  }

  wl_surface_attach(sg_surface->surface, sg_surface->buffer, 0, 0);
  wl_surface_commit(sg_surface->surface);

  display_flush();

  sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jcanvas_window == nullptr) {
    return;
  }

  Event *event;

  if (!is_input_inited()) {
    return;
  }

  std::thread
    redraw([]() {
        while (sg_quitting == false) {
          if (sg_repaint.exchange(false) == true) {
            InternalPaint();
          }
        }
    });

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  while (sg_quitting == false) {
    event = next_event();
    
    if (event != NULL) {
      if (event->kind == MOUSE_EVENT)  {
        static jcanvas::jmouseevent_button_t buttons = jcanvas::JMB_NONE;

        jcanvas::jmouseevent_button_t button = jcanvas::JMB_NONE;
        jcanvas::jmouseevent_type_t type = jcanvas::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event->e.m.x;
        sg_mouse_y = event->e.m.y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.x - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.y - 1);

        if (event->e.m.action == MOUSE_ENTER) {
          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_ENTERED));

          continue;
        } else if (event->e.m.action == MOUSE_LEAVE) {
          sg_jcanvas_window->DispatchWindowEvent(new jcanvas::WindowEvent(sg_jcanvas_window, jcanvas::JWET_LEAVED));

          continue;
        } else if (event->e.m.action == MOUSE_MOVE or event->e.m.action == MOUSE_DRAG) {
          type = jcanvas::JMT_MOVED;
        } else if (event->e.m.action == MOUSE_BUTTON_PRESS or event->e.m.action == MOUSE_BUTTON_RELEASE) {
          if (event->e.m.button == BTN_LEFT) {
            button = jcanvas::JMB_BUTTON1;
          } else if (event->e.m.button == BTN_MIDDLE) {
            button = jcanvas::JMB_BUTTON2;
          } else if (event->e.m.button == BTN_RIGHT) {
            button = jcanvas::JMB_BUTTON3;
          }

          if (event->e.m.action == MOUSE_BUTTON_PRESS) {
            type = jcanvas::JMT_PRESSED;
            buttons = (jcanvas::jmouseevent_button_t)(buttons | button);
          } else if (event->e.m.action == MOUSE_BUTTON_RELEASE) {
            type = jcanvas::JMT_RELEASED;
            buttons = (jcanvas::jmouseevent_button_t)(buttons & ~button);
          }
        } else if (event->e.m.action == MOUSE_CLICK) {
          // action = "click";
        } else if (event->e.m.action == MOUSE_WHEEL) {
          type = jcanvas::JMT_ROTATED;
          mouse_z = (int)event->e.m.click_count;
        }

        if (sg_jcanvas_window->GetEventManager()->IsAutoGrab() == true && buttons != jcanvas::JMB_NONE) {
          // grab input
        } else {
          // release input
        }

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::MouseEvent(sg_jcanvas_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (event->kind == KEY_EVENT)  {
        jcanvas::jkeyevent_type_t type;
        jcanvas::jkeyevent_modifiers_t mod;

        mod = (jcanvas::jkeyevent_modifiers_t)(0);

        if ((event->modifiers & MOD_SHIFT_MASK) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SHIFT);
        } else if ((event->modifiers & MOD_CONTROL_MASK) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CONTROL);
        } else if ((event->modifiers & MOD_ALT_MASK) != 0) {
          mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALT);
        // } else if ((event.key.keysym.mod & KMOD_LGUI) != 0) {
        //  mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
        // } else if ((event.key.keysym.mod & KMOD_RGUI) != 0) {
        //   mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_SUPER);
        // } else if ((event.key.keysym.mod & KMOD_MODE) != 0) {
        // 	mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_ALTGR);
        // } else if ((event.key.keysym.mod & KMOD_CAPS) != 0) {
        // 	mod = (jcanvas::jkeyevent_modifiers_t)(mod | jcanvas::JKM_CAPS_LOCK);
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

        if (event->e.k.action == KEY_PRESS) {
          type = jcanvas::JKT_PRESSED;
        } else if (event->e.k.action == KEY_RELEASE) {
          type = jcanvas::JKT_RELEASED;
        // } else if (event->e.k.action == KEY_TYPE) {
        }

        jcanvas::jkeyevent_symbol_t symbol = (jcanvas::jkeyevent_symbol_t)event->e.k.keyCode;

        sg_jcanvas_window->GetEventManager()->PostEvent(new jcanvas::KeyEvent(sg_jcanvas_window, type, mod, jcanvas::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event->kind == DISPLAY_EVENT)  {
        handle_display_event(event);
      } else if (event->kind == SURFACE_EVENT)  {
        handle_surface_event(event);
      }

      free(event);
    } else {
      struct epoll_event ep[16];
      int count;
      EventHandler fn;
      struct input* input = get_input();

      count = epoll_wait(input->epoll_fd, ep, sizeof(ep)/sizeof(struct epoll_event), EVENT_WAIT_TIMEOUT);

      for (int index = 0; index < count; index ++) {
        fn = (EventHandler)ep[index].data.ptr;

        fn();
      }
    }
  }

  redraw.join();

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

void Application::SetVerticalSyncEnabled(bool enabled)
{
}

bool Application::IsVerticalSyncEnabled()
{
  return true;
}

WindowAdapter::WindowAdapter(jcanvas::Window *parent, jcanvas::jrect_t<int> bounds)
{
	if (sg_surface != nullptr) {
		throw std::runtime_error("Cannot create more than one window");
  }

  // sg_jcanvas_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jcanvas_window = parent;

  sg_previous_bounds = bounds;

  sg_surface = CreateShmScreenSurface(0, 0, 0, bounds.size.x, bounds.size.y, 4); 

  RemapShmScreenSurface(sg_surface, sg_surface->width, sg_surface->height);
}

WindowAdapter::~WindowAdapter()
{
  if (sg_surface != nullptr) {
    new_surface_event(SURFACE_UNMAP, (ShmSurface*)sg_surface, 0, 0);
    new_surface_event(SURFACE_DISPOSE, (ShmSurface*)sg_surface, 0, 0);
  }

  wayland_cleanup();

  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void WindowAdapter::Repaint()
{
  sg_repaint.store(true);
}

void WindowAdapter::ToggleFullScreen()
{
}

void WindowAdapter::SetTitle(std::string title)
{
}

std::string WindowAdapter::GetTitle()
{
	return std::string();
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
	return 1.0;
}

void WindowAdapter::SetUndecorated(bool undecorated)
{
}

bool WindowAdapter::IsUndecorated()
{
  return true;
}

void WindowAdapter::SetBounds(jrect_t<int> bounds)
{
  ResizeShmScreenSurface(sg_surface, bounds.size.x, bounds.size.y);
}

jcanvas::jrect_t<int> WindowAdapter::GetBounds()
{
	jcanvas::jrect_t<int> t {
    .point = {
      sg_surface->x, sg_surface->y
    },
    .size = {
      sg_surface->width, sg_surface->height
    }
  };

	return t;
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
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	return p;
}

void WindowAdapter::SetVisible(bool visible)
{
}

bool WindowAdapter::IsVisible()
{
  return true;
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return sg_jcanvas_cursor;
}

void WindowAdapter::SetCursorEnabled(bool enabled)
{
}

bool WindowAdapter::IsCursorEnabled()
{
	return true; 
}

void WindowAdapter::SetCursor(jcursor_style_t style)
{
  /*
  SDL_SystemCursor type = SDL_SYSTEM_CURSOR_ARROW;

  if (style == JCS_DEFAULT) {
    type = SDL_SYSTEM_CURSOR_ARROW;
  } else if (style == JCS_CROSSHAIR) {
    type = SDL_SYSTEM_CURSOR_CROSSHAIR;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = SDL_SYSTEM_CURSOR_HAND;
  } else if (style == JCS_MOVE) {
    type = SDL_SYSTEM_CURSOR_SIZEALL;
  } else if (style == JCS_NS) {
    type = SDL_SYSTEM_CURSOR_SIZENS;
  } else if (style == JCS_WE) {
    type = SDL_SYSTEM_CURSOR_SIZEWE;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = SDL_SYSTEM_CURSOR_IBEAM;
  } else if (style == JCS_WAIT) {
    type = SDL_SYSTEM_CURSOR_WAIT;
  }

  SDL_Cursor
    *cursor = SDL_CreateSystemCursor(type);

  SDL_SetCursor(cursor);
  // TODO:: SDL_FreeCursor(cursor);
  */
  
  sg_jcanvas_cursor = style;
}

void WindowAdapter::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

	jpoint_t<int> 
    t = shape->GetSize();
	uint32_t 
    data[t.x*t.y];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.x, t.y});

	SDL_Surface 
    *surface = SDL_CreateRGBSurfaceFrom(data, t.x, t.y, 32, t.x*4, 0, 0, 0, 0);

	if (surface == nullptr) {
		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);
  */
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
	// TODO::
}

jwindow_rotation_t WindowAdapter::GetRotation()
{
	return jcanvas::JWR_NONE;
}

void WindowAdapter::SetIcon(jcanvas::Image *image)
{
  /*
  if (image == nullptr) {
    return;
  }

  jcanvas::jpoint_t<int> 
    size = image->GetSize();
  uint32_t 
    *data = (uint32_t *)image->LockData();

  sg_jcanvas_icon = image;

  // todo anything ...

  image->UnlockData();
  */
}

jcanvas::Image * WindowAdapter::GetIcon()
{
  return sg_jcanvas_icon;
}

}
