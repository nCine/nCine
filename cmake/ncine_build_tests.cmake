if(NCINE_BUILD_TESTS)
	set(NCINE_DATA_DIR ${PARENT_SOURCE_DIR}/nCine-data)

	if(NOT IS_DIRECTORY ${NCINE_DATA_DIR})
		message(WARNING "nCine data directory not found at: ${NCINE_DATA_DIR}")
	else()
		if(WIN32)
			if(NOT NCINE_EMBED_SHADERS)
				add_custom_target(copy_shaders_data ALL
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${NCINE_DATA_DIR}/shaders
					COMMENT "Copying shaders to data..."
				)
				set_target_properties(copy_shaders_data PROPERTIES FOLDER "CustomCopyTargets")
			endif()

			set(DATA_INSTALL_DESTINATION data)
			include(InstallRequiredSystemLibraries)
		else()
			if(NOT NCINE_EMBED_SHADERS)
				add_custom_target(symlink_shaders_data ALL
					COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/shaders ${NCINE_DATA_DIR}/shaders
					COMMENT "Symlinking shaders to data..."
				)
				set_target_properties(symlink_shaders_data PROPERTIES FOLDER "CustomSymlinkTargets")
			endif()

			set(DATA_INSTALL_DESTINATION share/ncine/data)
		endif()

		if(DEFAULT_DATA_DIR_DIST)
			if(WIN32)
				# Relative path from tests to data on Windows, where the user can choose the installation directory
				file(RELATIVE_PATH NCINE_DEFAULT_DATA_DIR ${CMAKE_INSTALL_PREFIX}/bin ${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION})
				set(NCINE_DEFAULT_DATA_DIR "${NCINE_DEFAULT_DATA_DIR}/")
			else()
				set(NCINE_DEFAULT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}/")
			endif()
		elseif(NOT NCINE_DEFAULT_DATA_DIR)
			set(NCINE_DEFAULT_DATA_DIR "${NCINE_DATA_DIR}/")
		endif()
		target_compile_definitions(ncine PRIVATE "NCINE_DEFAULT_DATA_DIR=\"${NCINE_DEFAULT_DATA_DIR}\"")
		message(STATUS "Default data directory: "  ${NCINE_DEFAULT_DATA_DIR})

		install(DIRECTORY ${NCINE_DATA_DIR}/fonts DESTINATION ${DATA_INSTALL_DESTINATION})
		install(DIRECTORY ${NCINE_DATA_DIR}/sounds DESTINATION ${DATA_INSTALL_DESTINATION})
		install(DIRECTORY ${NCINE_DATA_DIR}/textures DESTINATION ${DATA_INSTALL_DESTINATION})
	endif()

	if(NCINE_INSTALL_DEV_SUPPORT)
		if(WIN32)
			set(TEST_SOURCES_INSTALL_DESTINATION src)
		else()
			set(TEST_SOURCES_INSTALL_DESTINATION share/ncine)
		endif()

		file(GLOB TEST_SOURCES "tests/*test*.h" "tests/*test*.cpp")
		install(FILES ${TEST_SOURCES} DESTINATION ${TEST_SOURCES_INSTALL_DESTINATION})
		install(FILES cmake/ncine_devdist_tests.cmake DESTINATION ${TEST_SOURCES_INSTALL_DESTINATION} RENAME CMakeLists.txt)
	endif()

	add_subdirectory(tests)
	if(NOT NCINE_DYNAMIC_LIBRARY)
		add_subdirectory(src/tests)
	endif()
endif()
