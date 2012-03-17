#include <cstdlib> // for exit()
#include "ncIAudioLoader.h"
#include "ncAudioLoaderWav.h"
#include "ncAudioLoaderOgg.h"
#include "ncServiceLocator.h"
#include "ncIFile.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the proper audio loader according to the file extension
ncIAudioLoader* ncIAudioLoader::CreateFromFile(const char *pFilename)
{
	// Creating a handle from ncIFile static method to detect assets file
	ncIFile *pFileHandle = ncIFile::CreateFileHandle(pFilename);

	if (pFileHandle->HasExtension("wav"))
	{
		delete pFileHandle;
		return new ncAudioLoaderWav(pFilename);
	}
	else if (pFileHandle->HasExtension("ogg"))
	{
		delete pFileHandle;
		return new ncAudioLoaderOgg(pFilename);
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncIAudioLoader::CreateFromFile - Extension unknown \"%s\"", pFileHandle->Extension());
		delete pFileHandle;
		exit(-1);
	}
}
