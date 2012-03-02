#include <stdlib.h> // for exit()
#include "ncIAudioLoader.h"
#include "ncAudioLoaderWav.h"
#include "ncAudioLoaderOgg.h"
#include "ncServiceLocator.h"
#include "ncStandardFile.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the proper audio loader according to the file extension
ncIAudioLoader* ncIAudioLoader::CreateFromFile(const char *pFilename)
{
	ncStandardFile fileHandle(pFilename);

	if (fileHandle.HasExtension("wav"))
		return new ncAudioLoaderWav(pFilename);
	else if (fileHandle.HasExtension("ogg"))
		return new ncAudioLoaderOgg(pFilename);
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncIAudioLoader::CreateFromFile - Extension unknown \"%s\"", fileHandle.Extension());
		exit(-1);
	}
}
