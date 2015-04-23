cmake_minimum_required(VERSION 2.8.11)
project(nCine)

option(NCINE_BUILD_TESTS "Build the engine test programs" ON)
option(NCINE_EXTRA_OPTIMIZE "Compile the engine with extra optimizations in release mode" ON)
option(NCINE_AUTOVECTORIZATION_REPORT "Enable report generation from compiler auto-vectorization" OFF)
option(NCINE_DYNAMIC_LIBRARY "Compile the engine as a dynamic library" OFF)

find_package(Threads)
if(MSVC)
	set(EXTERNAL_MSVC_DIR "${CMAKE_SOURCE_DIR}/external")
	if(MSVC_C_ARCHITECTURE_ID MATCHES 64 OR MSVC_CXX_ARCHITECTURE_ID MATCHES 64)
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x64")
		set(BINDIR "${EXTERNAL_MSVC_DIR}/bin/x64")
	else()
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x86")
		set(BINDIR "${EXTERNAL_MSVC_DIR}/bin/x86")
	endif()
	if(NOT (MSVC_VERSION LESS 1800)) # If Visual Studio 2013 or greater
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS")
	endif()
	
	set(OPENGL_gl_LIBRARY opengl32.lib ${LIBDIR}/glew32.lib)
	set(GLFW_LIBRARY ${LIBDIR}/glfw3.lib)
	set(GLFW_FOUND 1)
	set(PNG_LIBRARY ${LIBDIR}/libpng.lib)
	set(PNG_FOUND 1)
	#set(SDL_LIBRARY ${LIBDIR}/SDLmain.lib ${LIBDIR}/SDL.lib)
	#set(SDL_FOUND 1)
	#set(SDLIMAGE_LIBRARY ${LIBDIR}/SDL_image.lib)
	#set(SDLIMAGE_FOUND 1)
	set(OPENAL_LIBRARY ${LIBDIR}/OpenAL32.lib)
	set(OPENAL_FOUND 1)
	set(WEBP_LIBRARY ${LIBDIR}/libwebp.lib)
	set(WEBP_FOUND 1)
	set(VORBISFILE_LIBRARY ${LIBDIR}/libogg.lib ${LIBDIR}/libvorbis.lib ${LIBDIR}/libvorbisfile.lib)
	set(VORBIS_FOUND 1)
	
	string(REGEX REPLACE "/Zm[0-9]*" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	if(NCINE_EXTRA_OPTIMIZATION)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /Qpar")
	endif()
	if(NCINE_AUTOVECTORIZATION_REPORT)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Qvec-report:2 /Qpar-report:2")
	endif()
	add_definitions(-DWITH_GLEW -D_CRT_SECURE_NO_DEPRECATE)
else() # GCC and LLVM
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")
	if(NCINE_DYNAMIC_LIBRARY)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
	endif()
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wold-style-cast -Wno-long-long -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-variadic-macros")
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1 -ggdb -fsanitize=address -fno-omit-frame-pointer -rdynamic")
			#set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fsanitize=address")
		endif()
		if(NCINE_EXTRA_OPTIMIZATION)
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -funsafe-loop-optimizations -ftree-loop-if-convert-stores")
		endif()
		if(NCINE_AUTOVECTORIZATION_REPORT)
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopt-info-vec-optimized")
		endif()
		if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9.0)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
			# Enabling Link Time Optimizations of GCC 4.9
			if(CMAKE_BUILD_TYPE MATCHES Release)
				set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
				set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -flto")
			endif()
		endif()
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Weverything -Wno-gnu-zero-variadic-macro-arguments")
		if(NCINE_EXTRA_OPTIMIZATION)
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -fslp-vectorize-aggressive")
		endif()
		if(NCINE_AUTOVECTORIZATION_REPORT)
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize")
		endif()
	endif()

	if(NOT CROSS_MINGW32) # Linux
		find_package(OpenGL REQUIRED)
		find_package(SDL)
		find_package(SDL_image)
		find_package(PNG)
		find_package(OpenAL REQUIRED)
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
		find_package(GLFW)
		find_package(WebP)
		find_package(Vorbis)
	else() # MinGW32 cross-compiler
		set(WIN32 1)
		set(CMAKE_SYSTEM_NAME Windows)
		set(CMAKE_SHARED_LIBRARY_PREFIX)
		set(CMAKE_SHARED_LIBRARY_SUFFIX .dll)
		set(CMAKE_EXECUTABLE_SUFFIX .exe)
		set(CMAKE_C_COMPILER i486-mingw32-gcc)
		set(CMAKE_CXX_COMPILER i486-mingw32-g++)
		set(CMAKE_RC_COMPILER i486-mingw32-windres)
		set(CMAKE_FIND_ROOT_PATH /usr/i486-mingw32)
		set(CMAKE_FIND_INCLUDE_PATH /usr/i486-mingw32)
		set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
		set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
		set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "") # suppressing '-fpic'
		set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # suppressing '-rdynamic'
		set(OPENGL_gl_LIBRARY opengl32 glew32)
		set(SDL_LIBRARY mingw32 SDLmain SDL)
		set(SDL_FOUND 1)
		set(SDLIMAGE_LIBRARY SDL_image)
		set(SDLIMAGE_FOUND 1)
		set(OPENAL_LIBRARY OpenAL32)
		set(OPENAL_FOUND 1)
		set(VORBISFILE_LIBRARY ogg vorbis vorbisfile)
		set(VORBIS_FOUND 1)
		add_definitions(-DWITH_GLEW)
	endif()
endif()

set(HEADERS
	include/common_headers.h
	include/common_defines.h
	include/common_functions.h
	include/Array.h
	include/ArrayIterator.h
	include/StaticArray.h
	include/List.h
	include/ListIterator.h
	include/ncString.h
	include/StringIterator.h
	include/HashMap.h
	include/HashFunctions.h
	include/HashMapIterator.h
	include/Point.h
	include/Rect.h
	include/Color.h
	include/Object.h
	include/DrawableNode.h
	include/IAppEventHandler.h
	include/IInputEventHandler.h
	include/IInputManager.h
	include/Vector2.h
	include/Vector3.h
	include/Vector4.h
	include/Matrix4x4.h
	include/Quaternion.h
	include/ServiceLocator.h
	include/DisplayMode.h
	include/Timer.h
	include/Font.h
	include/IFile.h
	include/IGfxDevice.h
	include/Texture.h
	include/SceneNode.h
	include/Sprite.h
	include/SpriteBatchNode.h
	include/Application.h
	include/AppConfiguration.h
	include/ParticleAffectors.h
	include/ParticleSystem.h
	include/TextNode.h
	include/RectAnimation.h
	include/AnimatedSprite.h
)

set(PRIVATE_HEADERS
	src/include/ArrayIndexer.h
	src/include/FrameTimer.h
	src/include/StandardFile.h
	src/include/FileLogger.h
	src/include/FontGlyph.h
	src/include/ProfileVariable.h
	src/include/PlottingVariable.h
	src/include/ProfilePlotter.h
	src/include/LinePlotter.h
	src/include/StackedBarPlotter.h
	src/include/RenderCommand.h
	src/include/RenderQueue.h
	src/include/Particle.h
	src/include/GfxCapabilities.h
	src/include/TextureFormat.h
	src/include/ITextureLoader.h
	src/include/TextureLoaderDds.h
	src/include/TextureLoaderPvr.h
	src/include/TextureLoaderKtx.h
	src/include/GLHashMap.h
	src/include/GLBufferObject.h
	src/include/GLBufferObject.h
	src/include/GLFramebufferObject.h
	src/include/GLRenderbuffer.h
	src/include/GLShader.h
	src/include/GLShaderProgram.h
	src/include/GLShaderUniforms.h
	src/include/GLUniform.h
	src/include/GLUniformCache.h
	src/include/GLShaderAttributes.h
	src/include/GLAttribute.h
	src/include/GLVertexAttribute.h
	src/include/GLTexture.h
)
set(SOURCES
	src/base/String.cpp
	src/ServiceLocator.cpp
	src/FileLogger.cpp
	src/ArrayIndexer.cpp
	src/Timer.cpp
	src/FrameTimer.cpp
	src/ProfileVariable.cpp
	src/Font.cpp
	src/FontGlyph.cpp
	src/IFile.cpp
	src/StandardFile.cpp
	src/graphics/IGfxDevice.cpp
	src/graphics/GfxCapabilities.cpp
	src/graphics/TextureFormat.cpp
	src/graphics/ITextureLoader.cpp
    src/graphics/TextureLoaderDds.cpp
    src/graphics/TextureLoaderPvr.cpp
    src/graphics/TextureLoaderKtx.cpp
	src/graphics/Texture.cpp
	src/graphics/ProfilePlotter.cpp
	src/graphics/PlottingVariable.cpp
	src/graphics/LinePlotter.cpp
	src/graphics/StackedBarPlotter.cpp
	src/graphics/DrawableNode.cpp
	src/graphics/SceneNode.cpp
	src/graphics/Sprite.cpp
	src/graphics/RenderCommand.cpp
	src/graphics/RenderQueue.cpp
	src/graphics/SpriteBatchNode.cpp
	src/Application.cpp
	src/AppConfiguration.cpp
	src/graphics/Particle.cpp
	src/graphics/ParticleAffectors.cpp
	src/graphics/ParticleSystem.cpp
	src/graphics/TextNode.cpp
	src/graphics/RectAnimation.cpp
	src/graphics/AnimatedSprite.cpp
	src/graphics/opengl/GLBufferObject.cpp
	src/graphics/opengl/GLFramebufferObject.cpp
	src/graphics/opengl/GLRenderbuffer.cpp
	src/graphics/opengl/GLShader.cpp
	src/graphics/opengl/GLShaderProgram.cpp
	src/graphics/opengl/GLShaderUniforms.cpp
	src/graphics/opengl/GLUniform.cpp
	src/graphics/opengl/GLUniformCache.cpp
	src/graphics/opengl/GLShaderAttributes.cpp
	src/graphics/opengl/GLAttribute.cpp
	src/graphics/opengl/GLVertexAttribute.cpp
	src/graphics/opengl/GLTexture.cpp
)

# Experimental depth test/alpha test renderer
#add_definitions(-DWITH_DEPTH_TEST)

# Forcing the use of SDL
#set(GLFW_FOUND 0)

if(NOT SDL_FOUND AND NOT GLFW_FOUND)
	message(FATAL_ERROR "Neither SDL nor GLFW have been found")
endif()

if(GLFW_FOUND)
	add_definitions(-DWITH_GLFW)
	add_definitions(-DGLFW_NO_GLU)
	list(APPEND LINK_LIBS ${GLFW_LIBRARY})

	list(APPEND HEADERS include/GlfwKeys.h)
	list(APPEND PRIVATE_HEADERS src/include/GlfwInputManager.h)
	list(APPEND PRIVATE_HEADERS src/include/GlfwGfxDevice.h)
    list(APPEND SOURCES src/GlfwInputManager.cpp)
    list(APPEND SOURCES src/graphics/GlfwGfxDevice.cpp)
endif()

if(SDL_FOUND AND NOT GLFW_FOUND)
	add_definitions(-DWITH_SDL)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # SDLmain on OS X
		list(APPEND LINK_LIBS ${SDLMAIN_LIBRARY})
	endif()
	list(APPEND LINK_LIBS ${SDL_LIBRARY})
	if(SDLIMAGE_FOUND)
		add_definitions(-DWITH_SDLIMAGE)
		list(APPEND LINK_LIBS ${SDLIMAGE_LIBRARY})
	endif()

	list(APPEND HEADERS include/SdlKeys.h)
	list(APPEND PRIVATE_HEADERS src/include/SdlInputManager.h)
	list(APPEND PRIVATE_HEADERS src/include/SdlGfxDevice.h)
    list(APPEND SOURCES src/SdlInputManager.cpp)
    list(APPEND SOURCES src/graphics/SdlGfxDevice.cpp)
	if(SDLIMAGE_FOUND)
		list(APPEND PRIVATE_HEADERS src/include/TextureLoaderSdl.h)
        list(APPEND SOURCES src/graphics/TextureLoaderSdl.cpp)
	endif()
endif()

if(OPENAL_FOUND)
	add_definitions(-DWITH_AUDIO)
	list(APPEND LINK_LIBS ${OPENAL_LIBRARY})
	if(VORBIS_FOUND)
		add_definitions(-DWITH_VORBIS)
		list(APPEND LINK_LIBS ${VORBISFILE_LIBRARY})
	endif()

	list(APPEND HEADERS
		include/IAudioDevice.h
		include/IAudioLoader.h
		include/AudioBuffer.h
		include/AudioStream.h
		include/IAudioPlayer.h
		include/AudioBufferPlayer.h
		include/AudioStreamPlayer.h
	)

	list(APPEND PRIVATE_HEADERS
		src/include/ALAudioDevice.h
		src/include/AudioLoaderWav.h
	)

	list(APPEND SOURCES
		src/audio/ALAudioDevice.cpp
		src/audio/IAudioLoader.cpp
		src/audio/AudioLoaderWav.cpp
		src/audio/AudioBuffer.cpp
		src/audio/AudioStream.cpp
		src/audio/AudioBufferPlayer.cpp
		src/audio/AudioStreamPlayer.cpp
	)

	if(VORBIS_FOUND)
		list(APPEND PRIVATE_HEADERS src/include/AudioLoaderOgg.h)
		list(APPEND SOURCES src/audio/AudioLoaderOgg.cpp)
	endif()
endif()

if(PNG_FOUND AND GLFW_FOUND)
	add_definitions(-DWITH_PNG)
	list(APPEND LINK_LIBS ${PNG_LIBRARY})

	list(APPEND PRIVATE_HEADERS src/include/TextureLoaderPng.h)
    list(APPEND SOURCES src/graphics/TextureLoaderPng.cpp)
endif()
if(WEBP_FOUND)
	add_definitions(-DWITH_WEBP)
	list(APPEND LINK_LIBS ${WEBP_LIBRARY})

	list(APPEND PRIVATE_HEADERS src/include/TextureLoaderWebP.h)
	list(APPEND SOURCES src/graphics/TextureLoaderWebP.cpp)
endif()

if(Threads_FOUND)
	add_definitions(-DWITH_THREADS)
	list(APPEND LINK_LIBS ${CMAKE_THREAD_LIBS_INIT})
	# Experimental multi-threading scene update
	#add_definitions(-DWITH_MULTITHREADING)

	list(APPEND HEADERS include/IThreadPool.h)
	list(APPEND HEADERS include/IThreadCommand.h)
	list(APPEND PRIVATE_HEADERS src/include/Thread.h)
	list(APPEND PRIVATE_HEADERS src/include/ThreadSync.h)

	if(MSVC OR CROSS_MINGW32)
		list(APPEND SOURCES src/threading/WindowsThread.cpp)
		list(APPEND SOURCES src/threading/WindowsThreadSync.cpp)
	else()
		list(APPEND SOURCES src/threading/PosixThread.cpp)
		list(APPEND SOURCES src/threading/PosixThreadSync.cpp)
	endif()

	list(APPEND PRIVATE_HEADERS src/include/ThreadPool.h)
	list(APPEND SOURCES src/threading/ThreadPool.cpp)
	list(APPEND PRIVATE_HEADERS src/include/ThreadCommands.h)
endif()

link_libraries(
	${OPENGL_gl_LIBRARY}
	${LINK_LIBS}
)

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS})
else()
	add_library(ncine STATIC ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS})
endif()

if(MSVC)
	# Disabling incremental linking and manifest generation
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /MANIFEST:NO /INCREMENTAL:NO")
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /MANIFEST:NO /INCREMENTAL:NO")

	if(NCINE_DYNAMIC_LIBRARY)
		# Disabling incremental linking and manifest generation
		set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /MANIFEST:NO /INCREMENTAL:NO")
		set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /MANIFEST:NO /INCREMENTAL:NO")

		# Disable linker warning about teamplates
		add_definitions("/wd4251")

		add_custom_target(copy_ncine_dll ALL
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/ncine.dll ${CMAKE_BINARY_DIR}/tests
			DEPENDS ncine
			COMMENT "Copying nCine DLL..."
		)
	else()
		add_definitions(-DNCINE_STATIC)
	endif()

	target_include_directories(ncine PUBLIC "${EXTERNAL_MSVC_DIR}/include")
else()
	if(PNG_FOUND AND GLFW_FOUND)
		target_include_directories(ncine PRIVATE ${PNG_INCLUDE_DIR})
	endif()

	if(CROSS_MINGW32)
		set_target_properties(ncine PROPERTIES LINK_FLAGS "-Wl,--output-def,${CMAKE_CURRENT_BINARY_DIR}/ncine.def")
	endif()
endif()
target_include_directories(ncine PUBLIC include PRIVATE src/include)

if(NCINE_BUILD_TESTS)
	set(NCINE_DATADIR ${CMAKE_SOURCE_DIR}/../nCine-data)

	add_subdirectory(tests)
	if(NOT NCINE_DYNAMIC_LIBRARY)
		add_subdirectory(src/tests)
	endif()
endif()
