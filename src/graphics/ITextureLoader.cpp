#include "common_macros.h"
#include "ITextureLoader.h"
#include "TextureLoaderDds.h"
#include "TextureLoaderPvr.h"
#include "TextureLoaderKtx.h"
#ifdef WITH_OPENGLES
	#include "TextureLoaderPkm.h"
#endif

#include "IImageLoader.h"
#ifdef WITH_PNG
	#include "ImageLoaderPng.h"
#endif
#ifdef WITH_WEBP
	#include "ImageLoaderWebP.h"
#endif

#include "FileSystem.h"
#include "IFile.h"

namespace ncine {

namespace {

	TextureFormat imageToTextureFormat(IImageLoader::Format imgFormat)
	{
		switch (imgFormat)
		{
			default:
			case IImageLoader::Format::RGBA8: return TextureFormat(GL_RGBA8);
			case IImageLoader::Format::RGB8: return TextureFormat(GL_RGB8);
			case IImageLoader::Format::RG8: return TextureFormat(GL_RG8);
			case IImageLoader::Format::R8: return TextureFormat(GL_R8);
		}
	}

}

///////////////////////////////////////////////////////////
// TextureLoaderImage
///////////////////////////////////////////////////////////

/// An adapter class to consume data from image loader classes
class TextureLoaderImage : public ITextureLoader
{
  public:
	explicit TextureLoaderImage(nctl::UniquePtr<IImageLoader> imageLoader)
	    : ITextureLoader(nctl::move(imageLoader)) {}
};

///////////////////////////////////////////////////////////
// InvalidTextureLoader
///////////////////////////////////////////////////////////

InvalidTextureLoader::InvalidTextureLoader(nctl::UniquePtr<IFile> fileHandle)
    : ITextureLoader(nctl::move(fileHandle))
{
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ITextureLoader::ITextureLoader()
    : hasLoaded_(false), width_(0), height_(0),
      headerSize_(0), dataSize_(0), mipMapCount_(1)
{
}

ITextureLoader::ITextureLoader(nctl::UniquePtr<IFile> fileHandle)
    : hasLoaded_(false), fileHandle_(nctl::move(fileHandle)),
      width_(0), height_(0), headerSize_(0), dataSize_(0), mipMapCount_(1)
{
}

ITextureLoader::ITextureLoader(nctl::UniquePtr<IImageLoader> imageLoader)
    : hasLoaded_(imageLoader->hasLoaded()), fileHandle_(nctl::move(imageLoader->fileHandle_)),
      width_(imageLoader->width()), height_(imageLoader->height()),
      headerSize_(0), dataSize_(imageLoader->dataSize()), mipMapCount_(1),
      texFormat_(imageToTextureFormat(imageLoader->format())), pixels_(nctl::move(imageLoader->pixels_))
{
}

ITextureLoader::~ITextureLoader() = default;

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
	if (fs::hasExtension(filename, "dds"))
		return nctl::makeUnique<TextureLoaderDds>(nctl::move(fileHandle));
	else if (fs::hasExtension(filename, "pvr"))
		return nctl::makeUnique<TextureLoaderPvr>(nctl::move(fileHandle));
	else if (fs::hasExtension(filename, "ktx"))
		return nctl::makeUnique<TextureLoaderKtx>(nctl::move(fileHandle));
#ifdef WITH_PNG
	else if (fs::hasExtension(filename, "png"))
	{
		nctl::UniquePtr<ImageLoaderPng> imgLoader = nctl::makeUnique<ImageLoaderPng>(nctl::move(fileHandle));
		return nctl::makeUnique<TextureLoaderImage>(nctl::move(imgLoader));
	}
#endif
#ifdef WITH_WEBP
	else if (fs::hasExtension(filename, "webp"))
	{
		nctl::UniquePtr<ImageLoaderWebP> imgLoader = nctl::makeUnique<ImageLoaderWebP>(nctl::move(fileHandle));
		return nctl::makeUnique<TextureLoaderImage>(nctl::move(imgLoader));
	}
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
