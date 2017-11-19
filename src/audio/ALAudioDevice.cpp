#include "common_macros.h"
#include "ALAudioDevice.h"
#include "AudioBufferPlayer.h"
#include "AudioStreamPlayer.h"

namespace ncine {

namespace {

void playPlayer(IAudioPlayer *player) { player->play(); }
void pausePlayer(IAudioPlayer *player) { player->pause(); }
void stopPlayer(IAudioPlayer *player) { player->stop(); }

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ALAudioDevice::ALAudioDevice()
	: device_(nullptr), context_(nullptr), gain_(1.0f)
{
	device_ = alcOpenDevice(nullptr);
	FATAL_ASSERT_MSG_X(device_ != nullptr, "alcOpenDevice failed: %x", alGetError());

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

	alGenSources(MaxSources, sources_.data());
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, gain_);
}

ALAudioDevice::~ALAudioDevice()
{
	for (unsigned int i = 0; i < MaxSources; i++)
		alSourcei(sources_[i], AL_BUFFER, AL_NONE);
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

void ALAudioDevice::stopPlayers()
{
	forEach(players_.begin(), players_.end(), stopPlayer);
	players_.clear();
}

void ALAudioDevice::pausePlayers()
{
	forEach(players_.begin(), players_.end(), pausePlayer);
	players_.clear();
}

void ALAudioDevice::stopPlayers(PlayerType playerType)
{
	Object::ObjectType objectType = AudioBufferPlayer::sType();
	if (playerType == AUDIOSTREAM_PLAYER)
		objectType = AudioStreamPlayer::sType();

	List<IAudioPlayer *>::ConstIterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->type() == objectType)
		{
			(*i)->stop();
			i = players_.erase(i);
		}
		else
			++i;
	}
}

void ALAudioDevice::pausePlayers(PlayerType playerType)
{
	Object::ObjectType objectType = AudioBufferPlayer::sType();
	if (playerType == AUDIOSTREAM_PLAYER)
		objectType = AudioStreamPlayer::sType();

	List<IAudioPlayer *>::ConstIterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->type() == objectType)
		{
			(*i)->pause();
			i = players_.erase(i);
		}
		else
			++i;
	}
}

void ALAudioDevice::freezePlayers()
{
	forEach(players_.begin(), players_.end(), pausePlayer);
	// The player list is not cleared at this point, it is needed as-is by the unfreeze method
}

void ALAudioDevice::unfreezePlayers()
{
	forEach(players_.begin(), players_.end(), playPlayer);
}

int ALAudioDevice::nextAvailableSource()
{
	ALint sourceState;

	for (unsigned int i = 0; i < MaxSources; i++)
	{
		alGetSourcei(sources_[i], AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING && sourceState != AL_PAUSED)
			return sources_[i];
	}

	return -1;
}

void ALAudioDevice::registerPlayer(IAudioPlayer *player)
{
	ASSERT(player);
	players_.pushBack(player);
}

void ALAudioDevice::updatePlayers()
{
	List<IAudioPlayer *>::ConstIterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->isPlaying())
		{
			(*i)->updateState();
			++i;
		}
		else
			i = players_.erase(i);
	}
}

}
