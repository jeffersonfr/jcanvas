cmake_minimum_required (VERSION 3.16)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

project (jcanvas)

option(TESTS "Enable unit tests" OFF)
option(EXAMPLES "Enable examples" OFF)
option(SANITIZE "Enable sanitize" OFF)
option(COVERAGE "Enable coverage" OFF)
option(PROFILE "Enable profile" OFF)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "-std=c++20")

set(GRAPHIC_BIND "sdl2" CACHE STRING "Default graphic bind")
set_property(CACHE GRAPHIC_BIND PROPERTY STRINGS 
  allegro5 caca directfb dispmanx drm efl egl fb flaschen gl gtk3 nanax openvg qt5 sdl2 sfml2 vdpau vnc wayland xcb xlib jx)

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

enable_testing()

add_subdirectory(core)
add_subdirectory(algebra)

# generate jcanvas.pc
set (JCANVAS_INCLUDE_DIRS "")
set (JCANVAS_LIBRARIES "-lpthread")

foreach(X IN LISTS CAIRO_INCLUDE_DIRS FREETYPE_INCLUDE_DIRS JMIXIN_INCLUDE_DIRS)
  set (JCANVAS_INCLUDE_DIRS "${JCANVAS_INCLUDE_DIRS} -I${X}")
endforeach()
  
foreach(X IN LISTS CAIRO_LIBRARIES FREETYPE_LIBRARIES JMIXIN_LIBRARIES)
  set (JCANVAS_LIBRARIES "${JCANVAS_LIBRARIES} -l${X}")
endforeach()
  
configure_file(
  ${PROJECT_SOURCE_DIR}/jcanvas.pc.in
  ${PROJECT_BINARY_DIR}/jcanvas.pc
  @ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/jcanvas.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
