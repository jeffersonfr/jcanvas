# This module defines
#   JASPER_DIR: library prefix

#   JASPER_INCLUDE_DIRS: headers directory
#   JASPER_LIBRARIES: libraries
#
#   JASPER_FOUND, If false, do not try to use JASPER.

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

if (JASPER_LIBRARIES AND JASPER_INCLUDE_DIRS)
  add_imported_library (Jasper ${JASPER_LIBRARIES} ${JASPER_INCLUDE_DIRS})

  return ()
endif ()

file (TO_CMAKE_PATH "$ENV{JASPER_DIR}" _JASPER_DIR_)

find_library (JASPER_LIBRARY_PATH NAMES jasper
  PATHS
    ${_JASPER_DIR_}/lib
    ${_JASPER_DIR_}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib 
    /usr/local/lib
  NO_DEFAULT_PATH)

find_path (JASPER_HEADER_PATH NAMES jasper/jasper.h
  PATHS
    ${_JASPER_DIR_}/include
    ${_JASPER_DIR_}/local/include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  Jasper DEFAULT_MSG JASPER_LIBRARY_PATH JASPER_HEADER_PATH)

if (Jasper_FOUND)
  add_imported_library (Jasper ${JASPER_LIBRARY_PATH} ${JASPER_HEADER_PATH})
endif ()

