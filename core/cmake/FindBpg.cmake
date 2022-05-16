# This module defines
#   BPG_DIR: library prefix

#   BPG_INCLUDE_DIRS: headers directory
#   BPG_LIBRARIES: libraries
#
#   BPG_FOUND, If false, do not try to use BPG.

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

if (BPG_LIBRARIES AND BPG_INCLUDE_DIRS)
  add_imported_library (Bpg ${BPG_LIBRARIES} ${BPG_INCLUDE_DIRS})

  return ()
endif ()

file (TO_CMAKE_PATH "$ENV{BPG_DIR}" _BPG_DIR_)

find_library (BPG_LIBRARY_PATH NAMES bpg
  PATHS
    ${_BPG_DIR_}/lib
    ${_BPG_DIR_}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib 
    /usr/local/lib
  NO_DEFAULT_PATH)

find_path (BPG_HEADER_PATH NAMES libbpg.h
  PATHS
    ${_BPG_DIR_}/include
    ${_BPG_DIR_}/local/include
    /usr/include
    /usr/local/include
  NO_DEFAULT_PATH)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  Bpg DEFAULT_MSG BPG_LIBRARY_PATH BPG_HEADER_PATH)

if (Bpg_FOUND)
  add_imported_library (Bpg ${BPG_LIBRARY_PATH} ${BPG_HEADER_PATH})
endif ()

