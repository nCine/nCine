#ifndef CLASS_NCINE_IMAGESAVERPNG
#define CLASS_NCINE_IMAGESAVERPNG

#include "IImageSaver.h"

namespace ncine {

/// PNG image saver
class DLL_PUBLIC ImageSaverPng : public IImageSaver
{
  public:
	struct PngProperties
	{
		PngProperties()
		    : title(nullptr) {}

		char *title;
	};

	bool saveToFile(const Properties &properties, const char *filename) override;
	bool saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle) override;

	bool saveToFile(const Properties &properties, const PngProperties &pngProperties, const char *filename);
	bool saveToFile(const Properties &properties, const PngProperties &pngProperties, nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
