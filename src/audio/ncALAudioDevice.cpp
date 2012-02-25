#include <cstdlib>
#include "ncALAudioDevice.h"
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
		exit(-1);
	}

	m_pContext = alcCreateContext(m_pDevice, NULL);
	if (m_pContext == NULL) {
		alcCloseDevice(m_pDevice);
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcCreateContext failed: %x", alGetError());
		exit(-1);
	  }

	if (!alcMakeContextCurrent(m_pContext)) {
		alcDestroyContext(m_pContext);
		alcCloseDevice(m_pDevice);
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcMakeContextCurrent failed: %x", alGetError());
		exit(-1);
	  }

	alGenSources(s_uMaxSources, m_uSources);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, m_fGain);
}

ncALAudioDevice::~ncALAudioDevice()
{
	for (int i = 0; i < s_uMaxSources; i++)
		alSourcei(m_uSources[i], AL_BUFFER, AL_NONE);
	alDeleteSources(s_uMaxSources, m_uSources);

	alcDestroyContext(m_pContext);

	ALCboolean result = alcCloseDevice(m_pDevice);
	if (!result) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::~ncALAudioDevice - alcCloseDevice failed: %d", alGetError());
		exit(-1);
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

int ncALAudioDevice::NextAvailableSource()
{
	ALint iState;

	for (int i = 0; i < s_uMaxSources; i++)
	{
		alGetSourcei(m_uSources[i], AL_SOURCE_STATE, &iState);
		if (iState != AL_PLAYING && iState != AL_PAUSED)
			return m_uSources[i];
	}

	return -1;
}

void ncALAudioDevice::RegisterStreamPlayer(ncAudioStreamPlayer *pStreamPlayer)
{
	m_streamPlayers.InsertBack(pStreamPlayer);
}

void ncALAudioDevice::UpdateStreams()
{
	ncList<ncAudioStreamPlayer *>::Const_Iterator i = m_streamPlayers.Begin();
	while(i != m_streamPlayers.End())
	{
		if ((*i)->isPlaying())
		{
			(*i)->UpdateStream();
			i++;
		}
		else
			m_streamPlayers.Remove(i++);
	}
}
