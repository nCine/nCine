#include "qoi.h"
#include "common_macros.h"
#include "ImageSaverQoi.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ImageSaverQoi::saveToFile(const Properties &properties, const char *filename)
{
	return saveToFile(properties, QoiProperties(), IFile::createFileHandle(filename));
}

bool ImageSaverQoi::saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle)
{
	return saveToFile(properties, QoiProperties(), nctl::move(fileHandle));
}

bool ImageSaverQoi::saveToFile(const Properties &properties, const QoiProperties &qoiProperties, const char *filename)
{
	return saveToFile(properties, qoiProperties, IFile::createFileHandle(filename));
}

bool ImageSaverQoi::saveToFile(const Properties &properties, const QoiProperties &qoiProperties, nctl::UniquePtr<IFile> fileHandle)
{
	FATAL_ASSERT(properties.width > 0);
	FATAL_ASSERT(properties.height > 0);
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

	qoi_desc desc;
	desc.width = properties.width;
	desc.height = properties.height;
	desc.channels = bpp;
	desc.colorspace = static_cast<uint8_t>(qoiProperties.colorSpace);

	const uint8_t *pixels = static_cast<uint8_t *>(texturePixels);
	uint8_t *output = nullptr;
	int bytesWritten = 0;

	output = reinterpret_cast<uint8_t *>(qoi_encode(pixels, &desc, &bytesWritten));
	if (bytesWritten > 0)
		fileHandle->write(output, bytesWritten);

	fileHandle->close();
	free(output);

	return (bytesWritten > 0);
}

}
