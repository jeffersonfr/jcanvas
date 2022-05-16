# This module defines
#   FLIF_DIR: library prefix

#   FLIF_INCLUDE_DIRS: headers directory
#   FLIF_LIBRARIES: libraries
#
#   FLIF_FOUND, If false, do not try to use FLIF.

function (add_imported_library name libraries headers)
  add_library (${name} UNKNOWN IMPORTED)

  set_target_properties (${name} PROPERTIES
    IMPORTED_LOCATION ${libraries}
    INTERFACE_INCLUDE_DIRECTORIES ${headers})

  set (${name}_FOUND 1 CACHE INTERNAL "${name} found" FORCE)
  set (${name}_LIBRARIES ${libraries} CACHE STRING "path to ${name} library" FORCE)
  set (${name}_INCLUDE_DIRS ${headers} CACHE STRING "path to ${name} headers" FORCE)

  mark_as_advanced(FORCE ${name}_LIBRARIES)
  mark_as_advanced(FORCE ${name}_INCLUDE_DIRS)
endfunction ()

if (FLIF_LIBRARIES AND FLIF_INCLUDE_DIRS)
  add_imported_library (Flif ${FLIF_LIBRARIES} ${FLIF_INCLUDE_DIRS})

  return ()
endif ()

file (TO_CMAKE_PATH "$ENV{FLIF_DIR}" _FLIF_DIR_)

find_library (FLIF_LIBRARY_PATH NAMES flif
  PATHS
    ${_FLIF_DIR_}/lib
    ${_FLIF_DIR_}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib 
    /usr/local/lib
    /opt/FLIF/src
  NO_DEFAULT_PATH)

find_path (FLIF_HEADER_PATH NAMES flif-enc.hpp flif-dec.hpp
  PATHS
    ${_FLIF_DIR_}/include
    ${_FLIF_DIR_}/local/include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  Flif DEFAULT_MSG FLIF_LIBRARY_PATH FLIF_HEADER_PATH)

if (Flif_FOUND)
  add_imported_library (Flif ${FLIF_LIBRARY_PATH} ${FLIF_HEADER_PATH})
endif ()

