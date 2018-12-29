cmake_minimum_required(VERSION 2.8.12)
project(nCine-benchmarks)

if(WIN32)
	if(MSVC)
		add_custom_target(copy_dlls_benchmarks ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/benchmarks
			COMMENT "Copying DLLs to tests..."
		)
		set_target_properties(copy_dlls_benchmarks PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		if(MSVC)
			set(NCINE_LIBRARY_NAME ncine.dll)
		else()
			set(NCINE_LIBRARY_NAME libncine.dll)
		endif()

		if(MSVC_IDE)
			add_custom_target(copy_ncine_dll_benchmarks ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/benchmarks
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		else()
			add_custom_target(copy_ncine_dll_benchmarks ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${NCINE_LIBRARY_NAME} ${CMAKE_BINARY_DIR}/benchmarks
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		endif()
		set_target_properties(copy_ncine_dll_benchmarks PROPERTIES FOLDER "CustomCopyTargets")
	endif()
elseif(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
endif()

list(APPEND BENCHMARKS
	gbench_std_vector gbench_array
	gbench_std_array gbench_staticarray
	gbench_std_list gbench_list
	gbench_std_string gbench_string
	gbench_std_unorderedmap gbench_hashmap gbench_statichashmap gbench_hashmaplist
)

foreach(BENCHMARK ${BENCHMARKS})
	add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
	target_link_libraries(${BENCHMARK} ncine benchmark ${CMAKE_THREAD_LIBS_INIT})
	set_target_properties(${BENCHMARK} PROPERTIES FOLDER "Benchmarks")

	if(APPLE)
		set_target_properties(${BENCHMARK} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
	elseif(MINGW OR MSYS)
		target_link_libraries(${BENCHMARK} shlwapi)
	endif()
endforeach()
