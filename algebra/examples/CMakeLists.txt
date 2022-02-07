cmake_minimum_required (VERSION 3.0)

macro(module_test)
  add_executable(${ARGV0}_example
    ${ARGV0}.cpp
  )

  target_link_libraries(${ARGV0}_example
    PUBLIC
      jcanvas-core
  )
endmacro()

module_test(example)
