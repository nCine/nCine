cmake_minimum_required(VERSION 2.6)
project(encine2d)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
include_directories(src src/base src/graphics)
add_definitions(-DNO_GLEW)
set(SOURCES
	src/base/Point.h
	src/base/Size.h
	src/base/Rect.h
	src/base/Vector.cpp
	src/ServiceLocator.cpp
	src/FileLogger.cpp
	src/Timer.cpp
	src/FrameTimer.cpp
	src/graphics/DisplayMode.h
	src/graphics/GfxDevice.cpp
	src/graphics/TextureFormat.cpp
	src/graphics/Texture.cpp
	test.cpp
)
link_libraries (
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${OPENGL_gl_LIBRARY}
)
add_executable(test ${SOURCES})
