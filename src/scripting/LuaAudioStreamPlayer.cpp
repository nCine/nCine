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

}
