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
			ALuint uSource = ncServiceLocator::GetAudioDevice().NextAvailableSource();
			// No sources available
			if (uSource < 0)
				return;
			m_uSource = uSource;

			alSourcef(m_uSource, AL_GAIN, m_fGain);
			alSourcef(m_uSource, AL_PITCH, m_fPitch);
			alSourcefv(m_uSource, AL_POSITION, m_fPosition);

			alSourcePlay(m_uSource);
			m_eState = STATE_PLAYING;

			ncServiceLocator::GetAudioDevice().RegisterStreamPlayer(this);
		}
			break;
		case STATE_PLAYING:
			break;
		case STATE_PAUSED:
		{
			alSourcePlay(m_uSource);
			m_eState = STATE_PLAYING;

			ncServiceLocator::GetAudioDevice().RegisterStreamPlayer(this);
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

/// Updates the stream buffer queue
void ncAudioStreamPlayer::UpdateStream()
{
	if (m_eState == STATE_PLAYING)
	{
		bool bShouldStillPlay = m_stream.Enqueue(m_uSource, m_bLooping);
		if (bShouldStillPlay == false)
			m_eState = STATE_STOPPED;
	}
}

ncAudioStreamPlayer* ncAudioStreamPlayer::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncAudioStreamPlayer *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncAudioStreamPlayer::FromId - Object of wrong type");
	//		exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncAudioStreamPlayer::FromId - Object not found");
		return NULL;
	}
}
