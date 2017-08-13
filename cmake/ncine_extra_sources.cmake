if(GLEW_FOUND)
	if (NOT WIN32)
		message(STATUS "GLEW has been found")
	endif()
	list(APPEND PRIVATE_LINK_LIBRARIES ${GLEW_LIBRARY})
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_GLEW")
endif()

if(GLFW_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_GLFW" "GLFW_NO_GLU")
	list(APPEND PRIVATE_LINK_LIBRARIES ${GLFW_LIBRARY})

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/GlfwInputManager.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/GlfwGfxDevice.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/GlfwInputManager.cpp)
	list(APPEND SOURCES ${NCINE_ROOT}/src/GlfwKeys.cpp)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/GlfwGfxDevice.cpp)
endif()

if(SDL2_FOUND AND NOT GLFW_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_SDL")
	if(${CMAKE_SYSTEM_NAME} MATCHES Darwin) # SDLmain on OS X
		list(APPEND PRIVATE_LINK_LIBRARIES ${SDL2MAIN_LIBRARY})
	endif()
	list(APPEND PRIVATE_LINK_LIBRARIES ${SDL2_LIBRARY})

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/SdlInputManager.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/SdlGfxDevice.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/SdlInputManager.cpp)
	list(APPEND SOURCES ${NCINE_ROOT}/src/SdlKeys.cpp)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/SdlGfxDevice.cpp)
endif()

if(OPENAL_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_AUDIO")
	list(APPEND PRIVATE_LINK_LIBRARIES ${OPENAL_LIBRARY})
	if(VORBIS_FOUND)
		list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_VORBIS")
		list(APPEND PRIVATE_LINK_LIBRARIES ${VORBISFILE_LIBRARY})
	endif()

	list(APPEND HEADERS
		${NCINE_ROOT}/include/IAudioDevice.h
		${NCINE_ROOT}/include/IAudioLoader.h
		${NCINE_ROOT}/include/AudioBuffer.h
		${NCINE_ROOT}/include/AudioStream.h
		${NCINE_ROOT}/include/IAudioPlayer.h
		${NCINE_ROOT}/include/AudioBufferPlayer.h
		${NCINE_ROOT}/include/AudioStreamPlayer.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/ALAudioDevice.h
		${NCINE_ROOT}/src/include/AudioLoaderWav.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/audio/ALAudioDevice.cpp
		${NCINE_ROOT}/src/audio/IAudioLoader.cpp
		${NCINE_ROOT}/src/audio/AudioLoaderWav.cpp
		${NCINE_ROOT}/src/audio/AudioBuffer.cpp
		${NCINE_ROOT}/src/audio/AudioStream.cpp
		${NCINE_ROOT}/src/audio/IAudioPlayer.cpp
		${NCINE_ROOT}/src/audio/AudioBufferPlayer.cpp
		${NCINE_ROOT}/src/audio/AudioStreamPlayer.cpp
	)

	if(VORBIS_FOUND)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/AudioLoaderOgg.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/audio/AudioLoaderOgg.cpp)
	endif()
endif()

if(PNG_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_PNG")
	list(APPEND PRIVATE_LINK_LIBRARIES ${PNG_LIBRARY})

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/TextureLoaderPng.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/TextureLoaderPng.cpp)
endif()
if(WEBP_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_WEBP")
	list(APPEND PRIVATE_LINK_LIBRARIES ${WEBP_LIBRARY})

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/TextureLoaderWebP.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/TextureLoaderWebP.cpp)
endif()

if(Threads_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_THREADS")
	list(APPEND PRIVATE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
	# Experimental multi-threading scene update
	#list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_MULTITHREADING")

	list(APPEND HEADERS ${NCINE_ROOT}/include/IThreadPool.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/IThreadCommand.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/Thread.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ThreadSync.h)

	if(WIN32)
		list(APPEND SOURCES ${NCINE_ROOT}/src/threading/WindowsThread.cpp)
		list(APPEND SOURCES ${NCINE_ROOT}/src/threading/WindowsThreadSync.cpp)
	else()
		list(APPEND SOURCES ${NCINE_ROOT}/src/threading/PosixThread.cpp)
		list(APPEND SOURCES ${NCINE_ROOT}/src/threading/PosixThreadSync.cpp)
	endif()

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ThreadPool.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/threading/ThreadPool.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ThreadCommands.h)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
	list(APPEND HEADERS
		${NCINE_ROOT}/include/AndroidApplication.h
		${NCINE_ROOT}/include/AssetFile.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/AndroidInputManager.h
		${NCINE_ROOT}/src/include/AndroidJniHelper.h
		${NCINE_ROOT}/src/include/EglGfxDevice.h
		${NCINE_ROOT}/src/include/TextureLoaderPkm.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/android/AndroidApplication.cpp
		${NCINE_ROOT}/src/android/AndroidInputManager.cpp
		${NCINE_ROOT}/src/android/AndroidJniHelper.cpp
		${NCINE_ROOT}/src/android/AndroidKeys.cpp
		${NCINE_ROOT}/src/android/AssetFile.cpp
		${NCINE_ROOT}/src/android/EglGfxDevice.cpp
		${NCINE_ROOT}/src/android/TextureLoaderPkm.cpp
	)
else()
	list(APPEND HEADERS ${NCINE_ROOT}/include/PCApplication.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/PCApplication.cpp)
endif()
