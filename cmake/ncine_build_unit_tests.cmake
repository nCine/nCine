if(NCINE_BUILD_UNIT_TESTS)
	if(NOT GTEST_ERROR)
		# Prevent overriding the parent project's compiler/linker
		# settings on Windows
		set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

		# Add googletest directly to our build. This defines
		# the gtest and gtest_main targets.
		add_subdirectory(${GTEST_SOURCE_DIR}
			${CMAKE_BINARY_DIR}/googletest-build
			EXCLUDE_FROM_ALL
		)
		if(MSVC)
			# Always use the non debug version of the runtime library
			target_compile_options(gtest PUBLIC /MD)
		elseif(EMSCRIPTEN)
			target_compile_options(gtest PRIVATE -fno-exceptions)
		endif()

		# The gtest/gtest_main targets carry header search path
		# dependencies automatically when using CMake 2.8.11 or
		# later. Otherwise we have to add them here ourselves.
		if (CMAKE_VERSION VERSION_LESS 2.8.11)
			include_directories("${GTEST_SOURCE_DIR}/include")
		endif()

		enable_testing()
		add_subdirectory(unit_tests)
	endif()
endif()
