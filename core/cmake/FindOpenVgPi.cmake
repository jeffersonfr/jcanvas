# This module defines
#   OpenVgPi_INCLUDE_DIRS: headers directory
#   OpenVgPi_LIBRARIES: libraries
#
#   OpenVgPi_FOUND, If false, do not try to use OpenVgPi.

set (ENV{PKG_CONFIG_LIBDIR} "$ENV{PKG_CONFIG_LIBDIR}:/opt/vc/lib/pkgconfig")

pkg_check_modules(LibBrcmGlesv2 REQUIRED IMPORTED_TARGET brcmglesv2)
pkg_check_modules(LibBrcmVg REQUIRED IMPORTED_TARGET brcmvg)
pkg_check_modules(LibBcmHost REQUIRED IMPORTED_TARGET bcm_host)

add_library (OpenVgPi UNKNOWN IMPORTED)

target_link_libraries (OpenVgPi
  INTERFACE
    PkgConfig::LibBrcmGlesv2
    PkgConfig::LibBrcmVg
    PkgConfig::LibBcmHost)

