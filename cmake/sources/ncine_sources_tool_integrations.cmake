if(NCINE_WITH_TRACY)
	target_compile_definitions(ncine PRIVATE "WITH_TRACY")
	target_compile_definitions(ncine PRIVATE "TRACY_DELAYED_INIT")
	if (WIN32)
		target_compile_definitions(ncine PRIVATE "TRACY_ENABLE")
		target_compile_definitions(ncine INTERFACE "TRACY_IMPORTS")
	else()
		target_compile_definitions(ncine PUBLIC "TRACY_ENABLE")
	endif()

	# For external projects compiling using an nCine build directory
	set(TRACY_INCLUDE_ONLY_DIR ${TRACY_SOURCE_DIR}/include_only)
	file(GLOB TRACY_ROOT_HPP "${TRACY_SOURCE_DIR}/public/tracy/*.hpp" "${TRACY_SOURCE_DIR}/public/tracy/*.h")
	file(COPY ${TRACY_ROOT_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/tracy)
	file(GLOB TRACY_COMMON_HPP "${TRACY_SOURCE_DIR}/public/common/*.hpp" "${TRACY_SOURCE_DIR}/public/common/*.h")
	file(COPY ${TRACY_COMMON_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/common)
	file(COPY "${TRACY_SOURCE_DIR}/public/common/TracySystem.cpp" DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/common)
	file(GLOB TRACY_CLIENT_HPP "${TRACY_SOURCE_DIR}/public/client/*.hpp" "${TRACY_SOURCE_DIR}/public/client/*.h")
	file(COPY ${TRACY_CLIENT_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/client)
	file(GLOB TRACY_LIBBACKTRACE_HPP "${TRACY_SOURCE_DIR}/public/libbacktrace/*.hpp" "${TRACY_SOURCE_DIR}/public/libbacktrace/*.h")
	file(COPY ${TRACY_LIBBACKTRACE_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/client)
	file(COPY "${TRACY_SOURCE_DIR}/LICENSE" DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy)

	# `tracy.h` and `tracy_opengl.h` have been already added to the headers
	list(APPEND SOURCES
		${NCINE_ROOT}/src/tracy_memory.cpp
		${TRACY_SOURCE_DIR}/public/TracyClient.cpp
	)
endif()

if(NCINE_WITH_RENDERDOC AND NOT APPLE)
	find_file(RENDERDOC_API_H
		NAMES renderdoc.h renderdoc_app.h
		PATHS "$ENV{ProgramW6432}/RenderDoc"
			"$ENV{ProgramFiles}/RenderDoc"
			"$ENV{ProgramFiles\(x86\)}/RenderDoc"
			${RENDERDOC_DIR}
		PATH_SUFFIXES "include"
		DOC "Path to the RenderDoc header file")

	if(NOT EXISTS ${RENDERDOC_API_H})
		message(FATAL_ERROR "RenderDoc header file not found")
	endif()

	get_filename_component(RENDERDOC_INCLUDE_DIR ${RENDERDOC_API_H} DIRECTORY)
	target_include_directories(ncine PRIVATE ${RENDERDOC_INCLUDE_DIR})

	target_compile_definitions(ncine PRIVATE "WITH_RENDERDOC")
	if(UNIX)
		target_link_libraries(ncine PRIVATE dl)
	endif()

	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/RenderDocCapture.h)
	list(APPEND PRIVATE_HEADERS ${RENDERDOC_API_H})
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/RenderDocCapture.cpp)
endif()

if(Crashpad_FOUND AND NOT EMSCRIPTEN)
	target_compile_definitions(ncine PRIVATE "WITH_CRASHPAD")
	target_link_libraries(ncine PRIVATE Crashpad::Crashpad)

	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/Crashpad.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/Crashpad.cpp)
endif()
