cmake_minimum_required(VERSION 3.1)
project(nCine-tests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC AND NOT DEFINED CMAKE_PREFIX_PATH AND NOT DEFINED nCine_DIR)
	get_filename_component(CMAKE_PREFIX_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\nCine]" ABSOLUTE)
endif()
find_package(nCine REQUIRED)
if(NOT NCINE_DYNAMIC_LIBRARY)
	include(ncine_imported_targets)
endif()

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

message(STATUS "nCine library: ${NCINE_LOCATION}")
if(WIN32 AND NCINE_DYNAMIC_LIBRARY)
	get_target_property(NCINE_IMPLIB ncine::ncine IMPORTED_IMPLIB_${NCINE_CONFIGURATION})
	message(STATUS "nCine import library: ${NCINE_IMPLIB}")
endif()
get_target_property(NCINE_INCLUDE_DIR ncine::ncine INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "nCine include directory: ${NCINE_INCLUDE_DIR}")
get_target_property(NCINE_MAIN_LOCATION ncine::ncine_main IMPORTED_LOCATION_${NCINE_CONFIGURATION})
message(STATUS "nCine main.cpp: ${NCINE_MAIN_LOCATION}")

if(MSVC)
	set(MSVC_ARCH_SUFFIX "x86")
	if(MSVC_C_ARCHITECTURE_ID MATCHES 64 OR MSVC_CXX_ARCHITECTURE_ID MATCHES 64)
		set(MSVC_ARCH_SUFFIX "x64")
	endif()
		
	get_filename_component(NCINE_LOCATION_DIR ${NCINE_LOCATION} DIRECTORY)
	get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
	get_filename_component(PARENT_BINARY_DIR ${CMAKE_BINARY_DIR} DIRECTORY)
	find_path(MSVC_BINDIR
		NAMES libpng16.dll libogg.dll libvorbis.dll
		PATHS ${NCINE_LOCATION_DIR} ${PARENT_SOURCE_DIR}/nCine-external ${PARENT_BINARY_DIR}/nCine-external
		PATH_SUFFIXES bin/${MSVC_ARCH_SUFFIX}
		DOC "Path to the nCine external libraries directory")
		
	file(GLOB MSVC_DLL_FILES ${MSVC_BINDIR}/*.dll)
	add_custom_target(copy_dlls_tests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_DLL_FILES} ${CMAKE_BINARY_DIR}
		COMMENT "Copying DLLs to tests..."
	)
	set_target_properties(copy_dlls_tests PROPERTIES FOLDER "CustomCopyTargets")
endif()

list(APPEND APPTESTS apptest_texformats apptest_joystick apptest_rotozoom apptest_animsprites
	apptest_audio apptest_particles apptest_scene apptest_font apptest_multitouch apptest_camera
	apptest_meshsprites apptest_meshdeform apptest_sinescroller apptest_lua apptest_luareload)

foreach(APPTEST ${APPTESTS})
	if(DEFINED ${APPTEST}_SOURCES)
		# More complex AppTests can define multiple sources
		list(LENGTH ${APPTEST}_SOURCES NUM_SOURCES)
		if(NUM_SOURCES GREATER 0)
			add_executable(${APPTEST} ${${APPTEST}_SOURCES} apptest_datapath.h)
		endif()
	else()
		add_executable(${APPTEST} ${APPTEST}.cpp ${APPTEST}.h apptest_datapath.h)
	endif()

	target_compile_definitions(${APPTEST} PRIVATE "NCINE_TESTS_DATA_DIR=\"${NCINE_TESTS_DATA_DIR}/\"")
	target_link_libraries(${APPTEST} PRIVATE ncine::ncine_main ncine::ncine)
	set_target_properties(${APPTEST} PROPERTIES FOLDER "AppTests")
endforeach()
