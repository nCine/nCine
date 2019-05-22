find_package(Threads)
if(MSVC)
	set(EXTERNAL_MSVC_DIR "${PARENT_SOURCE_DIR}/nCine-external" CACHE PATH "Set the path to the MSVC libraries directory")
	if(NOT IS_DIRECTORY ${EXTERNAL_MSVC_DIR})
		message(FATAL_ERROR "nCine external MSVC libraries directory not found at: ${EXTERNAL_MSVC_DIR}")
	else()
		message(STATUS "nCine external MSVC libraries directory: ${EXTERNAL_MSVC_DIR}")
	endif()

	if(MSVC_C_ARCHITECTURE_ID MATCHES 64 OR MSVC_CXX_ARCHITECTURE_ID MATCHES 64)
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x64")
		set(BINDIR "${EXTERNAL_MSVC_DIR}/bin/x64")
	else()
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x86")
		set(BINDIR "${EXTERNAL_MSVC_DIR}/bin/x86")
	endif()
elseif(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Android") # GCC and LLVM
	if(APPLE)
		set(CMAKE_FRAMEWORK_PATH "${PARENT_SOURCE_DIR}/nCine-external")
		set(CMAKE_MACOSX_RPATH ON)

		if(NOT IS_DIRECTORY ${CMAKE_FRAMEWORK_PATH})
			message(FATAL_ERROR "nCine external OS X frameworks directory not found at: ${CMAKE_FRAMEWORK_PATH}")
		else()
			message(STATUS "nCine external OS X frameworks directory: ${CMAKE_FRAMEWORK_PATH}")
		endif()
	endif()

	if(WIN32)
		find_package(GLEW REQUIRED)
	else()
		find_package(GLEW)
	endif()
	find_package(OpenGL REQUIRED)
	find_package(GLFW)
	find_package(SDL2)
	find_package(PNG)
	find_package(OpenAL)
	find_package(WebP)
	find_package(Vorbis)
	find_package(Lua)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	find_library(ANDROID_LIBRARY android)
	find_library(EGL_LIBRARY EGL)
	find_library(GLES3_LIBRARY GLESv3)
	find_library(LOG_LIBRARY log)
	find_library(OPENSLES_LIBRARY OpenSLES)
	find_library(ZLIB_LIBRARY z)

	if(EXISTS ${EXTERNAL_ANDROID_DIR}/png/${ANDROID_ABI}/libpng16.a)
		add_library(PNG::PNG STATIC IMPORTED)
		set_target_properties(PNG::PNG PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES "C"
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/png/${ANDROID_ABI}/libpng16.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/png/include"
			INTERFACE_LINK_LIBRARIES ${ZLIB_LIBRARY})
		set(PNG_FOUND 1)
	endif()

	if(EXISTS ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libwebp.a)
		add_library(WebP::WebP STATIC IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libwebp.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/webp/include")
		set(WEBP_FOUND 1)
	endif()

	if(EXISTS ${EXTERNAL_ANDROID_DIR}/openal/${ANDROID_ABI}/libopenal.so)
		add_library(OpenAL::AL SHARED IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/openal/${ANDROID_ABI}/libopenal.so
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/openal/include")
		set(OPENAL_FOUND 1)
	endif()

	if(EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbisfile.a AND
	   EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbis.a AND
	   EXISTS ${EXTERNAL_ANDROID_DIR}/ogg/${ANDROID_ABI}/libogg.a)
		add_library(Vorbis::Vorbisfile STATIC IMPORTED)
		set_target_properties(Vorbis::Vorbisfile PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbisfile.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/vorbis/include;${EXTERNAL_ANDROID_DIR}/ogg/include"
			INTERFACE_LINK_LIBRARIES "${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbis.a;${EXTERNAL_ANDROID_DIR}/ogg/${ANDROID_ABI}/libogg.a")
		set(VORBIS_FOUND 1)
	endif()

	if(EXISTS ${EXTERNAL_ANDROID_DIR}/lua/${ANDROID_ABI}/liblua.a)
		add_library(Lua::Lua STATIC IMPORTED)
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/lua/${ANDROID_ABI}/liblua.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/lua/include")
		set(LUA_FOUND 1)
	endif()
elseif(MSVC)
	if(EXISTS ${LIBDIR}/glew32.lib AND EXISTS ${BINDIR}/glew32.dll)
		add_library(GLEW::GLEW SHARED IMPORTED)
		set_target_properties(GLEW::GLEW PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/glew32.lib
			IMPORTED_LOCATION ${BINDIR}/glew32.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include")
		set(GLEW_FOUND 1)
	endif()

	add_library(OpenGL::GL SHARED IMPORTED)
	set_target_properties(OpenGL::GL PROPERTIES
			IMPORTED_IMPLIB opengl32.lib
			IMPORTED_LOCATION opengl32.dll)
	set(OPENGL_FOUND 1)

	if(EXISTS ${LIBDIR}/glfw3dll.lib AND EXISTS ${BINDIR}/glfw3.dll)
		add_library(GLFW::GLFW SHARED IMPORTED)
		set_target_properties(GLFW::GLFW PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/glfw3dll.lib
			IMPORTED_LOCATION ${BINDIR}/glfw3.dll
			INTERFACE_COMPILE_DEFINITIONS "GLFW_NO_GLU"
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include")
		set(GLFW_FOUND 1)
	endif()

	if(EXISTS ${LIBDIR}/SDL2.lib AND EXISTS ${LIBDIR}/SDL2main.lib AND EXISTS ${BINDIR}/SDL2.dll)
		add_library(SDL2::SDL2 SHARED IMPORTED)
		set_target_properties(SDL2::SDL2 PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/SDL2.lib
			IMPORTED_LOCATION ${BINDIR}/SDL2.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include"
			INTERFACE_LINK_LIBRARIES ${LIBDIR}/SDL2main.lib)
		set(SDL2_FOUND 1)
	endif()

	if(EXISTS ${LIBDIR}/libpng16.lib AND EXISTS ${LIBDIR}/zlib.lib AND
	   EXISTS ${BINDIR}/libpng16.dll AND EXISTS ${BINDIR}/zlib.dll)
		add_library(PNG::PNG SHARED IMPORTED)
		set_target_properties(PNG::PNG PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES "C"
			IMPORTED_IMPLIB ${LIBDIR}/libpng16.lib
			IMPORTED_LOCATION ${BINDIR}/libpng16.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include"
			INTERFACE_LINK_LIBRARIES ${LIBDIR}/zlib.lib)
		set(PNG_FOUND 1)
	endif()

	if(EXISTS ${LIBDIR}/OpenAL32.lib AND EXISTS ${BINDIR}/OpenAL32.dll)
		add_library(OpenAL::AL SHARED IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/OpenAL32.lib
			IMPORTED_LOCATION ${BINDIR}/OpenAL32.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include")
		set(OPENAL_FOUND 1)
	endif()

	if(EXISTS ${LIBDIR}/libwebp_dll.lib AND EXISTS ${BINDIR}/libwebp.dll AND EXISTS ${BINDIR}/libwebpdecoder.dll)
		add_library(WebP::WebP SHARED IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/libwebp_dll.lib
			IMPORTED_LOCATION ${BINDIR}/libwebp.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include")
		set(WEBP_FOUND 1)
	endif()

	if(EXISTS ${LIBDIR}/libogg.lib AND EXISTS ${LIBDIR}/libvorbis.lib AND EXISTS ${LIBDIR}/libvorbisfile.lib AND
	   EXISTS ${BINDIR}/libogg.dll AND EXISTS ${BINDIR}/libvorbis.dll AND EXISTS ${BINDIR}/libvorbisfile.dll)
		add_library(Vorbis::Vorbisfile SHARED IMPORTED)
		set_target_properties(Vorbis::Vorbisfile PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/libvorbisfile.lib
			IMPORTED_LOCATION ${BINDIR}/libvorbisfile.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include"
			INTERFACE_LINK_LIBRARIES "${LIBDIR}/libvorbis.lib;${LIBDIR}/libogg.lib")
		set(VORBIS_FOUND 1)
	endif()

	if(EXISTS ${LIBDIR}/lua53.lib AND EXISTS ${BINDIR}/lua53.dll)
		add_library(Lua::Lua SHARED IMPORTED)
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_IMPLIB ${LIBDIR}/lua53.lib
			IMPORTED_LOCATION ${BINDIR}/lua53.dll
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include")
		set(LUA_FOUND 1)
	endif()
elseif(MINGW OR MSYS)
	function(set_msys_dll PREFIX DLL_NAME)
		set(LIB_NAME "${DLL_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
		set(DLL_LIBRARY "$ENV{MINGW_PREFIX}/bin/${LIB_NAME}")
		if(EXISTS ${DLL_LIBRARY} AND NOT IS_DIRECTORY ${DLL_LIBRARY})
			set(${PREFIX}_DLL_LIBRARY ${DLL_LIBRARY} PARENT_SCOPE)
		else()
			message(WARNING "Could not find: ${DLL_LIBRARY}")
		endif()
	endfunction()

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
			string(FIND ${LIBRARY} ".a" FOUND_STATIC_LIB)
			if(FOUND_STATIC_LIB GREATER -1 AND NOT ${LIBRARY} STREQUAL ${SDL2MAIN_LIBRARY})
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

	if(OPENAL_FOUND)
		set_msys_dll(OPENAL libopenal-1)
		add_library(OpenAL::AL SHARED IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			IMPORTED_IMPLIB ${OPENAL_LIBRARY}
			IMPORTED_LOCATION ${OPENAL_DLL_LIBRARY}
			IMPORTED_LOCATION ${OPENAL_LIB_PATH}/${OPENAL_LIB_NAME}.dll
			INTERFACE_INCLUDE_DIRECTORIES ${OPENAL_INCLUDE_DIR})
	endif()

	if(WEBP_FOUND)
		set_msys_dll(WEBP libwebp-7)
		add_library(WebP::WebP SHARED IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_IMPLIB ${WEBP_LIBRARY}
			IMPORTED_LOCATION ${WEBP_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${WEBP_INCLUDE_DIR})
	endif()

	if(VORBIS_FOUND)
		set_msys_dll(VORBISFILE libvorbisfile-3)
		add_library(Vorbis::Vorbisfile SHARED IMPORTED)
		set_target_properties(Vorbis::Vorbisfile PROPERTIES
			IMPORTED_IMPLIB ${VORBISFILE_LIBRARY}
			IMPORTED_LOCATION ${VORBISFILE_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${VORBIS_INCLUDE_DIR}
			INTERFACE_LINK_LIBRARIES "${VORBIS_LIBRARY};${OGG_LIBRARY}")
	endif()

	if(LUA_FOUND)
		set_msys_dll(LUA lua53)
		add_library(Lua::Lua SHARED IMPORTED)
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_IMPLIB ${LUA_LIBRARY}
			IMPORTED_LOCATION ${LUA_DLL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR})
	endif()
else() # GCC and LLVM
	function(split_extra_libraries PREFIX LIBRARIES)
		foreach(LIBRARY ${LIBRARIES})
			string(FIND ${LIBRARY} "-l" FOUND_LINKER_ARG)
			string(FIND ${LIBRARY} ".a" FOUND_STATIC_LIB)
			if(FOUND_LINKER_ARG GREATER -1 OR FOUND_STATIC_LIB GREATER -1)
				list(APPEND EXTRA_LIBRARIES ${LIBRARY})
			else()
				list(APPEND LIBRARY_FILE ${LIBRARY})
			endif()
		endforeach()
		set(${PREFIX}_EXTRA_LIBRARIES ${EXTRA_LIBRARIES} PARENT_SCOPE)
		set(${PREFIX}_LIBRARY_FILE ${LIBRARY_FILE} PARENT_SCOPE)
	endfunction()

	if(GLFW_FOUND)
		add_library(GLFW::GLFW SHARED IMPORTED)
		set_target_properties(GLFW::GLFW PROPERTIES
			IMPORTED_LOCATION "${GLFW_LIBRARY}" # On macOS it's a list
			INTERFACE_COMPILE_DEFINITIONS "GLFW_NO_GLU"
			INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INCLUDE_DIR})
	endif()

	if(SDL2_FOUND)
		split_extra_libraries(SDL2 "${SDL2_LIBRARY}")
		add_library(SDL2::SDL2 SHARED IMPORTED)
		set_target_properties(SDL2::SDL2 PROPERTIES
			IMPORTED_LOCATION "${SDL2_LIBRARY_FILE}" # On macOS it's a list
			INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR}
			INTERFACE_LINK_LIBRARIES "${SDL2_EXTRA_LIBRARIES}")
	endif()

	if(OPENAL_FOUND)
		add_library(OpenAL::AL SHARED IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			IMPORTED_LOCATION ${OPENAL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${OPENAL_INCLUDE_DIR})
	endif()

	if(WEBP_FOUND)
		add_library(WebP::WebP SHARED IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_LOCATION ${WEBP_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${WEBP_INCLUDE_DIR})
	endif()

	if(VORBIS_FOUND)
		add_library(Vorbis::Vorbisfile SHARED IMPORTED)
		set_target_properties(Vorbis::Vorbisfile PROPERTIES
			IMPORTED_LOCATION ${VORBISFILE_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${VORBIS_INCLUDE_DIR}
			INTERFACE_LINK_LIBRARIES "${VORBIS_LIBRARY};${OGG_LIBRARY}")
	endif()

	if(LUA_FOUND)
		add_library(Lua::Lua SHARED IMPORTED)
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_LOCATION ${LUA_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR})
	endif()

	if(APPLE)
		function(split_extra_frameworks PREFIX LIBRARIES)
			foreach(LIBRARY ${LIBRARIES})
				string(FIND ${LIBRARY} "-framework " FOUND)
				if(FOUND GREATER -1)
					list(APPEND FRAMEWORK_LINKS ${LIBRARY})
				else()
					list(APPEND FRAMEWORK_DIR ${LIBRARY})
				endif()
			endforeach()
			set(${PREFIX}_FRAMEWORK_LINKS ${FRAMEWORK_LINKS} PARENT_SCOPE)
			set(${PREFIX}_FRAMEWORK_DIR ${FRAMEWORK_DIR} PARENT_SCOPE)
		endfunction()

		if(GLEW_FOUND)
			get_target_property(GLEW_LIBRARY_RELEASE GLEW::GLEW IMPORTED_LOCATION_RELEASE)
			set_target_properties(GLEW::GLEW PROPERTIES
				IMPORTED_LOCATION ${GLEW_LIBRARY_RELEASE}/glew
				IMPORTED_LOCATION_RELEASE ${GLEW_LIBRARY_RELEASE}/glew
				IMPORTED_LOCATION_DEBUG ${GLEW_LIBRARY_RELEASE}/glew)
		endif()

		if(GLFW_FOUND)
			split_extra_frameworks(GLFW "${GLFW_LIBRARY}")
			set_target_properties(GLFW::GLFW PROPERTIES
				IMPORTED_LOCATION ${GLFW_FRAMEWORK_DIR}/glfw
				INTERFACE_LINK_LIBRARIES ${GLFW_FRAMEWORK_LINKS})
		endif()

		if(SDL2_FOUND)
			split_extra_frameworks(SDL2 "${SDL2_LIBRARY}")
			set_target_properties(SDL2::SDL2 PROPERTIES
				IMPORTED_LOCATION ${SDL2_FRAMEWORK_DIR}/sdl2
				INTERFACE_LINK_LIBRARIES ${SDL2_FRAMEWORK_LINKS})
		endif()

		if(OPENAL_FOUND)
			set_target_properties(OpenAL::AL PROPERTIES
				IMPORTED_LOCATION ${OPENAL_LIBRARY}/openal)
		endif()

		if(PNG_FOUND)
			get_target_property(ZLIB_LIBRARY_RELEASE ZLIB::ZLIB IMPORTED_LOCATION_RELEASE)
			set_target_properties(ZLIB::ZLIB PROPERTIES
				IMPORTED_LOCATION ${ZLIB_LIBRARY_RELEASE}/zlib
				IMPORTED_LOCATION_RELEASE ${ZLIB_LIBRARY_RELEASE}/zlib
				IMPORTED_LOCATION_DEBUG ${ZLIB_LIBRARY_RELEASE}/zlib)
			get_target_property(PNG_LIBRARY_RELEASE PNG::PNG IMPORTED_LOCATION_RELEASE)
			set_target_properties(PNG::PNG PROPERTIES
				IMPORTED_LOCATION ${PNG_LIBRARY_RELEASE}/png
				IMPORTED_LOCATION_RELEASE ${PNG_LIBRARY_RELEASE}/png
				IMPORTED_LOCATION_DEBUG ${PNG_LIBRARY_RELEASE}/png)
		endif()

		if(WEBP_FOUND)
			set_target_properties(WebP::WebP PROPERTIES
				IMPORTED_LOCATION ${WEBP_LIBRARY}/webp)
		endif()

		if(VORBIS_FOUND)
			set_target_properties(Vorbis::Vorbisfile PROPERTIES
				IMPORTED_LOCATION ${VORBISFILE_LIBRARY}/vorbisfile)
		endif()

		if(LUA_FOUND)
			set_target_properties(Lua::Lua PROPERTIES
				IMPORTED_LOCATION ${LUA_LIBRARY}/lua)
		endif()
	endif()
endif()
