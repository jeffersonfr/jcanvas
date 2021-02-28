# Find bpg project

# This module defines
#   BPG_INCLUDE_DIRS: jmixin headers
#   BPG_LIBRARIES: jmixin libraries
#   BPG_DEFINITIONS: some definitions
#
#   BPG_FOUND, If false, do not try to use BPG.

find_path (BPG_INCLUDE_DIR libbpg.h
  /usr/include
  /usr/local/include
)

find_library (BPG_LIBRARY libbpg.a
  /usr/lib 
  /usr/local/lib 
)

if (BPG_LIBRARY)
  if (BPG_INCLUDE_DIR)
    set (BPG_INCLUDE_DIRS ${BPG_INCLUDE_DIR})
    set (BPG_LIBRARIES ${BPG_LIBRARY})
    set (BPG_FOUND "YES")
  endif ()
endif ()
