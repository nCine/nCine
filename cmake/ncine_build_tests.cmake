if(NCINE_BUILD_TESTS)
	if(IS_DIRECTORY ${NCINE_DATA_DIR})
		if(WIN32)
			if(NOT NCINE_EMBED_SHADERS)
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
			if(NOT NCINE_EMBED_SHADERS)
				add_custom_target(symlink_shaders_data ALL
					COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/shaders ${NCINE_DATA_DIR}/shaders
					COMMENT "Symlinking shaders to data..."
				)
				set_target_properties(symlink_shaders_data PROPERTIES FOLDER "CustomSymlinkTargets")
			endif()
		endif()

		if(DEFAULT_DATA_DIR_DIST)
			if(MSVC OR APPLE)
				# Relative path from tests to data on Windows and OS X, where the user can choose the installation directory
				file(RELATIVE_PATH NCINE_DEFAULT_DATA_DIR ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION} ${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION})
				set(NCINE_DEFAULT_DATA_DIR "${NCINE_DEFAULT_DATA_DIR}/")
			else()
				set(NCINE_DEFAULT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}/")
			endif()
		elseif(NOT NCINE_DEFAULT_DATA_DIR)
			set(NCINE_DEFAULT_DATA_DIR "${NCINE_DATA_DIR}/")
		endif()
		target_compile_definitions(ncine PRIVATE "NCINE_DEFAULT_DATA_DIR=\"${NCINE_DEFAULT_DATA_DIR}\"")
		message(STATUS "Default data directory: "  ${NCINE_DEFAULT_DATA_DIR})

		install(DIRECTORY ${NCINE_DATA_DIR}/fonts DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
		install(DIRECTORY ${NCINE_DATA_DIR}/sounds DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
		install(DIRECTORY ${NCINE_DATA_DIR}/textures DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
		install(FILES ${NCINE_DATA_DIR}/README.md DESTINATION ${DATA_INSTALL_DESTINATION} COMPONENT data)
	endif()

	if(NCINE_INSTALL_DEV_SUPPORT)
		file(GLOB TEST_SOURCES "tests/*test*.h" "tests/*test*.cpp")
		install(FILES ${TEST_SOURCES} DESTINATION ${TEST_SOURCES_INSTALL_DESTINATION} COMPONENT devsupport)
		install(FILES cmake/ncine_devdist_tests.cmake DESTINATION ${TEST_SOURCES_INSTALL_DESTINATION} RENAME CMakeLists.txt COMPONENT devsupport)
	endif()

	add_subdirectory(tests)
	if(NOT NCINE_DYNAMIC_LIBRARY)
		add_subdirectory(src/tests)
	endif()
endif()
