#include <stdlib.h> // for exit()
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

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ncAudioLoaderWav::Read(char *pBuffer, int iBufSize) const
{
	long lBytes;
	long int lBufSeek = 0;

	do {
		// Read up to a buffer's worth of decoded sound data
		lBytes = fread(pBuffer, sizeof(char), iBufSize, m_fileHandle.Ptr());

		if (lBytes == 0 && ferror(m_fileHandle.Ptr()))
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Read - Error reading the file");
			exit(-1);
		}

		lBufSeek += lBytes;
	}
	while (lBytes > 0 && iBufSize-lBufSeek > 0);

	return lBufSeek;
}

void ncAudioLoaderWav::Rewind() const
{
	clearerr(m_fileHandle.Ptr());
	fseek(m_fileHandle.Ptr(), sizeof(WAV_header), SEEK_SET);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioLoaderWav::Init()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderWav::Load - Loading \"%s\"", m_fileHandle.Filename());
	m_fileHandle.FOpen("rb");

	WAV_header header;
	fread(&header, sizeof(WAV_header), 1, m_fileHandle.Ptr());

	if (strncmp(header.cChunkID, "RIFF", 4) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Load - \"%s\" is not a WAV file", m_fileHandle.Filename());
		exit(-1);
	}
	if (strncmp(header.cFormat, "WAVE", 4) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Load - \"%s\" is not a WAV file", m_fileHandle.Filename());
		exit(-1);
	}
	if (strncmp(header.cSubchunk1Id, "fmt ", 4) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Load - \"%s\" is an invalid WAV file", m_fileHandle.Filename());
		exit(-1);
	}

	if (ncFile::Int16FromLE(header.uAudioFormat) != 1)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::Load - Data in \"%s\" is not in PCM format", m_fileHandle.Filename());
		exit(-1);
	}

	m_iBytesPerSample = ncFile::Int16FromLE(header.uBitsPerSample) / 8;
	m_iChannels = ncFile::Int16FromLE(header.uNumChannels);
	m_iFrequency = ncFile::Int32FromLE(header.uSampleRate);

	m_ulNumSamples = ncFile::Int32FromLE(header.uSubchunk2Size) / (m_iChannels * m_iBytesPerSample);
	m_fDuration = float(m_ulNumSamples) / m_iFrequency;

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderWav::Load - duration: %.2f, channels: %d, frequency: %d", m_fDuration, m_iChannels, m_iFrequency);
}
