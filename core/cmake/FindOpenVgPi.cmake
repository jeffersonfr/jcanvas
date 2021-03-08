# Find egl project

# This module defines
#   OPENVG_INCLUDE_DIRS: jmixin headers
#   OPENVG_LIBRARIES: jmixin libraries
#   OPENVG_DEFINITIONS: some definitions
#
#   OPENVG_FOUND, If false, do not try to use EGL.

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/opt/vc/lib/pkgconfig")

pkg_check_modules(LIBEGL REQUIRED brcmglesv2)
pkg_check_modules(LIBOPENVG REQUIRED brcmvg)
pkg_check_modules(LIBBCMHOST REQUIRED bcm_host)

if (${LIBEGL_FOUND} AND ${LIBOPENVG_FOUND} AND ${LIBBCMHOST_FOUND})
	set (OPENVG_INCLUDE_DIRS ${LIBEGL_INCLUDE_DIR} ${LIBOPENVG_INCLUDE_DIR} ${LIBBCMHOST_INCLUDE_DIRS})
  set (OPENVG_CFLAGS_OTHER ${LIBEGL_CFLAGS_OTHER} ${LIBOPENVG_CFLAGS_OTHER} ${LIBBCMHOST_CFLAGS_OTHER})
  set (OPENVG_CFLAGS ${OPENVG_CFLAGS_OTHER} ${OPENVG_INCLUDE_DIRS})

	set (OPENVG_LIBRARY_DIRS ${LIBEGL_LIBRARY_DIRS} ${LIBOPENVG_LIBRARY_DIRS} ${LIBBCMHOST_LIBRARY_DIRS})
  set (OPENVG_LDFLAGS_OTHER ${LIBEGL_LDFLAGS_OTHER} ${LIBOPENVG_LDFLAGS_OTHER} ${LIBBCMHOST_LDFLAGS_OTHER})
	set (OPENVG_LDFLAGS ${OPENVG_LDFLAGS_OTHER} ${OPENVG_LIBRARY_DIRS})

	set (OPENVG_LIBRARIES ${LIBEGL_LIBRARIES} ${LIBOPENVG_LIBRARIES} ${LIBBCMHOST_LIBRARIES})

	set (OPENVG_FOUND "YES")
endif()
