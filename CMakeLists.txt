cmake_minimum_required (VERSION 3.10)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source build is not allowed")
endif()

project(jcanvas
  LANGUAGES CXX
  HOMEPAGE_URL https://github.com/jeffersonfr/jcanvas
  DESCRIPTION "A library for abstract a host graphic engine"
  VERSION 1.0.0
)

option(JCANVAS_WIDGETS "Enable widgets module" ON)
option(JCANVAS_TESTS "Enable unit tests" OFF)
option(JCANVAS_EXAMPLES "Enable examples" OFF)
option(JCANVAS_SANITIZE "Enable sanitize" OFF)
option(JCANVAS_COVERAGE "Enable coverage" OFF)
option(JCANVAS_PROFILE "Enable profile" OFF)

set(JCANVAS_GRAPHIC_ENGINE "sdl2" CACHE STRING "Default graphic bind")

set_property(CACHE JCANVAS_GRAPHIC_ENGINE PROPERTY STRINGS 
  dummy allegro5 caca directfb drm efl egl fb flaschen gl gtk3 nanax qt5 sdl2 sfml2 vdpau vnc wayland xcb xlib jx dispmanx-pi egl-pi openvg-pi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -pedantic -Werror)

if (JCANVAS_COVERAGE)
  add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
  add_link_options(-fprofile-arcs -lgcov)
endif()

if (JCANVAS_SANITIZE)
  set(SANITIZE_FLAGS -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope -U_FORTIFY_SOURCE -fno-omit-frame-pointer -fno-common)

  add_compile_options(${SANITIZE_FLAGS})
  add_link_options(${SANITIZE_FLAGS} -lasan -lubsan)
endif()

if (JCANVAS_PROFILE)
  add_compile_options(-pg -fprofile-arcs -ftest-coverage -fprofile-generate -fno-inline -O0)
endif()

find_package(PkgConfig REQUIRED)

enable_testing()

add_subdirectory(algebra)
add_subdirectory(core)

if (JCANVAS_WIDGETS)
  add_subdirectory(widgets)
endif()

install(DIRECTORY resources/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/share)

# generate pc file
configure_file(jcanvas.pc.in jcanvas.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/jcanvas.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
