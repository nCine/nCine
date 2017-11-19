#include "common_macros.h"
#include "IAudioLoader.h"
#include "AudioLoaderWav.h"
#ifdef WITH_VORBIS
	#include "AudioLoaderOgg.h"
#endif
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IAudioLoader::IAudioLoader(const char *filename)
	: fileHandle_(nullptr), bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0L), duration_(0.0f)
{
	fileHandle_ = IFile::createFileHandle(filename);

	// Warning: Cannot call a virtual init() here, in the base constructor
}

IAudioLoader::IAudioLoader(IFile *fileHandle)
	: fileHandle_(fileHandle), bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0L), duration_(0.0f)
{
	// Warning: Cannot call a virtual init() here, in the base constructor
}

IAudioLoader::~IAudioLoader()
{
	if (fileHandle_)
		delete fileHandle_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

IAudioLoader *IAudioLoader::createFromFile(const char *filename)
{
	// Creating a handle from IFile static method to detect assets file
	IFile *fileHandle = IFile::createFileHandle(filename);

	if (fileHandle->hasExtension("wav"))
		return new AudioLoaderWav(fileHandle);
#ifdef WITH_VORBIS
	else if (fileHandle->hasExtension("ogg"))
		return new AudioLoaderOgg(fileHandle);
#endif
	else
	{
		LOGF_X("Extension unknown \"%s\"", fileHandle->extension());
		delete fileHandle;
		exit(EXIT_FAILURE);
	}
}

}
