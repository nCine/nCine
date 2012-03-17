#include <cstdlib> // for exit()
#include "ncAudioStreamPlayer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A constructor creating a player from a file
ncAudioStreamPlayer::ncAudioStreamPlayer(const char *pFilename)
	: m_stream(pFilename)
{
	m_eType = AUDIOSTREAMPLAYER_TYPE;
	SetName(pFilename);
}

ncAudioStreamPlayer::~ncAudioStreamPlayer()
{
	if (m_eState == STATE_PLAYING)
		m_stream.Stop(m_uSource);
}


///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioStreamPlayer::Play()
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

void ncAudioStreamPlayer::Pause()
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

void ncAudioStreamPlayer::Stop()
{
	switch(m_eState)
	{
		case STATE_INITIAL:
		case STATE_STOPPED:
			break;
		case STATE_PLAYING:
		{
			// Stop the source then unqueue every buffer
			m_stream.Stop(m_uSource);

			m_eState = STATE_STOPPED;
		}
			break;
		case STATE_PAUSED:
			break;
	}
}

/// Updates the player state and the stream buffer queue
void ncAudioStreamPlayer::UpdateState()
{
	if (m_eState == STATE_PLAYING)
	{
		bool bShouldStillPlay = m_stream.Enqueue(m_uSource, m_bLooping);
		if (bShouldStillPlay == false)
			m_eState = STATE_STOPPED;
	}
}
