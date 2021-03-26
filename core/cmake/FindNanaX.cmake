# Find nanax project

# This module defines
#   NANAX_INCLUDE_DIRS: jmixin headers
#   NANAX_LIBRARIES: jmixin libraries
#   NANAX_DEFINITIONS: some definitions
#
#   NANAX_FOUND, If false, do not try to use NANAX.

set (NANAX_INCLUDE_DIRS
  /usr/include
  /usr/local/include
  /opt/nana/include
)

set (NANAX_LIBRARY_DIRS
  /usr/lib 
  /usr/local/lib 
  /opt/nana/build/bin
)

find_path (NANAX_INCLUDE_DIR 
  NAMES nana/gui.hpp nana/gui/screen.hpp nana/paint/pixel_buffer.hpp
  PATHS ${NANAX_INCLUDE_DIRS}
)

find_library (NANAX_LIBRARY
  NAMES X11 pthread Xft fontconfig stdc++fs nana
  PATHS ${NANAX_LIBRARY_DIRS}
)

find_path (NANAX_LIBRARY
  NAMES libnana.so
  PATHS ${NANAX_LIBRARY_DIRS}
)

if (NANAX_LIBRARY)
  if (NANAX_INCLUDE_DIR)
    set (NANAX_INCLUDE_DIRS ${NANAX_INCLUDE_DIR})
    set (NANAX_LIBRARY_DIRS ${NANAX_INCLUDE_DIR})

    set (NANAX_CFLAGS_OTHER)
    set (NANAX_CFLAGS)

    set (NANAX_LDFLAGS_OTHER)
    set (NANAX_LDFLAGS)

    set (NANAX_LIBRARIES ${NANAX_LIBRARY})

    set (NANAX_FOUND "YES")
  endif ()
endif ()
