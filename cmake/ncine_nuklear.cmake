if(NCINE_WITH_NUKLEAR)
	set(NUKLEAR_VERSION_TAG "master" CACHE STRING "The Git release, tag, branch or commit for Nuklear")
	# Download release archive (TRUE) or Git repository (FALSE)
	set(NUKLEAR_DOWNLOAD_ARCHIVE TRUE)

	if(NUKLEAR_DOWNLOAD_ARCHIVE)
		# Strip the initial "v" character from the version tag
		string(REGEX MATCH "^v[0-9]" NUKLEAR_STRIP_VERSION ${NUKLEAR_VERSION_TAG})
		if(NUKLEAR_STRIP_VERSION STREQUAL "")
			set(NUKLEAR_VERSION_TAG_DIR ${NUKLEAR_VERSION_TAG})
		else()
			string(SUBSTRING ${NUKLEAR_VERSION_TAG} 1 -1 NUKLEAR_VERSION_TAG_DIR)
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/Nuklear-${NUKLEAR_VERSION_TAG_DIR})
			message(STATUS "Nuklear release file \"${NUKLEAR_VERSION_TAG}\" has been already downloaded")
		else()
			file(DOWNLOAD https://github.com/Immediate-Mode-UI/Nuklear/archive/${NUKLEAR_VERSION_TAG}.tar.gz
				${CMAKE_BINARY_DIR}/${NUKLEAR_VERSION_TAG}.tar.gz STATUS result)

			list(GET result 0 result_code)
			if(result_code)
				message(WARNING "Cannot download Nuklear release file \"${NUKLEAR_VERSION_TAG}\"")
			else()
				message(STATUS "Downloaded Nuklear release file \"${NUKLEAR_VERSION_TAG}\"")
				file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/${NUKLEAR_VERSION_TAG}.tar.gz DESTINATION ${CMAKE_BINARY_DIR})
				file(REMOVE ${CMAKE_BINARY_DIR}/${NUKLEAR_VERSION_TAG}.tar.gz)
			endif()
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/Nuklear-${NUKLEAR_VERSION_TAG_DIR})
			set(NUKLEAR_SOURCE_DIR ${CMAKE_BINARY_DIR}/Nuklear-${NUKLEAR_VERSION_TAG_DIR})
		else()
			set(NCINE_WITH_NUKLEAR FALSE)
		endif()
	else()
		# Download Nuklear repository at configure time
		configure_file(cmake/ncine_nuklear_download.in nuklear-download/CMakeLists.txt)

		execute_process(
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/nuklear-src/include_only)
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/nuklear-download
		)
		if(result)
			message(STATUS "CMake step for Nuklear failed: ${result}")
			set(NUKLEAR_ERROR TRUE)
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/nuklear-download
		)
		if(result)
			message(STATUS "Build step for Nuklear failed: ${result}")
			set(NUKLEAR_ERROR TRUE)
		endif()

		if(NUKLEAR_ERROR)
			message(FATAL_ERROR "Cannot download Nuklear")
		else()
			set(NUKLEAR_SOURCE_DIR ${CMAKE_BINARY_DIR}/nuklear-src)
		endif()
	endif()
endif()
