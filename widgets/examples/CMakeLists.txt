cmake_minimum_required (VERSION 3.0)

find_package(OpenMP)

macro(module_test)
  add_executable(${ARGV0}_example
    ${ARGV0}.cpp
  )

  target_link_libraries(${ARGV0}_example
    PRIVATE
      jcanvas-core
      jcanvas-widgets
  )

  if(OpenMP_CXX_FOUND)
    target_link_libraries(${ARGV0}_example
      PRIVATE
        OpenMP::OpenMP_CXX
    )
  endif()
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
module_test(texts)
module_test(columnlayout)
module_test(rowlayout)
module_test(tabs)
