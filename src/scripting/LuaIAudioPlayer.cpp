#include "LuaIAudioPlayer.h"
#include "LuaUntrackedUserData.h"
#include "LuaVector3Utils.h"
#include "LuaUtils.h"
#include "IAudioPlayer.h"

namespace ncine {

namespace LuaNames {
namespace IAudioPlayer {
	static const char *hasSource = "has_source";
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

	static const char *isSourceLocked = "is_source_locked";
	static const char *setSourceLocked = "set_source_locked";

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
	static const char *velocity = "get_velocity";
	static const char *setVelocity = "set_velocity";
	static const char *direction = "get_direction";
	static const char *setDirection = "set_direction";

	static const char *coneInnerAngle = "get_cone_inner_angle";
	static const char *setConeInnerAngle = "set_cone_inner_angle";
	static const char *coneOuterAngle = "get_cone_outer_angle";
	static const char *setConeOuterAngle = "set_cone_outer_angle";
	static const char *coneOuterGain = "get_cone_outer_gain";
	static const char *setConeOuterGain = "set_cone_outer_gain";

#ifdef WITH_OPENAL_EXT
	static const char *airAbsorptionFactor = "get_air_absorption_factor";
	static const char *setAirAbsorptionFactor = "set_air_absorption_factor";

	static const char *roomRolloffFactor = "get_room_rolloff_factor";
	static const char *setRoomRolloffFactor = "set_room_rolloff_factor";

	static const char *coneOuterGainHF = "get_cone_outer_gain_hf";
	static const char *setConeOuterGainHF = "set_cone_outer_gain_hf";

	static const char *hasEffectSlot = "has_effect_slot";
	static const char *effectSlotId = "get_effect_slot_id";
	static const char *hasAuxFilter = "has_aux_filter";
	static const char *auxFilterId = "get_aux_filter_id";
	static const char *hasDirectFilter = "has_direct_filter";
	static const char *directFilterId = "get_direct_filter_id";

	static const char *setEffectSlot = "set_effect_slot";
	static const char *setDirectFilter = "set_direct_filter";
#endif
}}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIAudioPlayer::exposeFunctions(lua_State *L)
{
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::hasSource, hasSource);
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

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::isSourceLocked, isSourceLocked);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setSourceLocked, setSourceLocked);

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
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::velocity, velocity);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setVelocity, setVelocity);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::direction, direction);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setDirection, setDirection);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::coneInnerAngle, coneInnerAngle);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setConeInnerAngle, setConeInnerAngle);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::coneOuterAngle, coneOuterAngle);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setConeOuterAngle, setConeOuterAngle);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::coneOuterGain, coneOuterGain);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setConeOuterGain, setConeOuterGain);

#ifdef WITH_OPENAL_EXT
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::airAbsorptionFactor, airAbsorptionFactor);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setAirAbsorptionFactor, setAirAbsorptionFactor);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::roomRolloffFactor, roomRolloffFactor);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setRoomRolloffFactor, setRoomRolloffFactor);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::coneOuterGainHF, coneOuterGainHF);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setConeOuterGainHF, setConeOuterGainHF);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::hasEffectSlot, hasEffectSlot);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::effectSlotId, effectSlotId);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::hasAuxFilter, hasAuxFilter);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::auxFilterId, auxFilterId);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::hasDirectFilter, hasDirectFilter);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::directFilterId, directFilterId);

	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setEffectSlot, setEffectSlot);
	LuaUtils::addFunction(L, LuaNames::IAudioPlayer::setDirectFilter, setDirectFilter);
#endif
}

int LuaIAudioPlayer::hasSource(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->hasSource());
	else
		LuaUtils::pushNil(L);

	return 1;
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

int LuaIAudioPlayer::isSourceLocked(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->isSourceLocked());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setSourceLocked(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const bool sourceLocked = LuaUtils::retrieve<bool>(L, -1);

	if (audioPlayer)
		audioPlayer->setSourceLocked(sourceLocked);

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

int LuaIAudioPlayer::velocity(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaVector3fUtils::push(L, audioPlayer->velocity());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setVelocity(lua_State *L)
{
	int vectorIndex = 0;
	const Vector3f vel = LuaVector3fUtils::retrieve(L, -1, vectorIndex);
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, vectorIndex - 1);

	if (audioPlayer)
		audioPlayer->setVelocity(vel);

	return 0;
}

int LuaIAudioPlayer::direction(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaVector3fUtils::push(L, audioPlayer->direction());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setDirection(lua_State *L)
{
	int vectorIndex = 0;
	const Vector3f dir = LuaVector3fUtils::retrieve(L, -1, vectorIndex);
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, vectorIndex - 1);

	if (audioPlayer)
		audioPlayer->setDirection(dir);

	return 0;
}

int LuaIAudioPlayer::coneInnerAngle(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->coneInnerAngle());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setConeInnerAngle(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float coneInnerAngle = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setConeInnerAngle(coneInnerAngle);

	return 0;
}

int LuaIAudioPlayer::coneOuterAngle(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->coneOuterAngle());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setConeOuterAngle(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float coneOuterAngle = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setConeOuterAngle(coneOuterAngle);

	return 0;
}

int LuaIAudioPlayer::coneOuterGain(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->coneOuterGain());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setConeOuterGain(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float coneOuterGain = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setConeOuterGain(coneOuterGain);

	return 0;
}

#ifdef WITH_OPENAL_EXT
int LuaIAudioPlayer::airAbsorptionFactor(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->airAbsorptionFactor());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setAirAbsorptionFactor(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float airAbsorptionFactor = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setAirAbsorptionFactor(airAbsorptionFactor);

	return 0;
}

int LuaIAudioPlayer::roomRolloffFactor(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->roomRolloffFactor());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setRoomRolloffFactor(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float roomRolloffFactor = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setRoomRolloffFactor(roomRolloffFactor);

	return 0;
}

int LuaIAudioPlayer::coneOuterGainHF(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->coneOuterGainHF());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setConeOuterGainHF(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	const float coneOuterGainHF = LuaUtils::retrieve<float>(L, -1);

	if (audioPlayer)
		audioPlayer->setConeOuterGainHF(coneOuterGainHF);

	return 0;
}

int LuaIAudioPlayer::hasEffectSlot(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->hasEffectSlot());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::effectSlotId(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->effectSlotId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::hasAuxFilter(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->hasAuxFilter());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::auxFilterId(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->auxFilterId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::hasDirectFilter(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->hasDirectFilter());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::directFilterId(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -1);

	if (audioPlayer)
		LuaUtils::push(L, audioPlayer->directFilterId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaIAudioPlayer::setEffectSlot(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -3);
	AudioEffectSlot *audioEffectslot = LuaUntrackedUserData<AudioEffectSlot>::retrieveOrNil(L, -2);
	AudioFilter *audioFilter = LuaUntrackedUserData<AudioFilter>::retrieveOrNil(L, -1);

	if (audioPlayer)
		audioPlayer->setEffectSlot(audioEffectslot, audioFilter);

	return 0;
}

int LuaIAudioPlayer::setDirectFilter(lua_State *L)
{
	IAudioPlayer *audioPlayer = LuaUntrackedUserData<IAudioPlayer>::retrieve(L, -2);
	AudioFilter *audioFilter = LuaUntrackedUserData<AudioFilter>::retrieveOrNil(L, -1);

	if (audioPlayer)
		audioPlayer->setDirectFilter(audioFilter);

	return 0;
}
#endif

}
