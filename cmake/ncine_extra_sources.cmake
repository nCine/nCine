if(WIN32)
	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/common_windefines.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/FileLoggerWindows.cpp)
	if(NOT MINGW)
		list(APPEND SOURCES ${NCINE_ROOT}/src/base/WindowsAtomic.cpp)
	else()
		list(APPEND SOURCES ${NCINE_ROOT}/src/base/GccAtomic.cpp)
	endif()
elseif(APPLE)
	list(APPEND SOURCES ${NCINE_ROOT}/src/base/StdAtomic.cpp)
else()
	list(APPEND SOURCES ${NCINE_ROOT}/src/base/GccAtomic.cpp)
endif()

if(EMSCRIPTEN)
	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/EmscriptenLocalFile.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/EmscriptenLocalFile.cpp)
endif()

if(ANGLE_FOUND)
	message(STATUS "ANGLE has been found")
	target_compile_definitions(ncine PRIVATE "WITH_ANGLE")
	target_link_libraries(ncine PRIVATE EGL::EGL OpenGLES::GLES)
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

	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/IAudioLoader.h
		${NCINE_ROOT}/include/ncine/AudioBuffer.h
		${NCINE_ROOT}/include/ncine/AudioStream.h
		${NCINE_ROOT}/include/ncine/IAudioPlayer.h
		${NCINE_ROOT}/include/ncine/AudioBufferPlayer.h
		${NCINE_ROOT}/include/ncine/AudioStreamPlayer.h
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
		target_compile_definitions(ncine PRIVATE "WITH_VORBIS")
		target_link_libraries(ncine PRIVATE Vorbis::Vorbisfile)

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
		${NCINE_ROOT}/include/ncine/LuaTypes.h
		${NCINE_ROOT}/include/ncine/LuaStateManager.h
		${NCINE_ROOT}/include/ncine/LuaClassWrapper.h
		${NCINE_ROOT}/include/ncine/LuaUtils.h
		${NCINE_ROOT}/include/ncine/LuaDebug.h
		${NCINE_ROOT}/include/ncine/LuaIAppEventHandler.h
		${NCINE_ROOT}/include/ncine/LuaRectUtils.h
		${NCINE_ROOT}/include/ncine/LuaVector2Utils.h
		${NCINE_ROOT}/include/ncine/LuaVector3Utils.h
		${NCINE_ROOT}/include/ncine/LuaColorUtils.h
		${NCINE_ROOT}/include/ncine/LuaIInputEventHandler.h
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
		${NCINE_ROOT}/src/include/LuaTimeStamp.h
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
		${NCINE_ROOT}/src/scripting/LuaTimeStamp.cpp
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
			${NCINE_ROOT}/src/include/LuaIAudioDevice.h
			${NCINE_ROOT}/src/include/LuaIAudioPlayer.h
			${NCINE_ROOT}/src/include/LuaAudioStreamPlayer.h
			${NCINE_ROOT}/src/include/LuaAudioBuffer.h
			${NCINE_ROOT}/src/include/LuaAudioBufferPlayer.h
		)

		list(APPEND SOURCES
			${NCINE_ROOT}/src/scripting/LuaIAudioDevice.cpp
			${NCINE_ROOT}/src/scripting/LuaIAudioPlayer.cpp
			${NCINE_ROOT}/src/scripting/LuaAudioStreamPlayer.cpp
			${NCINE_ROOT}/src/scripting/LuaAudioBuffer.cpp
			${NCINE_ROOT}/src/scripting/LuaAudioBufferPlayer.cpp
		)
	endif()

	if(ANDROID)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaAndroidEvents.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaAndroidEvents.cpp)
	else()
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaEventHandler.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaEventHandler.cpp)
	endif()
endif()

if(NCINE_WITH_IMGUI)
	target_compile_definitions(ncine PRIVATE "WITH_IMGUI")

	# For external projects compiling using an nCine build directory
	set(IMGUI_INCLUDE_ONLY_DIR ${IMGUI_SOURCE_DIR}/include_only)
	file(COPY ${IMGUI_SOURCE_DIR}/imgui.h DESTINATION ${IMGUI_INCLUDE_ONLY_DIR}/ncine)
	file(COPY ${IMGUI_SOURCE_DIR}/imconfig.h DESTINATION ${IMGUI_INCLUDE_ONLY_DIR}/ncine)

	list(APPEND HEADERS
		${IMGUI_INCLUDE_ONLY_DIR}/ncine/imgui.h
		${IMGUI_INCLUDE_ONLY_DIR}/ncine/imconfig.h
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
	elseif(ANDROID)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiAndroidInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/android/ImGuiAndroidInput.cpp)
	endif()

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiDebugOverlay.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/ImGuiDebugOverlay.cpp)
endif()

if(NCINE_WITH_NUKLEAR)
	target_compile_definitions(ncine PRIVATE "WITH_NUKLEAR")

	# For external projects compiling using an nCine build directory
	set(NUKLEAR_INCLUDE_ONLY_DIR ${NUKLEAR_SOURCE_DIR}/include_only)
	file(COPY ${NUKLEAR_SOURCE_DIR}/nuklear.h DESTINATION ${NUKLEAR_INCLUDE_ONLY_DIR}/ncine)

	list(APPEND HEADERS
		${NUKLEAR_INCLUDE_ONLY_DIR}/ncine/nuklear.h
		${NCINE_ROOT}/include/ncine/NuklearContext.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/NuklearDrawing.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/NuklearContext.cpp
		${NCINE_ROOT}/src/graphics/NuklearDrawing.cpp
	)

	if(GLFW_FOUND)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearGlfwInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/NuklearGlfwInput.cpp)
	elseif(SDL2_FOUND)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearSdlInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/NuklearSdlInput.cpp)
	elseif(ANDROID)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearAndroidInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/android/NuklearAndroidInput.cpp)
	endif()
endif()

if(NCINE_WITH_TRACY)
	target_compile_definitions(ncine PRIVATE "WITH_TRACY")
	if(NOT ANDROID AND NOT APPLE AND NOT EMSCRIPTEN)
		target_compile_definitions(ncine PRIVATE "WITH_TRACY_OPENGL")
	endif()
	target_compile_definitions(ncine PUBLIC "TRACY_ENABLE")

	# For external projects compiling using an nCine build directory
	set(TRACY_INCLUDE_ONLY_DIR ${TRACY_SOURCE_DIR}/include_only)
	file(GLOB TRACY_ROOT_HPP "${TRACY_SOURCE_DIR}/*.hpp" "${TRACY_SOURCE_DIR}/*.h")
	file(COPY ${TRACY_ROOT_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy)
	file(GLOB TRACY_COMMON_HPP "${TRACY_SOURCE_DIR}/common/*.hpp" "${TRACY_SOURCE_DIR}/common/*.h")
	file(COPY ${TRACY_COMMON_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/common)
	file(COPY "${TRACY_SOURCE_DIR}/common/TracySystem.cpp" DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/common)
	file(GLOB TRACY_CLIENT_HPP "${TRACY_SOURCE_DIR}/client/*.hpp" "${TRACY_SOURCE_DIR}/client/*.h")
	file(COPY ${TRACY_CLIENT_HPP} DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy/client)
	file(COPY "${TRACY_SOURCE_DIR}/LICENSE" DESTINATION ${TRACY_INCLUDE_ONLY_DIR}/tracy)

	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/tracy.h
		${NCINE_ROOT}/include/ncine/tracy_opengl.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/tracy_memory.cpp
		${TRACY_SOURCE_DIR}/TracyClient.cpp
	)
endif()

if(NCINE_WITH_RENDERDOC AND NOT APPLE)
	find_file(RENDERDOC_API_H
		NAMES renderdoc.h renderdoc_app.h
		PATHS "$ENV{ProgramW6432}/RenderDoc"
			"$ENV{ProgramFiles}/RenderDoc"
			"$ENV{ProgramFiles\(x86\)}/RenderDoc"
			${RENDERDOC_DIR}
		PATH_SUFFIXES "include"
		DOC "Path to the RenderDoc header file")

	if(NOT EXISTS ${RENDERDOC_API_H})
		message(FATAL_ERROR "RenderDoc header file not found")
	endif()

	get_filename_component(RENDERDOC_INCLUDE_DIR ${RENDERDOC_API_H} DIRECTORY)
	target_include_directories(ncine PRIVATE ${RENDERDOC_INCLUDE_DIR})

	target_compile_definitions(ncine PRIVATE "WITH_RENDERDOC")
	if(UNIX)
		target_link_libraries(ncine PRIVATE dl)
	endif()

	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/RenderDocCapture.h)
	list(APPEND PRIVATE_HEADERS ${RENDERDOC_API_H})
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/RenderDocCapture.cpp)
endif()

if(NCINE_BUILD_ANDROID)
	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/AndroidApplication.h
		${NCINE_ROOT}/include/ncine/AssetFile.h
	)
endif()

if(ANDROID)
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
