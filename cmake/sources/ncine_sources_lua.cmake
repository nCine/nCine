if(LUA_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_LUA")
	target_link_libraries(ncine PRIVATE Lua::Lua)

	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/LuaTypes.h
		${NCINE_ROOT}/include/ncine/LuaStateManager.h
		${NCINE_ROOT}/include/ncine/LuaUtils.h
		${NCINE_ROOT}/include/ncine/LuaDebug.h
		${NCINE_ROOT}/include/ncine/LuaRectUtils.h
		${NCINE_ROOT}/include/ncine/LuaVector2Utils.h
		${NCINE_ROOT}/include/ncine/LuaVector3Utils.h
		${NCINE_ROOT}/include/ncine/LuaVector4Utils.h
		${NCINE_ROOT}/include/ncine/LuaColorUtils.h
		${NCINE_ROOT}/include/ncine/LuaColorfUtils.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/LuaNames.h
		${NCINE_ROOT}/src/include/LuaStatistics.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/scripting/LuaStateManager.cpp
		${NCINE_ROOT}/src/scripting/LuaUtils.cpp
		${NCINE_ROOT}/src/scripting/LuaDebug.cpp
		${NCINE_ROOT}/src/scripting/LuaStatistics.cpp
		${NCINE_ROOT}/src/scripting/LuaColorUtils.cpp
		${NCINE_ROOT}/src/scripting/LuaColorfUtils.cpp
	)

	if(NCINE_WITH_SCRIPTING_API)
		target_compile_definitions(ncine PRIVATE "WITH_SCRIPTING_API")

		list(APPEND HEADERS
			${NCINE_ROOT}/include/ncine/LuaUntrackedUserData.h
			${NCINE_ROOT}/include/ncine/LuaIAppEventHandler.h
			${NCINE_ROOT}/include/ncine/LuaIInputEventHandler.h
		)

		list(APPEND PRIVATE_HEADERS
			${NCINE_ROOT}/src/include/LuaClassTracker.h
			${NCINE_ROOT}/src/include/LuaILogger.h
			${NCINE_ROOT}/src/include/LuaHash64.h
			${NCINE_ROOT}/src/include/LuaRect.h
			${NCINE_ROOT}/src/include/LuaVector2.h
			${NCINE_ROOT}/src/include/LuaVector3.h
			${NCINE_ROOT}/src/include/LuaVector4.h
			${NCINE_ROOT}/src/include/LuaColor.h
			${NCINE_ROOT}/src/include/LuaIInputManager.h
			${NCINE_ROOT}/src/include/LuaMouseEvents.h
			${NCINE_ROOT}/src/include/LuaKeys.h
			${NCINE_ROOT}/src/include/LuaKeyboardEvents.h
			${NCINE_ROOT}/src/include/LuaJoystickEvents.h
			${NCINE_ROOT}/src/include/LuaTouchEvents.h
			${NCINE_ROOT}/src/include/LuaTimeStamp.h
			${NCINE_ROOT}/src/include/LuaIFrameTimer.h
			${NCINE_ROOT}/src/include/LuaFileSystem.h
			${NCINE_ROOT}/src/include/LuaApplication.h
			${NCINE_ROOT}/src/include/LuaAppConfiguration.h
			${NCINE_ROOT}/src/include/LuaIGfxDevice.h
		)

		list(APPEND SOURCES
			${NCINE_ROOT}/src/scripting/LuaIAppEventHandler.cpp
			${NCINE_ROOT}/src/scripting/LuaILogger.cpp
			${NCINE_ROOT}/src/scripting/LuaHash64.cpp
			${NCINE_ROOT}/src/scripting/LuaColor.cpp
			${NCINE_ROOT}/src/scripting/LuaIInputManager.cpp
			${NCINE_ROOT}/src/scripting/LuaIInputEventHandler.cpp
			${NCINE_ROOT}/src/scripting/LuaMouseEvents.cpp
			${NCINE_ROOT}/src/scripting/LuaKeys.cpp
			${NCINE_ROOT}/src/scripting/LuaKeyboardEvents.cpp
			${NCINE_ROOT}/src/scripting/LuaJoystickEvents.cpp
			${NCINE_ROOT}/src/scripting/LuaTouchEvents.cpp
			${NCINE_ROOT}/src/scripting/LuaTimeStamp.cpp
			${NCINE_ROOT}/src/scripting/LuaIFrameTimer.cpp
			${NCINE_ROOT}/src/scripting/LuaFileSystem.cpp
			${NCINE_ROOT}/src/scripting/LuaApplication.cpp
			${NCINE_ROOT}/src/scripting/LuaAppConfiguration.cpp
			${NCINE_ROOT}/src/scripting/LuaIGfxDevice.cpp
		)

	if(NCINE_WITH_SCENEGRAPH)
		list(APPEND PRIVATE_HEADERS
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
			${NCINE_ROOT}/src/include/LuaParticleAffector.h
			${NCINE_ROOT}/src/include/LuaViewport.h
			${NCINE_ROOT}/src/include/LuaCamera.h
			${NCINE_ROOT}/src/include/LuaShader.h
			${NCINE_ROOT}/src/include/LuaShaderState.h
		)

		list(APPEND SOURCES
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
			${NCINE_ROOT}/src/scripting/LuaParticleAffector.cpp
			${NCINE_ROOT}/src/scripting/LuaViewport.cpp
			${NCINE_ROOT}/src/scripting/LuaCamera.cpp
			${NCINE_ROOT}/src/scripting/LuaShader.cpp
			${NCINE_ROOT}/src/scripting/LuaShaderState.cpp
		)
	endif()

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
			if(NCINE_WITH_OPENAL_EXT)
				list(APPEND PRIVATE_HEADERS
					${NCINE_ROOT}/src/include/LuaAudioFilter.h
					${NCINE_ROOT}/src/include/LuaAudioEffectSlot.h
					${NCINE_ROOT}/src/include/LuaAudioEffect.h
					${NCINE_ROOT}/src/include/LuaAudioEffectProperties.h
				)

				list(APPEND SOURCES
					${NCINE_ROOT}/src/scripting/LuaAudioFilter.cpp
					${NCINE_ROOT}/src/scripting/LuaAudioEffectSlot.cpp
					${NCINE_ROOT}/src/scripting/LuaAudioEffect.cpp
					${NCINE_ROOT}/src/scripting/LuaAudioEffectProperties.cpp
				)
			endif()
		endif()

		if(NOT ANDROID)
			list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/LuaEventHandler.h)
			list(APPEND SOURCES ${NCINE_ROOT}/src/scripting/LuaEventHandler.cpp)
		endif()
	endif()
endif()
