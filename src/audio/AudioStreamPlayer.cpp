#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "AudioStreamPlayer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioStreamPlayer::AudioStreamPlayer(const char *filename)
    : audioStream_(filename)
{
	type_ = ObjectType::AUDIOSTREAM_PLAYER;
	setName(filename);
}

AudioStreamPlayer::~AudioStreamPlayer()
{
	if (state_ != PlayerState::STOPPED)
		audioStream_.stop(sourceId_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioStreamPlayer::play()
{
	switch (state_)
	{
		case PlayerState::INITIAL:
		case PlayerState::STOPPED:
		{
			// source is a signed integer in order to check for unavailble source return value.
			// It is then converted to an OpenAL unsigned integer to be used as a valid source.
			int source = theServiceLocator().audioDevice().nextAvailableSource();
			// No sources available
			if (source < 0)
			{
				LOGW("No more available audio sources for playing");
				return;
			}
			sourceId_ = source;

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
		{
			// Stop the source then unqueue every buffer
			audioStream_.stop(sourceId_);

			state_ = PlayerState::STOPPED;
			break;
		}
		case PlayerState::PAUSED:
			break;
	}
}

void AudioStreamPlayer::updateState()
{
	if (state_ == PlayerState::PLAYING)
	{
		const bool shouldStillPlay = audioStream_.enqueue(sourceId_, isLooping_);
		if (shouldStillPlay == false)
			state_ = PlayerState::STOPPED;
	}
}

}
