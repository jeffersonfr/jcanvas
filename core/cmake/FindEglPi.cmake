# This module defines
#   EglPi_INCLUDE_DIRS: headers directory
#   EglPi_LIBRARIES: libraries
#
#   EglPi_FOUND, If false, do not try to use EglPi.

set (ENV{PKG_CONFIG_LIBDIR} "$ENV{PKG_CONFIG_LIBDIR}:/opt/vc/lib/pkgconfig")

pkg_check_modules(LibBrcmGlesv2 REQUIRED IMPORTED_TARGET brcmglesv2)
pkg_check_modules(LibBcmHost REQUIRED IMPORTED_TARGET bcm_host)

add_library (EglPi UNKNOWN IMPORTED)

target_link_libraries (EglPi
  INTERFACE
    PkgConfig::LibBrcmGlesv2
    PkgConfig::LibBcmHost)
