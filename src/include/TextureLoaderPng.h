#ifndef CLASS_NCINE_TEXTURELOADERPNG
#define CLASS_NCINE_TEXTURELOADERPNG

#include <png.h>
#include "ITextureLoader.h"

namespace ncine {

/// PNG texture loader
class TextureLoaderPng : public ITextureLoader
{
  public:
	explicit TextureLoaderPng(nctl::UniquePtr<IFile> fileHandle);

  private:
	static void readFromFileHandle(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead);
};

}

#endif
