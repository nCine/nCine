cmake_minimum_required(VERSION 2.6)
project(nCine)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
include_directories(src src/base src/graphics)
add_definitions(-DNO_GLEW)

set(SOURCES
	src/base/ncArray.h
	src/base/ncList.h
	src/base/ncPoint.h
	src/base/ncRect.h
	src/base/ncVector2f.cpp
	src/ncServiceLocator.cpp
	src/ncFileLogger.cpp
	src/ncTimer.cpp
	src/ncFrameTimer.cpp
	src/graphics/ncDisplayMode.h
	src/graphics/ncGfxDevice.cpp
	src/graphics/ncTextureFormat.cpp
	src/graphics/ncTexture.cpp
	src/graphics/ncSpriteBatch.cpp
)

link_libraries (
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${OPENGL_gl_LIBRARY}
)

add_executable(test_spritebatch ${SOURCES} tests/test_spritebatch.cpp)
add_executable(test_array ${SOURCES} tests/test_array.cpp)
add_executable(test_list ${SOURCES} tests/test_list.cpp)
