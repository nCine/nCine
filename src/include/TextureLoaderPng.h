#ifndef CLASS_NCINE_TEXTURELOADERPNG
#define CLASS_NCINE_TEXTURELOADERPNG

#include <png.h>
#include "ITextureLoader.h"

namespace ncine {

/// PNG texture loader
class TextureLoaderPng : public ITextureLoader
{
  public:
	TextureLoaderPng(const char *filename);
	TextureLoaderPng(IFile *fileHandle);

  private:
	void init();
	static void readFromFileHandle(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead);
};

}

#endif
