project (jcanvas-algebra LANGUAGES CXX VERSION 1.0.0)

set(SRCS
  src/jcircle.cpp
  src/jinsets.cpp
  src/jline.cpp
  src/jmatrix.cpp
  src/jpoint.cpp
  src/jrect.cpp
  src/jvector.cpp
)

add_library(${PROJECT_NAME} SHARED 
  ${SRCS} 
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  )

add_library(jCanvas::Algebra ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include"
)

if (TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

install(DIRECTORY src/include/jcanvas/algebra DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jcanvas)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

