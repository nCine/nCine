#ifndef CLASS_NCINE_IMAGELOADERPNG
#define CLASS_NCINE_IMAGELOADERPNG

#include <png.h>
#include "IImageLoader.h"

namespace ncine {

/// PNG image loader
class ImageLoaderPng : public IImageLoader
{
  public:
	explicit ImageLoaderPng(nctl::UniquePtr<IFile> fileHandle);

  private:
	static void readFromFileHandle(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead);
};

}

#endif
