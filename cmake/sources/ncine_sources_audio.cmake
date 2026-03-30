if(OPENAL_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_AUDIO")
	target_link_libraries(ncine PRIVATE OpenAL::AL)

	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/AudioBuffer.h
		${NCINE_ROOT}/include/ncine/AudioStream.h
		${NCINE_ROOT}/include/ncine/IAudioPlayer.h
		${NCINE_ROOT}/include/ncine/AudioBufferPlayer.h
		${NCINE_ROOT}/include/ncine/AudioStreamPlayer.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/ALAudioDevice.h
		${NCINE_ROOT}/src/include/IAudioLoader.h
		${NCINE_ROOT}/src/include/AudioLoaderWav.h
		${NCINE_ROOT}/src/include/AudioReaderWav.h
		${NCINE_ROOT}/src/include/IAudioReader.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/audio/ALAudioDevice.cpp
		${NCINE_ROOT}/src/audio/IAudioLoader.cpp
		${NCINE_ROOT}/src/audio/AudioLoaderWav.cpp
		${NCINE_ROOT}/src/audio/AudioReaderWav.cpp
		${NCINE_ROOT}/src/audio/AudioBuffer.cpp
		${NCINE_ROOT}/src/audio/AudioStream.cpp
		${NCINE_ROOT}/src/audio/IAudioPlayer.cpp
		${NCINE_ROOT}/src/audio/AudioBufferPlayer.cpp
		${NCINE_ROOT}/src/audio/AudioStreamPlayer.cpp
	)

	if(NCINE_WITH_OPENAL_EXT)
		target_compile_definitions(ncine PRIVATE "WITH_OPENAL_EXT")

		list(APPEND HEADERS
			${NCINE_ROOT}/include/ncine/AudioEffect.h
			${NCINE_ROOT}/include/ncine/AudioEffectProperties.h
			${NCINE_ROOT}/include/ncine/AudioEffectSlot.h
			${NCINE_ROOT}/include/ncine/AudioFilter.h
		)

		list(APPEND PRIVATE_HEADERS
			${NCINE_ROOT}/src/include/openal_proto.h
		)

		list(APPEND SOURCES
			${NCINE_ROOT}/src/audio/AudioEffect.cpp
			${NCINE_ROOT}/src/audio/AudioEffectProperties.cpp
			${NCINE_ROOT}/src/audio/AudioEffectSlot.cpp
			${NCINE_ROOT}/src/audio/AudioFilter.cpp
		)
	endif()

	if(VORBIS_FOUND)
		target_compile_definitions(ncine PRIVATE "WITH_VORBIS")
		target_link_libraries(ncine PRIVATE Vorbis::Vorbisfile)

		list(APPEND PRIVATE_HEADERS
			${NCINE_ROOT}/src/include/AudioLoaderOgg.h
			${NCINE_ROOT}/src/include/AudioReaderOgg.h)

		list(APPEND SOURCES
			${NCINE_ROOT}/src/audio/AudioLoaderOgg.cpp
			${NCINE_ROOT}/src/audio/AudioReaderOgg.cpp)
	endif()
endif()
