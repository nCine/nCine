#include <stdlib.h> // for exit()
#include "ncIAudioLoader.h"
#include "ncAudioLoaderWav.h"
#include "ncAudioLoaderOgg.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the proper audio loader according to the file extension
ncIAudioLoader* ncIAudioLoader::CreateFromFile(const char *pFilename)
{
	ncFile fileHandle(pFilename);

	if (fileHandle.IsExtension("wav"))
		return new ncAudioLoaderWav(pFilename);
	else if (fileHandle.IsExtension("ogg"))
		return new ncAudioLoaderOgg(pFilename);
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncIAudioLoader::CreateFromFile - Extension unknown \"%s\"", fileHandle.Extension());
		exit(-1);
	}
}
