#include "common_macros.h"
#include "IImageLoader.h"
#ifdef WITH_PNG
	#include "ImageLoaderPng.h"
#endif
#ifdef WITH_WEBP
	#include "ImageLoaderWebP.h"
#endif
#include "IFile.h"
#include "FileSystem.h"

namespace ncine {

///////////////////////////////////////////////////////////
// InvalidImageLoader
///////////////////////////////////////////////////////////

InvalidImageLoader::InvalidImageLoader(nctl::UniquePtr<IFile> fileHandle)
    : IImageLoader(nctl::move(fileHandle))
{
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IImageLoader::IImageLoader(nctl::UniquePtr<IFile> fileHandle)
    : hasLoaded_(false), fileHandle_(nctl::move(fileHandle)),
      width_(0), height_(0), dataSize_(0)
{
}

IImageLoader::~IImageLoader() = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int IImageLoader::numChannels() const
{
	switch (format_)
	{
		case Format::R8: return 1;
		case Format::RG8: return 2;
		case Format::RGB8: return 3;
		case Format::RGBA8: return 4;
		default: return 0;
	}
}

nctl::UniquePtr<IImageLoader> IImageLoader::createFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	LOGI_X("Loading memory file: \"%s\" (0x%lx, %lu bytes)", bufferName, bufferPtr, bufferSize);
	return createLoader(nctl::move(IFile::createFromMemory(bufferName, bufferPtr, bufferSize)), bufferName);
}

nctl::UniquePtr<IImageLoader> IImageLoader::createFromFile(const char *filename)
{
	LOGI_X("Loading file: \"%s\"", filename);
	// Creating a handle from IFile static method to detect assets file
	return createLoader(nctl::move(IFile::createFileHandle(filename)), filename);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<IImageLoader> IImageLoader::createLoader(nctl::UniquePtr<IFile> fileHandle, const char *filename)
{
#ifdef WITH_PNG
	if (fs::hasExtension(filename, "png"))
		return nctl::makeUnique<ImageLoaderPng>(nctl::move(fileHandle));
#endif
#ifdef WITH_WEBP
	if (fs::hasExtension(filename, "webp"))
		return nctl::makeUnique<ImageLoaderWebP>(nctl::move(fileHandle));
#endif

	LOGF_X("Extension unknown: \"%s\"", fs::extension(filename));
	fileHandle.reset(nullptr);
	return nctl::makeUnique<InvalidImageLoader>(nctl::move(fileHandle));
}

}
