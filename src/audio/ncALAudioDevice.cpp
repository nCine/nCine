#include <cstdlib> // for exit()
#include "ncALAudioDevice.h"
#include "ncAudioBufferPlayer.h"
#include "ncAudioStreamPlayer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncALAudioDevice::ncALAudioDevice()
	: m_pDevice(NULL), m_pContext(NULL), m_fGain(1.0f)
{
	m_pDevice = alcOpenDevice(NULL);
	if (m_pDevice == NULL) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcOpenDevice failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	m_pContext = alcCreateContext(m_pDevice, NULL);
	if (m_pContext == NULL) {
		alcCloseDevice(m_pDevice);
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcCreateContext failed: %x", alGetError());
		exit(EXIT_FAILURE);
	  }

	if (!alcMakeContextCurrent(m_pContext)) {
		alcDestroyContext(m_pContext);
		alcCloseDevice(m_pDevice);
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcMakeContextCurrent failed: %x", alGetError());
		exit(EXIT_FAILURE);
	  }

	alGenSources(s_uMaxSources, m_uSources);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, m_fGain);
}

ncALAudioDevice::~ncALAudioDevice()
{
	for (unsigned int i = 0; i < s_uMaxSources; i++)
		alSourcei(m_uSources[i], AL_BUFFER, AL_NONE);
	alDeleteSources(s_uMaxSources, m_uSources);

	alcDestroyContext(m_pContext);

	ALCboolean result = alcCloseDevice(m_pDevice);
	if (!result) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::~ncALAudioDevice - alcCloseDevice failed: %d", alGetError());
		exit(EXIT_FAILURE);
	}
}


///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncALAudioDevice::SetGain(ALfloat fGain)
{
	m_fGain = fGain;
	alListenerf(AL_GAIN, m_fGain);
}

void ncALAudioDevice::StopPlayers()
{
	for(ncList<ncIAudioPlayer *>::Const_Iterator i = m_players.Begin(); i != m_players.End(); i++)
		(*i)->Stop();

	m_players.Clear();
}

void ncALAudioDevice::PausePlayers()
{
	for(ncList<ncIAudioPlayer *>::Const_Iterator i = m_players.Begin(); i != m_players.End(); i++)
		(*i)->Pause();

	m_players.Clear();
}

int ncALAudioDevice::NextAvailableSource()
{
	ALint iState;

	for (unsigned int i = 0; i < s_uMaxSources; i++)
	{
		alGetSourcei(m_uSources[i], AL_SOURCE_STATE, &iState);
		if (iState != AL_PLAYING && iState != AL_PAUSED)
			return m_uSources[i];
	}

	return -1;
}

void ncALAudioDevice::RegisterPlayer(ncIAudioPlayer *pPlayer)
{
	m_players.InsertBack(pPlayer);
}

void ncALAudioDevice::UpdatePlayers()
{
	ncList<ncIAudioPlayer *>::Const_Iterator i = m_players.Begin();
	while(i != m_players.End())
	{
		if ((*i)->isPlaying())
		{
			(*i)->UpdateState();
			i++;
		}
		else
			m_players.Remove(i++);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncALAudioDevice::StopOrPauseBufferPlayers(bool bStop)
{
	ncList<ncIAudioPlayer *>::Const_Iterator i = m_players.Begin();
	while(i != m_players.End())
	{
		if ((*i)->Type() == ncAudioBufferPlayer::sType())
		{
			if (bStop)
				(*i)->Stop();
			else
				(*i)->Pause();
			m_players.Remove(i++);
		}
		else
			i++;
	}
}

void ncALAudioDevice::StopOrPauseStreamPlayers(bool bStop)
{
	ncList<ncIAudioPlayer *>::Const_Iterator i = m_players.Begin();
	while(i != m_players.End())
	{
		if ((*i)->Type() == ncAudioStreamPlayer::sType())
		{
			if (bStop)
				(*i)->Stop();
			else
				(*i)->Pause();
			m_players.Remove(i++);
		}
		else
			i++;
	}
}
