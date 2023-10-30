if(NCINE_WITH_TRACY)
	set(TRACY_VERSION_TAG "v0.10" CACHE STRING "The Git release, tag, branch or commit for Tracy")
	# Download release archive (TRUE) or Git repository (FALSE)
	set(TRACY_DOWNLOAD_ARCHIVE TRUE)

	if(TRACY_DOWNLOAD_ARCHIVE AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
		# Strip the initial "v" character from the version tag
		string(REGEX MATCH "^v[0-9]" TRACY_STRIP_VERSION ${TRACY_VERSION_TAG})
		if(TRACY_STRIP_VERSION STREQUAL "")
			set(TRACY_VERSION_TAG_DIR ${TRACY_VERSION_TAG})
		else()
			string(SUBSTRING ${TRACY_VERSION_TAG} 1 -1 TRACY_VERSION_TAG_DIR)
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/tracy-${TRACY_VERSION_TAG_DIR})
			message(STATUS "Tracy release file \"${TRACY_VERSION_TAG}\" has been already downloaded")
		else()
			file(DOWNLOAD https://github.com/wolfpld/tracy/archive/${TRACY_VERSION_TAG}.tar.gz
				${CMAKE_BINARY_DIR}/${TRACY_VERSION_TAG}.tar.gz STATUS result)

			list(GET result 0 result_code)
			if(result_code)
				message(WARNING "Cannot download Tracy release file \"${TRACY_VERSION_TAG}\"")
			else()
				message(STATUS "Downloaded Tracy release file \"${TRACY_VERSION_TAG}\"")
				file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/${TRACY_VERSION_TAG}.tar.gz DESTINATION ${CMAKE_BINARY_DIR})
				file(REMOVE ${CMAKE_BINARY_DIR}/${TRACY_VERSION_TAG}.tar.gz)
			endif()
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/tracy-${TRACY_VERSION_TAG_DIR})
			set(TRACY_SOURCE_DIR ${CMAKE_BINARY_DIR}/tracy-${TRACY_VERSION_TAG_DIR})
		else()
			set(NCINE_WITH_TRACY FALSE)
		endif()
	else()
		# Download Tracy repository at configure time
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
endif()
