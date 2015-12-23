if(NCINE_BUILD_TESTS)
	set(NCINE_DATA_DIR ${PARENT_SOURCE_DIR}/nCine-data)
	if(NOT IS_DIRECTORY ${NCINE_DATA_DIR})
		message(WARNING "nCine data directory not found at: ${NCINE_DATA_DIR}")
	else()
		if(WIN32)
			install(DIRECTORY ${NCINE_DATA_DIR}/ DESTINATION data)
		else()
			install(DIRECTORY ${NCINE_DATA_DIR}/ DESTINATION share/ncine)
		endif()
	endif()

	if(WIN32 AND NCINE_DYNAMIC_LIBRARY)
		if(MSVC_IDE)
			add_custom_target(copy_ncine_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/ncine.dll ${CMAKE_BINARY_DIR}/tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		else()
			add_custom_target(copy_ncine_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/ncine.dll ${CMAKE_BINARY_DIR}/tests
				DEPENDS ncine
				COMMENT "Copying nCine DLL..."
			)
		endif()
		set_target_properties(copy_ncine_dll PROPERTIES FOLDER "CustomCopyTargets")
	endif()
	
	add_subdirectory(tests)
	if(NOT NCINE_DYNAMIC_LIBRARY)
		add_subdirectory(src/tests)
	endif()
endif()