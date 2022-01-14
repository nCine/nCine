#include "ITextureSaver.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ITextureSaver::~ITextureSaver() = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ITextureSaver::bpp(const ITextureSaver::Format format)
{
	switch (format)
	{
		case ITextureSaver::Format::RGB8:
			return 3;
		default:
		case ITextureSaver::Format::RGBA8:
			return 4;
		case ITextureSaver::Format::RGB_FLOAT:
			return 12;
	}
}

unsigned int ITextureSaver::dataSize(const Properties &properties)
{
	return (properties.width * properties.height * bpp(properties.format));
}

void ITextureSaver::flipPixels(const Properties &properties, unsigned char *dest)
{
	const unsigned int pitch = bpp(properties.format) * properties.width;
	const unsigned char *src = reinterpret_cast<unsigned char *>(properties.pixels);

	for (int i = 0; i < properties.height; i++)
	{
		const unsigned char *srcRow = src + i * pitch;
		unsigned char *destRow = dest + (properties.height - i - 1) * pitch;
		memcpy(destRow, srcRow, pitch);
	}
}

}
