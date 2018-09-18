cmake_minimum_required(VERSION 2.8.12)
project(nCine-tests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC OR APPLE)
	if(DEFINED ENV{NCINE_HOME} AND IS_DIRECTORY $ENV{NCINE_HOME})
		set(NCINE_INSTALL_DIR $ENV{NCINE_HOME})
	endif()
endif()

if(MSVC)
	get_filename_component(NCINE_REGISTRY_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\nCine]" ABSOLUTE)
	if(IS_DIRECTORY ${NCINE_REGISTRY_PATH} AND NOT DEFINED ENV{NCINE_HOME})
		set(NCINE_INSTALL_DIR ${NCINE_REGISTRY_PATH})
	endif()
endif()

if(NCINE_INSTALL_DIR)
	file(TO_CMAKE_PATH ${NCINE_INSTALL_DIR} NCINE_INSTALL_DIR)
	message(STATUS "nCine installation found at: ${NCINE_INSTALL_DIR}")
	find_package(nCine PATHS ${NCINE_INSTALL_DIR} NO_DEFAULT_PATH)
else()
	find_package(nCine REQUIRED)
	if(nCine_FOUND)
		get_filename_component(NCINE_INSTALL_DIR ${NCINE_MAIN_CPP} DIRECTORY)
	endif()
endif()

if(APPLE)
	get_filename_component(RESOURCES_DIR ${NCINE_LIBRARY} DIRECTORY)
	get_filename_component(RESOURCES_DIR ${RESOURCES_DIR} DIRECTORY)
	set(NCINE_TESTS_DATA_DIR ${RESOURCES_DIR}/data/)
else()
	set(NCINE_TESTS_DATA_DIR ${NCINE_INSTALL_DIR}/data/)
endif()

message(STATUS "nCine library: ${NCINE_LIBRARY}")
message(STATUS "nCine include directory: ${NCINE_INCLUDE_DIR}")
message(STATUS "nCine main.cpp: ${NCINE_MAIN_CPP}")
message(STATUS "nCine data directory for tests: ${NCINE_TESTS_DATA_DIR}")

if(MSVC)
	file(GLOB MSVC_DLL_FILES ${NCINE_INSTALL_DIR}/bin/*.dll)

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
			add_executable(${APPTEST} ${NCINE_MAIN_CPP} ${${APPTEST}_SOURCES} apptest_datapath.h ${RESOURCE_RC_FILE})
		endif()
	else()
		add_executable(${APPTEST} ${NCINE_MAIN_CPP} ${APPTEST}.cpp ${APPTEST}.h apptest_datapath.h ${RESOURCE_RC_FILE})
	endif()

	target_include_directories(${APPTEST} PUBLIC ${NCINE_INCLUDE_DIR})
	target_compile_definitions(${APPTEST} PRIVATE "NCINE_TESTS_DATA_DIR=\"${NCINE_TESTS_DATA_DIR}\"")
	target_link_libraries(${APPTEST} ${NCINE_LIBRARY})
	set_target_properties(${APPTEST} PROPERTIES FOLDER "AppTests")
	if(MSVC)
		target_compile_options(${APPTEST} PRIVATE "/wd4251")
	endif()
endforeach()
