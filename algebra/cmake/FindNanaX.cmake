# Find nanax project

# This module defines
#   NANAX_INCLUDE_DIRS: jmixin headers
#   NANAX_LIBRARIES: jmixin libraries
#   NANAX_DEFINITIONS: some definitions
#
#   NANAX_FOUND, If false, do not try to use NANAX.

find_path (NANAX_INCLUDE_DIR nana/gui.hpp nana/gui/screen.hpp nana/paint/pixel_buffer.hpp
  /usr/include
  /usr/local/include
  /opt/nana/include
)

find_library (NANAX_LIBRARY X11 pthread Xft fontconfig stdc++fs nana 
  /usr/lib 
  /usr/local/lib 
  /opt/nana/build/bin
)

if (NANAX_LIBRARY)
  if (NANAX_INCLUDE_DIR)
    set (NANAX_INCLUDE_DIRS ${NANAX_INCLUDE_DIR})
    set (NANAX_LIBRARIES ${NANAX_LIBRARY})
    set (NANAX_FOUND "YES")
  endif ()
endif ()
