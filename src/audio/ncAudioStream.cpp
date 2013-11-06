#include <cstdlib> // for exit()
#include "ncAudioStream.h"
#include "ncIAudioLoader.h"
#include "ncServiceLocator.h"
#include "ncArray.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor creating an audio stream from an audio file
/*! Private constructor called only by ncAudioStreamPlayer */
ncAudioStream::ncAudioStream(const char *pFilename)
	: m_iNextAvailALBuffer(0), m_iFrequency(0)
{
	alGenBuffers(s_iNumBuffers, m_uALBuffers);
	m_pBuffer = new char[s_iBufSize];

	m_pAudioLoader = ncIAudioLoader::CreateFromFile(pFilename);
	m_iFrequency = m_pAudioLoader->Frequency();
	int iChannels = m_pAudioLoader->Channels();

	if (iChannels == 1)
		m_eFormat = AL_FORMAT_MONO16;
	else if (iChannels == 2)
		m_eFormat = AL_FORMAT_STEREO16;
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncAudioStream::ncAudioStream - Unsupported number of channels: %d", iChannels);
		exit(EXIT_FAILURE);
	}
}

ncAudioStream::~ncAudioStream()
{
	delete m_pAudioLoader;
	delete[] m_pBuffer;
	alDeleteBuffers(s_iNumBuffers, m_uALBuffers);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Enqueues new buffers and unqueues processed ones
/*! /return A flag indicating whether the stream has been entirely decoded and played or not */
bool ncAudioStream::Enqueue(ALuint uSource, bool bLooping)
{
	// Set to false when the queue is empty and there is no more data to decode
	bool bShouldKeepPlaying = true;

	ALint iProcessedBuffers;
	alGetSourcei(uSource, AL_BUFFERS_PROCESSED, &iProcessedBuffers);

	// Unqueueing
	while (iProcessedBuffers > 0)
	{
		ALuint uUnqueuedALBuffer;
		alSourceUnqueueBuffers(uSource, 1, &uUnqueuedALBuffer);
		m_iNextAvailALBuffer--;
		m_uALBuffers[m_iNextAvailALBuffer] = uUnqueuedALBuffer;
		iProcessedBuffers--;
	}


	// Queueing
	if (m_iNextAvailALBuffer < s_iNumBuffers)
	{
		ALuint uCurrBuffer = m_uALBuffers[m_iNextAvailALBuffer];

		long lBytes = m_pAudioLoader->Read(m_pBuffer, s_iBufSize);

		// EOF reached
		if(lBytes < s_iBufSize)
		{
			if (bLooping)
			{
				m_pAudioLoader->Rewind();
				long lMoreBytes = m_pAudioLoader->Read(m_pBuffer+lBytes, s_iBufSize-lBytes);
				lBytes += lMoreBytes;
			}
		}

		// If still decoding data then enqueue
		if (lBytes > 0)
		{
			// On iOS alBufferDataStatic could be used instead
			alBufferData(uCurrBuffer, m_eFormat, m_pBuffer, lBytes, m_iFrequency);
			alSourceQueueBuffers(uSource, 1, &uCurrBuffer);
			m_iNextAvailALBuffer++;
		}
		// If it has no more data to decode and the queue is empty
		else if (m_iNextAvailALBuffer == 0)
		{
			bShouldKeepPlaying = false;
			Stop(uSource);
		}
	}


	ALenum eState;
	alGetSourcei(uSource, AL_SOURCE_STATE, &eState);

	// Handle buffer underrun case
	if(eState != AL_PLAYING)
	{
		ALint iQueuedBuffers = 0;
		alGetSourcei(uSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
		if(iQueuedBuffers > 0)
		{
			// Need to restart play
			alSourcePlay(uSource);
		}
	}

	return bShouldKeepPlaying;
}

/// Unqueues any left buffer and rewinds the loader
void ncAudioStream::Stop(ALuint uSource)
{
	// In order to unqueue all the buffers, the source must be stopped first
	alSourceStop(uSource);

	ALint iProcessedBuffers;
	alGetSourcei(uSource, AL_BUFFERS_PROCESSED, &iProcessedBuffers);

	// Unqueueing
	while (iProcessedBuffers > 0)
	{
		ALuint uUnqueuedALBuffer;
		alSourceUnqueueBuffers(uSource, 1, &uUnqueuedALBuffer);
		m_iNextAvailALBuffer--;
		m_uALBuffers[m_iNextAvailALBuffer] = uUnqueuedALBuffer;
		iProcessedBuffers--;
	}

	m_pAudioLoader->Rewind();
}
