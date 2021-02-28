# Find openvg project

# This module defines
#   OPENVG_INCLUDE_DIRS: jmixin headers
#   OPENVG_LIBRARIES: jmixin libraries
#   OPENVG_DEFINITIONS: some definitions
#
#   OPENVG_FOUND, If false, do not try to use OPENVG.

find_path (OPENVG_INCLUDE_DIR bcm_host.h GLES/gl.h EGL/egl.h VG/openvg.h
  /usr/include
  /usr/local/include
  /opt/vc/include
)

find_library (OPENVG_LIBRARY brcmGLESv2 brcmEGL
  /usr/lib 
  /usr/local/lib
  /opt/vc/lib
)

if (OPENVG_LIBRARY)
  if (OPENVG_INCLUDE_DIR)
    set (OPENVG_INCLUDE_DIRS ${OPENVG_INCLUDE_DIR})
    set (OPENVG_LIBRARIES ${OPENVG_LIBRARY})
    set (OPENVG_FOUND "YES")
  endif ()
endif ()
