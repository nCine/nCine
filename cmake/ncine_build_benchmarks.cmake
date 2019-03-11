if(NCINE_BUILD_BENCHMARKS)
	# Download and unpack googlebenchmark at configure time
	configure_file(cmake/ncine_gbenchmark_download.in googlebenchmark-download/CMakeLists.txt)

	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googlebenchmark-download
	)
	if(result)
		message(STATUS "CMake step for googlebenchmark failed: ${result}")
		set(GBENCHMARK_ERROR TRUE)
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googlebenchmark-download
	)
	if(result)
		message(STATUS "Build step for googlebenchmark failed: ${result}")
		set(GBENCHMARK_ERROR TRUE)
	endif()

	if(NOT GBENCHMARK_ERROR)
		set(BENCHMARK_ENABLE_TESTING OFF)
		set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
		add_subdirectory(${CMAKE_BINARY_DIR}/googlebenchmark-src
			${CMAKE_BINARY_DIR}/googlebenchmark-build
			EXCLUDE_FROM_ALL
		)
		add_subdirectory(benchmarks)
	endif()
endif()
