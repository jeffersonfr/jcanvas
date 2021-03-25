cmake_minimum_required (VERSION 3.13)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

project (jcanvas)

option(WIDGETS "Enable widgets module" ON)
option(TESTS "Enable unit tests" OFF)
option(EXAMPLES "Enable examples" OFF)
option(SANITIZE "Enable sanitize" OFF)
option(COVERAGE "Enable coverage" OFF)
option(PROFILE "Enable profile" OFF)
option(DEBUG "Enable debug" OFF)

set(CMAKE_CXX_STANDARD 17)

set(GRAPHIC_BIND "sdl2" CACHE STRING "Default graphic bind")
set_property(CACHE GRAPHIC_BIND PROPERTY STRINGS 
  dummy allegro5 caca directfb drm efl egl fb flaschen gl gtk3 nanax qt5 sdl2 sfml2 vdpau vnc wayland xcb xlib jx dispmanx-pi egl-pi openvg-pi)

find_package(PkgConfig REQUIRED)

if (COVERAGE)
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} --coverage -lgcov"
  )
endif()

if (SANITIZE)
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope"
  )
endif()

if (PROFILE)
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -pg -fprofile-arcs -ftest-coverage -fprofile-generate -fno-inline -O0"
  )
  set(CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -pg"
  )
endif()

if (DEBUG)
  set(CMAKE_CXX_FLAGS "-g -ggdb")
endif()

add_definitions (-DJCANVAS_RESOURCES_DIR=\"${CMAKE_INSTALL_PREFIX}/share/jcanvas\")

enable_testing()

add_subdirectory(algebra)
add_subdirectory(core)

if (WIDGETS)
  add_subdirectory(widgets)
endif()

# generate jcanvas.pc
set (JCANVAS_INCLUDE_DIRS "")
set (JCANVAS_LIBRARIES "-lpthread")

foreach(X IN LISTS CAIRO_INCLUDE_DIRS FREETYPE_INCLUDE_DIRS JMIXIN_INCLUDE_DIRS)
  set (JCANVAS_INCLUDE_DIRS "${JCANVAS_INCLUDE_DIRS} -I${X}")
endforeach()
  
foreach(X IN LISTS CAIRO_LIBRARIES FREETYPE_LIBRARIES JMIXIN_LIBRARIES)
  set (JCANVAS_LIBRARIES "${JCANVAS_LIBRARIES} -l${X}")
endforeach()
  
if (WIDGETS)
  set (JCANVAS_LIBRARIES "${JCANVAS_LIBRARIES} -ljcanvas-widgets")
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/jcanvas.pc.in
  ${PROJECT_BINARY_DIR}/jcanvas.pc
  @ONLY
)

install(DIRECTORY resources/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
install(FILES ${CMAKE_BINARY_DIR}/jcanvas.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
