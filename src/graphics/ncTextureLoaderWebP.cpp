#include <cstdlib> // for exit()
#include <cstring>
#include "ncTextureLoaderWebP.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderWebP::ncTextureLoaderWebP(const char *filename)
	: ncITextureLoader(filename)
{
	init();
}

ncTextureLoaderWebP::ncTextureLoaderWebP(ncIFile *fileHandle)
	: ncITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderWebP::init()
{
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderWebP::init - Loading \"%s\"", fileHandle_->filename());

	// Loading the whole file in memory
	fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);
	long int fileSize = fileHandle_->size();
	unsigned char *fileBuffer = new unsigned char[fileSize];
	fileHandle_->read(fileBuffer, fileSize);

	if (WebPGetInfo(fileBuffer, fileSize, &width_, &height_) == 0)
	{
		delete[] fileBuffer;
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderWebP::init - Cannot read WebP header");
		exit(EXIT_FAILURE);
	}

	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderWebP::init - Header found: w:%d h:%d", width_, height_);

	mipMapCount_ = 1; // No MIP Mapping
	// HACK: assuming WebP always decodes to RGBA
	texFormat_ = ncTextureFormat(GL_RGBA);
	long int lDecodedSize = width_ * height_ * 4;
	pixels_ =  new unsigned char[lDecodedSize];

	if (WebPDecodeRGBAInto(fileBuffer, fileSize, pixels_, lDecodedSize, width_ * 4) == NULL)
	{
		delete[] fileBuffer;
		delete[] pixels_;
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderWebP::init - Cannot decode WebP image");
		exit(EXIT_FAILURE);
	}

	delete[] fileBuffer;
}
