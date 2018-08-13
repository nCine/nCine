cmake_minimum_required(VERSION 3.1)
project(nCine-tests)

if(WIN32)
	if(MSVC)
		add_custom_target(copy_dlls_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/tests
			COMMENT "Copying DLLs to tests..."
		)
		set_target_properties(copy_dlls_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		if(MSVC)
			set(NCINE_LIBRARY_NAME ncine.dll)
		else()
			set(NCINE_LIBRARY_NAME libncine.dll)
		endif()

		if(MSVC_IDE)
			add_custom_target(copy_ncine_dll_tests ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		else()
			add_custom_target(copy_ncine_dll_tests ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		endif()
		set_target_properties(copy_ncine_dll_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
elseif(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
endif()

if(PNG_FOUND)
	list(APPEND APPTESTS apptest_texformats apptest_joystick apptest_rotozoom apptest_animsprites
		apptest_particles apptest_scene apptest_font apptest_multitouch apptest_camera
		apptest_meshsprites apptest_meshdeform apptest_sinescroller)
	if(OPENAL_FOUND)
		list(APPEND APPTESTS apptest_audio)
	endif()
	if(LUA_FOUND)
		list(APPEND APPTESTS apptest_lua apptest_luareload)
	endif()
endif()

foreach(APPTEST ${APPTESTS})
	add_executable(${APPTEST} main.cpp ${APPTEST}.cpp apptest_datapath.h ${RESOURCE_RC_FILE})
	target_link_libraries(${APPTEST} ncine)
	set_target_properties(${APPTEST} PROPERTIES FOLDER "AppTests")
	target_compile_definitions(${APPTEST} PRIVATE "NCINE_TESTS_DATA_DIR=\"${NCINE_TESTS_DATA_DIR}\"")
	target_include_directories(${APPTEST} PRIVATE ${GENERATED_INCLUDE_DIR}) # for ncine_config.h and for the Windows executable icon
	if(APPLE)
		set_target_properties(${APPTEST} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
	endif()
	install(TARGETS ${APPTEST} RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
endforeach()

list(FIND APPTESTS ${NCINE_STARTUP_TEST} STARTUP_TEST_INDEX)
if(STARTUP_TEST_INDEX GREATER -1)
	if(NOT TESTS_DATA_DIR_DIST) # Don't set the startup test if it wouldn't find the data directory
		set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NCINE_STARTUP_TEST})
	endif()
	message(STATUS "Startup test target: \"${NCINE_STARTUP_TEST}\"")
else()
	message(FATAL_ERROR "Startup test target not found: \"${NCINE_STARTUP_TEST}\"")
endif()
