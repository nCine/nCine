#include "LuaIAudioPlayer.h"
#include "LuaUntrackedUserData.h"
#include "LuaVector3Utils.h"
#include "LuaUtils.h"
#include "IAudioPlayer.h"

namespace ncine {

namespace LuaNames {
namespace IAudioPlayer {
	static const char *sourceId = "source_id";
	static const char *bufferId = "buffer_id";

	static const char *bytesPerSample = "bytes_per_sample";
	static const char *numChannels = "num_channels";
	static const char *frequency = "frequency";

	static const char *numSamples = "num_samples";
	static const char *duration = "duration";

	static const char *bufferSize = "buffer_size";

	static const char *sampleOffset = "get_sample_offset";
	static const char *setSampleOffset = "set_sample_offset";

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
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::sourceId, sourceId);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::bufferId, bufferId);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::bytesPerSample, bytesPerSample);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::numChannels, numChannels);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::frequency, frequency);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::numSamples, numSamples);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::duration, duration);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::bufferSize, bufferSize);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::sampleOffset, sampleOffset);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setSampleOffset, setSampleOffset);

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

int LuaIAudioPlayer::sourceId(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->sourceId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::bufferId(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->bufferId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::bytesPerSample(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->bytesPerSample());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::numChannels(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->numChannels());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::frequency(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->frequency());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::numSamples(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, static_cast<uint64_t>(audioPlayer->numSamples()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::duration(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->duration());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::bufferSize(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, static_cast<uint64_t>(audioPlayer->bufferSize()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::sampleOffset(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->sampleOffset());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setSampleOffset(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const int sampleOffset = LuaUtils::retrieve<int>(L, -1);

	if (audioPlayer)
		audioPlayer->setSampleOffset(sampleOffset);

	return 0;
}

int LuaIAudioPlayer::play(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		audioPlayer->play();

	return 0;
}

int LuaIAudioPlayer::pause(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		audioPlayer->pause();

	return 0;
}

int LuaIAudioPlayer::stop(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		audioPlayer->stop();

	return 0;
}

int LuaIAudioPlayer::isPlaying(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->isPlaying());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::isPaused(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->isPaused());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::isStopped(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->isStopped());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::isLooping(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->isLooping());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setLooping(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const bool isLooping = LuaUtils::retrieve<bool>(L, -1);

	if (audioPlayer)
		audioPlayer->setLooping(isLooping);

	return 0;
}

int LuaIAudioPlayer::gain(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->gain());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setGain(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float gain = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setGain(gain);

	return 0;
}

int LuaIAudioPlayer::pitch(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->pitch());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setPitch(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float pitch = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setPitch(pitch);

	return 0;
}

int LuaIAudioPlayer::position(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaVector3fUtils::push(L, audioPlayer->position());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setPosition(lua_State *L)
{
	int vectorIndex = 0;
	const Vector3f pos = LuaVector3fUtils::retrieve(L, -1, vectorIndex);
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, vectorIndex - 1);

	if (audioPlayer)
		audioPlayer->setPosition(pos);

	return 0;
}

}
