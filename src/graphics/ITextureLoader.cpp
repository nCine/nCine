#include "common_macros.h"
#include "ITextureLoader.h"
#include "TextureLoaderDds.h"
#include "TextureLoaderPvr.h"
#include "TextureLoaderKtx.h"
#ifdef WITH_PNG
	#include "TextureLoaderPng.h"
#endif
#ifdef WITH_WEBP
	#include "TextureLoaderWebP.h"
#endif
#ifdef __ANDROID__
	#include "TextureLoaderPkm.h"
#endif
#include "FileSystem.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ITextureLoader::ITextureLoader()
    : hasLoaded_(false), width_(0), height_(0),
      bpp_(0), headerSize_(0), dataSize_(0), mipMapCount_(1)
{
}

ITextureLoader::ITextureLoader(nctl::UniquePtr<IFile> fileHandle)
    : hasLoaded_(false), fileHandle_(nctl::move(fileHandle)), width_(0),
      height_(0), bpp_(0), headerSize_(0), dataSize_(0), mipMapCount_(1)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ITextureLoader::dataSize(unsigned int mipMapLevel) const
{
	long int dataSize = 0;

	if (mipMapCount_ > 1 && int(mipMapLevel) < mipMapCount_)
		dataSize = mipDataSizes_[mipMapLevel];
	else if (mipMapLevel == 0)
		dataSize = dataSize_;

	return dataSize;
}

const GLubyte *ITextureLoader::pixels(unsigned int mipMapLevel) const
{
	const GLubyte *pixels = nullptr;

	if (pixels_ != nullptr)
	{
		if (mipMapCount_ > 1 && int(mipMapLevel) < mipMapCount_)
			pixels = pixels_.get() + mipDataOffsets_[mipMapLevel];
		else if (mipMapLevel == 0)
			pixels = pixels_.get();
	}

	return pixels;
}

nctl::UniquePtr<ITextureLoader> ITextureLoader::createFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	LOGI_X("Loading memory file: \"%s\" (0x%lx, %lu bytes)", bufferName, bufferPtr, bufferSize);
	return createLoader(nctl::move(IFile::createFromMemory(bufferName, bufferPtr, bufferSize)), bufferName);
}

nctl::UniquePtr<ITextureLoader> ITextureLoader::createFromFile(const char *filename)
{
	LOGI_X("Loading file: \"%s\"", filename);
	// Creating a handle from IFile static method to detect assets file
	return createLoader(nctl::move(IFile::createFileHandle(filename)), filename);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<ITextureLoader> ITextureLoader::createLoader(nctl::UniquePtr<IFile> fileHandle, const char *filename)
{
	fileHandle->setExitOnFailToOpen(false);

	if (fs::hasExtension(filename, "dds"))
		return nctl::makeUnique<TextureLoaderDds>(nctl::move(fileHandle));
	else if (fs::hasExtension(filename, "pvr"))
		return nctl::makeUnique<TextureLoaderPvr>(nctl::move(fileHandle));
	else if (fs::hasExtension(filename, "ktx"))
		return nctl::makeUnique<TextureLoaderKtx>(nctl::move(fileHandle));
#ifdef WITH_PNG
	else if (fs::hasExtension(filename, "png"))
		return nctl::makeUnique<TextureLoaderPng>(nctl::move(fileHandle));
#endif
#ifdef WITH_WEBP
	else if (fs::hasExtension(filename, "webp"))
		return nctl::makeUnique<TextureLoaderWebP>(nctl::move(fileHandle));
#endif
#ifdef __ANDROID__
	else if (fs::hasExtension(filename, "pkm"))
		return nctl::makeUnique<TextureLoaderPkm>(nctl::move(fileHandle));
#endif
	else
	{
		LOGF_X("Extension unknown: \"%s\"", fs::extension(filename));
		fileHandle.reset(nullptr);
		return nctl::makeUnique<InvalidTextureLoader>(nctl::move(fileHandle));
	}
}

void ITextureLoader::loadPixels(GLenum internalFormat)
{
	loadPixels(internalFormat, 0);
}

void ITextureLoader::loadPixels(GLenum internalFormat, GLenum type)
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());
	if (type) // overriding pixel type
		texFormat_ = TextureFormat(internalFormat, type);
	else
		texFormat_ = TextureFormat(internalFormat);

	// If the file has not been already opened by a header reader method
	if (fileHandle_->isOpened() == false)
		fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);

	dataSize_ = fileHandle_->size() - headerSize_;
	fileHandle_->seek(headerSize_, SEEK_SET);

	pixels_ = nctl::makeUnique<unsigned char[]>(dataSize_);
	fileHandle_->read(pixels_.get(), dataSize_);
}

}
