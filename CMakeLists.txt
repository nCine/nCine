cmake_minimum_required(VERSION 2.6)
project(nCine)

if(NOT CROSS_MINGW32)
    find_package(OpenGL REQUIRED)
    find_package(SDL REQUIRED)
    find_package(SDL_image REQUIRED)
    find_package(OpenAL REQUIRED)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
    find_package(Vorbis REQUIRED)
    add_definitions(-DNO_GLEW)
else()
    SET(CMAKE_SYSTEM_NAME Windows)
    SET(CMAKE_SHARED_LIBRARY_PREFIX)
    SET(CMAKE_SHARED_LIBRARY_SUFFIX .dll)
    SET(CMAKE_EXECUTABLE_SUFFIX .exe)
    SET(CMAKE_C_COMPILER i486-mingw32-gcc)
    SET(CMAKE_CXX_COMPILER i486-mingw32-g++)
    SET(CMAKE_RC_COMPILER i486-mingw32-windres)
    SET(CMAKE_FIND_ROOT_PATH /usr/i486-mingw32)
    SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    SET(CMAKE_SHARED_LIBRARY_CXX_FLAGS "") # suppressing '-fpic'
    SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # suppressing '-rdynamic'
endif()

include_directories(include)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS "-Wall")

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
	src/ncSDLInputManager.cpp
	src/graphics/ncIGfxDevice.cpp
	src/graphics/ncSDLGfxDevice.cpp
	src/graphics/ncTextureFormat.cpp
	src/graphics/ncTextureLoader.cpp
	src/graphics/ncTexture.cpp
	src/graphics/ncProfilePlotter.cpp
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
	src/audio/ncAudioLoaderOgg.cpp
	src/audio/ncAudioBuffer.cpp
	src/audio/ncAudioStream.cpp
	src/audio/ncAudioBufferPlayer.cpp
	src/audio/ncAudioStreamPlayer.cpp
)

if(NOT CROSS_MINGW32)
    link_libraries (
        ${SDL_LIBRARY}
        ${SDLIMAGE_LIBRARY}
        ${OPENGL_gl_LIBRARY}
        ${OPENAL_LIBRARY}
        ${VORBISFILE_LIBRARY}
    )
else()
    link_libraries (
        mingw32
        SDLmain
        SDL
        SDL_image
        glew32
        opengl32
        OpenAL32
        ogg
        vorbis
        vorbisfile
    )
endif()

add_library(ncine SHARED ${SOURCES})

add_executable(test_audio tests/test_audio.cpp)
add_executable(apptest_animsprites tests/main.cpp tests/apptest_animsprites.cpp)
add_executable(test_font tests/test_font.cpp)
add_executable(apptest_particles tests/main.cpp tests/apptest_particles.cpp)
add_executable(test_particles tests/test_particles.cpp)
add_executable(test_renderqueue tests/test_renderqueue.cpp)
add_executable(test_radixsort tests/test_radixsort.cpp)
add_executable(apptest_scene tests/main.cpp tests/apptest_scene.cpp)
add_executable(test_scene tests/test_scene.cpp)
add_executable(test_array tests/test_array.cpp)
add_executable(test_list tests/test_list.cpp)

target_link_libraries(test_audio ncine)
target_link_libraries(apptest_animsprites ncine)
target_link_libraries(test_font ncine)
target_link_libraries(apptest_particles ncine)
target_link_libraries(test_particles ncine)
target_link_libraries(test_renderqueue ncine)
target_link_libraries(apptest_scene ncine)
target_link_libraries(test_scene ncine)
target_link_libraries(test_array ncine)
target_link_libraries(test_list ncine)
