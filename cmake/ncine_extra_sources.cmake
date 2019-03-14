if(WIN32)
	list(APPEND SOURCES ${NCINE_ROOT}/src/FileLoggerWindows.cpp)
endif()

if(GLEW_FOUND)
	if (NOT WIN32)
		message(STATUS "GLEW has been found")
	endif()
	target_compile_definitions(ncine PRIVATE "WITH_GLEW")
	target_link_libraries(ncine PRIVATE GLEW::GLEW)
endif()

if(GLFW_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_GLFW")
	target_link_libraries(ncine PRIVATE GLFW::GLFW)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/GlfwInputManager.h
		${NCINE_ROOT}/src/include/GlfwGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/input/GlfwInputManager.cpp
		${NCINE_ROOT}/src/input/GlfwKeys.cpp
		${NCINE_ROOT}/src/graphics/GlfwGfxDevice.cpp
	)
endif()

if(SDL2_FOUND AND NOT GLFW_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_SDL")
	target_link_libraries(ncine PRIVATE SDL2::SDL2)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/SdlInputManager.h
		${NCINE_ROOT}/src/include/SdlGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/input/SdlInputManager.cpp
		${NCINE_ROOT}/src/input/SdlKeys.cpp
		${NCINE_ROOT}/src/graphics/SdlGfxDevice.cpp
	)
endif()

if(OPENAL_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_AUDIO")
	target_link_libraries(ncine PRIVATE OpenAL::AL)
	if(VORBIS_FOUND)
		target_compile_definitions(ncine PRIVATE "WITH_VORBIS")
		target_link_libraries(ncine PRIVATE Vorbis::Vorbisfile)
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
	target_compile_definitions(ncine PRIVATE "WITH_PNG")
	target_link_libraries(ncine PRIVATE PNG::PNG)

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/TextureLoaderPng.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/TextureLoaderPng.cpp)
endif()
if(WEBP_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_WEBP")
	target_link_libraries(ncine PRIVATE WebP::WebP)

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/TextureLoaderWebP.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/TextureLoaderWebP.cpp)
endif()

if(Threads_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_THREADS")
	target_link_libraries(ncine PRIVATE Threads::Threads)

	list(APPEND HEADERS
		${NCINE_ROOT}/include/IThreadPool.h
		${NCINE_ROOT}/include/IThreadCommand.h
	)
	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/Thread.h
		${NCINE_ROOT}/src/include/ThreadSync.h
	)

	if(WIN32)
		list(APPEND SOURCES
			${NCINE_ROOT}/src/threading/WindowsThread.cpp
			${NCINE_ROOT}/src/threading/WindowsThreadSync.cpp
		)
	else()
		list(APPEND SOURCES
			${NCINE_ROOT}/src/threading/PosixThread.cpp
			${NCINE_ROOT}/src/threading/PosixThreadSync.cpp
		)
	endif()

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ThreadPool.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/threading/ThreadPool.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ThreadCommands.h)
endif()

if(LUA_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_LUA")
	target_link_libraries(ncine PRIVATE Lua::Lua)

	list(APPEND HEADERS
		${NCINE_ROOT}/include/LuaTypes.h
		${NCINE_ROOT}/include/LuaStateManager.h
		${NCINE_ROOT}/include/LuaClassWrapper.h
		${NCINE_ROOT}/include/LuaUtils.h
		${NCINE_ROOT}/include/LuaDebug.h
		${NCINE_ROOT}/include/LuaIAppEventHandler.h
		${NCINE_ROOT}/include/LuaRectUtils.h
		${NCINE_ROOT}/include/LuaVector2Utils.h
		${NCINE_ROOT}/include/LuaVector3Utils.h
		${NCINE_ROOT}/include/LuaColorUtils.h
		${NCINE_ROOT}/include/LuaIInputEventHandler.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/LuaClassTracker.h
		${NCINE_ROOT}/src/include/LuaStatistics.h
		${NCINE_ROOT}/src/include/LuaNames.h
		${NCINE_ROOT}/src/include/LuaILogger.h
		${NCINE_ROOT}/src/include/LuaRect.h
		${NCINE_ROOT}/src/include/LuaVector2.h
		${NCINE_ROOT}/src/include/LuaVector3.h
		${NCINE_ROOT}/src/include/LuaColor.h
		${NCINE_ROOT}/src/include/LuaIInputManager.h
		${NCINE_ROOT}/src/include/LuaMouseEvents.h
		${NCINE_ROOT}/src/include/LuaKeys.h
		${NCINE_ROOT}/src/include/LuaKeyboardEvents.h
		${NCINE_ROOT}/src/include/LuaJoystickEvents.h
		${NCINE_ROOT}/src/include/LuaApplication.h
		${NCINE_ROOT}/src/include/LuaAppConfiguration.h
		${NCINE_ROOT}/src/include/LuaSceneNode.h
		${NCINE_ROOT}/src/include/LuaDrawableNode.h
		${NCINE_ROOT}/src/include/LuaTexture.h
		${NCINE_ROOT}/src/include/LuaBaseSprite.h
		${NCINE_ROOT}/src/include/LuaSprite.h
		${NCINE_ROOT}/src/include/LuaMeshSprite.h
		${NCINE_ROOT}/src/include/LuaRectAnimation.h
		${NCINE_ROOT}/src/include/LuaAnimatedSprite.h
		${NCINE_ROOT}/src/include/LuaFont.h
		${NCINE_ROOT}/src/include/LuaTextNode.h
		${NCINE_ROOT}/src/include/LuaParticleSystem.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/scripting/LuaStateManager.cpp
		${NCINE_ROOT}/src/scripting/LuaUtils.cpp
		${NCINE_ROOT}/src/scripting/LuaDebug.cpp
		${NCINE_ROOT}/src/scripting/LuaStatistics.cpp
		${NCINE_ROOT}/src/scripting/LuaIAppEventHandler.cpp
		${NCINE_ROOT}/src/scripting/LuaILogger.cpp
		${NCINE_ROOT}/src/scripting/LuaColor.cpp
		${NCINE_ROOT}/src/scripting/LuaColorUtils.cpp
		${NCINE_ROOT}/src/scripting/LuaIInputManager.cpp
		${NCINE_ROOT}/src/scripting/LuaIInputEventHandler.cpp
		${NCINE_ROOT}/src/scripting/LuaMouseEvents.cpp
		${NCINE_ROOT}/src/scripting/LuaKeys.cpp
		${NCINE_ROOT}/src/scripting/LuaKeyboardEvents.cpp
		${NCINE_ROOT}/src/scripting/LuaJoystickEvents.cpp
		${NCINE_ROOT}/src/scripting/LuaApplication.cpp
		${NCINE_ROOT}/src/scripting/LuaAppConfiguration.cpp
		${NCINE_ROOT}/src/scripting/LuaSceneNode.cpp
		${NCINE_ROOT}/src/scripting/LuaDrawableNode.cpp
		${NCINE_ROOT}/src/scripting/LuaTexture.cpp
		${NCINE_ROOT}/src/scripting/LuaBaseSprite.cpp
		${NCINE_ROOT}/src/scripting/LuaSprite.cpp
		${NCINE_ROOT}/src/scripting/LuaMeshSprite.cpp
		${NCINE_ROOT}/src/scripting/LuaRectAnimation.cpp
		${NCINE_ROOT}/src/scripting/LuaAnimatedSprite.cpp
		${NCINE_ROOT}/src/scripting/LuaFont.cpp
		${NCINE_ROOT}/src/scripting/LuaTextNode.cpp
		${NCINE_ROOT}/src/scripting/LuaParticleSystem.cpp
	)

	if(OPENAL_FOUND)
		list(APPEND PRIVATE_HEADERS
			${NCINE_ROOT}/src/include/LuaIAudioPlayer.h
			${NCINE_ROOT}/src/include/LuaAudioStreamPlayer.h
			${NCINE_ROOT}/src/include/LuaAudioBuffer.h
			${NCINE_ROOT}/src/include/LuaAudioBufferPlayer.h
		)

		list(APPEND SOURCES
			${NCINE_ROOT}/src/scripting/LuaIAudioPlayer.cpp
			${NCINE_ROOT}/src/scripting/LuaAudioStreamPlayer.cpp
			${NCINE_ROOT}/src/scripting/LuaAudioBuffer.cpp
			${NCINE_ROOT}/src/scripting/LuaAudioBufferPlayer.cpp
		)
	endif()

	if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAndroidEvents.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAndroidEvents.cpp)
	else()
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaEventHandler.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaEventHandler.cpp)
	endif()
endif()

if(NCINE_WITH_IMGUI)
	target_compile_definitions(ncine PRIVATE "WITH_IMGUI")

	list(APPEND HEADERS
		${IMGUI_SOURCE_DIR}/imgui.h
		${IMGUI_SOURCE_DIR}/imconfig.h
	)

	list(APPEND PRIVATE_HEADERS
		${IMGUI_SOURCE_DIR}/imgui_internal.h
		${IMGUI_SOURCE_DIR}/imstb_rectpack.h
		${IMGUI_SOURCE_DIR}/imstb_textedit.h
		${IMGUI_SOURCE_DIR}/imstb_truetype.h
		${NCINE_ROOT}/src/include/ImGuiDrawing.h
		${NCINE_ROOT}/src/include/ImGuiJoyMappedInput.h
	)

	list(APPEND SOURCES
		${IMGUI_SOURCE_DIR}/imgui.cpp
		${IMGUI_SOURCE_DIR}/imgui_draw.cpp
		${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
		${IMGUI_SOURCE_DIR}/imgui_demo.cpp
		${NCINE_ROOT}/src/graphics/ImGuiDrawing.cpp
		${NCINE_ROOT}/src/input/ImGuiJoyMappedInput.cpp
	)

	if(GLFW_FOUND)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiGlfwInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/ImGuiGlfwInput.cpp)
	elseif(SDL2_FOUND)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiSdlInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/ImGuiSdlInput.cpp)
	elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiAndroidInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/android/ImGuiAndroidInput.cpp)
	endif()

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiDebugOverlay.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/ImGuiDebugOverlay.cpp)
else()
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ProfileVariable.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/PlottingVariable.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/PlottingVariable.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ProfilePlotter.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/ProfilePlotter.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LinePlotter.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/LinePlotter.cpp)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/StackedBarPlotter.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/StackedBarPlotter.cpp)

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/DebugOverlay.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/DebugOverlay.cpp)
endif()

if(NCINE_WITH_TRACY)
	target_compile_definitions(ncine PRIVATE "WITH_TRACY")
	if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Android" AND NOT APPLE)
		target_compile_definitions(ncine PRIVATE "WITH_TRACY_OPENGL")
	endif()
	target_compile_definitions(ncine PUBLIC "TRACY_ENABLE")

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/tracy.h
		${NCINE_ROOT}/src/include/tracy_opengl.h
		${TRACY_SOURCE_DIR}/Tracy.hpp
		${TRACY_SOURCE_DIR}/TracyOpenGL.hpp
	)

	if(WITH_LUA)
		list(APPEND PRIVATE_HEADERS ${TRACY_SOURCE_DIR}/TracyLua.hpp)
	endif()

	list(APPEND SOURCES
		${NCINE_ROOT}/src/tracy_memory.cpp
		${TRACY_SOURCE_DIR}/TracyClient.cpp
	)
endif()

if(NCINE_BUILD_ANDROID)
	list(APPEND HEADERS
		${NCINE_ROOT}/include/AndroidApplication.h
		${NCINE_ROOT}/include/AssetFile.h
	)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
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
