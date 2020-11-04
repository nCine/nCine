#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "AudioStreamPlayer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioStreamPlayer::AudioStreamPlayer()
    : IAudioPlayer(ObjectType::AUDIOSTREAM_PLAYER), audioStream_()
{
}

AudioStreamPlayer::AudioStreamPlayer(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : IAudioPlayer(ObjectType::AUDIOSTREAM_PLAYER, bufferName), audioStream_(bufferName, bufferPtr, bufferSize)
{
}

AudioStreamPlayer::AudioStreamPlayer(const char *filename)
    : IAudioPlayer(ObjectType::AUDIOSTREAM_PLAYER, filename), audioStream_(filename)
{
}

AudioStreamPlayer::~AudioStreamPlayer()
{
	if (state_ != PlayerState::STOPPED)
		audioStream_.stop(sourceId_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool AudioStreamPlayer::loadFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	if (state_ != PlayerState::STOPPED)
		audioStream_.stop(sourceId_);

	const bool hasLoaded = audioStream_.loadFromMemory(bufferName, bufferPtr, bufferSize);
	if (hasLoaded == false)
		return false;

	setName(bufferName);
	return true;
}

bool AudioStreamPlayer::loadFromFile(const char *filename)
{
	if (state_ != PlayerState::STOPPED)
		audioStream_.stop(sourceId_);

	const bool hasLoaded = audioStream_.loadFromFile(filename);
	if (hasLoaded == false)
		return false;

	setName(filename);
	return true;
}

void AudioStreamPlayer::play()
{
	switch (state_)
	{
		case PlayerState::INITIAL:
		case PlayerState::STOPPED:
		{
			const unsigned int source = theServiceLocator().audioDevice().nextAvailableSource();
			if (source == IAudioDevice::UnavailableSource)
			{
				LOGW("No more available audio sources for playing");
				return;
			}
			sourceId_ = source;

			// Streams looping is not handled at enqueued buffer level
			alSourcei(sourceId_, AL_LOOPING, AL_FALSE);

			alSourcef(sourceId_, AL_GAIN, gain_);
			alSourcef(sourceId_, AL_PITCH, pitch_);
			alSourcefv(sourceId_, AL_POSITION, position_.data());

			alSourcePlay(sourceId_);
			state_ = PlayerState::PLAYING;

			theServiceLocator().audioDevice().registerPlayer(this);
			break;
		}
		case PlayerState::PLAYING:
			break;
		case PlayerState::PAUSED:
		{
			alSourcePlay(sourceId_);
			state_ = PlayerState::PLAYING;

			theServiceLocator().audioDevice().registerPlayer(this);
			break;
		}
	}
}

void AudioStreamPlayer::pause()
{
	switch (state_)
	{
		case PlayerState::INITIAL:
		case PlayerState::STOPPED:
			break;
		case PlayerState::PLAYING:
		{
			alSourcePause(sourceId_);
			state_ = PlayerState::PAUSED;
			break;
		}
		case PlayerState::PAUSED:
			break;
	}
}

void AudioStreamPlayer::stop()
{
	switch (state_)
	{
		case PlayerState::INITIAL:
		case PlayerState::STOPPED:
			break;
		case PlayerState::PLAYING:
		case PlayerState::PAUSED:
		{
			// Stop the source then unqueue every buffer
			audioStream_.stop(sourceId_);
			// Detach the buffer from source
			alSourcei(sourceId_, AL_BUFFER, 0);

			sourceId_ = 0;
			state_ = PlayerState::STOPPED;
			break;
		}
	}
}

void AudioStreamPlayer::updateState()
{
	if (state_ == PlayerState::PLAYING)
	{
		const bool shouldStillPlay = audioStream_.enqueue(sourceId_, isLooping_);
		if (shouldStillPlay == false)
		{
			// Detach the buffer from source
			alSourcei(sourceId_, AL_BUFFER, 0);
			sourceId_ = 0;
			state_ = PlayerState::STOPPED;
		}
	}
}

}
