#ifndef CLASS_NCINE_IMAGELOADERQOI
#define CLASS_NCINE_IMAGELOADERQOI

#include "IImageLoader.h"

namespace ncine {

/// QOI image loader
class ImageLoaderQoi : public IImageLoader
{
  public:
	explicit ImageLoaderQoi(nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
