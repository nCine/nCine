#include "IImageSaver.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IImageSaver::~IImageSaver() = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void IImageSaver::flipPixels(const Properties &properties, unsigned char *dest)
{
	const unsigned int pitch = imageFormatToBpp(properties.format) * properties.width;
	const unsigned char *src = reinterpret_cast<unsigned char *>(properties.pixels);

	for (int i = 0; i < properties.height; i++)
	{
		const unsigned char *srcRow = src + i * pitch;
		unsigned char *destRow = dest + (properties.height - i - 1) * pitch;
		memcpy(destRow, srcRow, pitch);
	}
}

unsigned int IImageSaver::imageFormatToBpp(const IImageSaver::Format format)
{
	switch (format)
	{
		case IImageSaver::Format::R8: return 1;
		case IImageSaver::Format::RG8: return 2;
		case IImageSaver::Format::RGB8: return 3;
		default:
		case IImageSaver::Format::RGBA8: return 4;
		case IImageSaver::Format::RGB_FLOAT: return 12;
	}
}

unsigned int IImageSaver::dataSize(const Properties &properties)
{
	return (properties.width * properties.height * imageFormatToBpp(properties.format));
}

}
