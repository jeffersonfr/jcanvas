# Find jmixin project

# This module defines
#   JMIXIN_INCLUDE_DIRS: jmixin headers
#   JMIXIN_LIBRARIES: jmixin libraries
#   JMIXIN_DEFINITIONS: some definitions
#
#   JMIXIN_FOUND, If false, do not try to use JMIXIN.

find_path (JMIXIN_INCLUDE_DIR jmixin/jstring.h 
  /usr/include
  /usr/local/include
)

find_library (JMIXIN_LIBRARY jmixin
  /usr/lib 
  /usr/local/lib 
)

if (JMIXIN_LIBRARY)
  if (JMIXIN_INCLUDE_DIR)
    set (JMIXIN_INCLUDE_DIRS ${JMIXIN_INCLUDE_DIR})
    set (JMIXIN_LIBRARIES ${JMIXIN_LIBRARY})
    set (JMIXIN_FOUND "YES")
  endif ()
endif ()

if (NOT JMIXIN_FOUND)
  message (SEND_ERROR "jmixin not found")
endif()
