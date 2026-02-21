#ifndef CLASS_NCINE_IMAGELOADERWEBP
#define CLASS_NCINE_IMAGELOADERWEBP

#include <webp/decode.h>
#include "IImageLoader.h"

namespace ncine {

/// WebP image loader
class ImageLoaderWebP : public IImageLoader
{
  public:
	explicit ImageLoaderWebP(nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
