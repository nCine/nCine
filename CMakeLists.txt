cmake_minimum_required(VERSION 3.13)
project(nCine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

include(ncine_options)

if(NOT IS_DIRECTORY ${NCINE_DATA_DIR})
	message(WARNING "nCine data directory not found at: ${NCINE_DATA_DIR}")
else()
	message(STATUS "nCine data directory: ${NCINE_DATA_DIR}")
endif()

include(ncine_get_version)

include(ProcessorCount)
ProcessorCount(NCINE_CORES)
if(NCINE_CORES EQUAL 0)
	set(NCINE_CORES 1)
endif()

include(ncine_imported_targets)
include(ncine_imgui)
include(ncine_nuklear)
include(ncine_tracy)

if(NOT GLFW_FOUND AND NOT SDL2_FOUND AND NOT Qt5_FOUND)
	message(FATAL_ERROR "No backend between SDL2, GLFW, and QT5 has been found")
elseif(GLFW_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
	message(STATUS "Using GLFW as the preferred backend")
elseif(SDL2_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
	message(STATUS "Using SDL2 as the preferred backend")
elseif(Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
	message(STATUS "Using QT5 as the preferred backend")
endif()

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED)
else()
	add_library(ncine STATIC)
	target_compile_definitions(ncine PUBLIC "NCINE_STATIC")
endif()

add_library(ncine_main STATIC ${CMAKE_SOURCE_DIR}/tests/main.cpp)
target_link_libraries(ncine_main PRIVATE ncine)
set_target_properties(ncine PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(ncine_main PROPERTIES DEBUG_POSTFIX "_d")

include(ncine_compiler_options)

set(NCINE_ROOT ${CMAKE_SOURCE_DIR})
include(ncine_headers)
include(ncine_private_headers)
include(ncine_sources)
include(ncine_extra_sources)
include(ncine_generated_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
foreach(SOURCE_FILE IN LISTS HEADERS)
	source_group("Headers" FILES ${SOURCE_FILE})
endforeach()
foreach(SOURCE_FILE IN LISTS NCTL_HEADERS)
	source_group("nCTL" FILES ${SOURCE_FILE})
endforeach()
foreach(SOURCE_FILE IN LISTS PRIVATE_HEADERS)
	source_group("Private Headers" FILES ${SOURCE_FILE})
endforeach()
foreach(SOURCE_FILE IN LISTS SOURCES)
	source_group("Sources" FILES ${SOURCE_FILE})
endforeach()
foreach(SOURCE_FILE IN LISTS SHADER_FILES)
	source_group("Shaders" FILES ${SOURCE_FILE})
endforeach()
foreach(SOURCE_FILE ${GENERATED_SOURCES})
	source_group("Generated" FILES ${SOURCE_FILE})
endforeach()

if(OPENGL_FOUND)
	target_link_libraries(ncine PRIVATE OpenGL::GL)
endif()
target_sources(ncine PRIVATE ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${NCTL_HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})

if(NCINE_WITH_IMGUI)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${IMGUI_INCLUDE_ONLY_DIR}>
		PRIVATE $<BUILD_INTERFACE:${IMGUI_INCLUDE_ONLY_DIR}/ncine>)
endif()
if(NCINE_WITH_NUKLEAR)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${NUKLEAR_INCLUDE_ONLY_DIR}>
		PRIVATE $<BUILD_INTERFACE:${NUKLEAR_INCLUDE_ONLY_DIR}/ncine>)
endif()
if(NCINE_WITH_TRACY)
	target_include_directories(ncine
		PUBLIC $<BUILD_INTERFACE:${TRACY_INCLUDE_ONLY_DIR}/tracy>
		PUBLIC $<INSTALL_INTERFACE:include/tracy>)
endif()
target_include_directories(ncine
	INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
	INTERFACE $<INSTALL_INTERFACE:include>
	PRIVATE include
	PRIVATE include/ncine
	PRIVATE src/include)
if(NOT NCINE_DYNAMIC_LIBRARY)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/ncine>)
endif()
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	get_filename_component(PARENT_GENERATED_INCLUDE_DIR ${GENERATED_INCLUDE_DIR} DIRECTORY)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${PARENT_GENERATED_INCLUDE_DIR}>
		# Internal sources can access a generated header with or without the parent directory
		PRIVATE $<BUILD_INTERFACE:${PARENT_GENERATED_INCLUDE_DIR}>
		PRIVATE $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>)
endif()

if(NCINE_TESTS_DATA_DIR)
	file(TO_CMAKE_PATH "${NCINE_TESTS_DATA_DIR}" NCINE_TESTS_DATA_DIR) # Always strips trailing slash
	set(NCINE_TESTS_DATA_DIR "${NCINE_TESTS_DATA_DIR}/")
endif()

include(ncine_installation)
include(ncine_build_documentation)
include(ncine_build_tests)
include(ncine_build_unit_tests)
include(ncine_build_benchmarks)
include(ncine_build_android)
include(ncine_strip_binaries)
