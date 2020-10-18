#include "LuaAudioStreamPlayer.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaIAudioPlayer.h"
#include "LuaUtils.h"
#include "AudioStreamPlayer.h"

namespace ncine {

namespace LuaNames {
namespace AudioStreamPlayer {
	static const char *AudioStreamPlayer = "audiostream_player";

	static const char *numStreamSamples = "num_stream_samples";
	static const char *streamBufferSize = "stream_bufferSize";
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

	LuaUtils::addFunction(L, LuaNames::AudioStreamPlayer::numStreamSamples, numStreamSamples);
	LuaUtils::addFunction(L, LuaNames::AudioStreamPlayer::streamBufferSize, streamBufferSize);

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

int LuaAudioStreamPlayer::numStreamSamples(lua_State *L)
{
	AudioStreamPlayer *audioStreamPlayer = LuaClassWrapper<AudioStreamPlayer>::unwrapUserData(L, -1);
	LuaUtils::push(L, static_cast<uint64_t>(audioStreamPlayer->numStreamSamples()));
	return 1;
}

int LuaAudioStreamPlayer::streamBufferSize(lua_State *L)
{
	AudioStreamPlayer *audioStreamPlayer = LuaClassWrapper<AudioStreamPlayer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioStreamPlayer->streamBufferSize());
	return 1;
}

}
