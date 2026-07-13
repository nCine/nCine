# Locate SDL3 library
# This module defines
# SDL3_LIBRARY, the name of the library to link against
# SDL3_FOUND, if false, do not try to link to SDL3
# SDL3_INCLUDE_DIR, where to find SDL.h
#
#
# Based on the `FindSDL2.cmake` script in the same directory

FIND_PATH(SDL3_INCLUDE_DIR SDL.h
	HINTS
	${SDL3}
	$ENV{SDL3}
	PATH_SUFFIXES include/SDL3 include SDL3
	i686-w64-mingw32/include/SDL3
	x86_64-w64-mingw32/include/SDL3
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/include/SDL3
	/usr/include/SDL3
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

# Lookup the 64 bit libs on x64
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(SDL3_LIBRARY_TEMP SDL3
		HINTS
		${SDL3}
		$ENV{SDL3}
		PATH_SUFFIXES lib64 lib
		lib/x64
		x86_64-w64-mingw32/lib
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
	)
# On 32bit build find the 32bit libs
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(SDL3_LIBRARY_TEMP SDL3
		HINTS
		${SDL3}
		$ENV{SDL3}
		PATH_SUFFIXES lib
		lib/x86
		i686-w64-mingw32/lib
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

# SDL3 may require threads on your system.
# The Apple build may not need an explicit flag because one of the
# frameworks may already provide it.
# But for non-OSX systems, I will use the CMake Threads package.
IF(NOT APPLE)
	FIND_PACKAGE(Threads)
ENDIF(NOT APPLE)

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDL3main -lSDL3 -lmwindows
# (Actually on second look, I think it only needs one of the m* libraries.)
IF(MINGW)
	SET(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
ENDIF(MINGW)

SET(SDL3_FOUND "NO")
IF(SDL3_LIBRARY_TEMP)
	# For threads, as mentioned Apple doesn't need this.
	# In fact, there seems to be a problem if I used the Threads package
	# and try using this line, so I'm just skipping it entirely for OS X.
	IF(NOT APPLE)
		SET(SDL3_LIBRARY_TEMP ${SDL3_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
	ENDIF(NOT APPLE)

	# For MinGW library
	IF(MINGW)
		SET(SDL3_LIBRARY_TEMP ${MINGW32_LIBRARY} ${SDL3_LIBRARY_TEMP})
	ENDIF(MINGW)

	# Set the final string here so the GUI reflects the final state.
	SET(SDL3_LIBRARY ${SDL3_LIBRARY_TEMP} CACHE STRING "Where the SDL3 Library can be found")
	# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
	SET(SDL3_LIBRARY_TEMP "${SDL3_LIBRARY_TEMP}" CACHE INTERNAL "")

	SET(SDL3_FOUND "YES")
ENDIF(SDL3_LIBRARY_TEMP)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL3 REQUIRED_VARS SDL3_LIBRARY SDL3_INCLUDE_DIR)

