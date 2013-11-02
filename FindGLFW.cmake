# Locate the glfw library
# This module defines the following variables:
# GLFW_LIBRARY, the name of the library;
# GLFW_INCLUDE_DIR, where to find glfw include files.
# GLFW_FOUND, true if both the GLFW_LIBRARY and GLFW_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you could define an environment variable called
# GLFW_ROOT which points to the root of the glfw library installation. This is pretty useful
# on a Windows platform.
#
#
# Usage example to compile an "executable" target to the glfw library:
#
# FIND_PACKAGE (glfw REQUIRED)
# INCLUDE_DIRECTORIES (${GLFW_INCLUDE_DIR})
# ADD_EXECUTABLE (executable ${EXECUTABLE_SRCS})
# TARGET_LINK_LIBRARIES (executable ${GLFW_LIBRARY})
#
# TODO:
# Allow the user to select to link to a shared library or to a static library.

#Search for the include file...
FIND_PATH(GLFW_INCLUDE_DIR GLFW/glfw3.h DOC "Path to GLFW include directory."
  HINTS
  $ENV{GLFW_ROOT}
  PATH_SUFFIX include #For finding the include file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/include/
  /usr/local/include/
  # By default headers are under GL subfolder
  /usr/include/GLFW
  /usr/local/include/GLFW
  ${GLFW_ROOT_DIR}/include/ # added by ptr
)
MARK_AS_ADVANCED(GLFW_INCLUDE_DIR)

FIND_LIBRARY(GLFW_LIBRARY DOC "Absolute path to GLFW library."
  NAMES glfw glfw3 glfw3.lib
  HINTS
  $ENV{GLFW_ROOT}
  PATH_SUFFIXES lib/win32 #For finding the library file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/local/lib
  /usr/lib
  ${GLFW_ROOT_DIR}/lib-msvc100/release # added by ptr
)
MARK_AS_ADVANCED(GLFW_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set WEBFOUND_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)

# On Mac OS X GLFW depends on the Cocoa framework
if(APPLE)
	SET(GLFW_LIBRARY "-framework Cocoa -framework IOKit" ${GLFW_LIBRARY})
endif()

SET(GLFW_LIBRARIES ${GLFW_LIBRARY})
SET(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})

