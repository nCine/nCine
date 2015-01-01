#include <cstdlib> // for exit()
#include "AudioStreamPlayer.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A constructor creating a player from a file
AudioStreamPlayer::AudioStreamPlayer(const char *filename)
	: audioStream_(filename)
{
	type_ = AUDIOSTREAMPLAYER_TYPE;
	setName(filename);
}

AudioStreamPlayer::~AudioStreamPlayer()
{
	if (state_ != STATE_STOPPED)
	{
		audioStream_.stop(sourceId_);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioStreamPlayer::play()
{
	switch (state_)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
		{
			// source is a signed integer in order to check for unavailble source return value.
			// It is then converted to an OpenAL unsigned integer to be used as a valid source.
			int source = ServiceLocator::audioDevice().nextAvailableSource();
			// No sources available
			if (source < 0)
			{
				LOGW("No more available audio sources for playing");
				return;
			}
			sourceId_ = source;

			alSourcef(sourceId_, AL_GAIN, gain_);
			alSourcef(sourceId_, AL_PITCH, pitch_);
			alSourcefv(sourceId_, AL_POSITION, position_);

			alSourcePlay(sourceId_);
			state_ = STATE_PLAYING;

			ServiceLocator::audioDevice().registerPlayer(this);
			break;
		}
		case STATE_PLAYING:
			break;
		case STATE_PAUSED:
		{
			alSourcePlay(sourceId_);
			state_ = STATE_PLAYING;

			ServiceLocator::audioDevice().registerPlayer(this);
			break;
		}
	}
}

void AudioStreamPlayer::pause()
{
	switch (state_)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
			break;
		case STATE_PLAYING:
		{
			alSourcePause(sourceId_);
			state_ = STATE_PAUSED;
			break;
		}
		case STATE_PAUSED:
			break;
	}
}

void AudioStreamPlayer::stop()
{
	switch (state_)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
			break;
		case STATE_PLAYING:
		{
			// Stop the source then unqueue every buffer
			audioStream_.stop(sourceId_);

			state_ = STATE_STOPPED;
			break;
		}
		case STATE_PAUSED:
			break;
	}
}

/// Updates the player state and the stream buffer queue
void AudioStreamPlayer::updateState()
{
	if (state_ == STATE_PLAYING)
	{
		bool shouldStillPlay = audioStream_.enqueue(sourceId_, isLooping_);
		if (shouldStillPlay == false)
		{
			state_ = STATE_STOPPED;
		}
	}
}

}
