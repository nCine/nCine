set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if(UNIX)
	if(IS_DIRECTORY ${NCINE_SHADERS_DIR})
		add_custom_target(symlink_shaders ALL
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_SHADERS_DIR} ${NCPROJECT_DEFAULT_DATA_DIR}/shaders
			COMMENT "Symlinking shaders..."
		)
		set_target_properties(symlink_shaders PROPERTIES FOLDER "CustomSymlinkTargets")
	endif()
elseif(WIN32)
	if(IS_DIRECTORY ${NCINE_SHADERS_DIR})
		add_custom_target(copy_shaders ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_SHADERS_DIR} ${NCPROJECT_DEFAULT_DATA_DIR}/shaders
			COMMENT "Copying shaders..."
		)
		set_target_properties(copy_shaders PROPERTIES FOLDER "CustomCopyTargets")
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		add_custom_target(copy_ncine_dll ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${NCINE_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
			COMMENT "Copying nCine DLL..."
		)
		set_target_properties(copy_ncine_dll PROPERTIES FOLDER "CustomCopyTargets")
	endif()
endif()

if(MSVC)
	if(EXISTS ${NCINE_CONFIG_H})
		if(ANGLE_FOUND AND NCINE_WITH_ANGLE)
			add_custom_target(copy_angle_dlls ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EGL_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLES_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying ANGLE DLLs...")
			set_target_properties(copy_angle_dlls PROPERTIES FOLDER "CustomCopyTargets")
		endif()
		if(GLEW_FOUND AND NCINE_WITH_GLEW)
			add_custom_target(copy_glew_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLEW_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying GLEW DLL...")
			set_target_properties(copy_glew_dll PROPERTIES FOLDER "CustomCopyTargets")
		endif()
		if(GLFW_FOUND AND NCINE_WITH_GLFW)
			add_custom_target(copy_glfw_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLFW_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying GLFW DLL...")
			set_target_properties(copy_glfw_dll PROPERTIES FOLDER "CustomCopyTargets")
		endif()
		if(SDL2_FOUND AND NCINE_WITH_SDL)
			add_custom_target(copy_sdl2_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying SDL2 DLL...")
			set_target_properties(copy_sdl2_dll PROPERTIES FOLDER "CustomCopyTargets")
		endif()
		if(PNG_FOUND AND NCINE_WITH_PNG)
			add_custom_target(copy_png_dlls ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ZLIB_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PNG_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying zlib and libpng DLLs...")
			set_target_properties(copy_png_dlls PROPERTIES FOLDER "CustomCopyTargets")
		endif()
		if(WEBP_FOUND AND NCINE_WITH_WEBP)
			add_custom_target(copy_webp_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBP_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying WebP DLL...")
			set_target_properties(copy_webp_dll PROPERTIES FOLDER "CustomCopyTargets")
		endif()
		if(OPENAL_FOUND AND NCINE_WITH_AUDIO)
			add_custom_target(copy_openal_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENAL_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying OpenAL DLL...")
			set_target_properties(copy_openal_dll PROPERTIES FOLDER "CustomCopyTargets")

			if(VORBIS_FOUND AND NCINE_WITH_VORBIS)
				add_custom_target(copy_vorbis_dlls ALL
					COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OGG_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
					COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
					COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBISFILE_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
					COMMENT "Copying Ogg and Vorbis DLLs...")
				set_target_properties(copy_vorbis_dlls PROPERTIES FOLDER "CustomCopyTargets")
			endif()
		endif()
		if(LUA_FOUND AND NCINE_WITH_LUA)
			add_custom_target(copy_lua_dll ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LUA_IMPORTED_LOCATION} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
				COMMENT "Copying Lua DLL...")
			set_target_properties(copy_lua_dll PROPERTIES FOLDER "CustomCopyTargets")
		endif()
	else()
		file(GLOB MSVC_DLL_FILES ${MSVC_BINDIR}/*.dll)
		add_custom_target(copy_dlls ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MSVC_DLL_FILES} $<TARGET_FILE_DIR:${NCPROJECT_EXE_NAME}>
			COMMENT "Copying DLLs..."
		)
		set_target_properties(copy_dlls PROPERTIES FOLDER "CustomCopyTargets")
	endif()
endif()
