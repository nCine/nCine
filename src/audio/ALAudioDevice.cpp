#include "common_macros.h"
#include "ALAudioDevice.h"
#include "AudioBufferPlayer.h"
#include "AudioStreamPlayer.h"
#include <nctl/algorithms.h>

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ALAudioDevice::ALAudioDevice()
    : device_(nullptr), context_(nullptr), gain_(1.0f),
      sources_(nctl::StaticArrayMode::EXTEND_SIZE), deviceName_(nullptr)
{
	device_ = alcOpenDevice(nullptr);
	FATAL_ASSERT_MSG_X(device_ != nullptr, "alcOpenDevice failed: %x", alGetError());
	deviceName_ = alcGetString(device_, ALC_DEVICE_SPECIFIER);

	context_ = alcCreateContext(device_, nullptr);
	if (context_ == nullptr)
	{
		alcCloseDevice(device_);
		FATAL_MSG_X("alcCreateContext failed: %x", alGetError());
	}

	if (!alcMakeContextCurrent(context_))
	{
		alcDestroyContext(context_);
		alcCloseDevice(device_);
		FATAL_MSG_X("alcMakeContextCurrent failed: %x", alGetError());
	}

	alGetError();
	alGenSources(MaxSources, sources_.data());
	const ALenum error = alGetError();
	ASSERT_MSG_X(error == AL_NO_ERROR, "alGenSources failed: %x", error);

	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, gain_);
}

ALAudioDevice::~ALAudioDevice()
{
	for (ALuint sourceId : sources_)
		alSourcei(sourceId, AL_BUFFER, AL_NONE);
	alDeleteSources(MaxSources, sources_.data());

	alcDestroyContext(context_);

	ALCboolean result = alcCloseDevice(device_);
	FATAL_ASSERT_MSG_X(result, "alcCloseDevice failed: %d", alGetError());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ALAudioDevice::setGain(ALfloat gain)
{
	gain_ = gain;
	alListenerf(AL_GAIN, gain_);
}

const IAudioPlayer *ALAudioDevice::player(unsigned int index) const
{
	if (index < players_.size())
		return players_[index];

	return nullptr;
}

void ALAudioDevice::stopPlayers()
{
	forEach(players_.begin(), players_.end(), [](IAudioPlayer *player) { player->stop(); });
	players_.clear();
}

void ALAudioDevice::pausePlayers()
{
	forEach(players_.begin(), players_.end(), [](IAudioPlayer *player) { player->pause(); });
	players_.clear();
}

void ALAudioDevice::stopPlayers(PlayerType playerType)
{
	const Object::ObjectType objectType = (playerType == PlayerType::BUFFER)
	                                          ? AudioBufferPlayer::sType()
	                                          : AudioStreamPlayer::sType();

	for (int i = players_.size() - 1; i >= 0; i--)
	{
		if (players_[i]->type() == objectType)
		{
			players_[i]->stop();
			players_.unorderedRemoveAt(i);
		}
	}
}

void ALAudioDevice::pausePlayers(PlayerType playerType)
{
	const Object::ObjectType objectType = (playerType == PlayerType::BUFFER)
	                                          ? AudioBufferPlayer::sType()
	                                          : AudioStreamPlayer::sType();

	for (int i = players_.size() - 1; i >= 0; i--)
	{
		if (players_[i]->type() == objectType)
		{
			players_[i]->pause();
			players_.unorderedRemoveAt(i);
		}
	}
}

void ALAudioDevice::freezePlayers()
{
	forEach(players_.begin(), players_.end(), [](IAudioPlayer *player) { player->pause(); });
	// The players array is not cleared at this point, it is needed as-is by the unfreeze method
}

void ALAudioDevice::unfreezePlayers()
{
	forEach(players_.begin(), players_.end(), [](IAudioPlayer *player) { player->play(); });
}

unsigned int ALAudioDevice::nextAvailableSource()
{
	ALint sourceState;

	for (ALuint sourceId : sources_)
	{
		alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING && sourceState != AL_PAUSED)
			return sourceId;
	}

	return UnavailableSource;
}

void ALAudioDevice::registerPlayer(IAudioPlayer *player)
{
	ASSERT(player);
	players_.pushBack(player);
}

void ALAudioDevice::updatePlayers()
{
	for (int i = players_.size() - 1; i >= 0; i--)
	{
		if (players_[i]->isPlaying())
			players_[i]->updateState();
		else
			players_.unorderedRemoveAt(i);
	}
}

}
