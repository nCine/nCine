#include "LuaAudioStreamPlayer.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaIAudioPlayer.h"
#include "LuaUtils.h"
#include "AudioStreamPlayer.h"

namespace ncine {

namespace LuaNames {
namespace AudioStreamPlayer {
	static const char *AudioStreamPlayer = "audio_stream_player";

	static const char *numSamplesInStreamBuffer = "num_samples_in_stream_buffer";
	static const char *streamBufferSize = "stream_buffer_size";
	static const char *sampleOffsetInStream = "sample_offset_in_stream";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioStreamPlayer::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioStreamPlayer>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioStreamPlayer::numSamplesInStreamBuffer, numSamplesInStreamBuffer);
	LuaUtils::addFunction(L, LuaNames::AudioStreamPlayer::streamBufferSize, streamBufferSize);
	LuaUtils::addFunction(L, LuaNames::AudioStreamPlayer::sampleOffsetInStream, sampleOffsetInStream);

	LuaIAudioPlayer::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::AudioStreamPlayer::AudioStreamPlayer);
}

void LuaAudioStreamPlayer::release(void *object)
{
	AudioStreamPlayer *audioStreamPlayer = reinterpret_cast<AudioStreamPlayer *>(object);
	delete audioStreamPlayer;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaAudioStreamPlayer::newObject(lua_State *L)
{
	const char *filename = LuaUtils::retrieve<const char *>(L, -1);

	LuaClassTracker<AudioStreamPlayer>::newObject(L, filename);

	return 1;
}

int LuaAudioStreamPlayer::numSamplesInStreamBuffer(lua_State *L)
{
	AudioStreamPlayer *audioStreamPlayer = LuaUntrackedUserData<AudioStreamPlayer>::retrieve(L, -1);

	if (audioStreamPlayer)
		LuaUtils::push(L, static_cast<uint64_t>(audioStreamPlayer->numSamplesInStreamBuffer()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioStreamPlayer::streamBufferSize(lua_State *L)
{
	AudioStreamPlayer *audioStreamPlayer = LuaUntrackedUserData<AudioStreamPlayer>::retrieve(L, -1);

	if (audioStreamPlayer)
		LuaUtils::push(L, audioStreamPlayer->streamBufferSize());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioStreamPlayer::sampleOffsetInStream(lua_State *L)
{
	AudioStreamPlayer *audioStreamPlayer = LuaUntrackedUserData<AudioStreamPlayer>::retrieve(L, -1);

	if (audioStreamPlayer)
		LuaUtils::push(L, static_cast<uint64_t>(audioStreamPlayer->sampleOffsetInStream()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

}
