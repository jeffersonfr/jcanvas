# This module defines
#   FLASCHEN_DIR: library prefix

#   FLASCHEN_INCLUDE_DIRS: headers directory
#   FLASCHEN_LIBRARIES: libraries
#
#   FLASCHEN_FOUND, If false, do not try to use FLASCHEN.

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

if (FLASCHEN_LIBRARIES AND FLASCHEN_INCLUDE_DIRS)
  add_imported_library (Flaschen ${FLASCHEN_LIBRARIES} ${FLASCHEN_INCLUDE_DIRS})

  return ()
endif ()

file (TO_CMAKE_PATH "$ENV{FLASCHEN_DIR}" _FLASCHEN_DIR_)

find_library (FLASCHEN_LIBRARY_PATH NAMES ftclient
  PATHS
    ${_FLASCHEN_DIR_}/lib
    ${_FLASCHEN_DIR_}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib 
    /usr/local/lib
    /opt/flaschen-taschen/api/lib
  NO_DEFAULT_PATH)

find_path (FLASCHEN_HEADER_PATH NAMES udp-flaschen-taschen.h
  PATHS
    ${_FLASCHEN_DIR_}/include
    ${_FLASCHEN_DIR_}/local/include
    /usr/include
    /usr/local/include
    /opt/flaschen-taschen/api/include
  NO_DEFAULT_PATH)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  FLASCHEN DEFAULT_MSG FLASCHEN_LIBRARY_PATH FLASCHEN_HEADER_PATH)

if (FLASCHEN_FOUND)
  add_imported_library (Flaschen ${FLASCHEN_LIBRARY_PATH} ${FLASCHEN_HEADER_PATH})
endif ()

