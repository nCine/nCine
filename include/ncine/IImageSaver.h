#ifndef CLASS_NCINE_IIMAGESAVER
#define CLASS_NCINE_IIMAGESAVER

#include "common_defines.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class IFile;

/// Image saver interface class
class DLL_PUBLIC IImageSaver
{
  public:
	enum class Format
	{
		R8,
		RG8,
		RGB8,
		RGBA8,
		RGB_FLOAT
	};

	struct Properties
	{
		Properties()
		    : width(0), height(0), format(Format::RGB8), verticalFlip(false), pixels(nullptr) {}

		int width;
		int height;
		Format format;
		bool verticalFlip;
		void *pixels;
	};

	virtual ~IImageSaver();

	virtual bool saveToFile(const Properties &properties, const char *filename) = 0;
	virtual bool saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle) = 0;

	void flipPixels(const Properties &properties, unsigned char *dest);

	static unsigned int imageFormatToBpp(const Format format);
	static unsigned int dataSize(const Properties &properties);
};

}

#endif
