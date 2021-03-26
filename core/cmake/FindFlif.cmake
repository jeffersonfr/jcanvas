# Find flif project

# This module defines
#   FLIF_INCLUDE_DIRS: jmixin headers
#   FLIF_LIBRARIES: jmixin libraries
#   FLIF_DEFINITIONS: some definitions
#
#   FLIF_FOUND, If false, do not try to use FLIF.

set (FLIF_INCLUDE_DIRS
  /usr/include
  /usr/local/include
)

set (FLIF_LIBRARY_DIRS
  /usr/lib 
  /usr/local/lib
  /opt/FLIF/src
)

find_path (FLIF_INCLUDE_DIR
  NAMES flif-enc.hpp flif-dec.hpp
  PATHS ${FLIF_INCLUDE_DIRS}
)

find_library (FLIF_LIBRARY
  NAMES flif
  PATHS ${FLIF_LIBRARY_DIRS}
)

find_path (FLIF_LIBRARY_DIR
  NAMES libflif.so
  PATHS ${FLIF_LIBRARY_DIRS}
)

if (FLIF_LIBRARY)
  if (FLIF_INCLUDE_DIR)
    set (FLIF_INCLUDE_DIRS ${FLIF_INCLUDE_DIR})
    set (FLIF_LIBRARY_DIRS ${FLIF_INCLUDE_DIR})

    set (FLIF_CFLAGS_OTHER )
    set (FLIF_CFLAGS)

    set (FLIF_LDFLAGS_OTHER)
    set (FLIF_LDFLAGS)

    set (FLIF_LIBRARIES ${FLIF_LIBRARY})
    set (FLIF_FOUND "YES")
  endif ()
endif ()
