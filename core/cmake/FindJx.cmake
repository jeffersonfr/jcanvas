# Find jx project

# This module defines
#   JX_INCLUDE_DIRS: jmixin headers
#   JX_LIBRARIES: jmixin libraries
#   JX_DEFINITIONS: some definitions
#
#   JX_FOUND, If false, do not try to use JX.

set (JX_INCLUDE_DIRS
  /usr/include
  /usr/local/include
  /home/jeff/projects/jx/jxclient/src/include
)

set (JX_LIBRARY_DIRS
  /usr/lib 
  /usr/local/lib 
  /home/jeff/projects/jx/build/jxclient
)

find_path (JX_INCLUDE_DIR
  NAMES jxclient.h
  PATHS ${JX_INCLUDE_DIRS}
)

find_library (JX_LIBRARY
  NAMES jxclient
  PATHS ${JX_LIBRARY_DIRS}
)

find_path (JX_LIBRARY_DIR
  NAMES libjxclient.so
  PATHS ${JX_LIBRARY_DIRS}
)

if (JX_LIBRARY)
  if (JX_INCLUDE_DIR)
    set (JX_INCLUDE_DIRS ${JX_INCLUDE_DIR})
    set (JX_LIBRARY_DIRS ${JX_INCLUDE_DIR})

    set (JX_CFLAGS_OTHER)
    set (JX_CFLAGS)

    set (JX_LDFLAGS_OTHER)
    set (JX_LDFLAGS)

    set (JX_LIBRARIES ${JX_LIBRARY})

    set (JX_FOUND "YES")
  endif ()
endif ()
