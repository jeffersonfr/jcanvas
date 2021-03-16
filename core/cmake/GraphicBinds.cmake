set(GRAPHIC_BINDS_SRCS)

if (GRAPHIC_BIND STREQUAL dummy)
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )
endif()

if (GRAPHIC_BIND STREQUAL allegro5)
  pkg_check_modules(ALLEGRO5 allegro-5)
  pkg_check_modules(ALLEGRO5VIDEO allegro_video-5)
  pkg_check_modules(ALLEGRO5MAIN allegro_main-5)

  if (NOT ALLEGRO5_FOUND OR NOT ALLEGRO5VIDEO_FOUND OR NOT ALLEGRO5MAIN_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${ALLEGRO5_INCLUDE_DIRS}
    ${ALLEGRO5VIDEO_INCLUDE_DIRS}
    ${ALLEGRO5MAIN_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${ALLEGRO5_LIBRARY_DIRS} 
    ${ALLEGRO5VIDEO_LIBRARY_DIRS} 
    ${ALLEGRO5MAIN_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${ALLEGRO5_LIBRARIES} 
    ${ALLEGRO5VIDEO_LIBRARIES} 
    ${ALLEGRO5MAIN_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL caca)
  pkg_check_modules(CACA REQUIRED caca)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${CACA_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${CACA_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${CACA_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL directfb)
  pkg_check_modules(DIRECTFB REQUIRED directfb)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${DIRECTFB_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${DIRECTFB_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${DIRECTFB_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL drm)
  pkg_check_modules(DRM REQUIRED libdrm)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${DRM_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${DRM_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${DRM_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL efl)
  pkg_check_modules(EFL REQUIRED elementary)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${EFL_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${EFL_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${EFL_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL fb)
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )
endif()

if (GRAPHIC_BIND STREQUAL flaschen)
  include (cmake/FindFlaschen.cmake)

  if (NOT FLASCHEN_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${FLASCHEN_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${FLASCHEN_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${FLASCHEN_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL gl)
  pkg_check_modules(GL REQUIRED gl)
  pkg_check_modules(GLU REQUIRED glu)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${GL_INCLUDE_DIRS}
    ${GLU_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${GL_LIBRARY_DIRS} 
    ${GLU_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${GL_LIBRARIES} 
    ${GLU_LIBRARIES} 
    glut
  )
endif()

if (GRAPHIC_BIND STREQUAL egl)
  include (cmake/FindEgl.cmake)

  if (NOT EGL_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${EGL_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${EGL_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${EGL_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${EGL_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL gtk3)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${GTK3_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${GTK3_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${GTK3_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL nanax)
  include (cmake/FindNanaX.cmake)

  if (NOT NANAX_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${NANAX_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${NANAX_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${NANAX_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL qt5)
  find_package(Qt5Widgets REQUIRED)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
  )

  set(GRAPHIC_BINDS_LIBRARIES
    Qt5::Widgets
  )
endif()

if (GRAPHIC_BIND STREQUAL sdl2)
  pkg_check_modules(SDL2 REQUIRED sdl2)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${SDL2_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${SDL2_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${SDL2_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL sfml2)
  pkg_check_modules(SFML2 REQUIRED sfml-graphics)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${SFML2_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${SFML2_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${SFML2_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL vdpau)
  pkg_check_modules(X11 REQUIRED x11)
  pkg_check_modules(VDPAU REQUIRED vdpau)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${X11_INCLUDE_DIRS}
    ${VDPAU_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${X11_LIBRARY_DIRS} 
    ${VDPAU_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${X11_LIBRARIES} 
    ${VDPAU_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL vnc)
  pkg_check_modules(VNC REQUIRED libvncserver)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${VNC_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${VNC_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${VNC_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL wayland)
  pkg_check_modules(WAYLAND REQUIRED wayland-client)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${WAYLAND_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${WAYLAND_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${WAYLAND_LIBRARIES} 
    xkbcommon
  )
endif()

if (GRAPHIC_BIND STREQUAL xcb)
  pkg_check_modules(XCB REQUIRED xcb)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${XCB_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${XCB_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${XCB_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL xlib)
  pkg_check_modules(X11 REQUIRED x11)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${X11_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${X11_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${X11_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL jx)
  include (cmake/FindJx.cmake)

  if (NOT JX_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
  )

  set(GRAPHIC_BINDS_LDFLAGS
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${JX_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${JX_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${JX_LIBRARIES} 
  )
endif()

# raspberry pi
if (GRAPHIC_BIND STREQUAL dispmanx-pi)
  include (cmake/FindDispmanXPi.cmake)

  if (NOT DISPMANX_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
    ${DISPMANX_CFLAGS_OTHERS}
  )

  set(GRAPHIC_BINDS_LDFLAGS
    ${DISPMANX_LDFLAGS_OTHERS}
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${DISPMANX_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${DISPMANX_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${DISPMANX_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL egl-pi)
  include (cmake/FindEglPi.cmake)

  if (NOT EGL_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
    ${EGL_CFLAGS_OTHERS}
  )

  set(GRAPHIC_BINDS_LDFLAGS
    ${EGL_LDFLAGS_OTHERS}
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${EGL_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${EGL_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${EGL_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${EGL_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL openvg-pi)
  include (cmake/FindOpenVgPi.cmake)

  if (NOT OPENVG_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_CFLAGS
    ${OPENVG_CFLAGS_OTHERS}
  )

  set(GRAPHIC_BINDS_LDFLAGS
    ${OPENVG_LDFLAGS_OTHERS}
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${OPENVG_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARY_DIRS
    ${OPENVG_LIBRARY_DIRS} 
  )

  set(GRAPHIC_BINDS_LIBRARIES
    ${OPENVG_LIBRARIES} 
  )
endif()

set(SRCS ${SRCS} ${GRAPHIC_BINDS_SRCS})
