#include <cstdlib> // for exit()
#include <cstring>
#include "ncITextureLoader.h"
#include "ncTextureLoaderDDS.h"
#include "ncTextureLoaderPVR.h"
#include "ncTextureLoaderKTX.h"
#if defined(WITH_SDLIMAGE)
	#include "ncTextureLoaderSDL.h"
#elif defined(WITH_PNG)
	#include "ncTextureLoaderPNG.h"
#endif
#ifdef WITH_WEBP
	#include "ncTextureLoaderWebP.h"
#endif
#ifdef __ANDROID__
	#include "ncTextureLoaderETC.h"
#endif
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncITextureLoader::ncITextureLoader(const char *filename)
	: fileHandle_(NULL), width_(0), height_(0), bpp_(0), headerSize_(0), dataSize_(0),
	  mipMapCount_(1), mipDataOffsets_(NULL), mipDataSizes_(NULL), pixels_(NULL)
{
	fileHandle_ = ncIFile::createFileHandle(filename);
}

ncITextureLoader::ncITextureLoader(ncIFile *fileHandle)
	: fileHandle_(fileHandle), width_(0), height_(0), bpp_(0), headerSize_(0), dataSize_(0),
	  mipMapCount_(1), mipDataOffsets_(NULL), mipDataSizes_(NULL), pixels_(NULL)
{

}

ncITextureLoader::~ncITextureLoader()
{
	if (pixels_)
	{
		delete[] pixels_;
	}

	if (mipDataSizes_)
	{
		delete[] mipDataSizes_;
	}

	if (mipDataOffsets_)
	{
		delete[] mipDataOffsets_;
	}

	if (fileHandle_)
	{
		delete fileHandle_;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the texture data size in bytes for the specified MIP map level
long ncITextureLoader::dataSize(unsigned int mipMapLevel) const
{
	long int dataSize = 0;

	if (mipMapCount_ > 1)
	{
		if (int(mipMapLevel) < mipMapCount_)
		{
			dataSize = mipDataSizes_[mipMapLevel];
		}
	}
	else if (mipMapLevel == 0)
	{
		dataSize = dataSize_;
	}

	return dataSize;
}

/// Returns the pointer to pixel data for the specified MIP map level
const GLubyte* ncITextureLoader::pixels(unsigned int mipMapLevel) const
{
	GLubyte *pixels = NULL;

	if (mipMapCount_ > 1)
	{
		if (int(mipMapLevel) < mipMapCount_)
		{
			pixels = pixels_ + mipDataOffsets_[mipMapLevel];
		}
	}
	else if (mipMapLevel == 0)
	{
		pixels = pixels_;
	}

	return pixels;
}

/// Returns the proper texture loader according to the file extension
ncITextureLoader* ncITextureLoader::createFromFile(const char *filename)
{
	// Creating a handle from ncIFile static method to detect assets file
	ncIFile *fileHandle = ncIFile::createFileHandle(filename);
	LOGI_X("Loading file: \"%s\"", fileHandle->filename());

	if (fileHandle->hasExtension("dds"))
	{
		return new ncTextureLoaderDDS(fileHandle);
	}
	else if (fileHandle->hasExtension("pvr"))
	{
		return new ncTextureLoaderPVR(fileHandle);
	}
	else if (fileHandle->hasExtension("ktx"))
	{
		return new ncTextureLoaderKTX(fileHandle);
	}
#if defined(WITH_SDLIMAGE)
	else if (fileHandle->hasExtension("png") || fileHandle->hasExtension("jpg"))
	{
		return new ncTextureLoaderSDL(fileHandle);
	}
#elif defined(WITH_PNG)
	else if (fileHandle->hasExtension("png"))
	{
		return new ncTextureLoaderPNG(fileHandle);
	}
#endif
#ifdef WITH_WEBP
	else if (fileHandle->hasExtension("webp"))
	{
		return new ncTextureLoaderWebP(fileHandle);
	}
#endif
#ifdef __ANDROID__
	else if (fileHandle->hasExtension("pkm"))
	{
		return new ncTextureLoaderETC(fileHandle);
	}
#endif
	else
	{
		LOGF_X("Extension unknown: \"%s\"", fileHandle->extension());
		delete fileHandle;
		exit(EXIT_FAILURE);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads pixel data from a texture file holding either compressed or uncompressed data
void ncITextureLoader::loadPixels(GLenum internalFormat)
{
	loadPixels(internalFormat, 0);
}

/// Loads pixel data from a texture file holding either compressed or uncompressed data, overriding pixel type
void ncITextureLoader::loadPixels(GLenum internalFormat, GLenum type)
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());
	if (type) // overriding pixel type
	{
		texFormat_ = ncTextureFormat(internalFormat, type);
	}
	else
	{
		texFormat_ = ncTextureFormat(internalFormat);
	}

	// If the file has not been already opened by a header reader method
	if (fileHandle_->isOpened() == false)
	{
		fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);
	}

	dataSize_ = fileHandle_->size() - headerSize_;
	fileHandle_->seek(headerSize_, SEEK_SET);

	pixels_ =  new unsigned char[dataSize_];
	fileHandle_->read(pixels_, dataSize_);
}
