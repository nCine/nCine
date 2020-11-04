#include "return_macros.h"
#include "TextureLoaderPng.h"

namespace ncine {

namespace {

	const char *colorTypeString(int colorType)
	{
		switch (colorType)
		{
			case PNG_COLOR_TYPE_GRAY: return "PNG_COLOR_TYPE_GRAY";
			case PNG_COLOR_TYPE_PALETTE: return "PNG_COLOR_TYPE_PALETTE";
			case PNG_COLOR_TYPE_RGB: return "PNG_COLOR_TYPE_RGB";
			case PNG_COLOR_TYPE_RGB_ALPHA: return "PNG_COLOR_TYPE_RGB_ALPHA";
			case PNG_COLOR_TYPE_GRAY_ALPHA: return "PNG_COLOR_TYPE_GRAY_ALPHA";
			default: return "unknown";
		}
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderPng::TextureLoaderPng(nctl::UniquePtr<IFile> fileHandle)
    : ITextureLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	const int SignatureLength = 8;
	unsigned char signature[SignatureLength];
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());
	fileHandle_->read(signature, SignatureLength);

	// Checking PNG signature
	const int isValid = png_check_sig(signature, SignatureLength);
	RETURN_ASSERT_MSG(isValid, "PNG signature check failed");

	// Get PNG file info struct (memory is allocated by libpng)
	png_structp pngPtr = nullptr;
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	RETURN_ASSERT_MSG(pngPtr, "Cannot create png read structure");

	// Get PNG image data info struct (memory is allocated by libpng)
	png_infop infoPtr = nullptr;
	infoPtr = png_create_info_struct(pngPtr);

	if (infoPtr == nullptr)
	{
		png_destroy_read_struct(&pngPtr, nullptr, nullptr);
		RETURN_MSG("Cannot create png info structure");
	}

	// Setting custom read function that uses an IFile as input
	png_set_read_fn(pngPtr, fileHandle_.get(), readFromFileHandle);
	// Telling libpng the signature has already be read
	png_set_sig_bytes(pngPtr, SignatureLength);

	png_read_info(pngPtr, infoPtr);

	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bitDepth = 0;
	int colorType = -1;
	png_uint_32 retVal = png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr);

	if (retVal != 1)
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		RETURN_MSG("Cannot create png info structure");
	}

	width_ = width;
	height_ = height;
	mipMapCount_ = 1; // No MIP Mapping
	LOGI_X("Header found: w:%d, h:%d, bitDepth:%d, colorType:%s", width_, height_, bitDepth, colorTypeString(colorType));

	switch (colorType)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			texFormat_ = TextureFormat(GL_RGBA8);
			bpp_ = 4;
			break;
		case PNG_COLOR_TYPE_RGB:
			texFormat_ = TextureFormat(GL_RGB8);
			bpp_ = 3;
			break;
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(pngPtr);
			texFormat_ = TextureFormat(GL_RGB8);
			bpp_ = 3;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			if (bitDepth < 8)
				png_set_expand_gray_1_2_4_to_8(pngPtr);
			texFormat_ = TextureFormat(GL_RG8);
			bpp_ = 2;
			break;
		case PNG_COLOR_TYPE_GRAY:
			if (bitDepth < 8)
				png_set_expand_gray_1_2_4_to_8(pngPtr);
			texFormat_ = TextureFormat(GL_R8);
			bpp_ = 1;
			break;
		default:
			png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
			RETURN_MSG_X("Color type not supported: %d", colorType);
			break;
	}

	if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(pngPtr);
		texFormat_ = TextureFormat(GL_RGBA8);
	}
	if (bitDepth == 16)
		png_set_strip_16(pngPtr);

	png_read_update_info(pngPtr, infoPtr);
	// Row size in bytes
	const png_size_t bytesPerRow = png_get_rowbytes(pngPtr, infoPtr);

	dataSize_ = bytesPerRow * height_;
	pixels_ = nctl::makeUnique<unsigned char[]>(static_cast<unsigned long>(dataSize_));

	nctl::UniquePtr<png_bytep[]> rowPointers = nctl::makeUnique<png_bytep[]>(height_);
	for (int i = 0; i < height_; i++)
		rowPointers[i] = pixels_.get() + i * bytesPerRow;

	// Decoding png data through row pointers
	png_read_image(pngPtr, rowPointers.get());

	png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
	hasLoaded_ = true;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderPng::readFromFileHandle(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead)
{
	IFile *fileHandle = reinterpret_cast<IFile *>(png_get_io_ptr(pngPtr));

	unsigned long int bytesRead = fileHandle->read(outBytes, static_cast<unsigned long int>(byteCountToRead));
	if (bytesRead > 0 && bytesRead != byteCountToRead)
		LOGW_X("Read %ul bytes instead of %ul", bytesRead, byteCountToRead);
}

}
