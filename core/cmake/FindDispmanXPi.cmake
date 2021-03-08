# Find egl project

# This module defines
#   DISPMANX_INCLUDE_DIRS: jmixin headers
#   DISPMANX_LIBRARIES: jmixin libraries
#   DISPMANX_DEFINITIONS: some definitions
#
#   DISPMANX_FOUND, If false, do not try to use EGL.

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/opt/vc/lib/pkgconfig")

pkg_check_modules(LIBBCMHOST REQUIRED bcm_host)

if (${LIBBCMHOST_FOUND})
  set (DISPMANX_INCLUDE_DIRS ${LIBBCMHOST_INCLUDE_DIRS})
  set (DISPMANX_CFLAGS_OTHER ${LIBBCMHOST_CFLAGS_OTHER})
  set (DISPMANX_CFLAGS ${LIBBCMHOST_CFLAGS})

  set (DISPMANX_LIBRARY_DIRS ${LIBBCMHOST_LIBRARY_DIRS})
  set (DISPMANX_LDFLAGS_OTHER ${LIBBCMHOST_LDFLAGS_OTHER})
  set (DISPMANX_LDFLAGS ${LIBBCMHOST_LDFLAGS})

  set (DISPMANX_LIBRARIES ${LIBBCMHOST_LIBRARIES})

  set (DISPMANX_FOUND "YES")
endif()
