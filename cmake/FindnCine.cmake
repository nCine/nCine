#.rst:
# FindnCine
# -----------
#
# Try to find the nCine game engine library
#
# Once done this will define
#
# ::
#
#   NCINE_FOUND - System has the nCine library
#   NCINE_INCLUDE_DIR - The nCine include directory
#   NCINE_LIBRARY - The library needed to use the nCine

find_path(NCINE_INCLUDE_DIR Application.h
	DOC "Path to the nCine include directory"
	HINTS
	$ENV{NCINE_ROOT}/include
	PATH_SUFFIXES
	ncine
	PATHS
	/usr/include
	/usr/local/include
	/sw/include
	/opt/local/include
	$ENV{PROGRAMFILES}/nCine/include
	${NCINE_ROOT_DIR}/include)
mark_as_advanced(NCINE_INCLUDE_DIR)

find_library(NCINE_LIBRARY
	DOC "Absolute path to the nCine library"
	NAMES ncine
	HINTS
	$ENV{NCINE_ROOT}
	PATHS
	/usr/lib64
	/usr/lib
	/usr/local/lib64
	/usr/local/lib
	/sw/lib
	/opt/local/lib
	$ENV{PROGRAMFILES}/nCine/lib
	${NCINE_ROOT_DIR}/lib)
mark_as_advanced(NCINE_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set NCINE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NCINE DEFAULT_MSG NCINE_LIBRARY NCINE_INCLUDE_DIR)

set(NCINE_LIBRARIES ${NCINE_LIBRARY})
set(NCINE_INCLUDE_DIRS ${NCINE_INCLUDE_DIR})
