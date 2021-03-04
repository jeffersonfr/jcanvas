# Find flif project

# This module defines
#   FLIF_INCLUDE_DIRS: jmixin headers
#   FLIF_LIBRARIES: jmixin libraries
#   FLIF_DEFINITIONS: some definitions
#
#   FLIF_FOUND, If false, do not try to use FLIF.

find_path (FLIF_INCLUDE_DIR flif-enc.hpp flif-dec.hpp
  /usr/include
  /usr/local/include
)

find_library (FLIF_LIBRARY flif
  /usr/lib 
  /usr/local/lib
  /opt/FLIF/src
)

if (FLIF_LIBRARY)
  if (FLIF_INCLUDE_DIR)
    set (FLIF_INCLUDE_DIRS ${FLIF_INCLUDE_DIR})
    set (FLIF_LIBRARIES ${FLIF_LIBRARY})
    set (FLIF_FOUND "YES")
  endif ()
endif ()
