list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(NCPROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_BINARY_DIR ${CMAKE_BINARY_DIR} DIRECTORY)

include(project_check_info)

# Copy dot files
foreach(DOT_FILE IN ITEMS ".clang-format" ".editorconfig")
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${DOT_FILE})
		file(COPY ${NCPROJECT_ROOT}/${DOT_FILE} DESTINATION ${CMAKE_SOURCE_DIR})
	endif()
endforeach()

if(COMMAND callback_start)
	callback_start()
endif()

include(project_options)

include(find_ncine)
include(project_imported_targets)
if(NOT ANDROID AND NOT EMSCRIPTEN AND NCINE_WITH_QT5)
	find_package(Qt5 COMPONENTS Widgets)
endif()

if(NOT IS_DIRECTORY ${NCPROJECT_DATA_DIR})
	if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/data AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/icons)
		set(NCPROJECT_DATA_DIR ${CMAKE_SOURCE_DIR})
		message(STATUS "${NCPROJECT_NAME} data directory not found at ${NCPROJECT_DATA_DIR}: using source directory for data")
	else()
		message(WARNING "${NCPROJECT_NAME} data directory not found at: ${NCPROJECT_DATA_DIR}")
	endif()
else()
	message(STATUS "${NCPROJECT_NAME} data directory: ${NCPROJECT_DATA_DIR}")
endif()

include(project_get_version)

if(COMMAND callback_before_target)
	callback_before_target()
endif()

add_executable(${NCPROJECT_EXE_NAME} WIN32 ${NCPROJECT_SOURCES})
include(project_generated_sources)
target_sources(${NCPROJECT_EXE_NAME} PRIVATE ${GENERATED_SOURCES})

if(NOT DEFAULT_DATA_DIR_DIST) # Don't set the startup project if it wouldn't find the data directory
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NCPROJECT_EXE_NAME})
endif()

target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${NCPROJECT_INCLUDE_DIRS})
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${GENERATED_INCLUDE_DIR})
endif()

if(WIN32 AND NCINE_WTH_QT5)
	target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE Qt5::WinMain)
elseif(MINGW)
	# Should be linked before `ncine_main`
	target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE mingw32)
endif()
target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE ncine::ncine_main ncine::ncine)

target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE "$<$<CONFIG:Debug>:${NCPROJECT_UPPER_NAME}_DEBUG>")
target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE "$<$<CONFIG:Debug>:NCINE_ASSERT_BREAK>")

if(MSVC)
	set_property(TARGET ${NCPROJECT_EXE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>")
	target_compile_options(${NCPROJECT_EXE_NAME} PRIVATE /MP)
endif()

if(EMSCRIPTEN)
	target_compile_options(${NCPROJECT_EXE_NAME} PRIVATE "SHELL:--preload-file ${NCPROJECT_DATA_DIR}/data@ --no-heap-copy")
	target_link_options(${NCPROJECT_EXE_NAME} PRIVATE "SHELL:--preload-file ${NCPROJECT_DATA_DIR}/data@ --no-heap-copy")
	configure_file(${NCPROJECT_ROOT}/emscripten_shell.html.in ${CMAKE_BINARY_DIR}/${NCPROJECT_EXE_NAME}.html @ONLY)
	if(EXISTS ${NCPROJECT_DATA_DIR}/icons/icon.ico)
		file(COPY ${NCPROJECT_DATA_DIR}/icons/icon.ico DESTINATION ${CMAKE_BINARY_DIR})
		file(RENAME ${CMAKE_BINARY_DIR}/icon.ico ${CMAKE_BINARY_DIR}/favicon.ico)
	endif()
endif()

if(COMMAND callback_after_target)
	callback_after_target()
endif()

include(project_installation)

if(DEFAULT_DATA_DIR_DIST)
	if(MSVC OR APPLE)
		# Relative path from tests to data on Windows and OS X, where the user can choose the installation directory
		file(RELATIVE_PATH NCPROJECT_DEFAULT_DATA_DIR
			${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION}
			${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}) # Always strips trailing slash
		set(NCPROJECT_DEFAULT_DATA_DIR "${NCPROJECT_DEFAULT_DATA_DIR}/")
	else()
		set(NCPROJECT_DEFAULT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}/")
	endif()
elseif(NOT NCPROJECT_DEFAULT_DATA_DIR)
	set(NCPROJECT_DEFAULT_DATA_DIR "${NCPROJECT_DATA_DIR}/data/")
endif()

if(NCPROJECT_DEFAULT_DATA_DIR)
	file(TO_CMAKE_PATH "${NCPROJECT_DEFAULT_DATA_DIR}" NCPROJECT_DEFAULT_DATA_DIR) # Always strips trailing slash
	set(NCPROJECT_DEFAULT_DATA_DIR "${NCPROJECT_DEFAULT_DATA_DIR}/")

	message(STATUS "Default data directory: ${NCPROJECT_DEFAULT_DATA_DIR}")
	target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE "NCPROJECT_DEFAULT_DATA_DIR=\"${NCPROJECT_DEFAULT_DATA_DIR}\"")
endif()

include(project_copy_targets)
include(project_build_android)
include(project_strip_binaries)
if(COMMAND callback_end)
	callback_end()
endif()
