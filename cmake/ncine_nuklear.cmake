if(NCINE_WITH_NUKLEAR)
	# Download and unpack nuklear at configure time
	configure_file(cmake/ncine_nuklear_download.in nuklear-download/CMakeLists.txt)

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/nuklear-src/include_only)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/nuklear-download
	)
	if(result)
		message(STATUS "CMake step for nuklear failed: ${result}")
		set(NUKLEAR_ERROR TRUE)
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/nuklear-download
	)
	if(result)
		message(STATUS "Build step for nuklear failed: ${result}")
		set(NUKLEAR_ERROR TRUE)
	endif()

	if(NUKLEAR_ERROR)
		message(FATAL_ERROR "Cannot download nuklear")
	else()
		set(NUKLEAR_SOURCE_DIR ${CMAKE_BINARY_DIR}/nuklear-src)
	endif()
endif()
