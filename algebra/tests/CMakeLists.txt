cmake_minimum_required (VERSION 3.0)

macro(module_test)
  add_executable(${ARGV0}_test
    ${ARGV0}.cpp
  )

  add_test(${ARGV0}_test ${ARGV0}_test
    COMMAND $<TARGET_FILE:${ARGV0}>
  )

  target_link_libraries(${ARGV0}_test
    PRIVATE
      jcanvas-core
  )
endmacro()

module_test(jvector_basics)
