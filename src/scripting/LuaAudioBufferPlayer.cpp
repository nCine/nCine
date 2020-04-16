#include "LuaAudioBufferPlayer.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaIAudioPlayer.h"
#include "LuaUtils.h"
#include "AudioBufferPlayer.h"

namespace ncine {

namespace LuaNames {
namespace AudioBufferPlayer {
	static const char *AudioBufferPlayer = "audiobuffer_player";
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
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);

	LuaClassTracker<AudioBufferPlayer>::newObject(L, audioBuffer);

	return 1;
}

}
