project (jcanvas-core LANGUAGES CXX VERSION 1.0.0)

list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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
  src/jlistener.cpp
  src/jmouseevent.cpp
  src/jmouselistener.cpp
  src/jraster.cpp
  src/jresources.cpp
  src/jwindow.cpp
  src/jwindowadapter.cpp
  src/jwindowevent.cpp
  src/jwindowlistener.cpp
)

pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(FreeType REQUIRED IMPORTED_TARGET freetype2)
pkg_check_modules(jMixin REQUIRED IMPORTED_TARGET jmixin)

add_library(${PROJECT_NAME}
  SHARED 
    ${SRCS} 
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
  )

add_library(jCanvas::Core ALIAS ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    JCANVAS_RESOURCES_DIR=\"${CMAKE_INSTALL_PREFIX}/share/jcanvas\"
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include"
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    PkgConfig::Cairo
    PkgConfig::FreeType
    PkgConfig::jMixin
    jCanvas::Algebra
)

include (${PROJECT_SOURCE_DIR}/cmake/Providers.cmake)
include (${PROJECT_SOURCE_DIR}/cmake/Binds.cmake)

if (TESTS)
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

install(DIRECTORY src/include/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

