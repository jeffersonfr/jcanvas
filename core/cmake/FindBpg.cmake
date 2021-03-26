# Find bpg project

# This module defines
#   BPG_INCLUDE_DIRS: jmixin headers
#   BPG_LIBRARIES: jmixin libraries
#   BPG_DEFINITIONS: some definitions
#
#   BPG_FOUND, If false, do not try to use BPG.

set (BPG_INCLUDE_DIRS 
  /usr/include
  /usr/local/include
)

set (BPG_LIBRARY_DIRS 
  /usr/lib 
  /usr/local/lib 
)

find_path (BPG_INCLUDE_DIR 
  NAMES libbpg.h
  PATHS ${BPG_INCLUDE_DIRS}
)

find_library (BPG_LIBRARY
  NAMES libbpg.a
  PATHS ${BPG_LIBRARY_DIRS}
)

find_path (BPG_LIBRARY_DIR
  NAMES libbpg.a
  PATHS ${BPG_LIBRARY_DIRS}
)

if (BPG_LIBRARY)
  if (BPG_INCLUDE_DIR)
    set (BGP_INCLUDE_DIRS ${BPG_INCLUDE_DIR})
    set (BGP_LIBRARY_DIRS ${BPG_INCLUDE_DIR})

    set (BPG_CFLAGS_OTHER)
    set (BPG_CFLAGS) 

    set (BPG_LDFLAGS_OTHER)
    set (BPG_LDFLAGS)

    set (BPG_LIBRARIES ${BPG_LIBRARY})

    set (BPG_FOUND "YES")
  endif ()
endif ()
