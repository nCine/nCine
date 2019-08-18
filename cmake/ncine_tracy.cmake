if(NCINE_WITH_TRACY)
	# Download and unpack Tracy at configure time
	configure_file(cmake/ncine_tracy_download.in tracy-download/CMakeLists.txt)

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

		# Patch `TracyProfiler.cpp` on v0.5 to export `GetTimeImpl()` on Android
		file(READ ${TRACY_SOURCE_DIR}/client/TracyProfiler.cpp TRACY_PROFILER_CPP)
		string(REPLACE ";" "\\\\;" TRACY_PROFILER_CPP "${TRACY_PROFILER_CPP}")
		string(REPLACE "\n" ";" TRACY_PROFILER_CPP "${TRACY_PROFILER_CPP}")
		foreach(FILE_LINE ${TRACY_PROFILER_CPP})
			if("${FILE_LINE}" STREQUAL "int64_t (*GetTimeImpl)()\;")
				list(APPEND NEW_FILE_CONTENT "TRACY_API ${FILE_LINE}\n")
			else()
				list(APPEND NEW_FILE_CONTENT "${FILE_LINE}\n")
			endif()
		endforeach()
		file(WRITE ${TRACY_SOURCE_DIR}/client/TracyProfiler.cpp ${NEW_FILE_CONTENT})
	endif()
endif()
