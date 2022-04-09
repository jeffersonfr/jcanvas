# This module defines
#   Egl_INCLUDE_DIRS: headers directory
#   Egl_LIBRARIES: libraries
#
#   Egl_FOUND, If false, do not try to use Egl.

set (ENV{PKG_CONFIG_LIBDIR} "$ENV{PKG_CONFIG_LIBDIR}:/opt/vc/lib/pkgconfig")

pkg_check_modules(LibXcb REQUIRED IMPORTED_TARGET xcb)
pkg_check_modules(LibX11Xcb REQUIRED IMPORTED_TARGET x11-xcb)
pkg_check_modules(LibEgl REQUIRED IMPORTED_TARGET egl)
pkg_check_modules(LibGlesv2 REQUIRED IMPORTED_TARGET glesv2)
pkg_check_modules(LibGl REQUIRED IMPORTED_TARGET gl)
pkg_check_modules(LibGlu REQUIRED IMPORTED_TARGET glu)

add_library (Egl UNKNOWN IMPORTED)

target_link_libraries (Egl
  INTERFACE
    PkgConfig::LibXcb
    PkgConfig::LibX11Xcb
    PkgConfig::LibEgl
    PkgConfig::LibGlesv2 
    PkgConfig::LibGl
    PkgConfig::LibGlu)
