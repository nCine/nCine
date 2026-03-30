if(ANDROID)
	find_library(ANDROID_LIBRARY android)
	find_library(EGL_LIBRARY EGL)
	find_library(GLES3_LIBRARY GLESv3)
	find_library(LOG_LIBRARY log)
	find_library(NATIVEWINDOW_LIBRARY nativewindow)
	find_library(OPENSLES_LIBRARY OpenSLES)
	find_library(ZLIB_LIBRARY z)

	if(NCINE_WITH_PNG AND EXISTS ${EXTERNAL_ANDROID_DIR}/png/${ANDROID_ABI}/libpng16.a)
		add_library(PNG::PNG STATIC IMPORTED)
		set_target_properties(PNG::PNG PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES "C"
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/png/${ANDROID_ABI}/libpng16.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/png/include"
			INTERFACE_LINK_LIBRARIES ${ZLIB_LIBRARY})
		set(PNG_FOUND 1)
	endif()

	if(NCINE_WITH_WEBP AND EXISTS ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libwebp.a)
		add_library(WebP::WebP STATIC IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libwebp.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/webp/include")
		if(EXISTS ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libsharpyuv.a)
			# Since libwebp 1.3.0, libwebp.a needs some symbols from libsharpyuv.a
			add_library(WebP::SharpYUV STATIC IMPORTED)
			set_target_properties(WebP::SharpYUV PROPERTIES
				IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libsharpyuv.a)
			set_target_properties(WebP::WebP PROPERTIES
				INTERFACE_LINK_LIBRARIES WebP::SharpYUV)
		endif()
		set(WEBP_FOUND 1)
	endif()

	if(NCINE_WITH_AUDIO AND EXISTS ${EXTERNAL_ANDROID_DIR}/openal/${ANDROID_ABI}/libopenal.so)
		add_library(OpenAL::AL SHARED IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/openal/${ANDROID_ABI}/libopenal.so
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/openal/include")
		set(OPENAL_FOUND 1)

		if(NCINE_WITH_VORBIS AND
		   EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbisfile.a AND
		   EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbis.a AND
		   EXISTS ${EXTERNAL_ANDROID_DIR}/ogg/${ANDROID_ABI}/libogg.a)
			add_library(Ogg::Ogg STATIC IMPORTED)
			set_target_properties(Ogg::Ogg PROPERTIES
				IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/ogg/${ANDROID_ABI}/libogg.a
				INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/ogg/include")

			add_library(Vorbis::Vorbis STATIC IMPORTED)
			set_target_properties(Vorbis::Vorbis PROPERTIES
				IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbis.a
				INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/vorbis/include"
				INTERFACE_LINK_LIBRARIES Ogg::Ogg)

			add_library(Vorbis::Vorbisfile STATIC IMPORTED)
			set_target_properties(Vorbis::Vorbisfile PROPERTIES
				IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbisfile.a
				INTERFACE_LINK_LIBRARIES Vorbis::Vorbis)
			set(VORBIS_FOUND 1)
		endif()
	endif()

	if(NCINE_WITH_LUA AND EXISTS ${EXTERNAL_ANDROID_DIR}/lua/${ANDROID_ABI}/liblua.a)
		add_library(Lua::Lua STATIC IMPORTED)
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/lua/${ANDROID_ABI}/liblua.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/lua/include")
		set(LUA_FOUND 1)
	endif()
endif()
