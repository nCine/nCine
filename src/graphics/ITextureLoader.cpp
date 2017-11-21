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

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ITextureLoader::ITextureLoader(const char *filename)
	: ITextureLoader(IFile::createFileHandle(filename))
{

}

ITextureLoader::ITextureLoader(IFile *fileHandle)
	: fileHandle_(fileHandle), width_(0), height_(0), bpp_(0), headerSize_(0), dataSize_(0),
	  mipMapCount_(1), mipDataOffsets_(nullptr), mipDataSizes_(nullptr), pixels_(nullptr)
{
	ASSERT(fileHandle);
}

ITextureLoader::~ITextureLoader()
{
	if (pixels_)
		delete[] pixels_;

	if (mipDataSizes_)
		delete[] mipDataSizes_;

	if (mipDataOffsets_)
		delete[] mipDataOffsets_;

	if (fileHandle_)
		delete fileHandle_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ITextureLoader::dataSize(unsigned int mipMapLevel) const
{
	long int dataSize = 0;

	if (mipMapCount_ > 1)
	{
		if (int(mipMapLevel) < mipMapCount_)
			dataSize = mipDataSizes_[mipMapLevel];
	}
	else if (mipMapLevel == 0)
		dataSize = dataSize_;

	return dataSize;
}

const GLubyte *ITextureLoader::pixels(unsigned int mipMapLevel) const
{
	GLubyte *pixels = nullptr;

	if (mipMapCount_ > 1)
	{
		if (int(mipMapLevel) < mipMapCount_)
			pixels = pixels_ + mipDataOffsets_[mipMapLevel];
	}
	else if (mipMapLevel == 0)
		pixels = pixels_;

	return pixels;
}

ITextureLoader *ITextureLoader::createFromFile(const char *filename)
{
	// Creating a handle from IFile static method to detect assets file
	IFile *fileHandle = IFile::createFileHandle(filename);
	LOGI_X("Loading file: \"%s\"", fileHandle->filename());

	if (fileHandle->hasExtension("dds"))
		return new TextureLoaderDds(fileHandle);
	else if (fileHandle->hasExtension("pvr"))
		return new TextureLoaderPvr(fileHandle);
	else if (fileHandle->hasExtension("ktx"))
		return new TextureLoaderKtx(fileHandle);
#ifdef WITH_PNG
	else if (fileHandle->hasExtension("png"))
		return new TextureLoaderPng(fileHandle);
#endif
#ifdef WITH_WEBP
	else if (fileHandle->hasExtension("webp"))
		return new TextureLoaderWebP(fileHandle);
#endif
#ifdef __ANDROID__
	else if (fileHandle->hasExtension("pkm"))
		return new TextureLoaderPkm(fileHandle);
#endif
	else
	{
		LOGF_X("Extension unknown: \"%s\"", fileHandle->extension());
		delete fileHandle;
		exit(EXIT_FAILURE);
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

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
		fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);

	dataSize_ = fileHandle_->size() - headerSize_;
	fileHandle_->seek(headerSize_, SEEK_SET);

	pixels_ =  new unsigned char[dataSize_];
	fileHandle_->read(pixels_, dataSize_);
}

}
