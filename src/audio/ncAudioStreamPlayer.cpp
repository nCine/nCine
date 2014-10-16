#include <cstdlib> // for exit()
#include "ncAudioStreamPlayer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A constructor creating a player from a file
ncAudioStreamPlayer::ncAudioStreamPlayer(const char *filename)
	: audioStream_(filename)
{
	type_ = AUDIOSTREAMPLAYER_TYPE;
	setName(filename);
}

ncAudioStreamPlayer::~ncAudioStreamPlayer()
{
	if (state_ != STATE_STOPPED)
	{
		audioStream_.stop(sourceId_);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioStreamPlayer::play()
{
	switch (state_)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
		{
			// source is a signed integer in order to check for unavailble source return value.
			// It is then converted to an OpenAL unsigned integer to be used as a valid source.
			int source = ncServiceLocator::audioDevice().nextAvailableSource();
			// No sources available
			if (source < 0)
			{
				return;
			}
			sourceId_ = source;

			alSourcef(sourceId_, AL_GAIN, gain_);
			alSourcef(sourceId_, AL_PITCH, pitch_);
			alSourcefv(sourceId_, AL_POSITION, position_);

			alSourcePlay(sourceId_);
			state_ = STATE_PLAYING;

			ncServiceLocator::audioDevice().registerPlayer(this);
			break;
		}
		case STATE_PLAYING:
			break;
		case STATE_PAUSED:
		{
			alSourcePlay(sourceId_);
			state_ = STATE_PLAYING;

			ncServiceLocator::audioDevice().registerPlayer(this);
			break;
		}
	}
}

void ncAudioStreamPlayer::pause()
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

void ncAudioStreamPlayer::stop()
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
void ncAudioStreamPlayer::updateState()
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
