#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaIAudioDevice.h"
#include "LuaUtils.h"
#include "LuaUntrackedUserData.h"
#include "LuaVector3Utils.h"

namespace ncine {

namespace LuaNames {
namespace IAudioDevice {
	static const char *IAudioDevice = "audio_device";

	static const char *name = "name";
	static const char *hasEfxExtension = "has_efx_extension";

	static const char *gain = "get_gain";
	static const char *setGain = "set_gain";

	static const char *position = "get_position";
	static const char *setPosition = "set_position";

	static const char *velocity = "get_velocity";
	static const char *setVelocity = "set_velocity";

	static const char *maxNumSources = "get_max_num_sources";
	static const char *numAvailableSources = "get_num_available_sources";

	static const char *numPlayers = "get_num_players";
	static const char *player = "get_player";

	static const char *pausePlayers = "pause_players";
	static const char *stopPlayers = "stop_players";
	static const char *resumePlayers = "resume_players";

	static const char *pauseDevice = "pause_device";
	static const char *resumeDevice = "resume_device";
}}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIAudioDevice::expose(lua_State *L)
{
	lua_createtable(L, 0, 17);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::name, name);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::hasEfxExtension, hasEfxExtension);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::gain, gain);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::setGain, setGain);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::position, position);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::setPosition, setPosition);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::velocity, velocity);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::setVelocity, setVelocity);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::maxNumSources, maxNumSources);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::numAvailableSources, numAvailableSources);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::numPlayers, numPlayers);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::player, player);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::pausePlayers, pausePlayers);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::stopPlayers, stopPlayers);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::resumePlayers, resumePlayers);

	LuaUtils::addFunction(L, LuaNames::IAudioDevice::pauseDevice, pauseDevice);
	LuaUtils::addFunction(L, LuaNames::IAudioDevice::resumeDevice, resumeDevice);

	lua_setfield(L, -2, LuaNames::IAudioDevice::IAudioDevice);
}

int LuaIAudioDevice::name(lua_State *L)
{
	const char *name = theServiceLocator().audioDevice().name();
	LuaUtils::push(L, name);

	return 1;
}

int LuaIAudioDevice::hasEfxExtension(lua_State *L)
{
	const bool withEfx = theServiceLocator().audioDevice().hasExtension(IAudioDevice::ALExtensions::EXT_EFX);
	LuaUtils::push(L, withEfx);

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

int LuaIAudioDevice::position(lua_State *L)
{
	const Vector3f pos = theServiceLocator().audioDevice().position();
	LuaVector3fUtils::push(L, pos);

	return 1;
}

int LuaIAudioDevice::setPosition(lua_State *L)
{
	int vectorIndex = 0;
	const Vector3f pos = LuaVector3fUtils::retrieve(L, -1, vectorIndex);
	theServiceLocator().audioDevice().setPosition(pos);

	return 0;
}

int LuaIAudioDevice::velocity(lua_State *L)
{
	const Vector3f vel = theServiceLocator().audioDevice().velocity();
	LuaVector3fUtils::push(L, vel);

	return 1;
}

int LuaIAudioDevice::setVelocity(lua_State *L)
{
	int vectorIndex = 0;
	const Vector3f vel = LuaVector3fUtils::retrieve(L, -1, vectorIndex);
	theServiceLocator().audioDevice().setVelocity(vel);

	return 0;
}

int LuaIAudioDevice::maxNumSources(lua_State *L)
{
	const unsigned int maxNumSources = theServiceLocator().audioDevice().maxNumSources();
	LuaUtils::push(L, maxNumSources);

	return 1;
}

int LuaIAudioDevice::numAvailableSources(lua_State *L)
{
	const unsigned int numAvailableSources = theServiceLocator().audioDevice().numAvailableSources();
	LuaUtils::push(L, numAvailableSources);

	return 1;
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
	LuaUntrackedUserData<IAudioPlayer>::push(L, player);

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

int LuaIAudioDevice::resumePlayers(lua_State *L)
{
	theServiceLocator().audioDevice().resumePlayers();
	return 0;
}

int LuaIAudioDevice::pauseDevice(lua_State *L)
{
	theServiceLocator().audioDevice().pauseDevice();
	return 0;
}

int LuaIAudioDevice::resumeDevice(lua_State *L)
{
	theServiceLocator().audioDevice().resumeDevice();
	return 0;
}

}
