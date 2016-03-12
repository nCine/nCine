cmake_minimum_required(VERSION 2.8.12)
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
			add_custom_target(copy_ncine_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		else()
			add_custom_target(copy_ncine_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		endif()
		set_target_properties(copy_ncine_dll PROPERTIES FOLDER "CustomCopyTargets")
	endif()
endif()

if((SDLIMAGE_FOUND OR PNG_FOUND) AND WEBP_FOUND)
	list(APPEND APPTESTS apptest_texformats apptest_joystick apptest_rotozoom
		apptest_animsprites apptest_particles apptest_scene apptest_font)
	if(OPENAL_FOUND)
		list(APPEND APPTESTS apptest_audio)
	endif()
endif()

foreach(APPTEST ${APPTESTS})
	add_executable(${APPTEST} main.cpp ${APPTEST}.cpp)
	target_link_libraries(${APPTEST} ncine)
	set_target_properties(${APPTEST} PROPERTIES FOLDER "AppTests")
	install(TARGETS ${APPTEST} RUNTIME DESTINATION bin)
endforeach()

list(APPEND TESTS test_array test_staticarray
	test_list test_string
	test_hashmap test2_hashmap)
list(APPEND TESTS test_algorithms_array
	test_algorithms_list
	test_algorithms_hashmap)
list(APPEND TESTS test_vector2 test_vector3 test_vector4
	test_matrix4x4 test2_matrix4x4
	test_quaternion test2_quaternion)
	
foreach(TEST ${TESTS})
	add_executable(${TEST} ${TEST}.cpp test_functions.h)
	target_link_libraries(${TEST} ncine)
	set_target_properties(${TEST} PROPERTIES FOLDER "Tests")
	install(TARGETS ${TEST} RUNTIME DESTINATION bin)
endforeach()
