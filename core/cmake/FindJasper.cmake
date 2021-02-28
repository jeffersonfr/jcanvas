# Find jasper project

# This module defines
#   JASPER_INCLUDE_DIRS: jmixin headers
#   JASPER_LIBRARIES: jmixin libraries
#   JASPER_DEFINITIONS: some definitions
#
#   JASPER_FOUND, If false, do not try to use JASPER.

find_path (JASPER_INCLUDE_DIR jasper/jasper.h
  /usr/include
  /usr/local/include
)

find_library (JASPER_LIBRARY jasper
  /usr/lib 
  /usr/local/lib 
)

if (JASPER_LIBRARY)
  if (JASPER_INCLUDE_DIR)
    set (JASPER_INCLUDE_DIRS ${JASPER_INCLUDE_DIR})
    set (JASPER_LIBRARIES ${JASPER_LIBRARY})
    set (JASPER_FOUND "YES")
  endif ()
endif ()
