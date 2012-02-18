#include <cstdlib> // for exit()
#include "ncAudioBuffer.h"
#include "ncAudioLoader.h"
#include "ncServiceLocator.h"
#include "ncArray.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAudioBuffer::ncAudioBuffer()
	: m_iChannels(0), m_iFrequency(0)
{
	m_eType = AUDIOBUFFER_TYPE;
	alGenBuffers(1, &m_uALId);
}

/// A constructor creating a buffer from a file
ncAudioBuffer::ncAudioBuffer(const char *pFilename)
	: m_iChannels(0), m_iFrequency(0)
{
	m_eType = AUDIOBUFFER_TYPE;
	alGenBuffers(1, &m_uALId);
	SetName(pFilename);

	ncAudioLoader audioLoader(pFilename);
	Load(audioLoader);
}

ncAudioBuffer::~ncAudioBuffer()
{
	alDeleteBuffers(1, &m_uALId);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

ncAudioBuffer* ncAudioBuffer::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::Indexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncAudioBuffer *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncAudioBuffer::FromId - Object of wrong type");
	//		exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncAudioBuffer::FromId - Object not found");
		return NULL;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads audio samples based on information from the audio loader
void ncAudioBuffer::Load(const ncAudioLoader& audioLoader)
{
	char *pBuffer;
	ALenum eFormat;
	m_iFrequency = audioLoader.Frequency();
	m_iChannels = audioLoader.Channels();

	if (m_iChannels == 1)
		eFormat = AL_FORMAT_MONO16;
	else if (m_iChannels == 2)
		eFormat = AL_FORMAT_STEREO16;
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncAudioBuffer::Load - Unsupported number of channels: %d", m_iChannels);
		exit(-1);
	}

	// Buffer size calculated as samples * channels * 16bit
	int iBufSize = audioLoader.NumSamples() * m_iChannels * 2;
	pBuffer = new char[iBufSize];

	audioLoader.Read(pBuffer, iBufSize);
	// On iOS alBufferDataStatic could be used instead
	alBufferData(m_uALId, eFormat, pBuffer, iBufSize, m_iFrequency);

	delete[] pBuffer;
}
