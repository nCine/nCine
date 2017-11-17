#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "AudioBufferPlayer.h"
#include "AudioBuffer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBufferPlayer::AudioBufferPlayer(AudioBuffer *audioBuffer)
	: audioBuffer_(audioBuffer)
{
	ASSERT(audioBuffer);
	type_ = AUDIOBUFFERPLAYER_TYPE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioBufferPlayer::play()
{
	switch (state_)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
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

			if (audioBuffer_)
			{
				alSourcei(sourceId_, AL_BUFFER, audioBuffer_->bufferId());
				// Setting OpenAL source looping only if not streaming
				alSourcei(sourceId_, AL_LOOPING, isLooping_);
			}

			alSourcef(sourceId_, AL_GAIN, gain_);
			alSourcef(sourceId_, AL_PITCH, pitch_);
			alSourcefv(sourceId_, AL_POSITION, position_.data());

			alSourcePlay(sourceId_);
			state_ = STATE_PLAYING;

			theServiceLocator().audioDevice().registerPlayer(this);
			break;
		}
		case STATE_PLAYING:
			break;
		case STATE_PAUSED:
		{
			alSourcePlay(sourceId_);
			state_ = STATE_PLAYING;

			theServiceLocator().audioDevice().registerPlayer(this);
			break;
		}
	}
}

void AudioBufferPlayer::pause()
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

void AudioBufferPlayer::stop()
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

void AudioBufferPlayer::updateState()
{
	if (state_ == STATE_PLAYING)
	{
		ALenum eALState;
		alGetSourcei(sourceId_, AL_SOURCE_STATE, &eALState);

		if (eALState != AL_PLAYING)
			state_ = STATE_STOPPED;
	}
}

}
