# This module defines
#   TIFF_DIR: library prefix

#   TIFF_INCLUDE_DIRS: headers directory
#   TIFF_LIBRARIES: libraries
#
#   TIFF_FOUND, If false, do not try to use TIFF.

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

if (TIFF_LIBRARIES AND TIFF_INCLUDE_DIRS)
  add_imported_library (Tiff ${TIFF_LIBRARIES} ${TIFF_INCLUDE_DIRS})

  return ()
endif ()

file (TO_CMAKE_PATH "$ENV{TIFF_DIR}" _TIFF_DIR_)

find_library (TIFF_LIBRARY_PATH NAMES tiffxx
  PATHS
    ${_TIFF_DIR_}/lib
    ${_TIFF_DIR_}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib 
    /usr/local/lib
  NO_DEFAULT_PATH)

find_path (TIFF_HEADER_PATH NAMES tiffio.h tiffio.hxx
  PATHS
    ${_TIFF_DIR_}/include
    ${_TIFF_DIR_}/local/include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  Tiff DEFAULT_MSG TIFF_LIBRARY_PATH TIFF_HEADER_PATH)

if (Tiff_FOUND)
  add_imported_library (Tiff ${TIFF_LIBRARY_PATH} ${TIFF_HEADER_PATH})
endif ()

