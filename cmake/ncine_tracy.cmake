if(NCINE_WITH_TRACY)
	# Download and unpack Tracy at configure time
	configure_file(cmake/ncine_tracy_download.in tracy-download/CMakeLists.txt)

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tracy-src/include_only)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tracy-download
	)
	if(result)
		message(STATUS "CMake step for Tracy failed: ${result}")
		set(TRACY_ERROR TRUE)
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tracy-download
	)
	if(result)
		message(STATUS "Build step for Tracy failed: ${result}")
		set(TRACY_ERROR TRUE)
	endif()

	if(TRACY_ERROR)
		set(NCINE_WITH_TRACY FALSE)
	else()
		set(TRACY_SOURCE_DIR ${CMAKE_BINARY_DIR}/tracy-src)
	endif()
endif()
