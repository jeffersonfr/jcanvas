project (jcanvas-widgets)

include (${CMAKE_SOURCE_DIR}/Defines.cmake)

set(SRCS
  src/jactionevent.cpp
  src/jactionlistener.cpp
  src/jadjustmentevent.cpp
  src/jadjustmentlistener.cpp
  src/janimation.cpp
  src/jborderlayout.cpp
  src/jbutton.cpp
  src/jcalendardialog.cpp
  src/jcardlayout.cpp
  src/jcheckbutton.cpp
  src/jcheckbuttongroup.cpp
  src/jcomponent.cpp
  src/jcomponentevent.cpp
  src/jcomponentlistener.cpp
  src/jcontainer.cpp
  src/jcontainerevent.cpp
  src/jcontainerlistener.cpp
  src/jcoordinatelayout.cpp
  src/jdialog.cpp
  src/jfilechooserdialog.cpp
  src/jflowlayout.cpp
  src/jfocusevent.cpp
  src/jfocuslistener.cpp
  src/jframe.cpp
  src/jgridbaglayout.cpp
  src/jgridlayout.cpp
  src/jicon.cpp
  src/jinputdialog.cpp
  src/jitemcomponent.cpp
  src/jkeyboarddialog.cpp
  src/jlayout.cpp
  src/jlistbox.cpp
  src/jmarquee.cpp
  src/jmessagedialog.cpp
  src/jnulllayout.cpp
  src/jpanel.cpp
  src/jprogressbar.cpp
  src/jscrollbar.cpp
  src/jselectevent.cpp
  src/jselectlistener.cpp
  src/jslider.cpp
  src/jslidercomponent.cpp
  src/jspin.cpp
  src/jtabbedpane.cpp
  #src/jtable.cpp
  src/jtext.cpp
  src/jtextarea.cpp
  src/jtextcomponent.cpp
  src/jtextevent.cpp
  src/jtextlistener.cpp
  src/jtextfield.cpp
  src/jtheme.cpp
  src/jtoastdialog.cpp
  src/jtoggleevent.cpp
  src/jtogglelistener.cpp
  #src/jtreelistview.cpp
  src/jyesnodialog.cpp
)

add_definitions (-DJCANVAS_RESOURCES_DIR=\"${CMAKE_INSTALL_PREFIX}/share/jcanvas\")

# find libcairo
pkg_check_modules(CAIRO REQUIRED cairo)

# find jmixin
pkg_check_modules(JMIXIN REQUIRED jmixin)

# widgets compilation
add_library(${PROJECT_NAME} SHARED 
  ${SRCS} 
)

include_directories(
  "${CMAKE_SOURCE_DIR}/widgets/src/include"
  "${CMAKE_SOURCE_DIR}/algebra/src/include"
  "${CMAKE_SOURCE_DIR}/core/src/include"
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CAIRO_INCLUDE_DIRS}
  ${JMIXIN_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${CAIRO_LIBRARIES}
  ${JMIXIN_LIBRARIES}
  pthread
)

if (TESTS)
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

install(DIRECTORY resources/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
install(DIRECTORY src/include/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

