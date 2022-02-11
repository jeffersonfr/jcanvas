project (jcanvas-widgets)

include (${PROJECT_SOURCE_DIR}/../Defines.cmake)

set(SRCS
  src/jactionevent.cpp
  src/jactionlistener.cpp
  src/jadjustmentevent.cpp
  src/jadjustmentlistener.cpp
  src/janimation.cpp
  src/jbackground.cpp
  src/jbeveledrectangleborder.cpp
  src/jborder.cpp
  src/jborderlayout.cpp
  src/jbutton.cpp
  src/jcalendardialog.cpp
  src/jcardlayout.cpp
  src/jcheckbutton.cpp
  src/jcheckbuttongroup.cpp
  src/jcolumnlayout.cpp
  src/jcomponent.cpp
  src/jcomponentevent.cpp
  src/jcomponentlistener.cpp
  src/jcontainer.cpp
  src/jcontainerevent.cpp
  src/jcontainerlistener.cpp
  src/jcoordinatelayout.cpp
  src/jdialog.cpp
  src/jfilechooserdialog.cpp
  src/jflatimage.cpp
  src/jflowlayout.cpp
  src/jfocusevent.cpp
  src/jfocuslistener.cpp
  src/jframe.cpp
  src/jgridbaglayout.cpp
  src/jgridlayout.cpp
  src/jimagebackground.cpp
  src/jimageborder.cpp
  src/jinputdialog.cpp
  src/jitemcomponent.cpp
  src/jkeyboarddialog.cpp
  src/jlayout.cpp
  src/jlistbox.cpp
  src/jloweredbeveledrectangleborder.cpp
  src/jloweredetchedrectangleborder.cpp
  src/jloweredgradientrectangleborder.cpp
  src/jmarquee.cpp
  src/jmessagedialog.cpp
  src/jnulllayout.cpp
  src/jpanel.cpp
  src/jprogressbar.cpp
  src/jraisedbeveledrectangleborder.cpp
  src/jraisedetchedrectangleborder.cpp
  src/jraisedgradientrectangleborder.cpp
  src/jrectangleborder.cpp
  src/jroundedrectangleborder.cpp
  src/jrowlayout.cpp
  src/jscrollbar.cpp
  src/jselectevent.cpp
  src/jselectlistener.cpp
  src/jslider.cpp
  src/jsolidbackground.cpp
  src/jspin.cpp
  #src/jtable.cpp
  src/jtext.cpp
  src/jtextevent.cpp
  src/jtextlistener.cpp
  src/jtheme.cpp
  src/jtoastdialog.cpp
  #src/jtreelistview.cpp
  src/jyesnodialog.cpp
)

# find libcairo
pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)

# find jmixin
pkg_check_modules(jMixin REQUIRED IMPORTED_TARGET jmixin)

# find threads
find_package(Threads REQUIRED)

# widgets compilation
add_library(${PROJECT_NAME}
  SHARED 
    ${SRCS} 
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include"
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    PkgConfig::Cairo
    PkgConfig::jMixin
    Threads::Threads
    jcanvas-core
)

if (TESTS)
  add_subdirectory(tests)
endif()

if (EXAMPLES)
  add_subdirectory(examples)
endif()

install(DIRECTORY src/include/jcanvas DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

