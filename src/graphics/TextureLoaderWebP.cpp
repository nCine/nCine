#include <cstdlib> // for exit()
#include <cstring>
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

	mipMapCount_ = 1; // No MIP Mapping
	// HACK: assuming WebP always decodes to RGBA
	texFormat_ = TextureFormat(GL_RGBA);
	long int lDecodedSize = width_ * height_ * 4;
	pixels_ =  new unsigned char[lDecodedSize];

	if (WebPDecodeRGBAInto(fileBuffer, fileSize, pixels_, lDecodedSize, width_ * 4) == NULL)
	{
		delete[] fileBuffer;
		delete[] pixels_;
		LOGF("Cannot decode WebP image");
		exit(EXIT_FAILURE);
	}

	delete[] fileBuffer;
}

}
