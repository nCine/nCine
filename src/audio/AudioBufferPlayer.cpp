#define NCINE_INCLUDE_OPENAL
#include "common_headers.h"
#include "AudioBufferPlayer.h"
#include "AudioBuffer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioBufferPlayer::AudioBufferPlayer(AudioBuffer *audioBuffer)
    : IAudioPlayer(ObjectType::AUDIOBUFFER_PLAYER), audioBuffer_(audioBuffer)
{
	ASSERT(audioBuffer);
	if (audioBuffer)
		setName(audioBuffer->name().data());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int AudioBufferPlayer::bufferId() const
{
	const unsigned int bufferId = audioBuffer_ ? audioBuffer_->bufferId() : 0U;
	return (state_ != PlayerState::INITIAL && state_ != PlayerState::STOPPED) ? bufferId : 0U;
}

int AudioBufferPlayer::bytesPerSample() const
{
	return (audioBuffer_ ? audioBuffer_->bytesPerSample() : 0);
}

int AudioBufferPlayer::numChannels() const
{
	return (audioBuffer_ ? audioBuffer_->numChannels() : 0);
}

int AudioBufferPlayer::frequency() const
{
	return (audioBuffer_ ? audioBuffer_->frequency() : 0);
}

unsigned long int AudioBufferPlayer::numSamples() const
{
	return (audioBuffer_ ? audioBuffer_->numSamples() : 0UL);
}

float AudioBufferPlayer::duration() const
{
	return (audioBuffer_ ? audioBuffer_->duration() : 0.0f);
}

unsigned long int AudioBufferPlayer::bufferSize() const
{
	return (audioBuffer_ ? audioBuffer_->bufferSize() : 0UL);
}

void AudioBufferPlayer::play()
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
		case PlayerState::PAUSED:
		{
			alSourceStop(sourceId_);
			// Detach the buffer from source
			alSourcei(sourceId_, AL_BUFFER, 0);

			sourceId_ = 0;
			state_ = PlayerState::STOPPED;
			break;
		}
	}
}

void AudioBufferPlayer::updateState()
{
	if (state_ == PlayerState::PLAYING)
	{
		ALenum alState;
		alGetSourcei(sourceId_, AL_SOURCE_STATE, &alState);

		if (alState != AL_PLAYING)
		{
			// Detach the buffer from source
			alSourcei(sourceId_, AL_BUFFER, 0);
			sourceId_ = 0;
			state_ = PlayerState::STOPPED;
		}
		else
			alSourcei(sourceId_, AL_LOOPING, isLooping_);
	}
}

}
