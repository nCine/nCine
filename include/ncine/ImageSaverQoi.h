#ifndef CLASS_NCINE_IMAGESAVERQOI
#define CLASS_NCINE_IMAGESAVERQOI

#include "IImageSaver.h"

namespace ncine {

/// QOI image saver
class DLL_PUBLIC ImageSaverQoi : public IImageSaver
{
  public:
	enum ColorSpace
	{
		SRGB = 0,
		LINEAR = 1
	};

	struct QoiProperties
	{
		ColorSpace colorSpace = LINEAR;
	};

	bool saveToFile(const Properties &properties, const char *filename) override;
	bool saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle) override;

	bool saveToFile(const Properties &properties, const QoiProperties &qoiProperties, const char *filename);
	bool saveToFile(const Properties &properties, const QoiProperties &qoiProperties, nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
