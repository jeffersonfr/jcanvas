# Find egl project

# This module defines
#   EGL_INCLUDE_DIRS: jmixin headers
#   EGL_LIBRARIES: jmixin libraries
#   EGL_DEFINITIONS: some definitions
#
#   EGL_FOUND, If false, do not try to use EGL.

find_path (EGL_INCLUDE_DIR bcm_host.h GLES/gl.h GLES/glext.h EGL/egl.h
  /usr/include
  /usr/local/include
  /opt/vc/include
)

find_library (EGL_LIBRARY xcb x11-xcb egl glesv2 glu gl brcmGLESv2 brcmEGL 
  /usr/lib 
  /usr/local/lib 
  /opt/vc/lib
)

if (EGL_LIBRARY)
  if (EGL_INCLUDE_DIR)
    set (EGL_INCLUDE_DIRS ${EGL_INCLUDE_DIR})
    set (EGL_LIBRARIES ${EGL_LIBRARY})
    set (EGL_FOUND "YES")
  endif ()
endif ()
