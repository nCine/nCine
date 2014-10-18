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

ncIAudioLoader::ncIAudioLoader(const char *filename)
	: fileHandle_(NULL), bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0L), duration_(0.0f)
{
	fileHandle_ = ncIFile::createFileHandle(filename);

	// Warning: Cannot call a virtual init() here, in the base constructor
}

ncIAudioLoader::ncIAudioLoader(ncIFile *fileHandle)
	: fileHandle_(fileHandle), bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0L), duration_(0.0f)
{
	// Warning: Cannot call a virtual init() here, in the base constructor
}

ncIAudioLoader::~ncIAudioLoader()
{
	if (fileHandle_)
	{
		delete fileHandle_;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the proper audio loader according to the file extension
ncIAudioLoader* ncIAudioLoader::createFromFile(const char *filename)
{
	// Creating a handle from ncIFile static method to detect assets file
	ncIFile *fileHandle = ncIFile::createFileHandle(filename);

	if (fileHandle->hasExtension("wav"))
	{
		return new ncAudioLoaderWav(fileHandle);
	}
#ifdef WITH_VORBIS
	else if (fileHandle->hasExtension("ogg"))
	{
		return new ncAudioLoaderOgg(fileHandle);
	}
#endif
	else
	{
		LOGF_X("Extension unknown \"%s\"", fileHandle->extension());
		delete fileHandle;
		exit(EXIT_FAILURE);
	}
}
