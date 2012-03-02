#include <cstdlib> // for exit()
#include "ncAudioBuffer.h"
#include "ncIAudioLoader.h"
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

	ncIAudioLoader* pAudioLoader = ncIAudioLoader::CreateFromFile(pFilename);
	Load(pAudioLoader);
	delete pAudioLoader;
}

ncAudioBuffer::~ncAudioBuffer()
{
	alDeleteBuffers(1, &m_uALId);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads audio samples based on information from the audio loader
void ncAudioBuffer::Load(const ncIAudioLoader *pAudioLoader)
{
	char *pBuffer;
	ALenum eFormat;
	m_iFrequency = pAudioLoader->Frequency();
	m_iChannels = pAudioLoader->Channels();

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
	int iBufSize = pAudioLoader->BufferSize();
	pBuffer = new char[iBufSize];

	pAudioLoader->Read(pBuffer, iBufSize);
	// On iOS alBufferDataStatic could be used instead
	alBufferData(m_uALId, eFormat, pBuffer, iBufSize, m_iFrequency);

	delete[] pBuffer;
}
