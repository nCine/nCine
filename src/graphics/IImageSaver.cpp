#include "common_macros.h"
#include "IImageSaver.h"
#ifdef WITH_PNG
	#include "ImageSaverPng.h"
#endif
#ifdef WITH_WEBP
	#include "ImageSaverWebP.h"
#endif
#ifdef WITH_QOI
	#include "ImageSaverQoi.h"
#endif
#include "FileSystem.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IImageSaver::~IImageSaver() = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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

bool IImageSaver::save(const Properties &properties, const char *filename)
{
	nctl::UniquePtr<IImageSaver> saver = createSaver(filename);
	if (saver == nullptr)
		return false;
	return saver->saveToFile(properties, filename);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<IImageSaver> IImageSaver::createSaver(const char *filename)
{
#ifdef WITH_PNG
	if (fs::hasExtension(filename, "png"))
		return nctl::makeUnique<ImageSaverPng>();
#endif
#ifdef WITH_WEBP
	if (fs::hasExtension(filename, "webp"))
		return nctl::makeUnique<ImageSaverWebP>();
#endif
#ifdef WITH_QOI
	if (fs::hasExtension(filename, "qoi"))
		return nctl::makeUnique<ImageSaverQoi>();
#endif

	LOGF_X("Extension unknown: \"%s\"", fs::extension(filename));
	return nctl::UniquePtr<IImageSaver>();
}

}
