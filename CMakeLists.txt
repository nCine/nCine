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
include(ncine_installation)

include(ProcessorCount)
ProcessorCount(NCINE_CORES)
if(NCINE_CORES EQUAL 0)
	set(NCINE_CORES 1)
endif()

include(ncine_imported_targets)
include(ncine_imgui)
include(ncine_tracy)

if(SDL2_FOUND AND GLFW_FOUND)
	if(NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
		set(GLFW_FOUND 0)
		message(STATUS "Using SDL2 as the preferred backend")
	elseif(NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
		set(SDL2_FOUND 0)
		message(STATUS "Using GLFW as the preferred backend")
	endif()
elseif(NOT SDL2_FOUND AND NOT GLFW_FOUND)
	message(FATAL_ERROR "Neither SDL2 nor GLFW have been found")
elseif(NOT GLFW_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
	message(WARNING "GLFW, the preferred backend, has not been found: using SDL2")
elseif(NOT SDL2_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
	message(WARNING "SDL2, the preferred backend, has not been found: using GLFW")
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

target_link_libraries(ncine PRIVATE OpenGL::GL)
target_sources(ncine PRIVATE ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${NCTL_HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})

if(NCINE_WITH_IMGUI)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${IMGUI_INCLUDE_ONLY_DIR}>
		PRIVATE $<BUILD_INTERFACE:${IMGUI_INCLUDE_ONLY_DIR}/ncine>)
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
		PRIVATE $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>)
endif()
if(APPLE)
	file(RELATIVE_PATH RELPATH_TO_FRAMEWORKS ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${FRAMEWORKS_INSTALL_DESTINATION})
	set_target_properties(ncine PROPERTIES INSTALL_RPATH "@loader_path/${RELPATH_TO_FRAMEWORKS}")
endif()

if(NCINE_TESTS_DATA_DIR)
	file(TO_CMAKE_PATH "${NCINE_TESTS_DATA_DIR}" NCINE_TESTS_DATA_DIR) # Always strips trailing slash
	set(NCINE_TESTS_DATA_DIR "${NCINE_TESTS_DATA_DIR}/")
endif()

if(WIN32)
	if(NCINE_INSTALL_DEV_SUPPORT)
		install(TARGETS ncine EXPORT nCineTargets RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT libraries
			ARCHIVE DESTINATION ${ARCHIVE_INSTALL_DESTINATION} COMPONENT libraries)
	else()
		install(TARGETS ncine EXPORT nCineTargets RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT libraries)
	endif()
else()
	install(TARGETS ncine EXPORT nCineTargets DESTINATION ${LIBRARY_INSTALL_DESTINATION} COMPONENT libraries)
endif()

install(FILES README.md DESTINATION ${README_INSTALL_DESTINATION} COMPONENT libraries)
if(MSVC OR APPLE)
	install(FILES LICENSE DESTINATION ${README_INSTALL_DESTINATION} COMPONENT libraries RENAME LICENSE.txt)
endif()
if(MSVC)
	install(DIRECTORY ${MSVC_BINDIR}/ DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT libraries)
elseif(APPLE)
	install(DIRECTORY ${CMAKE_FRAMEWORK_PATH}/ DESTINATION ${FRAMEWORKS_INSTALL_DESTINATION} COMPONENT libraries)
elseif(EMSCRIPTEN)
	install(DIRECTORY ${EXTERNAL_EMSCRIPTEN_DIR}/lib/ DESTINATION ${LIBRARY_INSTALL_DESTINATION} COMPONENT libraries)
endif()
if(NCINE_INSTALL_DEV_SUPPORT)
	install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DESTINATION}/ncine COMPONENT devsupport)
	install(FILES ${NCTL_HEADERS} DESTINATION ${INCLUDE_INSTALL_DESTINATION}/nctl COMPONENT devsupport)
	if(NOT NCINE_DYNAMIC_LIBRARY)
		install(FILES ${PRIVATE_HEADERS} DESTINATION ${INCLUDE_INSTALL_DESTINATION}/ncine COMPONENT devsupport)
	endif()
	if(NCINE_WITH_TRACY)
		install(DIRECTORY ${TRACY_INCLUDE_ONLY_DIR}/tracy/ DESTINATION ${INCLUDE_INSTALL_DESTINATION}/tracy COMPONENT devsupport)
	endif()
	install(TARGETS ncine_main EXPORT nCineTargets ARCHIVE DESTINATION ${ARCHIVE_INSTALL_DESTINATION} COMPONENT devsupport)
endif()
if(NOT NCINE_EMBED_SHADERS)
	install(DIRECTORY src/shaders/ DESTINATION ${SHADERS_INSTALL_DESTINATION} COMPONENT devsupport)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_SOURCE_DIR}/cmake/nCineConfig.cmake.in"
	"${CMAKE_BINARY_DIR}/nCineConfig.cmake"
	INSTALL_DESTINATION "${NCINE_CONFIG_INSTALL_DESTINATION}"
	PATH_VARS CMAKE_INSTALL_PREFIX
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/nCineConfigVersion.cmake" VERSION ${NCINE_VERSION} COMPATIBILITY SameMajorVersion)
# This makes the project importable from the build directory
export(TARGETS ncine ncine_main NAMESPACE ncine:: FILE nCineTargets.cmake)
if(NCINE_INSTALL_DEV_SUPPORT)
	install(FILES ${CMAKE_BINARY_DIR}/nCineConfig.cmake DESTINATION ${NCINE_CONFIG_INSTALL_DESTINATION} COMPONENT devsupport)
	install(FILES ${CMAKE_BINARY_DIR}/nCineConfigVersion.cmake DESTINATION ${NCINE_CONFIG_INSTALL_DESTINATION} COMPONENT devsupport)
	install(EXPORT nCineTargets DESTINATION ${NCINE_CONFIG_INSTALL_DESTINATION} NAMESPACE ncine:: COMPONENT devsupport)
endif()

include(ncine_build_documentation)
include(ncine_build_tests)
include(ncine_build_unit_tests)
include(ncine_build_benchmarks)
include(ncine_build_android)
include(ncine_strip_binaries)
