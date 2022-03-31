cmake_minimum_required (VERSION 3.10)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source build is not allowed")
endif()

project (jcanvas)

option(WIDGETS "Enable widgets module" ON)
option(TESTS "Enable unit tests" OFF)
option(EXAMPLES "Enable examples" OFF)
option(SANITIZE "Enable sanitize" OFF)
option(COVERAGE "Enable coverage" OFF)
option(PROFILE "Enable profile" OFF)

set(GRAPHIC_BIND "sdl2" CACHE STRING "Default graphic bind")

set_property(CACHE GRAPHIC_BIND PROPERTY STRINGS 
  dummy allegro5 caca directfb drm efl egl fb flaschen gl gtk3 nanax qt5 sdl2 sfml2 vdpau vnc wayland xcb xlib jx dispmanx-pi egl-pi openvg-pi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -pedantic -Werror)

if (COVERAGE)
  add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
  add_link_options(-fprofile-arcs -lgcov)
endif()

if (SANITIZE)
  set(SANITIZE_FLAGS -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope -U_FORTIFY_SOURCE -fno-omit-frame-pointer -fno-common)

  add_compile_options(${SANITIZE_FLAGS})
  add_link_options(${SANITIZE_FLAGS} -lasan -lubsan)
endif()

if (PROFILE)
  add_compile_options(-pg -fprofile-arcs -ftest-coverage -fprofile-generate -fno-inline -O0)
endif()

find_package(PkgConfig REQUIRED)

enable_testing()

add_subdirectory(algebra)
add_subdirectory(core)

if (WIDGETS)
  add_subdirectory(widgets)
endif()

install(DIRECTORY resources/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/share)

# generate jcanvas.pc
configure_file(${PROJECT_SOURCE_DIR}/jcanvas.pc.in ${PROJECT_BINARY_DIR}/jcanvas.pc.gen @ONLY)
file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/jcanvas.pc INPUT "${CMAKE_BINARY_DIR}/jcanvas.pc.gen")
install(FILES ${CMAKE_BINARY_DIR}/jcanvas.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
