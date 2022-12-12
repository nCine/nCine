cmake_minimum_required(VERSION 3.1)
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

list(APPEND SRCAPPTESTS glapptest_fbo_cube)
if(Threads_FOUND)
	list(APPEND SRCAPPTESTS apptest_threads apptest_threadpool)
endif()

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

include(ncine_strip_binaries)
