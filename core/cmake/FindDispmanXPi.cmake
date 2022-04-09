# This module defines
#   DispmanXPi_INCLUDE_DIRS: headers directory
#   DispmanXPi_LIBRARIES: libraries
#
#   DispmanXPi_FOUND, If false, do not try to use DISPMANXPI.

set (ENV{PKG_CONFIG_LIBDIR} "$ENV{PKG_CONFIG_LIBDIR}:/opt/vc/lib/pkgconfig")

pkg_check_modules(LibBcmHost REQUIRED IMPORTED_TARGET bcm_host)

add_library(DispmanXPi ALIAS PkgConfig::LibBcmHost)
