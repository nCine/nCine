if(NCINE_WITH_QOI)
	set(QOI_VERSION_TAG "master" CACHE STRING "The Git release, tag, branch or commit for QOI")
	# Download release archive (TRUE) or Git repository (FALSE)
	set(QOI_DOWNLOAD_ARCHIVE TRUE)

	if(QOI_DOWNLOAD_ARCHIVE AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
		# Strip the initial "v" character from the version tag
		string(REGEX MATCH "^v[0-9]" QOI_STRIP_VERSION ${QOI_VERSION_TAG})
		if(QOI_STRIP_VERSION STREQUAL "")
			set(QOI_VERSION_TAG_DIR ${QOI_VERSION_TAG})
		else()
			string(SUBSTRING ${QOI_VERSION_TAG} 1 -1 QOI_VERSION_TAG_DIR)
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/qoi-${QOI_VERSION_TAG_DIR})
			message(STATUS "QOI release file \"${QOI_VERSION_TAG}\" has been already downloaded")
		else()
			file(DOWNLOAD https://github.com/phoboslab/qoi/archive/${QOI_VERSION_TAG}.tar.gz
				${CMAKE_BINARY_DIR}/${QOI_VERSION_TAG}.tar.gz STATUS result)

			list(GET result 0 result_code)
			if(result_code)
				message(WARNING "Cannot download QOI release file \"${QOI_VERSION_TAG}\"")
			else()
				message(STATUS "Downloaded QOI release file \"${QOI_VERSION_TAG}\"")
				file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/${QOI_VERSION_TAG}.tar.gz DESTINATION ${CMAKE_BINARY_DIR})
				file(REMOVE ${CMAKE_BINARY_DIR}/${QOI_VERSION_TAG}.tar.gz)
			endif()
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/qoi-${QOI_VERSION_TAG_DIR})
			set(QOI_SOURCE_DIR ${CMAKE_BINARY_DIR}/qoi-${QOI_VERSION_TAG_DIR})
		else()
			set(NCINE_WITH_QOI FALSE)
		endif()
	else()
		# Download QOI repository at configure time
		configure_file(cmake/download/ncine_qoi_download.in qoi-download/CMakeLists.txt)

		execute_process(
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/qoi-src/include_only)
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qoi-download
		)
		if(result)
			message(STATUS "CMake step for QOI failed: ${result}")
			set(QOI_ERROR TRUE)
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qoi-download
		)
		if(result)
			message(STATUS "Build step for QOI failed: ${result}")
			set(QOI_ERROR TRUE)
		endif()

		if(QOI_ERROR)
			message(FATAL_ERROR "Cannot download QOI")
		else()
			set(QOI_SOURCE_DIR ${CMAKE_BINARY_DIR}/qoi-src)
		endif()
	endif()
endif()
