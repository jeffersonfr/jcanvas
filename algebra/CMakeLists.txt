project (jcanvas-algebra)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

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

include_directories(
  "${CMAKE_SOURCE_DIR}/algebra/src/include"
)

target_include_directories(${PROJECT_NAME} PUBLIC
)

target_link_libraries(${PROJECT_NAME} PUBLIC
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

