cmake_minimum_required(VERSION 2.6)
project(nCine)

find_package(Threads)
if(MSVC)
	include_directories("${CMAKE_BINARY_DIR}/win32/include")
	set(LIBDIR "${CMAKE_BINARY_DIR}/win32/lib")
	set(OPENGL_gl_LIBRARY opengl32.lib ${LIBDIR}/glew32.lib)
	#set(GLFW_LIBRARY ${LIBDIR}/GLFW.lib)
	#set(GLFW_FOUND 1)
	#set(PNG_LIBRARY ${LIBDIR}/libpng.lib)
	#set(PNG_FOUND 1)
	set(SDL_LIBRARY ${LIBDIR}/SDLmain.lib ${LIBDIR}/SDL.lib)
	set(SDL_FOUND 1)
	set(SDLIMAGE_LIBRARY ${LIBDIR}/SDL_image.lib)
	set(SDLIMAGE_FOUND 1)
	set(OPENAL_LIBRARY ${LIBDIR}/OpenAL32.lib)
	set(VORBISFILE_LIBRARY ${LIBDIR}/libogg.lib ${LIBDIR}/libvorbis.lib ${LIBDIR}/libvorbisfile.lib)
	set(VORBIS_FOUND 1)
	add_definitions(-DWITH_GLEW)
	add_definitions(/W0) # Turn-off warnings
else() # GCC and LLVM
	set(CMAKE_CXX_FLAGS "-Wall")
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
		set(VORBISFILE_LIBRARY ogg vorbis vorbisfile)
		set(VORBIS_FOUND 1)
		add_definitions(-DWITH_GLEW)
	endif()
endif()

# Experimental depth test/alpha test renderer
#add_definitions(-DWITH_DEPTH_TEST)

# Forcing the use of SDL
set(GLFW_FOUND 0)

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
	list(APPEND LINK_LIBS ${SDL_LIBRARY})
	if(SDLIMAGE_FOUND)
		add_definitions(-DWITH_SDLIMAGE)
		list(APPEND LINK_LIBS ${SDLIMAGE_LIBRARY})
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
if(VORBIS_FOUND)
	add_definitions(-DWITH_VORBIS)
	list(APPEND LINK_LIBS ${VORBISFILE_LIBRARY})
endif()
if(Threads_FOUND)
	add_definitions(-DWITH_THREADS)
	list(APPEND LINK_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

include_directories(include)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES
	include/ncArray.h
	include/ncList.h
	include/ncListIterator.h
	include/ncPoint.h
	include/ncRect.h
	src/base/ncVector2f.cpp
	src/ncServiceLocator.cpp
	src/ncFileLogger.cpp
	src/ncArrayIndexer.cpp
	src/ncTimer.cpp
	src/ncFrameTimer.cpp
	src/ncProfileVariable.cpp
	src/ncFont.cpp
	src/ncFontGlyph.cpp
	src/ncIFile.cpp
	src/ncStandardFile.cpp
	src/graphics/ncIGfxDevice.cpp
	src/graphics/ncGfxCapabilities.cpp
	src/graphics/ncTextureFormat.cpp
	src/graphics/ncITextureLoader.cpp
	src/graphics/ncTextureLoaderDDS.cpp
	src/graphics/ncTextureLoaderPVR.cpp
	src/graphics/ncTexture.cpp
	src/graphics/ncProfilePlotter.cpp
	src/graphics/ncPlottingVariable.cpp
	src/graphics/ncLinePlotter.cpp
	src/graphics/ncStackedBarPlotter.cpp
	src/graphics/ncSceneNode.cpp
	src/graphics/ncSprite.cpp
	src/graphics/ncRenderCommand.cpp
	src/graphics/ncRenderQueue.cpp
	src/graphics/ncSpriteBatchNode.cpp
	src/ncApplication.cpp
	src/graphics/ncParticle.cpp
	src/graphics/ncParticleAffectors.cpp
	src/graphics/ncParticleSystem.cpp
	src/graphics/ncTextNode.cpp
	src/graphics/ncRectAnimation.cpp
	src/graphics/ncAnimatedSprite.cpp
	src/audio/ncALAudioDevice.cpp
	src/audio/ncIAudioLoader.cpp
	src/audio/ncAudioLoaderWav.cpp
	src/audio/ncAudioBuffer.cpp
	src/audio/ncAudioStream.cpp
	src/audio/ncAudioBufferPlayer.cpp
	src/audio/ncAudioStreamPlayer.cpp
)

if(GLFW_FOUND)
	list(APPEND SOURCES src/ncGLFWInputManager.cpp)
	list(APPEND SOURCES src/graphics/ncGLFWGfxDevice.cpp)
endif()

if(SDL_FOUND AND NOT GLFW_FOUND)
	list(APPEND SOURCES src/ncSDLInputManager.cpp)
	list(APPEND SOURCES src/graphics/ncSDLGfxDevice.cpp)
	if(SDLIMAGE_FOUND)
		list(APPEND SOURCES src/graphics/ncTextureLoaderSDL.cpp)
	endif()
endif()

if(PNG_FOUND AND GLFW_FOUND)
	list(APPEND SOURCES src/graphics/ncTextureLoaderPNG.cpp)
endif()
if(WEBP_FOUND)
	list(APPEND SOURCES src/graphics/ncTextureLoaderWebP.cpp)
endif()
if(VORBIS_FOUND)
	list(APPEND SOURCES src/audio/ncAudioLoaderOgg.cpp)
endif()

if(Threads_FOUND)
	if(MSVC OR CROSS_MINGW32)
		list(APPEND SOURCES src/threading/ncWindowsThread.cpp)
		list(APPEND SOURCES src/threading/ncWindowsThreadSync.cpp)
	else()
		list(APPEND SOURCES src/threading/ncPOSIXThread.cpp)
		list(APPEND SOURCES src/threading/ncPOSIXThreadSync.cpp)
	endif()
	list(APPEND SOURCES src/threading/ncThreadPool.cpp)
endif()

list(APPEND LINK_LIBS ${OPENGL_gl_LIBRARY})

link_libraries(
	${OPENGL_gl_LIBRARY}
	${OPENAL_LIBRARY}
	${LINK_LIBS}
)

if(MSVC)
	add_Library(ncine STATIC ${SOURCES})
else()
	add_library(ncine SHARED ${SOURCES})
	if(CROSS_MINGW32)
		set_target_properties(ncine PROPERTIES LINK_FLAGS "-Wl,--output-def,${CMAKE_CURRENT_BINARY_DIR}/ncine.def")
	endif()
endif()

add_executable(apptest_joystick tests/main.cpp tests/apptest_joystick.cpp)
add_executable(apptest_overdraw tests/main.cpp tests/apptest_overdraw.cpp)
add_executable(apptest_rotozoom tests/main.cpp tests/apptest_rotozoom.cpp)
add_executable(apptest_animsprites tests/main.cpp tests/apptest_animsprites.cpp)
add_executable(apptest_particles tests/main.cpp tests/apptest_particles.cpp)
#add_executable(test_radixsort tests/test_radixsort.cpp)
add_executable(apptest_scene tests/main.cpp tests/apptest_scene.cpp)
add_executable(test_array tests/test_array.cpp)
add_executable(test_list tests/test_list.cpp)

target_link_libraries(apptest_joystick ncine)
target_link_libraries(apptest_overdraw ncine)
target_link_libraries(apptest_rotozoom ncine)
target_link_libraries(apptest_animsprites ncine)
target_link_libraries(apptest_particles ncine)
target_link_libraries(apptest_scene ncine)
target_link_libraries(test_array ncine)
target_link_libraries(test_list ncine)

if(SDL_FOUND AND NOT GLFW_FOUND)
	add_executable(test_audio tests/test_audio.cpp)
	add_executable(test_font tests/test_font.cpp)
	add_executable(test_particles tests/test_particles.cpp)
	add_executable(test_renderqueue tests/test_renderqueue.cpp)
	add_executable(test_scene tests/test_scene.cpp)

	target_link_libraries(test_audio ncine)
	target_link_libraries(test_font ncine)
	target_link_libraries(test_particles ncine)
	target_link_libraries(test_renderqueue ncine)
	target_link_libraries(test_scene ncine)
endif()

if(Threads_FOUND)
	add_executable(apptest_threads tests/main.cpp tests/apptest_threads.cpp)
	add_executable(apptest_threadpool tests/main.cpp tests/apptest_threadpool.cpp)

	target_link_libraries(apptest_threads ncine)
	target_link_libraries(apptest_threadpool ncine)
endif()
