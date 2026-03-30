list(APPEND HEADERS
	${NCINE_ROOT}/include/ncine/common_defines.h
	${NCINE_ROOT}/include/ncine/common_constants.h
	${NCINE_ROOT}/include/ncine/common_macros.h
	${NCINE_ROOT}/include/ncine/tracy.h
	${NCINE_ROOT}/include/ncine/tracy_opengl.h
	${NCINE_ROOT}/include/ncine/Random.h
	${NCINE_ROOT}/include/ncine/Hash64.h
	${NCINE_ROOT}/include/ncine/Rect.h
	${NCINE_ROOT}/include/ncine/Color.h
	${NCINE_ROOT}/include/ncine/Colorf.h
	${NCINE_ROOT}/include/ncine/ColorHdr.h
	${NCINE_ROOT}/include/ncine/IAppEventHandler.h
	${NCINE_ROOT}/include/ncine/IInputEventHandler.h
	${NCINE_ROOT}/include/ncine/InputEvents.h
	${NCINE_ROOT}/include/ncine/IInputManager.h
	${NCINE_ROOT}/include/ncine/Keys.h
	${NCINE_ROOT}/include/ncine/Vector2.h
	${NCINE_ROOT}/include/ncine/Vector3.h
	${NCINE_ROOT}/include/ncine/Vector4.h
	${NCINE_ROOT}/include/ncine/Matrix4x4.h
	${NCINE_ROOT}/include/ncine/Quaternion.h
	${NCINE_ROOT}/include/ncine/IIndexer.h
	${NCINE_ROOT}/include/ncine/ILogger.h
	${NCINE_ROOT}/include/ncine/IAudioDevice.h
	${NCINE_ROOT}/include/ncine/IJobSystem.h
	${NCINE_ROOT}/include/ncine/IGfxCapabilities.h
	${NCINE_ROOT}/include/ncine/ServiceLocator.h
	${NCINE_ROOT}/include/ncine/DisplayMode.h
	${NCINE_ROOT}/include/ncine/TimeStamp.h
	${NCINE_ROOT}/include/ncine/Timer.h
	${NCINE_ROOT}/include/ncine/IFrameTimer.h
	${NCINE_ROOT}/include/ncine/FileSystem.h
	${NCINE_ROOT}/include/ncine/IFile.h
	${NCINE_ROOT}/include/ncine/IGfxDevice.h
	${NCINE_ROOT}/include/ncine/IImageSaver.h
	${NCINE_ROOT}/include/ncine/Application.h
	${NCINE_ROOT}/include/ncine/PCApplication.h
	${NCINE_ROOT}/include/ncine/AppConfiguration.h
	${NCINE_ROOT}/include/ncine/IDebugOverlay.h
)

list(APPEND PRIVATE_HEADERS
	${NCINE_ROOT}/src/include/common_headers.h
	${NCINE_ROOT}/src/include/return_macros.h
	${NCINE_ROOT}/src/include/Clock.h
	${NCINE_ROOT}/src/include/FrameTimer.h
	${NCINE_ROOT}/src/include/MemoryFile.h
	${NCINE_ROOT}/src/include/StandardFile.h
	${NCINE_ROOT}/src/include/FileLogger.h
	${NCINE_ROOT}/src/include/JoyMapping.h
	${NCINE_ROOT}/src/include/IImageLoader.h
)

list(APPEND SOURCES
	${NCINE_ROOT}/src/base/Random.cpp
	${NCINE_ROOT}/src/base/Hash64.cpp
	${NCINE_ROOT}/src/base/Object.cpp
	${NCINE_ROOT}/src/base/HashFunctions.cpp
	${NCINE_ROOT}/src/base/CString.cpp
	${NCINE_ROOT}/src/base/Utf8.cpp
	${NCINE_ROOT}/src/base/String.cpp
	${NCINE_ROOT}/src/base/StringView.cpp
	${NCINE_ROOT}/src/base/Clock.cpp
	${NCINE_ROOT}/src/ServiceLocator.cpp
	${NCINE_ROOT}/src/FileLogger.cpp
	${NCINE_ROOT}/src/TimeStamp.cpp
	${NCINE_ROOT}/src/Timer.cpp
	${NCINE_ROOT}/src/FrameTimer.cpp
	${NCINE_ROOT}/src/FileSystem.cpp
	${NCINE_ROOT}/src/IFile.cpp
	${NCINE_ROOT}/src/MemoryFile.cpp
	${NCINE_ROOT}/src/StandardFile.cpp
	${NCINE_ROOT}/src/input/IInputManager.cpp
	${NCINE_ROOT}/src/input/JoyMappingDb.h
	${NCINE_ROOT}/src/input/JoyMapping.cpp
	${NCINE_ROOT}/src/threading/IJobSystem.cpp
	${NCINE_ROOT}/src/graphics/Color.cpp
	${NCINE_ROOT}/src/graphics/Colorf.cpp
	${NCINE_ROOT}/src/graphics/ColorHdr.cpp
	${NCINE_ROOT}/src/graphics/IGfxDevice.cpp
	${NCINE_ROOT}/src/graphics/IImageLoader.cpp
	${NCINE_ROOT}/src/graphics/IImageSaver.cpp
	${NCINE_ROOT}/src/Application.cpp
	${NCINE_ROOT}/src/AppConfiguration.cpp
)

if(NCINE_WITH_AUDIO OR NCINE_WITH_SCENEGRAPH)
	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/Object.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ArrayIndexer.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/ArrayIndexer.cpp)
endif()
