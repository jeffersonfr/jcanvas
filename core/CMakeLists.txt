project (jcanvas-core)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

set(SRCS
  src/japplication.cpp
  src/jbufferedimage.cpp
  src/jcolor.cpp
  src/jenum.cpp
  src/jeventmanager.cpp
  src/jeventobject.cpp
  src/jfont.cpp
  src/jgraphics.cpp
  src/jhslcolorspace.cpp
  src/jimage.cpp
  src/jindexedimage.cpp
  src/jkeyevent.cpp
  src/jkeylistener.cpp
  src/jkeymap.cpp
  src/jlistener.cpp
  src/jmouseevent.cpp
  src/jmouselistener.cpp
  src/jraster.cpp
  src/jwindow.cpp
  src/jwindowadapter.cpp
  src/jwindowevent.cpp
  src/jwindowlistener.cpp
)

include (cmake/GraphicBinds.cmake)
include (cmake/ImageProviders.cmake)

# find libcairo
pkg_check_modules(CAIRO REQUIRED cairo)

# find freetype2
pkg_check_modules(FREETYPE REQUIRED freetype2)

# find jmixin
pkg_check_modules(JMIXIN REQUIRED jmixin)

# core compilation
add_library(${PROJECT_NAME} SHARED 
  ${SRCS} 
  ${GRAPHIC_BINDS_SRCS} 
  ${IMAGE_PROVIDERS_SRCS}
)

include_directories(
  "${CMAKE_SOURCE_DIR}/core/src/include"
  "${CMAKE_SOURCE_DIR}/algebra/src/include"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
  ${GRAPHIC_BINDS_CFLAGS} 
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CAIRO_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  ${JMIXIN_INCLUDE_DIRS}
  ${GRAPHIC_BINDS_INCLUDE_DIRS}
  ${IMAGE_PROVIDERS_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GRAPHIC_BINDS_LIBRARY_DIRS} 
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${CAIRO_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${JMIXIN_LIBRARIES}
  ${GRAPHIC_BINDS_LIBRARIES}
  ${IMAGE_PROVIDERS_LIBRARIES}
  jcanvas-algebra
)

if (TESTS)
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

message ("\tGraphic Bind: ${GRAPHIC_BIND}")
message ("\t\tInclude Directories: ${GRAPHIC_BINDS_INCLUDE_DIRS}")
message ("\t\tLibrary Directories: ${GRAPHIC_BINDS_LIBRARY_DIRS}")
message ("\t\tLibraries: ${GRAPHIC_BINDS_LIBRARIES}")
message ("\tImage Providers: ${IMAGE_LIST}")
message ("\t\tInclude Directories: ${IMAGE_PROVIDERS_INCLUDE_DIRS}")
message ("\t\tLibrary Directories: ${IMAGE_PROVIDERS_LIBRARY_DIRS}")
message ("\t\tLibraries: ${IMAGE_PROVIDERS_LIBRARIES}")

install(DIRECTORY src/include/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

