if(NCINE_BUILD_BENCHMARKS)
	set(GBENCHMARK_VERSION_TAG "v1.8.4" CACHE STRING "The Git release, tag, branch or commit for Googlebenchmark")
	# Download release archive (TRUE) or Git repository (FALSE)
	set(GBENCHMARK_DOWNLOAD_ARCHIVE TRUE)

	if(GBENCHMARK_DOWNLOAD_ARCHIVE AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
		# Strip the initial "v" character from the version tag
		string(REGEX MATCH "^v[0-9]" GBENCHMARK_STRIP_VERSION ${GBENCHMARK_VERSION_TAG})
		if(GBENCHMARK_STRIP_VERSION STREQUAL "")
			set(GBENCHMARK_VERSION_TAG_DIR ${GBENCHMARK_VERSION_TAG})
		else()
			string(SUBSTRING ${GBENCHMARK_VERSION_TAG} 1 -1 GBENCHMARK_VERSION_TAG_DIR)
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark-${GBENCHMARK_VERSION_TAG_DIR})
			message(STATUS "Googlebenchmark release file \"${GBENCHMARK_VERSION_TAG}\" has been already downloaded")
		else()
			file(DOWNLOAD https://github.com/google/benchmark/archive/${GBENCHMARK_VERSION_TAG}.tar.gz
				${CMAKE_BINARY_DIR}/${GBENCHMARK_VERSION_TAG}.tar.gz STATUS result)

			list(GET result 0 result_code)
			if(result_code)
				message(WARNING "Cannot download Googlebenchmark release file \"${GBENCHMARK_VERSION_TAG}\"")
			else()
				message(STATUS "Downloaded Googlebenchmark release file \"${GBENCHMARK_VERSION_TAG}\"")
				file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/${GBENCHMARK_VERSION_TAG}.tar.gz DESTINATION ${CMAKE_BINARY_DIR})
				file(REMOVE ${CMAKE_BINARY_DIR}/${GBENCHMARK_VERSION_TAG}.tar.gz)
			endif()
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark-${GBENCHMARK_VERSION_TAG_DIR})
			set(GBENCHMARK_SOURCE_DIR ${CMAKE_BINARY_DIR}/benchmark-${GBENCHMARK_VERSION_TAG_DIR})
		else()
			set(GBENCHMARK_ERROR TRUE)
		endif()
	else()
		# Download Googlebenchmark repository at configure time
		configure_file(cmake/ncine_gbenchmark_download.in googlebenchmark-download/CMakeLists.txt)

		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googlebenchmark-download
		)
		if(result)
			message(STATUS "CMake step for Googlebenchmark failed: ${result}")
			set(GBENCHMARK_ERROR TRUE)
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googlebenchmark-download
		)
		if(result)
			message(STATUS "Build step for Googlebenchmark failed: ${result}")
			set(GBENCHMARK_ERROR TRUE)
		endif()

		if(NOT GBENCHMARK_ERROR)
			set(GBENCHMARK_SOURCE_DIR ${CMAKE_BINARY_DIR}/googlebenchmark-src)
		endif()
	endif()

	if(NOT GBENCHMARK_ERROR)
		option(BENCHMARK_ENABLE_TESTING "" OFF)
		option(BENCHMARK_ENABLE_GTEST_TESTS "" OFF)
		add_subdirectory(${GBENCHMARK_SOURCE_DIR}
			${CMAKE_BINARY_DIR}/googlebenchmark-build
			EXCLUDE_FROM_ALL
		)
		if(MSVC)
			# Always use the non debug version of the runtime library
			target_compile_options(benchmark PUBLIC /MD)
		endif()

		add_subdirectory(benchmarks)
	endif()
endif()
