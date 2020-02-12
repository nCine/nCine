if(NCINE_WITH_IMGUI)
	# Download and unpack ImGui at configure time
	configure_file(cmake/ncine_imgui_download.in imgui-download/CMakeLists.txt)

	execute_process(COMMAND ${CMAKE_COMMAND} -E remove -f
		${CMAKE_BINARY_DIR}/imgui-src/imconfig.h)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/imgui-download
	)
	if(result)
		message(STATUS "CMake step for ImGui failed: ${result}")
		set(IMGUI_ERROR TRUE)
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/imgui-download
	)
	if(result)
		message(STATUS "Build step for ImGui failed: ${result}")
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
