#include "LuaAudioBufferPlayer.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaIAudioPlayer.h"
#include "LuaUtils.h"
#include "AudioBufferPlayer.h"

namespace ncine {

namespace LuaNames {
namespace AudioBufferPlayer {
	static const char *AudioBufferPlayer = "audio_buffer_player";

	static const char *audioBuffer = "get_audio_buffer";
	static const char *setAudioBuffer = "set_audio_buffer";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioBufferPlayer::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioBufferPlayer>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioBufferPlayer::audioBuffer, audioBuffer);
	LuaUtils::addFunction(L, LuaNames::AudioBufferPlayer::setAudioBuffer, setAudioBuffer);

	LuaIAudioPlayer::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::AudioBufferPlayer::AudioBufferPlayer);
}

void LuaAudioBufferPlayer::release(void *object)
{
	AudioBufferPlayer *audioBufferPlayer = reinterpret_cast<AudioBufferPlayer *>(object);
	delete audioBufferPlayer;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaAudioBufferPlayer::newObject(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaUntrackedUserData<AudioBuffer>::retrieveOrNil(L, -1);

	LuaClassTracker<AudioBufferPlayer>::newObject(L, audioBuffer);

	return 1;
}

int LuaAudioBufferPlayer::audioBuffer(lua_State *L)
{
	AudioBufferPlayer *audioBufferPlayer = LuaUntrackedUserData<AudioBufferPlayer>::retrieve(L, -1);

	if (audioBufferPlayer)
		LuaUntrackedUserData<AudioBuffer>::push(L, audioBufferPlayer->audioBuffer());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioBufferPlayer::setAudioBuffer(lua_State *L)
{
	AudioBufferPlayer *audioBufferPlayer = LuaUntrackedUserData<AudioBufferPlayer>::retrieve(L, -2);
	AudioBuffer *audioBuffer = LuaUntrackedUserData<AudioBuffer>::retrieveOrNil(L, -1);

	if (audioBufferPlayer)
		audioBufferPlayer->setAudioBuffer(audioBuffer);

	return 0;
}

}
