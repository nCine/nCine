#include <cstdlib> // for exit()
#include "ncAudioBufferPlayer.h"
#include "ncAudioBuffer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A constructor creating a player from a shared buffer
ncAudioBufferPlayer::ncAudioBufferPlayer(ncAudioBuffer *audioBuffer)
	: audioBuffer_(audioBuffer)
{
	type_ = AUDIOBUFFERPLAYER_TYPE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioBufferPlayer::play()
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

			if (audioBuffer_)
			{
				alSourcei(sourceId_, AL_BUFFER, audioBuffer_->bufferId());
				// Setting OpenAL source looping only if not streaming
				alSourcei(sourceId_, AL_LOOPING, isLooping_);
			}

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

void ncAudioBufferPlayer::pause()
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

void ncAudioBufferPlayer::stop()
{
	switch (state_)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
			break;
		case STATE_PLAYING:
		{
			alSourceStop(sourceId_);
			state_ = STATE_STOPPED;
			break;
		}
		case STATE_PAUSED:
			break;
	}
}

/// Updates the player state
void ncAudioBufferPlayer::updateState()
{
	if (state_ == STATE_PLAYING)
	{
		ALenum eALState;
		alGetSourcei(sourceId_, AL_SOURCE_STATE, &eALState);
		if (eALState != AL_PLAYING)
		{
			state_ = STATE_STOPPED;
		}
	}
}
