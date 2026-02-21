#ifndef CLASS_NCINE_IMAGESAVERWEBP
#define CLASS_NCINE_IMAGESAVERWEBP

#include "IImageSaver.h"

namespace ncine {

/// WebP image saver
class DLL_PUBLIC ImageSaverWebP : public IImageSaver
{
  public:
	struct WebPProperties
	{
		WebPProperties()
		    : quality(75.0f), lossless(false) {}

		float quality;
		bool lossless;
	};

	bool saveToFile(const Properties &properties, const char *filename) override;
	bool saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle) override;

	bool saveToFile(const Properties &properties, const WebPProperties &webpProperties, const char *filename);
	bool saveToFile(const Properties &properties, const WebPProperties &webpProperties, nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
