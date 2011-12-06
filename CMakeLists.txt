cmake_minimum_required(VERSION 2.6)
project(nCine)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
include_directories(include)
add_definitions(-DNO_GLEW)

set(SOURCES
	include/ncArray.h
	include/ncList.h
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
)

link_libraries (
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${OPENGL_gl_LIBRARY}
)

add_library(ncine SHARED ${SOURCES})

add_executable(test_font tests/test_font.cpp)
add_executable(apptest_particles tests/main.cpp tests/apptest_particles.cpp)
add_executable(test_particles tests/test_particles.cpp)
add_executable(test_renderqueue tests/test_renderqueue.cpp)
add_executable(test_radixsort tests/test_radixsort.cpp)
add_executable(apptest_scene tests/main.cpp tests/apptest_scene.cpp)
add_executable(test_scene tests/test_scene.cpp)
add_executable(test_array tests/test_array.cpp)
add_executable(test_list tests/test_list.cpp)

target_link_libraries(test_font ncine)
target_link_libraries(apptest_particles ncine)
target_link_libraries(test_particles ncine)
target_link_libraries(test_renderqueue ncine)
target_link_libraries(apptest_scene ncine)
target_link_libraries(test_scene ncine)
target_link_libraries(test_array ncine)
target_link_libraries(test_list ncine)
