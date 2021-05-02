if(NCINE_BUILD_UNIT_TESTS)
	set(GTEST_VERSION_TAG "release-1.10.0" CACHE STRING "The Git release, tag, branch or commit for Googletest")
	# Download release archive (TRUE) or Git repository (FALSE)
	set(GTEST_DOWNLOAD_ARCHIVE TRUE)

	if(GTEST_DOWNLOAD_ARCHIVE)
		# Strip the initial "v" character from the version tag
		string(REGEX MATCH "^v[0-9]" GTEST_STRIP_VERSION ${GTEST_VERSION_TAG})
		if(GTEST_STRIP_VERSION STREQUAL "")
			set(GTEST_VERSION_TAG_DIR ${GTEST_VERSION_TAG})
		else()
			string(SUBSTRING ${GTEST_VERSION_TAG} 1 -1 GTEST_VERSION_TAG_DIR)
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-${GTEST_VERSION_TAG_DIR})
			message(STATUS "Googletest release file \"${GTEST_VERSION_TAG}\" has been already downloaded")
		else()
			file(DOWNLOAD https://github.com/google/googletest/archive/${GTEST_VERSION_TAG}.tar.gz
				${CMAKE_BINARY_DIR}/${GTEST_VERSION_TAG}.tar.gz STATUS result)

			list(GET result 0 result_code)
			if(result_code)
				message(WARNING "Cannot download Googletest release file \"${GTEST_VERSION_TAG}\"")
			else()
				message(STATUS "Downloaded Googletest release file \"${GTEST_VERSION_TAG}\"")
				file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/${GTEST_VERSION_TAG}.tar.gz DESTINATION ${CMAKE_BINARY_DIR})
				file(REMOVE ${CMAKE_BINARY_DIR}/${GTEST_VERSION_TAG}.tar.gz)
			endif()
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-${GTEST_VERSION_TAG_DIR})
			set(GTEST_SOURCE_DIR ${CMAKE_BINARY_DIR}/googletest-${GTEST_VERSION_TAG_DIR})
		else()
			set(GTEST_ERROR TRUE)
		endif()
	else()
		# Download Googletest repository at configure time
		configure_file(cmake/ncine_gtest_download.in googletest-download/CMakeLists.txt)

		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
		)
		if(result)
			message(STATUS "CMake step for Googletest failed: ${result}")
			set(GTEST_ERROR TRUE)
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
		)
		if(result)
			message(STATUS "Build step for Googletest failed: ${result}")
			set(GTEST_ERROR TRUE)
		endif()

		if(NOT GTEST_ERROR)
			set(GTEST_SOURCE_DIR ${CMAKE_BINARY_DIR}/googletest-src)
		endif()
	endif()

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
			include_directories("${gtest_SOURCE_DIR}/include")
		endif()

		enable_testing()
		add_subdirectory(unit_tests)
	endif()
endif()
