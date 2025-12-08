cmake_minimum_required(VERSION 3.10)
project(nCine-srctests)

if(MSVC)
	add_custom_target(copy_dlls_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${MSVC_BINDIR} ${CMAKE_BINARY_DIR}/src/tests
		COMMENT "Copying DLLs to src/tests..."
	)
	set_target_properties(copy_dlls_srctests PROPERTIES FOLDER "CustomCopyTargets")

	if(ANGLE_FOUND)
		get_target_property(EGL_IMPORTED_LOCATION EGL::EGL IMPORTED_LOCATION)
		get_target_property(GLES_IMPORTED_LOCATION OpenGLES::GLES IMPORTED_LOCATION)
		add_custom_target(copy_angle_dll_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EGL_IMPORTED_LOCATION} ${CMAKE_BINARY_DIR}/src/tests
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLES_IMPORTED_LOCATION} ${CMAKE_BINARY_DIR}/src/tests
			COMMENT "Copying ANGLE DLLs to src/tests..."
		)
		set_target_properties(copy_angle_dll_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
endif()

if(NCINE_WITH_CRASHPAD AND EXISTS ${CRASHPAD_HANDLER_EXE})
	add_custom_target(copy_crashpad_handler_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CRASHPAD_HANDLER_EXE} ${CMAKE_BINARY_DIR}/src/tests
		COMMENT "Copying Crashpad handler to sc/tests..."
	)
	set_target_properties(copy_crashpad_handler_srctests PROPERTIES FOLDER "CustomCopyTargets")
endif()

list(APPEND SRCAPPTESTS glapptest_fbo_cube)

foreach(SRCAPPTEST ${SRCAPPTESTS})
	add_executable(${SRCAPPTEST} WIN32 ${SRCAPPTEST}.cpp ${RESOURCE_RC_FILE})

	if(NCINE_EMBED_SHADERS)
		target_compile_definitions(${SRCAPPTEST} PRIVATE "WITH_EMBEDDED_SHADERS")
		target_include_directories(${SRCAPPTEST} PRIVATE ${GENERATED_INCLUDE_DIR})
	endif()
	if(WIN32)
		target_compile_definitions(${SRCAPPTEST} PRIVATE "WITH_GLEW")
		if(MSVC)
			target_include_directories(${SRCAPPTEST} PRIVATE "${EXTERNAL_MSVC_DIR}/include")
		endif()
		if(Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
			target_link_libraries(${SRCAPPTEST} PRIVATE Qt5::WinMain)
		elseif(MINGW)
			# Should be linked before `ncine_main`
			target_link_libraries(${SRCAPPTEST} PRIVATE mingw32)
		endif()

		# Embedding HiDPI awareness manifest or resource file
		if(NOT WITH_GLFW)
			if(MSVC)
				target_sources(${SRCAPPTEST} PRIVATE "${CMAKE_SOURCE_DIR}/WindowsHiDPI.manifest")
			elseif(MINGW)
				target_sources(${SRCAPPTEST} PRIVATE ${HIDPI_RC_FILE})
			endif()
		endif()
	endif()

	target_include_directories(${SRCAPPTEST} PRIVATE ${CMAKE_SOURCE_DIR}/include/ncine ${CMAKE_SOURCE_DIR}/src/include)
	target_link_libraries(${SRCAPPTEST} PRIVATE ncine_main ncine)
	set_target_properties(${SRCAPPTEST} PROPERTIES FOLDER "SrcTests")
	install(TARGETS ${SRCAPPTEST} RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)

	if(EMSCRIPTEN)
		set(EMSCRIPTEN_PAGE_TITLE ${SRCAPPTEST})
		set(EMSCRIPTEN_SCRIPTS "\n    <script type='text/javascript' src='${SRCAPPTEST}.js'></script>")

		configure_file(${CMAKE_SOURCE_DIR}/emscripten_shell.html.in ${CMAKE_BINARY_DIR}/src/tests/${SRCAPPTEST}.html @ONLY)
		install(FILES ${CMAKE_BINARY_DIR}/src/tests/${SRCAPPTEST}.html DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
		install(FILES ${CMAKE_BINARY_DIR}/src/tests/${SRCAPPTEST}.wasm DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
		unset(EMSCRIPTEN_SCRIPTS)
		unset(EMSCRIPTEN_PAGE_TITLE)
	endif()
endforeach()

if(EMSCRIPTEN)
	if(EXISTS ${NCINE_ICONS_DIR}/icon.ico)
		file(COPY ${NCINE_ICONS_DIR}/icon.ico DESTINATION ${CMAKE_BINARY_DIR})
		file(RENAME ${CMAKE_BINARY_DIR}/icon.ico ${CMAKE_BINARY_DIR}/favicon.ico)
		install(FILES ${CMAKE_BINARY_DIR}/favicon.ico DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	endif()
endif()

list(FIND SRCAPPTESTS ${NCINE_STARTUP_TEST} STARTUP_TEST_INDEX)
if(STARTUP_TEST_INDEX GREATER -1)
	if(NOT TESTS_DATA_DIR_DIST) # Don't set the startup test if it wouldn't find the data directory
		set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NCINE_STARTUP_TEST})
	endif()
	message(STATUS "Startup test target found in `src/tests/`: \"${NCINE_STARTUP_TEST}\"")
endif()

include(ncine_extract_symbols)
include(ncine_strip_binaries)
