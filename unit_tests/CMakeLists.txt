cmake_minimum_required(VERSION 2.8.12)
project(nCine-unit_tests)

if(WIN32)
	if(MSVC)
		add_custom_target(copy_dlls_unit_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/unit_tests
			COMMENT "Copying DLLs to tests..."
		)
		set_target_properties(copy_dlls_unit_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		if(MSVC)
			set(NCINE_LIBRARY_NAME ncine.dll)
		else()
			set(NCINE_LIBRARY_NAME libncine.dll)
		endif()

		if(MSVC_IDE)
			add_custom_target(copy_ncine_dll_unit_tests ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/unit_tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		else()
			add_custom_target(copy_ncine_dll_unit_tests ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/unit_tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		endif()
		set_target_properties(copy_ncine_dll_unit_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
elseif(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
endif()

list(APPEND TESTS
	gtest_array gtest_array_iterator gtest_array_reverseiterator gtest_array_operations gtest_array_algorithms gtest_carray_iterator gtest_array_movable
	gtest_staticarray gtest_staticarray_iterator gtest_staticarray_reverseiterator gtest_staticarray_operations gtest_staticarray_algorithms gtest_staticarray_movable
	gtest_list gtest_list_iterator gtest_list_operations gtest_list_algorithms gtest_list_movable
	gtest_string gtest_string_iterator gtest_string_reverseiterator gtest_string_operations
	gtest_hashmap gtest_hashmap_iterator gtest_hashmap_algorithms gtest_hashmap_string gtest_hashmap_movable
	gtest_vector2 gtest_vector3 gtest_vector4 gtest_rect
	gtest_matrix4x4 gtest_matrix4x4_operations gtest_quaternion gtest_quaternion_operations
	gtest_atomic32 gtest_atomic64
	gtest_uniqueptr gtest_uniqueptr_array gtest_sharedptr
	gtest_color gtest_colorf
)

foreach(TEST ${TESTS})
	add_executable(${TEST} ${TEST}.cpp test_functions.h)
	target_link_libraries(${TEST} ncine gtest_main)
	set_target_properties(${TEST} PROPERTIES FOLDER "UnitTests")
	add_test(NAME Tests-${TEST} COMMAND ${TEST})

	if(CMAKE_BUILD_TYPE MATCHES "Debug" AND NCINE_CODE_COVERAGE AND CODE_COVERAGE_SUPPORTED)
		set_target_properties(${TEST} PROPERTIES COMPILE_FLAGS "--coverage")
		set_target_properties(${TEST} PROPERTIES LINK_FLAGS "--coverage")
	endif()

	if(APPLE)
		set_target_properties(${APPTEST} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
	endif()
endforeach()
