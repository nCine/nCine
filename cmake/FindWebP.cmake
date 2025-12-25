# - Find WebP library
# Find the native WebP headers and libraries.
#
#  WEBP_INCLUDE_DIRS - where to find webp/decode.h, etc.
#  WEBP_LIBRARIES    - List of libraries when using webp.
#  WEBP_FOUND        - True if webp is found.

# Look for the header file.
find_path(WEBP_INCLUDE_DIR NAMES webp/decode.h)
mark_as_advanced(WEBP_INCLUDE_DIR)

# Look for the library.
find_library(WEBP_LIBRARY NAMES webp)
mark_as_advanced(WEBP_LIBRARY)

find_library(SHARPYUV_LIBRARY NAMES sharpyuv)
mark_as_advanced(SHARPYUV_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set WEBFOUND_FOUND to TRUE if 
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(WebP DEFAULT_MSG WEBP_LIBRARY WEBP_INCLUDE_DIR)

set(WEBP_LIBRARIES ${WEBP_LIBRARY})
set(WEBP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
