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
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcOpenDevice failed: %x", alGetError());
		exit(-1);
	}

	m_pContext = alcCreateContext(m_pDevice, NULL);
	if (m_pContext == NULL) {
		alcCloseDevice(m_pDevice);
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcCreateContext failed: %x", alGetError());
		exit(-1);
	  }

	if (!alcMakeContextCurrent(m_pContext)) {
		alcDestroyContext(m_pContext);
		alcCloseDevice(m_pDevice);
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::ncALAudioDevice - alcMakeContextCurrent failed: %x", alGetError());
		exit(-1);
	  }

	alGenSources(s_iMaxSources, m_uSources);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, m_fGain);
}

ncALAudioDevice::~ncALAudioDevice()
{
	for (int i = 0; i < s_iMaxSources; i++)
		alSourcei(m_uSources[i], AL_BUFFER, AL_NONE);
	alDeleteSources(s_iMaxSources, m_uSources);

	alcDestroyContext(m_pContext);

	ALCboolean result = alcCloseDevice(m_pDevice);
	if (!result) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncALAudioDevice::~ncALAudioDevice - alcCloseDevice failed: %d", alGetError());
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

	for (int i = 0; i < s_iMaxSources; i++)
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
	if(!m_streamPlayers.isEmpty())
	{
		ncListNode<ncAudioStreamPlayer *> *pListNode = m_streamPlayers.Head();

		while (pListNode)
		{
			ncAudioStreamPlayer *pStreamPlayer = pListNode->Data();
			// Updating next before a potential removal
			pListNode = pListNode->Next();

			if (pStreamPlayer->isPlaying())
				pStreamPlayer->UpdateStream();
			else
			{
				// HACK: Linear search, not efficient
				// TODO: predicate class as ncList::Remove() argument
				m_streamPlayers.Remove(pStreamPlayer);
			}
		}
	}
}
