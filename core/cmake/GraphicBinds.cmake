set(GRAPHIC_BINDS_SRCS)

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

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${ALLEGRO5_INCLUDE_DIRS}
    ${ALLEGRO5VIDEO_INCLUDE_DIRS}
    ${ALLEGRO5MAIN_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${ALLEGRO5_LIBRARIES} 
    ${ALLEGRO5VIDEO_LIBRARIES} 
    ${ALLEGRO5MAIN_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL caca)
  pkg_check_modules(CACA caca)

  if (NOT CACA_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${CACA_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${CACA_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL directfb)
  pkg_check_modules(DIRECTFB directfb)

  if (NOT DIRECTFB_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${DIRECTFB_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${DIRECTFB_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL dispmanx)
  include (cmake/FindDispmanX.cmake)

  if (NOT DISPMANX_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${DISPMANX_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${DISPMANX_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL drm)
  pkg_check_modules(DRM libdrm)

  if (NOT DRM_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${DRM_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${DRM_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL efl)
  pkg_check_modules(EFL elementary)

  if (NOT EFL_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${EFL_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${EFL_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL fb)
  # all done :)
endif()

if (GRAPHIC_BIND STREQUAL flaschen)
  include (cmake/FindFlaschen.cmake)

  if (NOT FLASCHEN_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${FLASCHEN_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${FLASCHEN_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL gl)
  pkg_check_modules(GL gl)
  pkg_check_modules(GLU glu)

  if (NOT GL_FOUND OR NOT GLU_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${GL_INCLUDE_DIRS}
    ${GLU_INCLUDE_DIRS}
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

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${EGL_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${EGL_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL gtk3)
  pkg_check_modules(GTK3 gtk+-3.0)

  if (NOT GTK3_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${GTK3_INCLUDE_DIRS}
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

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${NANAX_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${NANAX_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL openvg)
  include (cmake/FindOpenVg.cmake)

  if (NOT OPENVG_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()
  
  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${OPENVG_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${OPENVG_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL qt5)
  find_package(Qt5Widgets REQUIRED)

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    Qt5::Widgets
  )
endif()

if (GRAPHIC_BIND STREQUAL sdl2)
  pkg_check_modules(SDL2 sdl2)

  if (NOT SDL2_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${SDL2_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${SDL2_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL sfml2)
  pkg_check_modules(SFML2 sfml-graphics)

  if (NOT SFML2_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${SFML2_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${SFML2_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL vdpau)
  pkg_check_modules(X11 x11)
  pkg_check_modules(VDPAU vdpau)

  if (NOT X11_FOUND OR NOT VDPAU_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${X11_INCLUDE_DIRS}
    ${VDPAU_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${X11_LIBRARIES} 
    ${VDPAU_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL vnc)
  pkg_check_modules(VNC libvncserver)

  if (NOT VNC_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${VNC_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${VNC_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL wayland)
  pkg_check_modules(WAYLAND wayland-client)

  if (NOT WAYLAND_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${WAYLAND_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${WAYLAND_LIBRARIES} 
    xkbcommon
  )
endif()

if (GRAPHIC_BIND STREQUAL xcb)
  pkg_check_modules(XCB xcb)

  if (NOT XCB_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${XCB_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${XCB_LIBRARIES} 
  )
endif()

if (GRAPHIC_BIND STREQUAL xlib)
  pkg_check_modules(X11 x11)

  if (NOT X11_FOUND)
    message (SEND_ERROR "unable to find graphic bind ${GRAPHIC_BIND}")
  endif()

  set(GRAPHIC_BINDS_SRCS 
    src/binds/${GRAPHIC_BIND}/bind.cpp
  )

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${X11_INCLUDE_DIRS}
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

  set(GRAPHIC_BINDS_INCLUDE_DIRS 
    ${JX_INCLUDE_DIRS}
  )
  
  set(GRAPHIC_BINDS_LIBRARIES
    ${JX_LIBRARIES} 
  )
endif()

set(SRCS ${SRCS} ${GRAPHIC_BINDS_SRCS})
