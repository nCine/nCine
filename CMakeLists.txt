cmake_minimum_required(VERSION 2.6)
project(nCine)

option(NCINE_BUILD_TESTS "Build the nCine test programs" ON)

find_package(Threads)
if(MSVC)
	set(EXTERNAL_MSVC_DIR "${CMAKE_SOURCE_DIR}/external")
	include_directories("${EXTERNAL_MSVC_DIR}/include")
	if(MSVC_C_ARCHITECTURE_ID MATCHES 64 OR MSVC_CXX_ARCHITECTURE_ID MATCHES 64)
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x64")
	else()
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x86")
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
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-DWITH_GLEW)
	#add_definitions(/W0) # Turn off warnings
else() # GCC and LLVM
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wold-style-cast -Wno-long-long -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-variadic-macros")
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1 -ggdb -fsanitize=address -fno-omit-frame-pointer -rdynamic")
			#set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fsanitize=address")
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

# Experimental depth test/alpha test renderer
#add_definitions(-DWITH_DEPTH_TEST)

# Forcing the use of SDL
#set(GLFW_FOUND 0)

if(NOT SDL_FOUND AND NOT GLFW_FOUND)
	message(FATAL_ERROR "Neither SDL nor GLFW have been found!")
endif()

if(GLFW_FOUND)
	add_definitions(-DWITH_GLFW)
	add_definitions(-DGLFW_NO_GLU)
	list(APPEND LINK_LIBS ${GLFW_LIBRARY})
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
endif()

if(OPENAL_FOUND)
	add_definitions(-DWITH_AUDIO)
	list(APPEND LINK_LIBS ${OPENAL_LIBRARY})
	if(VORBIS_FOUND)
		add_definitions(-DWITH_VORBIS)
		list(APPEND LINK_LIBS ${VORBISFILE_LIBRARY})
	endif()
endif()

if(PNG_FOUND AND GLFW_FOUND)
	add_definitions(-DWITH_PNG)
	include_directories(${PNG_INCLUDE_DIR})
	list(APPEND LINK_LIBS ${PNG_LIBRARY})
endif()
if(WEBP_FOUND)
	add_definitions(-DWITH_WEBP)
	list(APPEND LINK_LIBS ${WEBP_LIBRARY})
endif()

if(Threads_FOUND)
	add_definitions(-DWITH_THREADS)
	list(APPEND LINK_LIBS ${CMAKE_THREAD_LIBS_INIT})
	# Experimental multi-threading scene update
	#add_definitions(-DWITH_MULTITHREADING)
endif()

include_directories(include)
set(HEADERS
	include/Array.h
	include/List.h
	include/ListIterator.h
	include/Point.h
	include/Rect.h
	include/Color.h
	include/Object.h
	include/DrawableNode.h
	include/IAppEventHandler.h
	include/IInputEventHandler.h
	include/Vector2f.h
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
	include/ParticleAffectors.h
	include/ParticleSystem.h
	include/TextNode.h
	include/RectAnimation.h
	include/AnimatedSprite.h
)
include_directories(src/include)
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
)
set(SOURCES
	src/base/Vector2f.cpp
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
	src/graphics/Particle.cpp
	src/graphics/ParticleAffectors.cpp
	src/graphics/ParticleSystem.cpp
	src/graphics/TextNode.cpp
	src/graphics/RectAnimation.cpp
	src/graphics/AnimatedSprite.cpp
)

if(GLFW_FOUND)
	list(APPEND HEADERS include/GlfwKeys.h)
	list(APPEND PRIVATE_HEADERS src/include/GlfwInputManager.h)
	list(APPEND PRIVATE_HEADERS src/include/GlfwGfxDevice.h)
    list(APPEND SOURCES src/GlfwInputManager.cpp)
    list(APPEND SOURCES src/graphics/GlfwGfxDevice.cpp)
endif()

if(SDL_FOUND AND NOT GLFW_FOUND)
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
	list(APPEND PRIVATE_HEADERS src/include/TextureLoaderPng.h)
    list(APPEND SOURCES src/graphics/TextureLoaderPng.cpp)
endif()
if(WEBP_FOUND)
	list(APPEND PRIVATE_HEADERS src/include/TextureLoaderWebP.h)
	list(APPEND SOURCES src/graphics/TextureLoaderWebP.cpp)
endif()

if(Threads_FOUND)
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

if(MSVC)
	add_Library(ncine STATIC ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS})
else()
	add_library(ncine SHARED ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS})
	if(CROSS_MINGW32)
		set_target_properties(ncine PROPERTIES LINK_FLAGS "-Wl,--output-def,${CMAKE_CURRENT_BINARY_DIR}/ncine.def")
	endif()
endif()

if(NCINE_BUILD_TESTS)
	add_subdirectory(tests)
endif()
