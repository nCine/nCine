#ifndef CLASS_NCINE_TEXTURESAVERPNG
#define CLASS_NCINE_TEXTURESAVERPNG

#include "ITextureSaver.h"

namespace ncine {

/// PNG texture saver
class TextureSaverPng : public ITextureSaver
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
