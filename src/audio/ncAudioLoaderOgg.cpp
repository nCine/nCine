#include <stdlib.h> // for exit()
#include <cstring>
#include "ncAudioLoaderOgg.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAudioLoaderOgg::ncAudioLoaderOgg(const char *pFilename)
	: ncIAudioLoader(pFilename)
{
	Init();
}

ncAudioLoaderOgg::~ncAudioLoaderOgg()
{
	ov_clear(&m_oggFile);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ncAudioLoaderOgg::Read(char *pBuffer, int iBufSize) const
{
	static int iBitStream = 0;
	long lBytes;
	long int lBufSeek = 0;

	do {
		// Read up to a buffer's worth of decoded sound data
#ifdef __ANDROID__
		lBytes = ov_read(&m_oggFile, pBuffer+lBufSeek, iBufSize-lBufSeek, &iBitStream);
#else
		// 0 - little endian, 2 - 16bit, 1 - signed
		lBytes = ov_read(&m_oggFile, pBuffer+lBufSeek, iBufSize-lBufSeek, 0, 2, 1, &iBitStream);
#endif

		if (lBytes < 0)
		{
			ov_clear(&m_oggFile);
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::Read - Error decoding at bitstream %d", iBitStream);
			exit(-1);
		}

		// Reset the static variable at the end of a decoding process
		if (lBytes <= 0)
			iBitStream = 0;

		lBufSeek += lBytes;
	}
	while (lBytes > 0 && iBufSize-lBufSeek > 0);

	return lBufSeek;
}

void ncAudioLoaderOgg::Rewind() const
{
	ov_raw_seek(&m_oggFile, 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioLoaderOgg::Init()
{
	vorbis_info *pInfo;

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderOgg::Load - Loading \"%s\"", m_fileHandle.Filename());

	// File is closed by ov_clear()
	m_fileHandle.SetCloseOnExit(false);

#ifdef __ANDROID__
	m_fileHandle.FOpen("rb");

	if (ov_open(m_fileHandle.Ptr(), &m_oggFile, NULL, 0) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::Load - Cannot open \"%s\" with ov_open", m_fileHandle.Filename());
		m_fileHandle.Close();
		exit(-1);
	}
#else
	if (ov_fopen(m_fileHandle.Filename(), &m_oggFile) != 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::Load - Cannot open \"%s\" with ov_fopen", m_fileHandle.Filename());
		exit(-1);
	}
#endif

	// Get some information about the OGG file
	pInfo = ov_info(&m_oggFile, -1);

	m_iBytesPerSample = 2; // Ogg is always 16 bits
	m_iChannels = pInfo->channels;
	m_iFrequency = pInfo->rate;

	m_ulNumSamples = ov_pcm_total(&m_oggFile, -1);
	m_fDuration = float(ov_time_total(&m_oggFile, -1));

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderOgg::Load - duration: %.2f, channels: %d, frequency: %d", m_fDuration, m_iChannels, m_iFrequency);
}
