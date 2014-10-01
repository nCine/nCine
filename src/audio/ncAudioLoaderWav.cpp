#include <cstdlib> // for exit()
#include <cstring>
#include <stdint.h> // for WAV header
#include "ncAudioLoaderWav.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAudioLoaderWav::ncAudioLoaderWav(const char *pFilename)
	: ncIAudioLoader(pFilename)
{
	Init();
}

ncAudioLoaderWav::ncAudioLoaderWav(ncIFile *pFileHandle)
	: ncIAudioLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ncAudioLoaderWav::Read(char *pBuffer, int iBufSize) const
{
	long lBytes;
	long int lBufSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
		lBytes = m_pFileHandle->Read(pBuffer, iBufSize);

		if (lBytes == 0 && ferror(m_pFileHandle->Ptr()))
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Read - Error reading the file");
			exit(EXIT_FAILURE);
		}

		lBufSeek += lBytes;
	}
	while (lBytes > 0 && iBufSize - lBufSeek > 0);

	return lBufSeek;
}

void ncAudioLoaderWav::Rewind() const
{
	clearerr(m_pFileHandle->Ptr());
	m_pFileHandle->Seek(sizeof(WAV_header), SEEK_SET);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioLoaderWav::Init()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderWav::Init - Loading \"%s\"", m_pFileHandle->Filename());
	m_pFileHandle->Open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);

	WAV_header header;
	m_pFileHandle->Read(&header, sizeof(WAV_header));

	if (strncmp(header.cChunkID, "RIFF", 4) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Init - \"%s\" is not a WAV file", m_pFileHandle->Filename());
		exit(EXIT_FAILURE);
	}
	if (strncmp(header.cFormat, "WAVE", 4) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Init - \"%s\" is not a WAV file", m_pFileHandle->Filename());
		exit(EXIT_FAILURE);
	}
	if (strncmp(header.cSubchunk1Id, "fmt ", 4) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Init - \"%s\" is an invalid WAV file", m_pFileHandle->Filename());
		exit(EXIT_FAILURE);
	}

	if (ncIFile::Int16FromLE(header.uAudioFormat) != 1)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Init - Data in \"%s\" is not in PCM format", m_pFileHandle->Filename());
		exit(EXIT_FAILURE);
	}

	m_iBytesPerSample = ncIFile::Int16FromLE(header.uBitsPerSample) / 8;
	m_iChannels = ncIFile::Int16FromLE(header.uNumChannels);
	m_iFrequency = ncIFile::Int32FromLE(header.uSampleRate);

	m_ulNumSamples = ncIFile::Int32FromLE(header.uSubchunk2Size) / (m_iChannels * m_iBytesPerSample);
	m_fDuration = float(m_ulNumSamples) / m_iFrequency;

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderWav::Init - duration: %.2f, channels: %d, frequency: %d", m_fDuration, m_iChannels, m_iFrequency);
}
