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

if(LUA_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_LUA")
	list(APPEND PRIVATE_LINK_LIBRARIES ${LUA_LIBRARY})

	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaTypes.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaStateManager.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaStateManager.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaClassTracker.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaClassWrapper.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaUtils.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaUtils.cpp)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaDebug.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaDebug.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaStatistics.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaStatistics.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaNames.h)

	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaIAppEventHandler.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaIAppEventHandler.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaILogger.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaILogger.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaRect.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaRectUtils.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaVector2.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaVector2Utils.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaVector3.h)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaVector3Utils.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaColor.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaColor.cpp)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaColorUtils.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaColorUtils.cpp)

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaIInputManager.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaIInputManager.cpp)
	list(APPEND HEADERS ${NCINE_ROOT}/include/LuaIInputEventHandler.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaIInputEventHandler.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaMouseEvents.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaMouseEvents.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaKeys.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaKeys.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaKeyboardEvents.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaKeyboardEvents.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaJoystickEvents.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaJoystickEvents.cpp)

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaApplication.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaApplication.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAppConfiguration.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAppConfiguration.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaSceneNode.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaSceneNode.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaDrawableNode.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaDrawableNode.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaTexture.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaTexture.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaBaseSprite.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaBaseSprite.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaSprite.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaSprite.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaMeshSprite.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaMeshSprite.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaRectAnimation.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaRectAnimation.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAnimatedSprite.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAnimatedSprite.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaFont.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaFont.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaTextNode.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaTextNode.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaParticleSystem.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaParticleSystem.cpp)

	if(OPENAL_FOUND)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaIAudioPlayer.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaIAudioPlayer.cpp)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAudioStreamPlayer.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAudioStreamPlayer.cpp)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAudioBuffer.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAudioBuffer.cpp)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAudioBufferPlayer.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAudioBufferPlayer.cpp)
	endif()

	if(CMAKE_SYSTEM_NAME STREQUAL "Android")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAndroidEvents.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAndroidEvents.cpp)
	else()
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaEventHandler.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaEventHandler.cpp)
	endif()
endif()

if(NCINE_BUILD_ANDROID)
	list(APPEND HEADERS
		${NCINE_ROOT}/include/AndroidApplication.h
		${NCINE_ROOT}/include/AssetFile.h
	)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
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
	list(APPEND SOURCES ${NCINE_ROOT}/src/PCApplication.cpp)
endif()
