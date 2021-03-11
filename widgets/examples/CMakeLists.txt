cmake_minimum_required (VERSION 3.0)

macro(module_test)
  add_executable(${ARGV0}_example
    ${ARGV0}.cpp
  )

  include_directories(
    "../core/src/include"
    "../widgets/src/include"
  )

  target_link_libraries(${ARGV0}_example PUBLIC
    jcanvas-core
    jcanvas-widgets
  )
endmacro()

file(COPY images DESTINATION .)

module_test(border)
module_test(calc)
module_test(clip)
module_test(custom)
module_test(dialogs)
module_test(layouts)
module_test(menu)
#module_test(mhp)
module_test(scroll)
module_test(sort)
module_test(widgets)
module_test(flatbutton)
