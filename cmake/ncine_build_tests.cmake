if(NCINE_BUILD_TESTS)
	if(IS_DIRECTORY ${NCINE_DATA_DIR})
		if(WIN32)
			if(NOT NCINE_EMBED_SHADERS)
				# Copying all shaders even if not all are required by the current configuration
				add_custom_target(copy_shaders_data ALL
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${NCINE_DATA_DIR}/shaders
					COMMENT "Copying shaders to data..."
				)
				set_target_properties(copy_shaders_data PROPERTIES FOLDER "CustomCopyTargets")
			endif()

			if(MSVC)
				include(InstallRequiredSystemLibraries)
			endif()
		else()
			# Symlinking all shaders even if not all are required by the current configuration
			if(NOT NCINE_EMBED_SHADERS)
				add_custom_target(symlink_shaders_data ALL
					COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/shaders ${NCINE_DATA_DIR}/shaders
					COMMENT "Symlinking shaders to data..."
				)
				set_target_properties(symlink_shaders_data PROPERTIES FOLDER "CustomSymlinkTargets")
			endif()
		endif()

		# Embed the installation data directory path in test executables
		if(TESTS_DATA_DIR_DIST)
			if(MSVC OR APPLE)
				# Relative path from tests to data on Windows and OS X, where the user can choose the installation directory
				file(RELATIVE_PATH NCINE_TESTS_DATA_DIR
					${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION}
					${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}) # Always strips trailing slash
				set(NCINE_TESTS_DATA_DIR "${NCINE_TESTS_DATA_DIR}/")
			else()
				set(NCINE_TESTS_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}/")
				if("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
					# Allow relative path from tests to data on Linux, if the install prefix is empty
					set(NCINE_TESTS_DATA_DIR "${DATA_INSTALL_DESTINATION}/")
				endif()
			endif()
		elseif(NOT NCINE_TESTS_DATA_DIR)
			file(TO_CMAKE_PATH "${NCINE_DATA_DIR}" NCINE_TESTS_DATA_DIR) # Always strips trailing slash
			set(NCINE_TESTS_DATA_DIR "${NCINE_TESTS_DATA_DIR}/")
		endif()
		message(STATUS "Data directory for tests: ${NCINE_TESTS_DATA_DIR}")

		# Emscripten does not need any data to run tests
		if(NOT (EMSCRIPTEN AND ("${NCINE_OPTIONS_PRESETS}" STREQUAL "BinDist" OR "${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")))
			if("${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")
				# Minimal data files installation to only support the `script.lua` example script
				install(FILES ${NCINE_DATA_DIR}/fonts/DroidSans32_256.fnt DESTINATION ${DATA_INSTALL_DESTINATION}/fonts COMPONENT data)
				install(FILES ${NCINE_DATA_DIR}/fonts/DroidSans32_256.png DESTINATION ${DATA_INSTALL_DESTINATION}/fonts COMPONENT data)
				install(FILES ${NCINE_DATA_DIR}/sounds/coins.ogg DESTINATION ${DATA_INSTALL_DESTINATION}/sounds COMPONENT data)
				install(FILES ${NCINE_DATA_DIR}/textures/texture2.png DESTINATION ${DATA_INSTALL_DESTINATION}/textures COMPONENT data)
				install(FILES ${NCINE_DATA_DIR}/textures/texture3.png DESTINATION ${DATA_INSTALL_DESTINATION}/textures COMPONENT data)
				install(FILES ${NCINE_DATA_DIR}/textures/smoke_256.png DESTINATION ${DATA_INSTALL_DESTINATION}/textures COMPONENT data)
				install(FILES ${NCINE_DATA_DIR}/textures/spritesheet.png DESTINATION ${DATA_INSTALL_DESTINATION}/textures COMPONENT data)
				install(FILES ${CMAKE_SOURCE_DIR}/scripts/script.lua DESTINATION ${DATA_INSTALL_DESTINATION}/scripts COMPONENT data)
				install(FILES ${CMAKE_SOURCE_DIR}/scripts/ncine_header.lua DESTINATION ${DATA_INSTALL_DESTINATION}/scripts COMPONENT data)
				install(FILES ${CMAKE_SOURCE_DIR}/scripts/ncine_footer.lua DESTINATION ${DATA_INSTALL_DESTINATION}/scripts COMPONENT data)
			else()
				install(DIRECTORY ${NCINE_DATA_DIR}/fonts DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
				install(DIRECTORY ${NCINE_DATA_DIR}/sounds DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
				install(DIRECTORY ${NCINE_DATA_DIR}/textures DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
				if(LUA_FOUND)
					target_compile_definitions(ncine PRIVATE "NCINE_TESTS_DATA_DIR=\"${NCINE_TESTS_DATA_DIR}\"") # for LuaEventHandler
					install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
				endif()
			endif()

			install(FILES ${NCINE_DATA_DIR}/README.md DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
		endif()
	endif()

	add_subdirectory(tests)
	# Don't build and install srctests on a Lua distribution for Emscripten
	if(NOT NCINE_DYNAMIC_LIBRARY AND NOT "${NCINE_OPTIONS_PRESETS}" STREQUAL "LuaDist")
		add_subdirectory(src/tests)
	endif()
endif()
