#include <cstdlib> // for exit()
#include "ncIAudioLoader.h"
#include "ncAudioLoaderWav.h"
#ifdef WITH_VORBIS
	#include "ncAudioLoaderOgg.h"
#endif
#include "ncServiceLocator.h"
#include "ncIFile.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncIAudioLoader::ncIAudioLoader(const char *pFilename)
	: m_pFileHandle(NULL), m_iBytesPerSample(0), m_iChannels(0), m_iFrequency(0), m_ulNumSamples(0L), m_fDuration(0.0f)
{
	m_pFileHandle = ncIFile::CreateFileHandle(pFilename);

	// Warning: Cannot call a virtual Init() here, in the base constructor
}

ncIAudioLoader::ncIAudioLoader(ncIFile *pFileHandle)
	: m_pFileHandle(pFileHandle), m_iBytesPerSample(0), m_iChannels(0), m_iFrequency(0), m_ulNumSamples(0L), m_fDuration(0.0f)
{
	// Warning: Cannot call a virtual Init() here, in the base constructor
}

ncIAudioLoader::~ncIAudioLoader()
{
	if (m_pFileHandle)
		delete m_pFileHandle;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the proper audio loader according to the file extension
ncIAudioLoader* ncIAudioLoader::CreateFromFile(const char *pFilename)
{
	// Creating a handle from ncIFile static method to detect assets file
	ncIFile *pFileHandle = ncIFile::CreateFileHandle(pFilename);

	if (pFileHandle->HasExtension("wav"))
		return new ncAudioLoaderWav(pFileHandle);
#ifdef WITH_VORBIS
	else if (pFileHandle->HasExtension("ogg"))
		return new ncAudioLoaderOgg(pFileHandle);
#endif
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncIAudioLoader::CreateFromFile - Extension unknown \"%s\"", pFileHandle->Extension());
		delete pFileHandle;
		exit(EXIT_FAILURE);
	}
}
