cmake_minimum_required(VERSION 3.13)
project(nCine-unit_tests)

if(WIN32)
	if(MSVC)
		add_custom_target(copy_dlls_unit_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${MSVC_BINDIR} ${CMAKE_BINARY_DIR}/unit_tests
			COMMENT "Copying DLLs to tests..."
		)
		set_target_properties(copy_dlls_unit_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		add_custom_target(copy_ncine_dll_unit_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ncine> ${CMAKE_BINARY_DIR}/unit_tests
			DEPENDS ncine
			COMMENT "Copying nCine DLL..."
		)
		set_target_properties(copy_ncine_dll_unit_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
elseif(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
endif()

list(APPEND TESTS
	gtest_array gtest_array_zerocapacity gtest_array_iterator gtest_array_reverseiterator gtest_array_operations gtest_array_algorithms gtest_carray_iterator gtest_array_movable gtest_array_refcounted
	gtest_staticarray gtest_staticarray_iterator gtest_staticarray_reverseiterator gtest_staticarray_operations gtest_staticarray_algorithms gtest_staticarray_movable gtest_staticarray_refcounted
	gtest_list gtest_list_iterator gtest_list_operations gtest_list_algorithms gtest_list_movable gtest_list_refcounted
	gtest_string gtest_string_iterator gtest_string_reverseiterator gtest_string_operations
	gtest_hashmap gtest_hashmap_iterator gtest_hashmap_algorithms gtest_hashmap_string gtest_hashmap_cstring gtest_hashmap_movable gtest_hashmap_refcounted
	gtest_statichashmap gtest_statichashmap_iterator gtest_statichashmap_algorithms gtest_statichashmap_string gtest_statichashmap_cstring gtest_statichashmap_movable gtest_statichashmap_refcounted
	gtest_hashmaplist gtest_hashmaplist_iterator gtest_hashmaplist_algorithms gtest_hashmaplist_string gtest_hashmaplist_cstring gtest_hashmaplist_movable gtest_hashmaplist_refcounted
	gtest_hashset gtest_hashset_iterator gtest_hashset_algorithms gtest_hashset_string gtest_hashset_cstring gtest_hashset_movable gtest_hashset_refcounted
	gtest_statichashset gtest_statichashset_iterator gtest_statichashset_algorithms gtest_statichashset_string gtest_statichashset_cstring gtest_statichashset_movable gtest_statichashset_refcounted
	gtest_hashsetlist gtest_hashsetlist_iterator gtest_hashsetlist_algorithms gtest_hashsetlist_string gtest_hashsetlist_cstring gtest_hashsetlist_movable gtest_hashsetlist_refcounted
	gtest_sparseset gtest_sparseset_iterator gtest_sparseset_algorithms
	gtest_vector2 gtest_vector3 gtest_vector4 gtest_rect
	gtest_matrix4x4 gtest_matrix4x4_operations gtest_quaternion gtest_quaternion_operations
	gtest_uniqueptr gtest_uniqueptr_array gtest_sharedptr
	gtest_color gtest_colorf gtest_colorhdr
	gtest_random gtest_filesystem gtest_pointermath
)

if(Threads_FOUND)
	list(APPEND TESTS
		gtest_atomic32 gtest_atomic64
		gtest_sharedptr_threads
	)
endif()

if(NCINE_WITH_ALLOCATORS)
	list(APPEND TESTS
		gtest_allocator_malloc
		gtest_allocator_linear
		gtest_allocator_stack
		gtest_allocator_pool
		gtest_allocator_freelist
		gtest_allocator_containers
	)
endif()

foreach(TEST ${TESTS})
	add_executable(${TEST} ${TEST}.cpp test_functions.h)
	target_link_libraries(${TEST} PRIVATE ncine gtest_main)
	set_target_properties(${TEST} PROPERTIES FOLDER "UnitTests")
	add_test(NAME Tests-${TEST} COMMAND ${TEST})

	target_compile_definitions(${TEST} PRIVATE "$<$<CONFIG:Debug>:NCINE_DEBUG>")

	if(APPLE)
		set_target_properties(${TEST} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
	endif()
endforeach()

include(ncine_strip_binaries)
