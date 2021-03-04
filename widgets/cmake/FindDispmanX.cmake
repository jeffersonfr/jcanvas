# Find dispmanx project

# This module defines
#   DISPMANX_INCLUDE_DIRS: jmixin headers
#   DISPMANX_LIBRARIES: jmixin libraries
#   DISPMANX_DEFINITIONS: some definitions
#
#   DISPMANX_FOUND, If false, do not try to use DISPMANX.

find_path (DISPMANX_INCLUDE_DIR bcm_host.h
  /usr/include
  /usr/local/include
  /opt/vc/include
)

find_library (DISPMANX_LIBRARY bcm_host m png pthread jpeg
  /usr/lib 
  /usr/local/lib 
  /opt/vc/lib
)

if (DISPMANX_LIBRARY)
  if (DISPMANX_INCLUDE_DIR)
    set (DISPMANX_INCLUDE_DIRS ${DISPMANX_INCLUDE_DIR})
    set (DISPMANX_LIBRARIES ${DISPMANX_LIBRARY})
    set (DISPMANX_FOUND "YES")
  endif ()
endif ()
