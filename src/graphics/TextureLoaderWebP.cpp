#include <cstdlib> // for exit()
#include "TextureLoaderWebP.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderWebP::TextureLoaderWebP(const char *filename)
	: ITextureLoader(filename)
{
	init();
}

TextureLoaderWebP::TextureLoaderWebP(IFile *fileHandle)
	: ITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderWebP::init()
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	// Loading the whole file in memory
	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);
	long int fileSize = fileHandle_->size();
	unsigned char *fileBuffer = new unsigned char[fileSize];
	fileHandle_->read(fileBuffer, fileSize);

	if (WebPGetInfo(fileBuffer, fileSize, &width_, &height_) == 0)
	{
		delete[] fileBuffer;
		LOGF("Cannot read WebP header");
		exit(EXIT_FAILURE);
	}

	LOGI_X("Header found: w:%d h:%d", width_, height_);

	WebPBitstreamFeatures features;
	if (WebPGetFeatures(fileBuffer, fileSize, &features) != VP8_STATUS_OK)
	{
		delete[] fileBuffer;
		LOGF("Cannot retrieve WebP features from headers");
		exit(EXIT_FAILURE);
	}

	LOGI_X("Bitstream features found: alpha:%d animation:%d format:%d",
	       features.has_alpha, features.has_animation, features.format);

	mipMapCount_ = 1; // No MIP Mapping
	texFormat_ = features.has_alpha ? TextureFormat(GL_RGBA) : TextureFormat(GL_RGB);
	bpp_ = features.has_alpha ? 4 : 3;
	long int decodedSize = width_ * height_ * bpp_;
	pixels_ =  new unsigned char[decodedSize];

	if (features.has_alpha)
	{
		if (WebPDecodeRGBAInto(fileBuffer, fileSize, pixels_, decodedSize, width_ * bpp_) == NULL)
		{
			delete[] fileBuffer;
			delete[] pixels_;
			LOGF("Cannot decode RGBA WebP image");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if (WebPDecodeRGBInto(fileBuffer, fileSize, pixels_, decodedSize, width_ * bpp_) == NULL)
		{
			delete[] fileBuffer;
			delete[] pixels_;
			LOGF("Cannot decode RGB WebP image");
			exit(EXIT_FAILURE);
		}
	}

	delete[] fileBuffer;
}

}
