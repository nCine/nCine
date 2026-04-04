if(EMSCRIPTEN)
	set(EXTERNAL_EMSCRIPTEN_DIR "${PARENT_SOURCE_DIR}/nCine-external-emscripten" CACHE PATH "Set the path to the Emscripten libraries directory")
	if(NOT IS_DIRECTORY ${EXTERNAL_EMSCRIPTEN_DIR})
		message(FATAL_ERROR "nCine external Emscripten libraries directory not found at: ${EXTERNAL_EMSCRIPTEN_DIR}")
	else()
		message(STATUS "nCine external Emscripten libraries directory: ${EXTERNAL_EMSCRIPTEN_DIR}")
	endif()

	# --------------------------------
	if(NCINE_WITH_THREADS)
		add_library(Threads::Threads INTERFACE IMPORTED)
		set_target_properties(Threads::Threads PROPERTIES
			INTERFACE_COMPILE_OPTIONS "SHELL:-pthread"
			INTERFACE_LINK_OPTIONS "SHELL:-pthread -s PTHREAD_POOL_SIZE=4 -s WASM_MEM_MAX=128MB")
		set(Threads_FOUND 1)
	endif()

	if(NCINE_GRAPHICS_API STREQUAL "OPENGL")
		add_library(OpenGL::GL INTERFACE IMPORTED)
		set_target_properties(OpenGL::GL PROPERTIES
			INTERFACE_LINK_OPTIONS "SHELL:-s USE_WEBGL2=1 -s FULL_ES3=1 -s FULL_ES2=1")
		set(OPENGL_FOUND 1)
	endif()

	if(NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
		if(EMSCRIPTEN_VERSION VERSION_GREATER_EQUAL "3.1.57")
			set(IMGUI_EMSCRIPTEN_GLFW3 "SHELL:--use-port=contrib.glfw3" CACHE STRING "Choose between --use-port=contrib.glfw3 and -sUSE_GLFW=3 for GLFW implementation (default to --use-port=contrib.glfw3)")
			if(EMSCRIPTEN_VERSION VERSION_LESS_EQUAL "3.1.59")
				set(IMGUI_EMSCRIPTEN_GLFW3_DEFINE "-DEMSCRIPTEN_USE_PORT_CONTRIB_GLFW3")
			endif()
		else()
			# cannot use contrib.glfw3 prior to 3.1.57
			set(IMGUI_EMSCRIPTEN_GLFW3 "SHELL:-sUSE_GLFW=3" CACHE STRING "Use -sUSE_GLFW=3 for GLFW implementation" FORCE)
		endif()

		add_library(GLFW::GLFW INTERFACE IMPORTED)
		set_target_properties(GLFW::GLFW PROPERTIES
			INTERFACE_COMPILE_OPTIONS "${IMGUI_EMSCRIPTEN_GLFW3} ${IMGUI_EMSCRIPTEN_GLFW3_DEFINE}"
			INTERFACE_LINK_OPTIONS ${IMGUI_EMSCRIPTEN_GLFW3})
		set(GLFW_FOUND 1)
	elseif(NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
		add_library(SDL2::SDL2 INTERFACE IMPORTED)
		set_target_properties(SDL2::SDL2 PROPERTIES
			INTERFACE_COMPILE_OPTIONS "SHELL:-s USE_SDL=2"
			INTERFACE_LINK_OPTIONS "SHELL:-s USE_SDL=2")
		set(SDL2_FOUND 1)
	endif()

	if(NCINE_WITH_PNG)
		add_library(PNG::PNG INTERFACE IMPORTED)
		set_target_properties(PNG::PNG PROPERTIES
			INTERFACE_COMPILE_OPTIONS "SHELL:-s USE_LIBPNG=1"
			INTERFACE_LINK_OPTIONS "SHELL:-s USE_LIBPNG=1")
		set(PNG_FOUND 1)
	endif()

	if(NCINE_WITH_WEBP)
		add_library(WebP::WebP STATIC IMPORTED)
		set_target_properties(WebP::WebP PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_EMSCRIPTEN_DIR}/lib/libwebp.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_EMSCRIPTEN_DIR}/include")
		if(EXISTS ${EXTERNAL_EMSCRIPTEN_DIR}/lib/libsharpyuv.a)
			# Since libwebp 1.3.0, libwebp.a needs some symbols from libsharpyuv.a
			add_library(WebP::SharpYUV STATIC IMPORTED)
			set_target_properties(WebP::SharpYUV PROPERTIES
				IMPORTED_LOCATION ${EXTERNAL_EMSCRIPTEN_DIR}/lib/libsharpyuv.a)
			set_target_properties(WebP::WebP PROPERTIES
				INTERFACE_LINK_LIBRARIES WebP::SharpYUV)
		endif()
		set(WEBP_FOUND 1)
	endif()

	if(NCINE_WITH_AUDIO)
		add_library(OpenAL::AL INTERFACE IMPORTED)
		set_target_properties(OpenAL::AL PROPERTIES
			INTERFACE_LINK_OPTIONS "SHELL:-lopenal")
		set(OPENAL_FOUND 1)

		if(NCINE_WITH_VORBIS)
			add_library(Vorbis::Vorbisfile INTERFACE IMPORTED)
			set_target_properties(Vorbis::Vorbisfile PROPERTIES
				INTERFACE_COMPILE_OPTIONS "SHELL:-s USE_VORBIS=1"
				INTERFACE_LINK_OPTIONS "SHELL:-s USE_VORBIS=1")
			set(VORBIS_FOUND 1)
		endif()
	endif()

	if(NCINE_WITH_LUA)
		add_library(Lua::Lua STATIC IMPORTED)
		set(EMSCRIPTEN_LUA_LIBRARY "liblua.a")
		if(NCINE_WITH_THREADS)
			set(EMSCRIPTEN_LUA_LIBRARY "liblua-mt.a")
		endif()
		set_target_properties(Lua::Lua PROPERTIES
			IMPORTED_LOCATION ${EXTERNAL_EMSCRIPTEN_DIR}/lib/${EMSCRIPTEN_LUA_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_EMSCRIPTEN_DIR}/include")
		set(LUA_FOUND 1)
	endif()
endif()
