# Find jasper project

# This module defines
#   JASPER_INCLUDE_DIRS: jmixin headers
#   JASPER_LIBRARIES: jmixin libraries
#   JASPER_DEFINITIONS: some definitions
#
#   JASPER_FOUND, If false, do not try to use JASPER.

set (JASPER_INCLUDE_DIRS
  /usr/include
  /usr/local/include
)

set (JASPER_LIBRARY_DIRS
  /usr/lib 
  /usr/local/lib 
)

find_path (JASPER_INCLUDE_DIR
  NAMES jasper/jasper.h
  PATHS ${JASPER_INCLUDE_DIRS}
)

find_library (JASPER_LIBRARY
  NAMES jasper
  PATHS ${JASPER_LIBRARY_DIRS}
)

find_path (JASPER_LIBRARY
  NAMES libjasper.so
  PATHS ${JASPER_LIBRARY_DIRS}
)

if (JASPER_LIBRARY)
  if (JASPER_INCLUDE_DIR)
    set (JASPER_INCLUDE_DIRS ${JASPER_INCLUDE_DIR})
    set (JASPER_LIBRARY_DIRS ${JASPER_INCLUDE_DIR})

    set (JASPER_CFLAGS_OTHER)
    set (JASPER_CFLAGS)

    set (JASPER_LDFLAGS_OTHER)
    set (JASPER_LDFLAGS)

    set (JASPER_LIBRARIES ${JASPER_LIBRARY})

    set (JASPER_FOUND "YES")
  endif ()
endif ()
