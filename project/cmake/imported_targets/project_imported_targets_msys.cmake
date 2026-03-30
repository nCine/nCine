if(MINGW OR MSYS)
	function(set_msys_dll PREFIX DLL_NAMES)
		set(FOUND FALSE)

		# 1. Try CMAKE_PREFIX_PATH
		foreach(PREFIX_PATH IN LISTS CMAKE_PREFIX_PATH)
			foreach(DLL_NAME IN LISTS DLL_NAMES)
				set(CANDIDATE_DLL "${PREFIX_PATH}/bin/${DLL_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
				if(EXISTS "${CANDIDATE_DLL}" AND NOT IS_DIRECTORY "${CANDIDATE_DLL}")
					set(${PREFIX}_DLL_LIBRARY "${CANDIDATE_DLL}" PARENT_SCOPE)
					set(FOUND TRUE)
					break()
				endif()
			endforeach()
			if(FOUND)
				break()
			endif()
		endforeach()

		# 2. Fallback to MinGW
		if(NOT FOUND)
			foreach(DLL_NAME IN LISTS DLL_NAMES)
				set(CANDIDATE_DLL "$ENV{MINGW_PREFIX}/bin/${DLL_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
				if(EXISTS "${CANDIDATE_DLL}" AND NOT IS_DIRECTORY "${CANDIDATE_DLL}")
					set(${PREFIX}_DLL_LIBRARY "${CANDIDATE_DLL}" PARENT_SCOPE)
					set(FOUND TRUE)
					break()
				endif()
			endforeach()
		endif()

		if(NOT FOUND)
			message(WARNING "Could not find DLLs: ${DLL_NAMES}")
		endif()
	endfunction()

	if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
		set(INSTALL_LIBRARIES_MINGW TRUE)

		set(OLD_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
		set(CMAKE_FIND_LIBRARY_SUFFIXES .dll ${OLD_CMAKE_FIND_LIBRARY_SUFFIXES})
		find_library(LIBSTDCPP_LOCATION libstdc++-6)
		find_library(LIBGCCSEH_LOCATION gcc_s_seh-1)
		find_library(LIBWINPTHEAD_LOCATION winpthread-1)
		set(MINGW_SYSTEMLIB_LOCATIONS ${LIBSTDCPP_LOCATION} ${LIBGCCSEH_LOCATION} ${LIBWINPTHEAD_LOCATION})
		set(CMAKE_FIND_LIBRARY_SUFFIXES ${OLD_CMAKE_FIND_LIBRARY_SUFFIXES})
	endif()

	# --------------------------------
	if(GLEW_FOUND)
		set_msys_dll(GLEW glew32)
		set_target_properties(GLEW::GLEW PROPERTIES
			IMPORTED_IMPLIB ${GLEW_LIBRARIES}
			IMPORTED_LOCATION ${GLEW_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIR})
	endif()

	if(GLFW_FOUND)
		set_msys_dll(GLFW glfw3)
		add_library(GLFW::GLFW SHARED IMPORTED)
		set_target_properties(GLFW::GLFW PROPERTIES
			IMPORTED_IMPLIB ${GLFW_LIBRARY}
			IMPORTED_LOCATION ${GLFW_DLL_LIBRARY}
			INTERFACE_COMPILE_DEFINITIONS "GLFW_NO_GLU"
			INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INCLUDE_DIR})
	endif()

	if(SDL2_FOUND)
		foreach(LIBRARY ${SDL2_LIBRARY})
			string(REGEX MATCH "\.a$" FOUND_STATIC_LIB ${LIBRARY})
			if(NOT FOUND_STATIC_LIB STREQUAL "" AND NOT ${LIBRARY} STREQUAL ${SDL2MAIN_LIBRARY})
				set(SDL2_IMPORT_LIBRARY ${LIBRARY})
				break()
			endif()
		endforeach()

		set_msys_dll(SDL2 SDL2)
		add_library(SDL2::SDL2 SHARED IMPORTED)
		set_target_properties(SDL2::SDL2 PROPERTIES
			IMPORTED_IMPLIB ${SDL2_IMPORT_LIBRARY}
			IMPORTED_LOCATION ${SDL2_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR}
			INTERFACE_LINK_LIBRARIES ${SDL2MAIN_LIBRARY})
	endif()

	if(PNG_FOUND)
		set_msys_dll(ZLIB "libzlib;zlib1")
		set_target_properties(ZLIB::ZLIB PROPERTIES
			IMPORTED_LOCATION ${ZLIB_DLL_LIBRARY})

		set_msys_dll(PNG "libpng16;libpng16-16")
		set_target_properties(PNG::PNG PROPERTIES
			IMPORTED_LOCATION ${PNG_DLL_LIBRARY})
	endif()

	if(WEBP_FOUND)
		set_msys_dll(WEBP "libwebp;libwebp-7")
		add_library(WebP::WebP SHARED IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_IMPLIB ${WEBP_LIBRARY}
			IMPORTED_LOCATION ${WEBP_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${WEBP_INCLUDE_DIR})
		if(EXISTS ${SHARPYUV_LIBRARY})
			# Since libwebp 1.3.0, libwebp.dll needs some symbols from libsharpyuv.dll
			set_msys_dll(SHARPYUV "libsharpyuv;libsharpyuv-0")
			add_library(WebP::SharpYUV SHARED IMPORTED)
			set_target_properties(WebP::SharpYUV PROPERTIES
				IMPORTED_IMPLIB ${SHARPYUV_LIBRARY}
				IMPORTED_LOCATION ${SHARPYUV_DLL_LIBRARY})
			set_target_properties(WebP::WebP PROPERTIES
				INTERFACE_LINK_LIBRARIES WebP::SharpYUV)
		endif()
	endif()

	if(OPENAL_FOUND)
		set_msys_dll(OPENAL "OpenAL32;libopenal-1")
		add_library(OpenAL::AL SHARED IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			IMPORTED_IMPLIB ${OPENAL_LIBRARY}
			IMPORTED_LOCATION ${OPENAL_DLL_LIBRARY})

		if(VORBIS_FOUND)
			set_msys_dll(OGG "libogg;libogg-0")
			add_library(Ogg::Ogg SHARED IMPORTED)
			set_target_properties(Ogg::Ogg PROPERTIES
				IMPORTED_IMPLIB ${OGG_LIBRARY}
				IMPORTED_LOCATION ${OGG_DLL_LIBRARY})

			set_msys_dll(VORBIS "libvorbis;libvorbis-0")
			add_library(Vorbis::Vorbis SHARED IMPORTED)
			set_target_properties(Vorbis::Vorbis PROPERTIES
				IMPORTED_IMPLIB ${VORBIS_LIBRARY}
				IMPORTED_LOCATION ${VORBIS_DLL_LIBRARY}
				INTERFACE_INCLUDE_DIRECTORIES ${VORBIS_INCLUDE_DIR}
				INTERFACE_LINK_LIBRARIES "${OGG_LIBRARY}")

			set_msys_dll(VORBISFILE "libvorbisfile;libvorbisfile-3")
			add_library(Vorbis::Vorbisfile SHARED IMPORTED)
			set_target_properties(Vorbis::Vorbisfile PROPERTIES
				IMPORTED_IMPLIB ${VORBISFILE_LIBRARY}
				IMPORTED_LOCATION ${VORBISFILE_DLL_LIBRARY}
				INTERFACE_INCLUDE_DIRECTORIES ${VORBIS_INCLUDE_DIR}
				INTERFACE_LINK_LIBRARIES "${VORBIS_LIBRARY}")
		endif()
	endif()

	if(LUA_FOUND)
		set_msys_dll(LUA "liblua55;lua55")
		add_library(Lua::Lua SHARED IMPORTED)
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_IMPLIB ${LUA_LIBRARY}
			IMPORTED_LOCATION ${LUA_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR})
	endif()
endif()
