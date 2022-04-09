# This module defines
#   NANAX_DIR: library prefix

#   NANAX_INCLUDE_DIRS: headers directory
#   NANAX_LIBRARIES: libraries
#
#   NANAX_FOUND, If false, do not try to use NANAX.

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

if (NANAX_LIBRARIES AND NANAX_INCLUDE_DIRS)
  add_imported_library (NanaX ${NANAX_LIBRARIES} ${NANAX_INCLUDE_DIRS})

  return ()
endif ()

file (TO_CMAKE_PATH "$ENV{NANAX_DIR}" _NANAX_DIR_)

find_library (NANAX_LIBRARY_PATH NAMES X11 pthread Xft fontconfig stdc++fs nana
  PATHS
    ${_NANAX_DIR_}/lib
    ${_NANAX_DIR_}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    /usr/lib 
    /usr/local/lib
    /opt/nana/build/bin
  NO_DEFAULT_PATH)

find_path (NANAX_HEADER_PATH NAMES nana/gui.hpp nana/gui/screen.hpp nana/paint/pixel_buffer.hpp
  PATHS
    ${_NANAX_DIR_}/include
    ${_NANAX_DIR_}/local/include
    /usr/include
    /usr/local/include
    /opt/nana/include
  NO_DEFAULT_PATH)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  NANAX DEFAULT_MSG NANAX_LIBRARY_PATH NANAX_HEADER_PATH)

if (NANAX_FOUND)
  add_imported_library (NanaX ${NANAX_LIBRARY_PATH} ${NANAX_HEADER_PATH})
endif ()

