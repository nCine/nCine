cmake_minimum_required(VERSION 3.10)
project(nCine-tests)

if(WIN32)
	if(MSVC)
		add_custom_target(copy_dlls_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${MSVC_BINDIR} ${CMAKE_BINARY_DIR}/tests
			COMMENT "Copying DLLs to tests..."
		)
		set_target_properties(copy_dlls_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		add_custom_target(copy_ncine_dll_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ncine> ${CMAKE_BINARY_DIR}/tests
			DEPENDS ncine
			COMMENT "Copying nCine DLL to tests..."
		)
		set_target_properties(copy_ncine_dll_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(ANGLE_FOUND)
		get_target_property(EGL_IMPORTED_LOCATION EGL::EGL IMPORTED_LOCATION)
		get_target_property(GLES2_IMPORTED_LOCATION OpenGLES2::GLES2 IMPORTED_LOCATION)
		add_custom_target(copy_angle_dll_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EGL_IMPORTED_LOCATION} ${CMAKE_BINARY_DIR}/tests
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLES2_IMPORTED_LOCATION} ${CMAKE_BINARY_DIR}/tests
			COMMENT "Copying ANGLE DLLs to tests..."
		)
		set_target_properties(copy_angle_dll_tests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
elseif(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
endif()

if(NCINE_WITH_CRASHPAD AND EXISTS ${CRASHPAD_HANDLER_EXE})
	add_custom_target(copy_crashpad_handler_tests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CRASHPAD_HANDLER_EXE} ${CMAKE_BINARY_DIR}/tests
		COMMENT "Copying Crashpad handler to tests..."
	)
	set_target_properties(copy_crashpad_handler_tests PROPERTIES FOLDER "CustomCopyTargets")

	# On Linux check if the build is a preset, thus probably a distro package,
	# so to avoid installing the crashpad handler in a system wide bin directory.
	if(NOT LINUX OR (LINUX AND NOT DEFINED NCINE_OPTIONS_PRESETS))
		install(FILES ${CRASHPAD_HANDLER_EXE} DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
	endif()
endif()

if("${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")
	list(APPEND APPTESTS ${NCINE_STARTUP_TEST})
	list(APPEND ${NCINE_STARTUP_TEST}_SOURCES apptest_lua.h apptest_lua.cpp)
else()
	if(PNG_FOUND)
		list(APPEND APPTESTS apptest_texformats apptest_joystick apptest_rotozoom apptest_animsprites
			apptest_particles apptest_gui apptest_font apptest_multitouch apptest_camera apptest_meshsprites
			apptest_meshdeform apptest_sinescroller apptest_clones apptest_shaders apptest_bunnymark)
		if(OPENAL_FOUND)
			list(APPEND APPTESTS apptest_audio)
		endif()
		if(LUA_FOUND AND NCINE_WITH_SCRIPTING_API)
			list(APPEND APPTESTS apptest_lua apptest_luareload)
		endif()
		if(NCINE_WITH_IMGUI)
			list(APPEND APPTESTS apptest_anchor apptest_filebrowser apptest_loading apptest_viewports apptest_scaling)
			if(NCINE_WITH_ALLOCATORS)
				list(APPEND APPTESTS apptest_allocators)
			endif()
			if(LUA_FOUND)
				list(APPEND APPTESTS apptest_simdbench)
			endif()
		endif()
	endif()
	if(NCINE_WITH_CRASHPAD AND EXISTS ${CRASHPAD_HANDLER_EXE})
		list(APPEND APPTESTS apptest_crash)
	endif()
endif()

foreach(APPTEST ${APPTESTS})
	add_executable(${APPTEST} WIN32 apptest_datapath.h ${RESOURCE_RC_FILE})

	if(DEFINED ${APPTEST}_SOURCES)
		# More complex AppTests can define multiple sources (does not work on Android)
		list(LENGTH ${APPTEST}_SOURCES NUM_SOURCES)
		if(NUM_SOURCES GREATER 0)
			target_sources(${APPTEST} PRIVATE ${${APPTEST}_SOURCES})
		endif()
	else()
		target_sources(${APPTEST} PRIVATE ${APPTEST}.cpp ${APPTEST}.h)
	endif()

	if(WIN32 AND Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
		target_link_libraries(${APPTEST} PRIVATE Qt5::WinMain)
	elseif(MINGW)
		# Should be linked before `ncine_main`
		target_link_libraries(${APPTEST} PRIVATE mingw32)
	endif()

	# Embedding HiDPI awareness manifest or resource file
	if(WIN32 AND NOT WITH_GLFW)
		if(MSVC)
			target_sources(${APPTEST} PRIVATE "${CMAKE_SOURCE_DIR}/WindowsHiDPI.manifest")
		elseif(MINGW)
			target_sources(${APPTEST} PRIVATE ${HIDPI_RC_FILE})
		endif()
	endif()

	target_link_libraries(${APPTEST} PRIVATE ncine_main ncine)
	set_target_properties(${APPTEST} PROPERTIES FOLDER "AppTests")
	target_compile_definitions(${APPTEST} PRIVATE "NCINE_TESTS_DATA_DIR=\"${NCINE_TESTS_DATA_DIR}\"")
	if(APPLE)
		set_target_properties(${APPTEST} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
	endif()

	if("${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")
		install(TARGETS ${APPTEST} RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT executable)
	else()
		install(TARGETS ${APPTEST} RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
	endif()

	if(EMSCRIPTEN)
		set(EMSCRIPTEN_PAGE_TITLE ${APPTEST})
		list(APPEND EMSCRIPTEN_SCRIPT_NAMES nCine-data.js ${APPTEST}.js)
		foreach(EMSCRIPTEN_SCRIPT_NAME ${EMSCRIPTEN_SCRIPT_NAMES})
			set(EMSCRIPTEN_SCRIPTS "${EMSCRIPTEN_SCRIPTS}\n    <script type='text/javascript' src='${EMSCRIPTEN_SCRIPT_NAME}'></script>")
		endforeach()

		configure_file(${CMAKE_SOURCE_DIR}/emscripten_shell.html.in ${CMAKE_BINARY_DIR}/tests/${APPTEST}.html @ONLY)
		install(FILES ${CMAKE_BINARY_DIR}/tests/${APPTEST}.html DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
		install(FILES ${CMAKE_BINARY_DIR}/tests/${APPTEST}.wasm DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)

		unset(EMSCRIPTEN_SCRIPTS)
		unset(EMSCRIPTEN_SCRIPT_NAMES)
		unset(EMSCRIPTEN_PAGE_TITLE)
	endif()
endforeach()

if(EMSCRIPTEN)
	if(EXISTS ${NCINE_ICONS_DIR}/icon.ico)
		file(COPY ${NCINE_ICONS_DIR}/icon.ico DESTINATION ${CMAKE_BINARY_DIR})
		file(RENAME ${CMAKE_BINARY_DIR}/icon.ico ${CMAKE_BINARY_DIR}/favicon.ico)
		install(FILES ${CMAKE_BINARY_DIR}/favicon.ico DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	endif()

	find_program(EMSCRIPTEN_PACKAGER_SCRIPT file_packager.py
		PATHS $ENV{EMSDK}
		PATH_SUFFIXES upstream/emscripten/tools fastcomp/emscripten/tools
		DOC "The Emscripten file packager Python script")

	if(EMSCRIPTEN_PACKAGER_SCRIPT)
		message(STATUS "Emscripten file packager script: ${EMSCRIPTEN_PACKAGER_SCRIPT}")
		if("${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")
			# Minimal data files installation to only support the `script.lua` example script
			add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tests/nCine-data.js ${CMAKE_BINARY_DIR}/tests/nCine-data.data
				COMMAND python ${EMSCRIPTEN_PACKAGER_SCRIPT}
					${CMAKE_BINARY_DIR}/tests/nCine-data.data
					--preload ${NCINE_DATA_DIR}/fonts/DroidSans32_256.fnt@fonts/DroidSans32_256.fnt
					--preload ${NCINE_DATA_DIR}/fonts/DroidSans32_256.png@fonts/DroidSans32_256.png
					--preload ${NCINE_DATA_DIR}/sounds/coins.ogg@sounds/coins.ogg
					--preload ${NCINE_DATA_DIR}/textures/texture2.png@textures/texture2.png
					--preload ${NCINE_DATA_DIR}/textures/texture3.png@textures/texture3.png
					--preload ${NCINE_DATA_DIR}/textures/smoke_256.png@textures/smoke_256.png
					--preload ${CMAKE_SOURCE_DIR}/scripts/script.lua@scripts/script.lua
					--js-output=${CMAKE_BINARY_DIR}/tests/nCine-data.js)
		else()
			add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tests/nCine-data.js ${CMAKE_BINARY_DIR}/tests/nCine-data.data
				COMMAND python ${EMSCRIPTEN_PACKAGER_SCRIPT}
					${CMAKE_BINARY_DIR}/tests/nCine-data.data
					--preload ${NCINE_DATA_DIR}/fonts@fonts
					--preload ${NCINE_DATA_DIR}/sounds@sounds
					--preload ${NCINE_DATA_DIR}/textures@textures
					--preload ${CMAKE_SOURCE_DIR}/scripts@scripts
					--js-output=${CMAKE_BINARY_DIR}/tests/nCine-data.js)
		endif()
		add_custom_target(ncine_emscripten_data ALL DEPENDS ${CMAKE_BINARY_DIR}/tests/nCine-data.js ${CMAKE_BINARY_DIR}/tests/nCine-data.data)
		set_target_properties(ncine_emscripten_data PROPERTIES FOLDER "Emscripten")
		add_dependencies(ncine_emscripten_data ncine)

		install(FILES ${CMAKE_BINARY_DIR}/tests/nCine-data.js DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
		install(FILES ${CMAKE_BINARY_DIR}/tests/nCine-data.data DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)
	endif()
endif()

list(FIND APPTESTS ${NCINE_STARTUP_TEST} STARTUP_TEST_INDEX)
if(STARTUP_TEST_INDEX GREATER -1)
	if(NOT TESTS_DATA_DIR_DIST) # Don't set the startup test if it wouldn't find the data directory
		set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NCINE_STARTUP_TEST})
	endif()
	message(STATUS "Startup test target: \"${NCINE_STARTUP_TEST}\"")
else()
	message(WARNING "Startup test target not found: \"${NCINE_STARTUP_TEST}\"")
endif()

include(ncine_extract_symbols)
include(ncine_strip_binaries)
