# Find flaschen project

# This module defines
#   FLASCHEN_INCLUDE_DIRS: jmixin headers
#   FLASCHEN_LIBRARIES: jmixin libraries
#   FLASCHEN_DEFINITIONS: some definitions
#
#   FLASCHEN_FOUND, If false, do not try to use FLASCHEN.

find_path (FLASCHEN_INCLUDE_DIR udp-flaschen-taschen.h
  /usr/include
  /usr/local/include
  /opt/flaschen-taschen/api/include
)

find_library (FLASCHEN_LIBRARY ftclient
  /usr/lib 
  /usr/local/lib 
  /opt/flaschen-taschen/api/lib
)

if (FLASCHEN_LIBRARY)
  if (FLASCHEN_INCLUDE_DIR)
    set (FLASCHEN_INCLUDE_DIRS ${FLASCHEN_INCLUDE_DIR})
    set (FLASCHEN_LIBRARIES ${FLASCHEN_LIBRARY})
    set (FLASCHEN_FOUND "YES")
  endif ()
endif ()
