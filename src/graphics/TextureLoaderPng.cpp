#include "common_macros.h"
#include "TextureLoaderPng.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderPng::TextureLoaderPng(const char *filename)
	: ITextureLoader(filename)
{
	init();
}

TextureLoaderPng::TextureLoaderPng(IFile *fileHandle)
	: ITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderPng::init()
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	const int SignatureLength = 8;
	unsigned char signature[SignatureLength];
	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);
	fileHandle_->read(signature, SignatureLength);

	// Checking PNG signature
	const int isValid = png_check_sig(signature, SignatureLength);
	FATAL_ASSERT_MSG(isValid, "PNG signature check failed");

	// Get PNG file info struct (memory is allocated by libpng)
	png_structp pngPtr = NULL;
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	FATAL_ASSERT_MSG(pngPtr, "Cannot create png read structure");

	// Get PNG image data info struct (memory is allocated by libpng)
	png_infop infoPtr = NULL;
	infoPtr = png_create_info_struct(pngPtr);

	if (infoPtr == NULL)
	{
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		FATAL_MSG("Cannot create png info structure");
	}

	// Setting custom read function that uses an IFile as input
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
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		FATAL_MSG("Cannot create png info structure");
	}

	width_ = width;
	height_ = height;
	mipMapCount_ = 1; // No MIP Mapping
	LOGI_X("Header found: w:%d h:%d bitDepth:%d colorType:%d", width_, height_, bitDepth, colorType);

	switch (colorType)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			texFormat_ = TextureFormat(GL_RGBA);
			bpp_ = 4;
			break;
		case PNG_COLOR_TYPE_RGB:
			texFormat_ = TextureFormat(GL_RGB);
			bpp_ = 3;
			break;
		case PNG_COLOR_TYPE_GRAY:
			texFormat_ = TextureFormat(GL_LUMINANCE);
			bpp_ = 1;
			break;
		default:
			png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
			FATAL_MSG_X("Color type not supported: %d", colorType);
			break;
	}

	// Row size in bytes
	const png_size_t bytesPerRow = png_get_rowbytes(pngPtr, infoPtr);

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

void TextureLoaderPng::readFromFileHandle(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead)
{
	IFile *fileHandle = reinterpret_cast<IFile *>(png_get_io_ptr(pngPtr));

	unsigned long int bytesRead = fileHandle->read(outBytes, static_cast<unsigned long int>(byteCountToRead));
	if (bytesRead > 0 && bytesRead != byteCountToRead)
	{
		LOGW_X("Read %ul bytes instead of %ul", bytesRead, byteCountToRead);
	}
}

}
