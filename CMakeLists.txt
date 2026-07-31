cmake_minimum_required(VERSION 3.13)
project(nCine)

# Avoid warning about path normalization in CMake 3.31
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.31.0")
	cmake_policy(SET CMP0177 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

string(FIND ${CMAKE_SYSTEM_PROCESSOR} "arm" ARM_SUBSTRING_FOUND)
string(FIND ${CMAKE_SYSTEM_PROCESSOR} "aarch64" AARCH64_SUBSTRING_FOUND)
if((ARM_SUBSTRING_FOUND GREATER -1 OR AARCH64_SUBSTRING_FOUND GREATER -1) AND NOT APPLE)
	set(NCINE_ARM_PROCESSOR TRUE)
endif()

include(ncine_options)

if(NOT IS_DIRECTORY ${NCINE_DATA_DIR})
	message(WARNING "nCine data directory not found at: ${NCINE_DATA_DIR}")
else()
	message(STATUS "nCine data directory: ${NCINE_DATA_DIR}")
endif()

# imported_targets CMake scripts
include(imported_targets/ncine_imported_targets_common)
include(imported_targets/ncine_imported_targets_msvc)
include(imported_targets/ncine_imported_targets_msys)
include(imported_targets/ncine_imported_targets_linux)
include(imported_targets/ncine_imported_targets_apple)
include(imported_targets/ncine_imported_targets_android)
include(imported_targets/ncine_imported_targets_emscripten)

include(ncine_get_version)
include(download/ncine_imgui)
include(download/ncine_nuklear)
include(download/ncine_tracy)
include(download/ncine_qoi)

if(NOT (
	(NCINE_PREFERRED_BACKEND STREQUAL "GLFW" AND GLFW_FOUND) OR
	(NCINE_PREFERRED_BACKEND STREQUAL "SDL3" AND SDL3_FOUND) OR
	(NCINE_PREFERRED_BACKEND STREQUAL "SDL2" AND SDL2_FOUND) OR
	(NCINE_PREFERRED_BACKEND STREQUAL "QT6" AND Qt6_FOUND) OR
	(NCINE_PREFERRED_BACKEND STREQUAL "QT5" AND Qt5_FOUND)
))
	# Choose a fallback backend
	if(GLFW_FOUND)
		set(NCINE_PREFERRED_BACKEND "GLFW")
	elseif(SDL3_FOUND)
		set(NCINE_PREFERRED_BACKEND "SDL3")
	elseif(SDL2_FOUND)
		set(NCINE_PREFERRED_BACKEND "SDL2")
	elseif(Qt6_FOUND)
		set(NCINE_PREFERRED_BACKEND "QT6")
	elseif(Qt5_FOUND)
		set(NCINE_PREFERRED_BACKEND "QT5")
	else()
		message(FATAL_ERROR "None of the supported backends (GLFW, SDL3, SDL2, Qt6, and Qt5) was found.")
	endif()

	message(WARNING "Preferred backend unavailable, using ${NCINE_PREFERRED_BACKEND}")
else()
	message(STATUS "Using ${NCINE_PREFERRED_BACKEND} as the preferred backend")
endif()

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED)
else()
	add_library(ncine STATIC)
	target_compile_definitions(ncine PUBLIC "NCINE_STATIC")

	if(INSTALL_LIBRARIES_MINGW)
		target_include_directories(ncine INTERFACE ${EXTERNAL_MSYS_DIR}/include)
	endif()
endif()

if(EMSCRIPTEN AND EMSCRIPTEN_VERSION VERSION_GREATER_EQUAL "3.1.57")
	# embind requires -std=c++17 or newer
	set_target_properties(ncine PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS off)
endif()

add_library(ncine_main STATIC ${CMAKE_SOURCE_DIR}/tests/main.cpp)
target_link_libraries(ncine_main PRIVATE ncine)
set_target_properties(ncine PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(ncine_main PROPERTIES DEBUG_POSTFIX "_d")

include(ncine_compiler_options)

set(NCINE_ROOT ${CMAKE_SOURCE_DIR})
# sources CMake scripts
include(sources/ncine_sources_nctl)
include(sources/ncine_sources_base)
include(sources/ncine_sources_opengl)
include(sources/ncine_sources_scenegraph)
include(sources/ncine_sources_platforms)
include(sources/ncine_sources_backends)
include(sources/ncine_sources_audio)
include(sources/ncine_sources_image_formats)
include(sources/ncine_sources_threads)
include(sources/ncine_sources_lua)
include(sources/ncine_sources_allocators)
include(sources/ncine_sources_imgui)
include(sources/ncine_sources_nuklear)
include(sources/ncine_sources_tool_integrations)
include(sources/ncine_sources_generated)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("Headers" FILES ${HEADERS})
source_group("nCTL" FILES ${NCTL_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})
source_group("Sources" FILES ${SOURCES})
source_group("Shaders" FILES ${SHADER_FILES})
source_group("Generated" FILES ${GENERATED_SOURCES})
target_sources(ncine PRIVATE ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${NCTL_HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})

if("${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist" AND NOT NCINE_WITH_LUA)
	message(FATAL_ERROR "Cannot build a LuaDist version without Lua")
endif()

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
if(NCINE_WITH_QOI)
	target_include_directories(ncine
		PRIVATE $<BUILD_INTERFACE:${QOI_SOURCE_DIR}>)
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
	if(IS_DIRECTORY ${GENERATED_PRIVATE_INCLUDE_DIR})
		# If nCine has been built as a static library, projects need to access its private headers
		get_filename_component(PARENT_GENERATED_PRIVATE_INCLUDE_DIR ${GENERATED_PRIVATE_INCLUDE_DIR} DIRECTORY)
		target_include_directories(ncine
			INTERFACE $<BUILD_INTERFACE:${PARENT_GENERATED_PRIVATE_INCLUDE_DIR}>)
	endif()
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

if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/project)
	file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/project)
endif()
file(COPY ${CMAKE_SOURCE_DIR}/project DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/.clang-format DESTINATION ${CMAKE_BINARY_DIR}/project)
file(COPY ${CMAKE_SOURCE_DIR}/.editorconfig DESTINATION ${CMAKE_BINARY_DIR}/project)
file(COPY ${CMAKE_SOURCE_DIR}/.gitignore DESTINATION ${CMAKE_BINARY_DIR}/project)
foreach(FILE IN ITEMS "check_atomic.cmake" "FindGLFW.cmake" "FindOpenGLES2.cmake" "FindSDL2.cmake" "FindSDL3.cmake" "FindVorbis.cmake" "FindWebP.cmake")
	file(COPY ${CMAKE_SOURCE_DIR}/cmake/${FILE} DESTINATION ${CMAKE_BINARY_DIR}/project/cmake)
endforeach()
if(WIN32)
	file(COPY ${CMAKE_SOURCE_DIR}/WindowsHiDPI.manifest DESTINATION ${CMAKE_BINARY_DIR}/project)
endif()

include(ncine_extract_symbols)
include(ncine_strip_binaries)
include(ncine_installation)
include(ncine_build_documentation)
include(ncine_build_tests)
include(download/ncine_gtest)
include(ncine_build_unit_tests)
include(download/ncine_gbenchmark)
include(ncine_build_benchmarks)
include(ncine_build_android)
