# dummy
if (JCANVAS_GRAPHIC_ENGINE STREQUAL dummy)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)
endif()

# fb
if (JCANVAS_GRAPHIC_ENGINE STREQUAL fb)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)
endif()

# qt5
if (JCANVAS_GRAPHIC_ENGINE STREQUAL allegro5)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Allegro5 REQUIRED IMPORTED_TARGET allegro-5)
  pkg_check_modules(Allegro5Main REQUIRED IMPORTED_TARGET allegro_main-5)
  pkg_check_modules(Allegro5Video REQUIRED IMPORTED_TARGET allegro_video-5)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Allegro5
      PkgConfig::Allegro5Video
      PkgConfig::Allegro5Main
  )
endif()

# caca
if (JCANVAS_GRAPHIC_ENGINE STREQUAL caca)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Caca REQUIRED IMPORTED_TARGET caca)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Caca
  )
endif()

# directfb
if (JCANVAS_GRAPHIC_ENGINE STREQUAL directfb)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(DirectFB REQUIRED IMPORTED_TARGET directfb)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::DirectFB
  )
endif()

# drm
if (JCANVAS_GRAPHIC_ENGINE STREQUAL drm)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Drm REQUIRED IMPORTED_TARGET drm)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Drm
  )
endif()

# efl
if (JCANVAS_GRAPHIC_ENGINE STREQUAL efl)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Efl REQUIRED IMPORTED_TARGET efl)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Efl
  )
endif()

# gl
if (JCANVAS_GRAPHIC_ENGINE STREQUAL gl)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Gl REQUIRED IMPORTED_TARGET gl)
  pkg_check_modules(Glu REQUIRED IMPORTED_TARGET glu)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Gl
      PkgConfig::Glu
      glut
  )
endif()

# gtk3
if (JCANVAS_GRAPHIC_ENGINE STREQUAL gtk3)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Gtk3 REQUIRED IMPORTED_TARGET gtk+-3.0)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Gtk3
  )
endif()

# sdl2
if (JCANVAS_GRAPHIC_ENGINE STREQUAL sdl2)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Sdl2 REQUIRED IMPORTED_TARGET sdl2)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Sdl2
  )
endif()

# sfml2
if (JCANVAS_GRAPHIC_ENGINE STREQUAL sfml2)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Sfml2 REQUIRED IMPORTED_TARGET sfml2-graphics)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Sfml2
  )
endif()

# vdpau
if (JCANVAS_GRAPHIC_ENGINE STREQUAL vdpau)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)
  pkg_check_modules(Vdpau REQUIRED IMPORTED_TARGET vdpau)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::X11
      PkgConfig::Vdpau
  )
endif()

# vnc
if (JCANVAS_GRAPHIC_ENGINE STREQUAL vnc)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(VncServer REQUIRED IMPORTED_TARGET libvncserver)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::VncServer
  )
endif()

# wayland
if (JCANVAS_GRAPHIC_ENGINE STREQUAL wayland)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Wayland REQUIRED IMPORTED_TARGET wayland-client)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig::Wayland
  )
endif()

# xcb
if (JCANVAS_GRAPHIC_ENGINE STREQUAL xcb)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Xcb REQUIRED IMPORTED_TARGET xcb)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig:Xcb
  )
endif()

# xlib
if (JCANVAS_GRAPHIC_ENGINE STREQUAL xlib)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Xlib REQUIRED IMPORTED_TARGET x11)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig:Xlib
  )
endif()

# jx
if (JCANVAS_GRAPHIC_ENGINE STREQUAL jx)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  pkg_check_modules(Jx REQUIRED IMPORTED_TARGET jx)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      PkgConfig:Jx
  )
endif()

# flaschen
if (JCANVAS_GRAPHIC_ENGINE STREQUAL flaschen)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(Flaschen REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      Flaschen
  )
endif()

# egl
if (JCANVAS_GRAPHIC_ENGINE STREQUAL egl)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(Egl REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      Egl
  )
endif()

# nanax
if (JCANVAS_GRAPHIC_ENGINE STREQUAL nanax)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(Nanax REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      Nanax
  )
endif()

# qt5
if (JCANVAS_GRAPHIC_ENGINE STREQUAL qt5)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(Qt5Widgets REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      Qt5::Widgets
  )
endif()

# dispmanx-pi
if (JCANVAS_GRAPHIC_ENGINE STREQUAL dispmanx-pi)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(DispmanXPi REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      DispmanXPi
  )
endif()

# egl-pi
if (JCANVAS_GRAPHIC_ENGINE STREQUAL egl-pi)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(EglPi REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      EglPi
  )
endif()

# openvg-pi
if (JCANVAS_GRAPHIC_ENGINE STREQUAL openvg-pi)
  target_sources(${PROJECT_NAME} PRIVATE src/binds/${JCANVAS_GRAPHIC_ENGINE}/bind.cpp)

  find_package(OpenVgPi REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      OpenVgPi
  )
endif()

message ("\tGraphic Engine: ${JCANVAS_GRAPHIC_ENGINE}")
