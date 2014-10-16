#include <cstdlib> // for exit()
#include <cstring>
#include "ncTextureLoaderPNG.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderPNG::ncTextureLoaderPNG(const char *filename)
	: ncITextureLoader(filename)
{
	init();
}

ncTextureLoaderPNG::ncTextureLoaderPNG(ncIFile *fileHandle)
	: ncITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderPNG::init()
{
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPNG::init - Loading \"%s\"", fileHandle_->filename());

	const int SignatureLength = 8;
	unsigned char signature[SignatureLength];
	fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);
	fileHandle_->read(signature, SignatureLength);

	// Checking PNG signature
	if (!png_check_sig(signature, SignatureLength))
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::init - PNG signature check failed");
		exit(EXIT_FAILURE);
	}

	// Get PNG file info struct (memory is allocated by libpng)
	png_structp pngPtr = NULL;
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (pngPtr == NULL)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::init - Cannot create png read structure");
		exit(EXIT_FAILURE);
	}

	// Get PNG image data info struct (memory is allocated by libpng)
	png_infop infoPtr = NULL;
	infoPtr = png_create_info_struct(pngPtr);

	if (infoPtr == NULL)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::init - Cannot create png info structure");
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		exit(EXIT_FAILURE);
	}

	// Setting custom read function that uses an ncIFile as input
	png_set_read_fn(pngPtr, fileHandle_, readFromFileHandle);
	// Telling libpng the signature has already be read
	png_set_sig_bytes(pngPtr, SignatureLength);

	png_read_info(pngPtr, infoPtr);

	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bitDepth = 0;
	int colorType = -1;
	png_uint_32 retVal = png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);

	if (retVal != 1)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::init - Cannot create png info structure");
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		exit(EXIT_FAILURE);
	}

	width_ = width;
	height_ = height;
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPNG::init - Header found: w:%d h:%d", width_, height_);

	switch (colorType)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			texFormat_ = ncTextureFormat(GL_RGBA);
			break;
		case PNG_COLOR_TYPE_RGB:
			texFormat_ = ncTextureFormat(GL_RGB);
			break;
		case PNG_COLOR_TYPE_GRAY:
			texFormat_ = ncTextureFormat(GL_ALPHA);;
			break;
		default:
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::init - Color type not supported: %d", colorType);
			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
			exit(EXIT_FAILURE);
			break;
	}

	// Row size in bytes
	const png_uint_32 bytesPerRow = png_get_rowbytes(pngPtr, infoPtr);

	pixels_ = new unsigned char[bytesPerRow * height_];

	png_bytep *rowPointers = new png_bytep[height_];
	for (int i = 0; i < height_; i++)
	{
		rowPointers[i] = pixels_ + i * bytesPerRow;
	}

	// Decoding png data through row pointers
	png_read_image(pngPtr, rowPointers);

	delete[] rowPointers;

	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
}

void ncTextureLoaderPNG::readFromFileHandle(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead)
{
	ncIFile *fileHandle = reinterpret_cast<ncIFile *>(png_get_io_ptr(pngPtr));

	unsigned long int bytesRead = fileHandle->read(outBytes, byteCountToRead);
	if (bytesRead != byteCountToRead)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncTextureLoaderPNG::readFromFileHandle - Read %l bytes instead of %u: %d",
			bytesRead, byteCountToRead);
	}
}
