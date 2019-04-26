#include "LuaIAudioPlayer.h"
#include "LuaClassWrapper.h"
#include "LuaVector3Utils.h"
#include "LuaUtils.h"
#include "IAudioPlayer.h"

namespace ncine {

namespace LuaNames {
namespace IAudioPlayer {
	static const char *play = "play";
	static const char *pause = "pause";
	static const char *stop = "stop";

	static const char *isPlaying = "is_playing";
	static const char *isPaused = "is_paused";
	static const char *isStopped = "is_stopped";

	static const char *isLooping = "is_looping";
	static const char *setLooping = "set_looping";

	static const char *gain = "get_gain";
	static const char *setGain = "set_gain";
	static const char *pitch = "get_pitch";
	static const char *setPitch = "set_pitch";
	static const char *position = "get_position";
	static const char *setPosition = "set_position";
}}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIAudioPlayer::exposeFunctions(lua_State *L)
{
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::play, play);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::pause, pause);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::stop, stop);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::isPlaying, isPlaying);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::isPaused, isPaused);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::isStopped, isStopped);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::isLooping, isLooping);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setLooping, setLooping);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::gain, gain);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setGain, setGain);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::pitch, pitch);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setPitch, setPitch);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::position, position);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setPosition, setPosition);
}

int LuaIAudioPlayer::play(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	audioPlayer->play();

	return 0;
}

int LuaIAudioPlayer::pause(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	audioPlayer->pause();

	return 0;
}

int LuaIAudioPlayer::stop(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	audioPlayer->stop();

	return 0;
}

int LuaIAudioPlayer::isPlaying(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	const bool isPlaying = audioPlayer->isPlaying();
	LuaUtils::push(L, isPlaying);

	return 1;
}

int LuaIAudioPlayer::isPaused(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	const bool isPaused = audioPlayer->isPaused();
	LuaUtils::push(L, isPaused);

	return 1;
}

int LuaIAudioPlayer::isStopped(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	const bool isStopped = audioPlayer->isStopped();
	LuaUtils::push(L, isStopped);

	return 1;
}

int LuaIAudioPlayer::isLooping(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	const bool isLooping = audioPlayer->isLooping();
	LuaUtils::push(L, isLooping);

	return 1;
}

int LuaIAudioPlayer::setLooping(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -2);
	const bool isLooping = LuaUtils::retrieve<bool>(L, -1);

	audioPlayer->setLooping(isLooping);

	return 0;
}

int LuaIAudioPlayer::gain(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	const float gain = audioPlayer->gain();
	LuaUtils::push(L, gain);

	return 1;
}

int LuaIAudioPlayer::setGain(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -2);
	const float gain = LuaUtils::retrieve<float>(L, -1);

	audioPlayer->setGain(gain);

	return 0;
}

int LuaIAudioPlayer::pitch(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	const float pitch = audioPlayer->pitch();
	LuaUtils::push(L, pitch);

	return 1;
}

int LuaIAudioPlayer::setPitch(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -2);
	const float pitch = LuaUtils::retrieve<float>(L, -1);

	audioPlayer->setPitch(pitch);

	return 0;
}

int LuaIAudioPlayer::position(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, -1);

	LuaVector3fUtils::push(L, audioPlayer->position());

	return 1;
}

int LuaIAudioPlayer::setPosition(lua_State *L)
{
	int vectorIndex = 0;
	const Vector3f pos = LuaVector3fUtils::retrieve(L, -1, vectorIndex);
	IAudioPlayer *audioPlayer = LuaClassWrapper<IAudioPlayer>::unwrapUserData(L, vectorIndex - 1);

	audioPlayer->setPosition(pos);

	return 0;
}

}
