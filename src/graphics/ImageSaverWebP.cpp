#include <webp/encode.h>
#include "common_macros.h"
#include "ImageSaverWebP.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ImageSaverWebP::saveToFile(const Properties &properties, const char *filename)
{
	return saveToFile(properties, WebPProperties(), IFile::createFileHandle(filename));
}

bool ImageSaverWebP::saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle)
{
	return saveToFile(properties, WebPProperties(), nctl::move(fileHandle));
}

bool ImageSaverWebP::saveToFile(const Properties &properties, const WebPProperties &webpProperties, const char *filename)
{
	return saveToFile(properties, webpProperties, IFile::createFileHandle(filename));
}

bool ImageSaverWebP::saveToFile(const Properties &properties, const WebPProperties &webpProperties, nctl::UniquePtr<IFile> fileHandle)
{
	FATAL_ASSERT(properties.width > 0);
	FATAL_ASSERT(properties.height > 0);
	FATAL_ASSERT_MSG(properties.width <= 16383, "Image width exceeds WebP maximum pixel dimensions");
	FATAL_ASSERT_MSG(properties.height <= 16383, "Image height exceeds WebP maximum pixel dimensions");
	FATAL_ASSERT(properties.pixels != nullptr);
	ASSERT(properties.format == Format::RGB8 || properties.format == Format::RGBA8);

	LOGI_X("Saving \"%s\"", fileHandle->filename());
	fileHandle->open(IFile::OpenMode::WRITE | IFile::OpenMode::BINARY);
	if (fileHandle->isOpened() == false)
		return false;

	// Flip pixels data vertically if the corresponding flag is true
	nctl::UniquePtr<unsigned char[]> flippedPixels;
	if (properties.verticalFlip)
	{
		flippedPixels = nctl::makeUnique<unsigned char[]>(dataSize(properties));
		flipPixels(properties, flippedPixels.get());
	}
	void *texturePixels = properties.verticalFlip ? flippedPixels.get() : properties.pixels;
	const unsigned int bpp = imageFormatToBpp(properties.format);

	// Using the WebP Simple Encoding API
	const uint8_t *pixels = static_cast<uint8_t *>(texturePixels);
	uint8_t *output = nullptr;
	size_t bytesWritten = 0;
	switch (properties.format)
	{
		default:
		case Format::RGB8:
		{
			if (webpProperties.lossless)
				bytesWritten = WebPEncodeLosslessRGB(pixels, properties.width, properties.height, properties.width * bpp, &output);
			else
				bytesWritten = WebPEncodeRGB(pixels, properties.width, properties.height, properties.width * bpp, webpProperties.quality, &output);
			break;
		}
		case Format::RGBA8:
		{
			if (webpProperties.lossless)
				bytesWritten = WebPEncodeLosslessRGBA(pixels, properties.width, properties.height, properties.width * bpp, &output);
			else
				bytesWritten = WebPEncodeRGBA(pixels, properties.width, properties.height, properties.width * bpp, webpProperties.quality, &output);
			break;
		}
	}

	if (bytesWritten > 0)
		fileHandle->write(output, bytesWritten);

	fileHandle->close();
	WebPFree(output);

	return (bytesWritten > 0);
}

}
