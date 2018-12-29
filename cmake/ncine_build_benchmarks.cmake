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

	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . -B${CMAKE_BINARY_DIR}/googlebenchmark-build -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_TESTING=OFF -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googlebenchmark-src
	)
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googlebenchmark-build
	)
	if(result)
		message(STATUS "Second build step for googlebenchmark failed: ${result}")
		set(GBENCHMARK_ERROR TRUE)
	endif()

	if(NOT GBENCHMARK_ERROR)
		include_directories(${CMAKE_BINARY_DIR}/googlebenchmark-src/include)
		add_library(benchmark STATIC IMPORTED GLOBAL)

		if(MSVC)
			set(BENCHMARK_LOCATION "$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>/benchmark.lib")
		else()
			set(BENCHMARK_LOCATION "libbenchmark.a")
		endif()
		set_target_properties(benchmark PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/googlebenchmark-build/src/${BENCHMARK_LOCATION}")

		add_subdirectory(benchmarks)
	endif()
endif()
