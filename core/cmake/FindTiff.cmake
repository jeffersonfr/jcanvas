# Find tiff project

# This module defines
#   TIFF_INCLUDE_DIRS: jmixin headers
#   TIFF_LIBRARIES: jmixin libraries
#   TIFF_DEFINITIONS: some definitions
#
#   TIFF_FOUND, If false, do not try to use TIFF.

find_path (TIFF_INCLUDE_DIR tiffio.h tiffio.hxx
  /usr/include
  /usr/local/include
)

find_library (TIFF_LIBRARY tiffxx
  /usr/lib 
  /usr/local/lib 
)

find_path (TIFF_LIBRARY libtiffxx.so
  /usr/lib 
  /usr/local/lib 
)

if (TIFF_LIBRARY)
  if (TIFF_INCLUDE_DIR)
    set (TIFF_INCLUDE_DIRS ${TIFF_INCLUDE_DIR})
    set (TIFF_CFLAGS_OTHER)
    set (TIFF_CFLAGS)

    set (TIFF_LIBRARY_DIRS ${TIFF_LIBRARY_DIR})
    set (TIFF_LDFLAGS_OTHER)
    set (TIFF_LDFLAGS)

    set (TIFF_LIBRARIES ${TIFF_LIBRARY})

    set (TIFF_FOUND "YES")
  endif ()
endif ()
