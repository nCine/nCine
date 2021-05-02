#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaIAudioDevice.h"
#include "LuaUtils.h"
#include "LuaClassWrapper.h"

namespace ncine {

namespace LuaNames {
namespace IAudioDevice {
	static const char *IAudioDevice = "audio_device";

	static const char *name = "name";

	static const char *gain = "get_gain";
	static const char *setGain = "set_gain";

	static const char *numPlayers = "num_players";
	static const char *player = "get_player";

	static const char *pausePlayers = "pause_players";
	static const char *stopPlayers = "stop_players";

	static const char *freezePlayers = "freeze_players";
	static const char *unfreezePlayers = "unfreeze_players";
}}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIAudioDevice::expose(lua_State *L)
{
	lua_createtable(L, 0, 9);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::name, name);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::gain, gain);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::setGain, setGain);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::numPlayers, numPlayers);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::player, player);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::pausePlayers, pausePlayers);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::stopPlayers, stopPlayers);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::freezePlayers, freezePlayers);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::unfreezePlayers, unfreezePlayers);

	lua_setfield(L, -2, LuaNames::IAudioDevice::IAudioDevice);
}

int LuaIAudioDevice::name(lua_State *L)
{
	const char *name = theServiceLocator().audioDevice().name();
	LuaUtils::push(L, name);

	return 1;
}

int LuaIAudioDevice::gain(lua_State *L)
{
	const float gain = theServiceLocator().audioDevice().gain();
	LuaUtils::push(L, gain);

	return 1;
}

int LuaIAudioDevice::setGain(lua_State *L)
{
	const float gain = LuaUtils::retrieve<float>(L, -1);
	theServiceLocator().audioDevice().setGain(gain);

	return 0;
}

int LuaIAudioDevice::numPlayers(lua_State *L)
{
	const unsigned int numPlayers = theServiceLocator().audioDevice().numPlayers();
	LuaUtils::push(L, numPlayers);

	return 1;
}

int LuaIAudioDevice::player(lua_State *L)
{
	const int unsigned index = LuaUtils::retrieve<uint32_t>(L, -1);
	const IAudioPlayer *player = theServiceLocator().audioDevice().player(index);
	LuaClassWrapper<IAudioPlayer>::pushUntrackedUserData(L, player);

	return 1;
}

int LuaIAudioDevice::pausePlayers(lua_State *L)
{
	theServiceLocator().audioDevice().pausePlayers();
	return 0;
}

int LuaIAudioDevice::stopPlayers(lua_State *L)
{
	theServiceLocator().audioDevice().stopPlayers();
	return 0;
}

int LuaIAudioDevice::freezePlayers(lua_State *L)
{
	theServiceLocator().audioDevice().freezePlayers();
	return 0;
}

int LuaIAudioDevice::unfreezePlayers(lua_State *L)
{
	theServiceLocator().audioDevice().unfreezePlayers();
	return 0;
}

}
