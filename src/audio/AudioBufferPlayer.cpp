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
	type_ = ObjectType::AUDIOBUFFER_PLAYER;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AudioBufferPlayer::play()
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

void AudioBufferPlayer::pause()
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

void AudioBufferPlayer::stop()
{
	switch (state_)
	{
		case PlayerState::INITIAL:
		case PlayerState::STOPPED:
			break;
		case PlayerState::PLAYING:
		{
			alSourceStop(sourceId_);
			state_ = PlayerState::STOPPED;
			break;
		}
		case PlayerState::PAUSED:
			break;
	}
}

void AudioBufferPlayer::updateState()
{
	if (state_ == PlayerState::PLAYING)
	{
		ALenum eALState;
		alGetSourcei(sourceId_, AL_SOURCE_STATE, &eALState);

		if (eALState != AL_PLAYING)
			state_ = PlayerState::STOPPED;
	}
}

}
