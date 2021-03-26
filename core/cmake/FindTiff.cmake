# Find tiff project

# This module defines
#   TIFF_INCLUDE_DIRS: jmixin headers
#   TIFF_LIBRARIES: jmixin libraries
#   TIFF_DEFINITIONS: some definitions
#
#   TIFF_FOUND, If false, do not try to use TIFF.

set (TIFF_INCLUDE_DIRS
  /usr/include
  /usr/local/include
)

set (TIFF_LIBRARY_DIRS
  /usr/lib 
  /usr/local/lib 
)

find_path (TIFF_INCLUDE_DIR
  NAMES tiffio.h tiffio.hxx
  PATHS ${TIFF_INCLUDE_DIRS}
)

find_library (TIFF_LIBRARY
  NAMES tiffxx
  PATHS ${TIFF_LIBRARY_DIRS}
)

find_path (TIFF_LIBRARY
  NAMES libtiffxx.so
  PATHS ${TIFF_LIBRARY_DIRS}
)

if (TIFF_LIBRARY)
  if (TIFF_INCLUDE_DIR)
    set (TIFF_INCLUDE_DIRS ${TIFF_INCLUDE_DIR})
    set (TIFF_LIBRARY_DIRS ${TIFF_INCLUDE_DIR})

    set (TIFF_CFLAGS_OTHER)
    set (TIFF_CFLAGS)

    set (TIFF_LDFLAGS_OTHER)
    set (TIFF_LDFLAGS)

    set (TIFF_LIBRARIES ${TIFF_LIBRARY})

    set (TIFF_FOUND "YES")
  endif ()
endif ()
