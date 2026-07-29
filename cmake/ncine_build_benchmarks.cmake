if(NCINE_BUILD_BENCHMARKS)
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
