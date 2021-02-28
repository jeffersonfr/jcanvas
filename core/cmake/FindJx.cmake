# Find jx project

# This module defines
#   JX_INCLUDE_DIRS: jmixin headers
#   JX_LIBRARIES: jmixin libraries
#   JX_DEFINITIONS: some definitions
#
#   JX_FOUND, If false, do not try to use JX.

find_path (JX_INCLUDE_DIR jxclient.h
  /usr/include
  /usr/local/include
  /home/jeff/projects/jx/jxclient/src/include
)

find_library (JX_LIBRARY jxclient
  /usr/lib 
  /usr/local/lib 
  /home/jeff/projects/jx/build/jxclient
)

if (JX_LIBRARY)
  if (JX_INCLUDE_DIR)
    set (JX_INCLUDE_DIRS ${JX_INCLUDE_DIR})
    set (JX_LIBRARIES ${JX_LIBRARY})
    set (JX_FOUND "YES")
  endif ()
endif ()
