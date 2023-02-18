if(NCINE_WITH_IMGUI)
	set(IMGUI_VERSION_TAG "v1.89.3" CACHE STRING "The Git release, tag, branch or commit for ImGui")
	# Download release archive (TRUE) or Git repository (FALSE)
	set(IMGUI_DOWNLOAD_ARCHIVE TRUE)

	if(IMGUI_DOWNLOAD_ARCHIVE AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
		# Strip the initial "v" character from the version tag
		string(REGEX MATCH "^v[0-9]" IMGUI_STRIP_VERSION ${IMGUI_VERSION_TAG})
		if(IMGUI_STRIP_VERSION STREQUAL "")
			set(IMGUI_VERSION_TAG_DIR ${IMGUI_VERSION_TAG})
		else()
			string(SUBSTRING ${IMGUI_VERSION_TAG} 1 -1 IMGUI_VERSION_TAG_DIR)
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION_TAG_DIR})
			message(STATUS "Dear ImGui release file \"${IMGUI_VERSION_TAG}\" has been already downloaded")
		else()
			file(DOWNLOAD https://github.com/ocornut/imgui/archive/${IMGUI_VERSION_TAG}.tar.gz
				${CMAKE_BINARY_DIR}/${IMGUI_VERSION_TAG}.tar.gz STATUS result)

			list(GET result 0 result_code)
			if(result_code)
				message(WARNING "Cannot download Dear ImGui release file ${IMGUI_VERSION_TAG}")
			else()
				message(STATUS "Downloaded Dear ImGui release file \"${IMGUI_VERSION_TAG}\"")
				file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/${IMGUI_VERSION_TAG}.tar.gz DESTINATION ${CMAKE_BINARY_DIR})
				file(REMOVE ${CMAKE_BINARY_DIR}/${IMGUI_VERSION_TAG}.tar.gz)
			endif()
		endif()

		if(IS_DIRECTORY ${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION_TAG_DIR})
			file(COPY ${CMAKE_SOURCE_DIR}/imconfig.h DESTINATION ${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION_TAG_DIR})
			set(IMGUI_SOURCE_DIR ${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION_TAG_DIR})
		else()
			set(NCINE_WITH_IMGUI FALSE)
		endif()
	else()
		# Download Dear ImGui repository at configure time
		configure_file(cmake/ncine_imgui_download.in imgui-download/CMakeLists.txt)

		execute_process(
			COMMAND	${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/imgui-src/imconfig.h
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/imgui-src/include_only)
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/imgui-download
		)
		if(result)
			message(STATUS "CMake step for Dear ImGui failed: ${result}")
			set(IMGUI_ERROR TRUE)
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/imgui-download
		)
		if(result)
			message(STATUS "Build step for Dear ImGui failed: ${result}")
			set(IMGUI_ERROR TRUE)
		endif()

		if(IMGUI_ERROR)
			set(NCINE_WITH_IMGUI FALSE)
		else()
			execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_SOURCE_DIR}/imconfig.h ${CMAKE_BINARY_DIR}/imgui-src)
			set(IMGUI_SOURCE_DIR ${CMAKE_BINARY_DIR}/imgui-src)
		endif()
	endif()
endif()
