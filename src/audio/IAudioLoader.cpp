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
	: IAudioLoader(IFile::createFileHandle(filename))
{
	// Warning: Cannot call a virtual init() here, in the base constructor
}

IAudioLoader::IAudioLoader(nctl::UniquePtr<IFile> fileHandle)
	: fileHandle_(nctl::move(fileHandle)), bytesPerSample_(0), numChannels_(0), frequency_(0), numSamples_(0L), duration_(0.0f)
{
	// Warning: Cannot call a virtual init() here, in the base constructor
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<IAudioLoader> IAudioLoader::createFromFile(const char *filename)
{
	// Creating a handle from IFile static method to detect assets file
	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filename);

	if (fileHandle->hasExtension("wav"))
		return nctl::makeUnique<AudioLoaderWav>(nctl::move(fileHandle));
#ifdef WITH_VORBIS
	else if (fileHandle->hasExtension("ogg"))
		return nctl::makeUnique<AudioLoaderOgg>(nctl::move(fileHandle));
#endif
	else
	{
		LOGF_X("Extension unknown \"%s\"", fileHandle->extension());
		fileHandle.reset(nullptr);
		exit(EXIT_FAILURE);
	}
}

}
