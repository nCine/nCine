#include <png.h>
#include "common_macros.h"
#include "TextureSaverPng.h"
#include "IFile.h"

namespace ncine {

namespace {
	void freePngMemory(png_structp &pngPtr, png_infop &infoPtr)
	{
		if (infoPtr != nullptr)
			png_free_data(pngPtr, infoPtr, PNG_FREE_ALL, -1);
		if (pngPtr != nullptr)
			png_destroy_write_struct(&pngPtr, png_infopp(nullptr));
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool TextureSaverPng::saveToFile(const Properties &properties, const char *filename)
{
	return saveToFile(properties, PngProperties(), IFile::createFileHandle(filename));
}

bool TextureSaverPng::saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle)
{
	return saveToFile(properties, PngProperties(), nctl::move(fileHandle));
}

bool TextureSaverPng::saveToFile(const Properties &properties, const PngProperties &pngProperties, const char *filename)
{
	return saveToFile(properties, pngProperties, IFile::createFileHandle(filename));
}

bool TextureSaverPng::saveToFile(const Properties &properties, const PngProperties &pngProperties, nctl::UniquePtr<IFile> fileHandle)
{
	const unsigned int bpp = (properties.format == Format::RGB8) ? 3 : 4;

	FATAL_ASSERT(properties.width > 0);
	FATAL_ASSERT(properties.height > 0);
	FATAL_ASSERT_MSG(properties.height <= PNG_SIZE_MAX / (properties.width * bpp), "Image data buffer would be too large");
	FATAL_ASSERT_MSG(properties.height <= PNG_UINT_32_MAX / sizeof(png_bytep), "Image is too tall to process in memory");
	FATAL_ASSERT(properties.pixels != nullptr);
	ASSERT(properties.format == Format::RGB8 || properties.format == Format::RGBA8);

	LOGI_X("Saving \"%s\"", fileHandle->filename());
	fileHandle->open(IFile::OpenMode::WRITE | IFile::OpenMode::BINARY);
	if (fileHandle->isOpened() == false)
		return false;

	png_structp pngPtr = nullptr;
	png_infop infoPtr = nullptr;

	// Initialize write structure
	pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (pngPtr == nullptr)
	{
		LOGE_X("Could not allocate write struct");
		freePngMemory(pngPtr, infoPtr);
		return false;
	}

	// Initialize info structure
	infoPtr = png_create_info_struct(pngPtr);
	if (infoPtr == nullptr)
	{
		LOGE_X("Could not allocate info struct");
		freePngMemory(pngPtr, infoPtr);
		return false;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(pngPtr)))
	{
		fprintf(stderr, "Error during png creation\n");
		freePngMemory(pngPtr, infoPtr);
		return false;
	}

	png_init_io(pngPtr, fileHandle->filePointer_);

	const unsigned int pngColorType = (properties.format == Format::RGB8) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;
	// Write header (8 bit colour depth)
	png_set_IHDR(pngPtr, infoPtr, properties.width, properties.height,
	             8, pngColorType, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (pngProperties.title != nullptr)
	{
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = pngProperties.title;
		png_set_text(pngPtr, infoPtr, &title_text, 1);
	}

	png_write_info(pngPtr, infoPtr);

	// Write image data
	png_byte *pixels = reinterpret_cast<png_byte *>(properties.pixels);

	const unsigned int rowLength = bpp * properties.width * sizeof(png_byte);
	for (int y = 0; y < properties.height; y++)
		png_write_row(pngPtr, &pixels[y * rowLength]);

	// End write
	png_write_end(pngPtr, nullptr);

	freePngMemory(pngPtr, infoPtr);
	return true;
}

}
