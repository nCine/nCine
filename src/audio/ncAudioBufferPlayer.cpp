#include <cstdlib> // for exit()
#include "ncAudioBufferPlayer.h"
#include "ncAudioBuffer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A constructor creating a player from a shared buffer
ncAudioBufferPlayer::ncAudioBufferPlayer(ncAudioBuffer *pBuffer)
	: m_pBuffer(pBuffer)
{
	m_eType = AUDIOBUFFERPLAYER_TYPE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioBufferPlayer::Play()
{
	switch(m_eState)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
		{
			// iSource is a signed integer in order to check for unavailble source return value.
			// It is then converted to an OpenAL unsigned integer to be used as a valid source.
			int iSource = ncServiceLocator::AudioDevice().NextAvailableSource();
			// No sources available
			if (iSource < 0)
				return;
			m_uSource = iSource;

			if (m_pBuffer)
			{
				alSourcei(m_uSource, AL_BUFFER, m_pBuffer->ALId());
				// Setting OpenAL source looping only if not streaming
				alSourcei(m_uSource, AL_LOOPING, m_bLooping);
			}

			alSourcef(m_uSource, AL_GAIN, m_fGain);
			alSourcef(m_uSource, AL_PITCH, m_fPitch);
			alSourcefv(m_uSource, AL_POSITION, m_fPosition);

			alSourcePlay(m_uSource);
			m_eState = STATE_PLAYING;

			ncServiceLocator::AudioDevice().RegisterPlayer(this);
		}
			break;
		case STATE_PLAYING:
			break;
		case STATE_PAUSED:
		{
			alSourcePlay(m_uSource);
			m_eState = STATE_PLAYING;

			ncServiceLocator::AudioDevice().RegisterPlayer(this);
		}
			break;
	}
}

void ncAudioBufferPlayer::Pause()
{
	switch(m_eState)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
			break;
		case STATE_PLAYING:
		{
			alSourcePause(m_uSource);
			m_eState = STATE_PAUSED;
		}
			break;
		case STATE_PAUSED:
			break;
	}
}

void ncAudioBufferPlayer::Stop()
{
	switch(m_eState)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
			break;
		case STATE_PLAYING:
		{
			alSourceStop(m_uSource);
			m_eState = STATE_STOPPED;
		}
			break;
		case STATE_PAUSED:
			break;
	}
}

/// Updates the player state
void ncAudioBufferPlayer::UpdateState()
{
	if (m_eState == STATE_PLAYING)
	{
		ALenum eALState;
		alGetSourcei(m_uSource, AL_SOURCE_STATE, &eALState);
		if (eALState != AL_PLAYING)
			m_eState = STATE_STOPPED;
	}
}
