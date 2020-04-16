cmake_minimum_required(VERSION 3.1)
project(nCine-benchmarks)

if(WIN32)
	if(MSVC)
		add_custom_target(copy_dlls_benchmarks ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${MSVC_BINDIR} ${CMAKE_BINARY_DIR}/benchmarks
			COMMENT "Copying DLLs to tests..."
		)
		set_target_properties(copy_dlls_benchmarks PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		add_custom_target(copy_ncine_dll_benchmarks ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ncine> ${CMAKE_BINARY_DIR}/benchmarks
			DEPENDS ncine
			COMMENT "Copying nCine DLL..."
		)
		set_target_properties(copy_ncine_dll_benchmarks PROPERTIES FOLDER "CustomCopyTargets")
	endif()
elseif(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
endif()

if(Threads_FOUND)
	list(APPEND BENCHMARKS
		gbench_copy_int gbench_copy_trivial gbench_copy_movable
		gbench_std_vector gbench_array
		gbench_std_bigvector gbench_bigarray
		gbench_std_array gbench_staticarray
		gbench_std_list gbench_list
		gbench_std_biglist gbench_biglist
		gbench_std_string gbench_string
		gbench_std_unorderedmap gbench_hashmap
		gbench_std_bigunorderedmap gbench_bighashmap
		gbench_std_unorderedset gbench_hashset
		gbench_statichashmap gbench_hashmaplist
		gbench_statichashset gbench_hashsetlist
		gbench_bighashmaplist
		gbench_sparseset
		gbench_std_rand gbench_random
		gbench_matrix4x4f)

	if(NCINE_WITH_ALLOCATORS)
		list(APPEND BENCHMARKS
			gbench_fixed_allocations gbench_random_allocations
			gbench_array_allocators)
	endif()
endif()

foreach(BENCHMARK ${BENCHMARKS})
	add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
	target_link_libraries(${BENCHMARK} PRIVATE ncine benchmark Threads::Threads)
	set_target_properties(${BENCHMARK} PROPERTIES FOLDER "Benchmarks")

	if(APPLE)
		set_target_properties(${BENCHMARK} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
	elseif(MINGW OR MSYS)
		target_link_libraries(${BENCHMARK} PRIVATE shlwapi)
	endif()
endforeach()

include(ncine_strip_binaries)
