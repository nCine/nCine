if(NOT DEFINED nCine_DIR)
	if(DEFINED ENV{NCINE_DIR} AND IS_DIRECTORY $ENV{NCINE_DIR})
		set(CMAKE_PREFIX_PATH $ENV{NCINE_DIR})
	elseif(MSVC AND NOT DEFINED CMAKE_PREFIX_PATH)
		get_filename_component(CMAKE_PREFIX_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\nCine]" ABSOLUTE)
	endif()
endif()
find_package(nCine REQUIRED)

set(NCINE_CONFIGURATION "RELEASE" CACHE STRING "Preferred nCine configuration type when more than one has been exported")
get_target_property(NCINE_CONFIGURATIONS ncine::ncine IMPORTED_CONFIGURATIONS)
message(STATUS "nCine exported build configurations: ${NCINE_CONFIGURATIONS} (preferred: ${NCINE_CONFIGURATION})")

get_target_property(NCINE_LOCATION ncine::ncine IMPORTED_LOCATION_${NCINE_CONFIGURATION})
if(NOT EXISTS ${NCINE_LOCATION})
	unset(NCINE_CONFIGURATION CACHE)
	foreach(NCINE_CFG ${NCINE_CONFIGURATIONS})
		get_target_property(NCINE_LOCATION ncine::ncine IMPORTED_LOCATION_${NCINE_CFG})
		if(EXISTS ${NCINE_LOCATION})
			message(STATUS "Preferred configuration unavailable, changing to ${NCINE_CFG}")
			set(NCINE_CONFIGURATION ${NCINE_CFG})
			break()
		endif()
	endforeach()
endif()
if(NOT DEFINED NCINE_CONFIGURATION)
	message(FATAL_ERROR "No nCine build configuration found")
endif()

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
	# Remap project configurations to match the available engine configuration
	foreach(NCPROJECT_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${NCPROJECT_CONFIGURATION_TYPE} NCPROJECT_CONFIGURATION)
		set_target_properties(ncine::ncine PROPERTIES
			MAP_IMPORTED_CONFIG_${NCPROJECT_CONFIGURATION} ${NCINE_CONFIGURATION})
		set_target_properties(ncine::ncine_main PROPERTIES
			MAP_IMPORTED_CONFIG_${NCPROJECT_CONFIGURATION} ${NCINE_CONFIGURATION})
	endforeach()
endif()

message(STATUS "NCINE_DYNAMIC_LIBRARY: " ${NCINE_DYNAMIC_LIBRARY})
message(STATUS "nCine library: ${NCINE_LOCATION}")
if(WIN32 AND NCINE_DYNAMIC_LIBRARY)
	get_target_property(NCINE_IMPLIB ncine::ncine IMPORTED_IMPLIB_${NCINE_CONFIGURATION})
	message(STATUS "nCine import library: ${NCINE_IMPLIB}")
endif()
get_target_property(NCINE_INCLUDE_DIR ncine::ncine INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "nCine include directory: ${NCINE_INCLUDE_DIR}")
get_target_property(NCINE_MAIN_LOCATION ncine::ncine_main IMPORTED_LOCATION_${NCINE_CONFIGURATION})
message(STATUS "nCine main function library: ${NCINE_MAIN_LOCATION}")

find_file(NCINE_CONFIG_H config.h
	PATHS ${NCINE_INCLUDE_DIR}
	PATH_SUFFIXES ncine
	NO_DEFAULT_PATH
	NO_CMAKE_FIND_ROOT_PATH) # To find the file on Emscripten

if(EXISTS ${NCINE_CONFIG_H})
	message(STATUS "Configuration file: \"${NCINE_CONFIG_H}\"")

	file(STRINGS ${NCINE_CONFIG_H} NCINE_CONFIG_STRINGS REGEX "#define NCINE_WITH_")
	foreach(NCINE_CONFIG_STRING ${NCINE_CONFIG_STRINGS})
		if(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_THREADS 1")
			set(NCINE_WITH_THREADS ON)
			message(STATUS "NCINE_WITH_THREADS: " ${NCINE_WITH_THREADS})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_JOBSYSTEM 1")
			set(NCINE_WITH_JOBSYSTEM ON)
			message(STATUS "NCINE_WITH_JOBSYSTEM: " ${NCINE_WITH_JOBSYSTEM})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_JOBSYSTEM_STATISTICS 1")
			set(NCINE_WITH_JOBSYSTEM_STATISTICS ON)
			message(STATUS "NCINE_WITH_JOBSYSTEM_STATISTICS: " ${NCINE_WITH_JOBSYSTEM_STATISTICS})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_OPENGLES 1")
			set(NCINE_WITH_OPENGLES ON)
			message(STATUS "NCINE_WITH_OPENGLES: " ${NCINE_WITH_OPENGLES})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_ANGLE 1")
			set(NCINE_WITH_ANGLE ON)
			message(STATUS "NCINE_WITH_ANGLE: " ${NCINE_WITH_ANGLE})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_GLEW 1")
			set(NCINE_WITH_GLEW ON)
			message(STATUS "NCINE_WITH_GLEW: " ${NCINE_WITH_GLEW})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_GLFW 1")
			set(NCINE_WITH_GLFW ON)
			message(STATUS "NCINE_WITH_GLFW: " ${NCINE_WITH_GLFW})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_SDL 1")
			set(NCINE_WITH_SDL ON)
			message(STATUS "NCINE_WITH_SDL: " ${NCINE_WITH_SDL})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_QT5 1")
			set(NCINE_WITH_QT5 ON)
			message(STATUS "NCINE_WITH_QT5: " ${NCINE_WITH_QT5})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_AUDIO 1")
			set(NCINE_WITH_AUDIO ON)
			message(STATUS "NCINE_WITH_AUDIO: " ${NCINE_WITH_AUDIO})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_VORBIS 1")
			set(NCINE_WITH_VORBIS ON)
			message(STATUS "NCINE_WITH_VORBIS: " ${NCINE_WITH_VORBIS})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_OPENAL_EXT 1")
			set(NCINE_WITH_OPENAL_EXT ON)
			message(STATUS "NCINE_WITH_OPENAL_EXT: " ${NCINE_WITH_OPENAL_EXT})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_PNG 1")
			set(NCINE_WITH_PNG ON)
			message(STATUS "NCINE_WITH_PNG: " ${NCINE_WITH_PNG})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_WEBP 1")
			set(NCINE_WITH_WEBP ON)
			message(STATUS "NCINE_WITH_WEBP: " ${NCINE_WITH_WEBP})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_LUA 1")
			set(NCINE_WITH_LUA ON)
			message(STATUS "NCINE_WITH_LUA: " ${NCINE_WITH_LUA})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_SCRIPTING_API 1")
			set(NCINE_WITH_SCRIPTING_API ON)
			message(STATUS "NCINE_WITH_SCRIPTING_API: " ${NCINE_WITH_SCRIPTING_API})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_SCENEGRAPH 1")
			set(NCINE_WITH_SCENEGRAPH ON)
			message(STATUS "NCINE_WITH_SCENEGRAPH: " ${NCINE_WITH_SCENEGRAPH})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_ALLOCATORS 1")
			set(NCINE_WITH_ALLOCATORS ON)
			message(STATUS "NCINE_WITH_ALLOCATORS: " ${NCINE_WITH_ALLOCATORS})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_IMGUI 1")
			set(NCINE_WITH_IMGUI ON)
			message(STATUS "NCINE_WITH_IMGUI: " ${NCINE_WITH_IMGUI})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_NUKLEAR 1")
			set(NCINE_WITH_NUKLEAR ON)
			message(STATUS "NCINE_WITH_NUKLEAR: " ${NCINE_WITH_NUKLEAR})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_TRACY 1")
			set(NCINE_WITH_TRACY ON)
			message(STATUS "NCINE_WITH_TRACY: " ${NCINE_WITH_TRACY})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_RENDERDOC 1")
			set(NCINE_WITH_RENDERDOC ON)
			message(STATUS "NCINE_WITH_RENDERDOC: " ${NCINE_WITH_RENDERDOC})
		elseif(NCINE_CONFIG_STRING STREQUAL "#define NCINE_WITH_CRASHPAD 1")
			set(NCINE_WITH_CRASHPAD ON)
			message(STATUS "NCINE_WITH_CRASHPAD: " ${NCINE_WITH_CRASHPAD})
		endif()
	endforeach()
endif()

if(NOT NCINE_EMBEDDED_SHADERS)
	if(IS_DIRECTORY ${NCINE_SHADERS_DIR})
		message(STATUS "nCine shaders directory: ${NCINE_SHADERS_DIR}")
	else()
		message(FATAL_ERROR "nCine shaders directory not found at: ${NCINE_SHADERS_DIR}")
	endif()
endif()

if(NCPROJECT_BUILD_ANDROID)
	if(IS_DIRECTORY ${NCINE_ANDROID_DIR})
		message(STATUS "nCine Android directory: ${NCINE_ANDROID_DIR}")
	else()
		message(FATAL_ERROR "nCine Android directory not found at: ${NCINE_ANDROID_DIR}")
	endif()

	set(NCINE_EXTERNAL_ANDROID_DIR "" CACHE PATH "Path to the nCine external Android libraries directory")
	if(NOT IS_DIRECTORY ${NCINE_EXTERNAL_ANDROID_DIR})
		unset(NCINE_EXTERNAL_ANDROID_DIR CACHE)
		find_path(NCINE_EXTERNAL_ANDROID_DIR
			NAMES libopenal.so
			PATHS ${PARENT_SOURCE_DIR}/nCine-android-external ${PARENT_BINARY_DIR}/nCine-android-external
			PATH_SUFFIXES openal/armeabi-v7a openal/arm64-v8a openal/x86_64
			DOC "Path to the nCine external Android libraries directory")

		if(IS_DIRECTORY ${NCINE_EXTERNAL_ANDROID_DIR})
			get_filename_component(NCINE_EXTERNAL_ANDROID_DIR ${NCINE_EXTERNAL_ANDROID_DIR} DIRECTORY)
			get_filename_component(NCINE_EXTERNAL_ANDROID_DIR ${NCINE_EXTERNAL_ANDROID_DIR} DIRECTORY)
			set(NCINE_EXTERNAL_ANDROID_DIR ${NCINE_EXTERNAL_ANDROID_DIR} CACHE PATH "Path to the nCine external Android libraries directory" FORCE)
		endif()
	endif()

	if(IS_DIRECTORY ${NCINE_EXTERNAL_ANDROID_DIR})
		message(STATUS "nCine external Android libraries directory: ${NCINE_EXTERNAL_ANDROID_DIR}")
	else()
		message(STATUS "nCine external Android libraries directory not found at: ${NCINE_EXTERNAL_ANDROID_DIR}")
	endif()
endif()

get_filename_component(NCINE_LOCATION_DIR ${NCINE_LOCATION} DIRECTORY)
if(MSVC)
	set(MSVC_ARCH_SUFFIX "x86")
	if(MSVC_C_ARCHITECTURE_ID MATCHES 64 OR MSVC_CXX_ARCHITECTURE_ID MATCHES 64)
		set(MSVC_ARCH_SUFFIX "x64")
	endif()

	find_path(MSVC_BINDIR
		NAMES glfw3.dll SDL2.dll
		PATHS ${NCINE_LOCATION_DIR} ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external ${PARENT_BINARY_DIR}/nCine-external
		PATH_SUFFIXES bin bin/${MSVC_ARCH_SUFFIX}
		DOC "Path to the nCine external MSVC DLL libraries directory"
		NO_DEFAULT_PATH) # To avoid finding MSYS/MinGW libraries

	if(NOT NCINE_DYNAMIC_LIBRARY)
		find_path(MSVC_LIBDIR
			NAMES glfw3.lib SDL2.lib
			PATHS ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external ${PARENT_BINARY_DIR}/nCine-external
			PATH_SUFFIXES lib/${MSVC_ARCH_SUFFIX}
			DOC "Path to the nCine external MSVC import libraries directory"
			NO_DEFAULT_PATH) # To avoid finding MSYS/MinGW libraries

		get_filename_component(EXTERNAL_MSVC_DIR ${MSVC_LIBDIR} DIRECTORY)
		get_filename_component(EXTERNAL_MSVC_DIR ${EXTERNAL_MSVC_DIR} DIRECTORY)

		if(IS_DIRECTORY ${MSVC_BINDIR} AND IS_DIRECTORY ${MSVC_LIBDIR} AND IS_DIRECTORY ${EXTERNAL_MSVC_DIR})
			message(STATUS "nCine external MSVC directory: ${EXTERNAL_MSVC_DIR}")
		else()
			message(FATAL_ERROR "nCine external MSVC directory not found at: ${EXTERNAL_MSVC_DIR}")
		endif()
	else()
		if(IS_DIRECTORY ${MSVC_BINDIR})
			message(STATUS "nCine MSVC binaries directory: ${MSVC_BINDIR}")
		else()
			message(FATAL_ERROR "nCine MSVC binaries directory not found at: ${MSVC_BINDIR}")
		endif()
	endif()
elseif(APPLE)
	find_path(FRAMEWORKS_DIR
		NAMES glfw.framework sdl2.framework
		PATHS ${NCINE_LOCATION_DIR}/../../Frameworks ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external ${PARENT_BINARY_DIR}/nCine-external
		PATH_SUFFIXES bin/${ARCH_SUFFIX}
		DOC "Path to the nCine frameworks directory")

	if(IS_DIRECTORY ${FRAMEWORKS_DIR})
		message(STATUS "nCine frameworks directory: ${FRAMEWORKS_DIR}")
	else()
		message(FATAL_ERROR "nCine frameworks directory not found at: ${FRAMEWORKS_DIR}")
	endif()
elseif(EMSCRIPTEN)
	find_path(EMSCRIPTEN_LIBDIR
		NAMES libwebp.a liblua.a
		PATHS ${NCINE_LOCATION_DIR} ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external-emscripten ${PARENT_BINARY_DIR}/nCine-external-emscripten
		PATH_SUFFIXES lib
		DOC "Path to the nCine external Emscripten libraries directory"
		NO_DEFAULT_PATH # To avoid finding MSYS/MinGW libraries
		NO_CMAKE_FIND_ROOT_PATH) # To avoid using the cross-compiler root

	get_filename_component(EXTERNAL_EMSCRIPTEN_DIR ${EMSCRIPTEN_LIBDIR} DIRECTORY)
	if(IS_DIRECTORY ${EMSCRIPTEN_LIBDIR} AND IS_DIRECTORY ${EXTERNAL_EMSCRIPTEN_DIR})
		message(STATUS "nCine external Emscripten directory: ${EXTERNAL_EMSCRIPTEN_DIR}")
	else()
		message(FATAL_ERROR "nCine external Emscripten directory not found at: ${EXTERNAL_EMSCRIPTEN_DIR}")
	endif()
endif()

if(NCPROJECT_WITH_CRASHPAD)
	find_program(CRASHPAD_HANDLER_EXE
		NAMES crashpad_handler crashpad_handler.exe
		PATHS ${NCINE_LOCATION_DIR} ${NCINE_EXTERNAL_DIR}
		PATH_SUFFIXES bin tests
		DOC "The Crashpad handler executable")
	if (EXISTS ${CRASHPAD_HANDLER_EXE})
		message(STATUS "Crashpad handler: ${CRASHPAD_HANDLER_EXE}")
	else()
		message(WARNING "Crashpad handler not found")
	endif()
endif()
