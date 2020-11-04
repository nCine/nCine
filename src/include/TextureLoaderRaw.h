#ifndef CLASS_NCINE_TEXTURELOADERRAW
#define CLASS_NCINE_TEXTURELOADERRAW

#include "ITextureLoader.h"

namespace ncine {

/// A texture loader used to specify a raw format when loading texels
class TextureLoaderRaw : public ITextureLoader
{
  public:
	TextureLoaderRaw(int width, int height, int mipMapCount, GLenum internalFormat);
};

}

#endif
