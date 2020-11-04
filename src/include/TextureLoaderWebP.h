#ifndef CLASS_NCINE_TEXTURELOADERWEBP
#define CLASS_NCINE_TEXTURELOADERWEBP

#include <webp/decode.h>
#include "ITextureLoader.h"

namespace ncine {

/// WebP texture loader
class TextureLoaderWebP : public ITextureLoader
{
  public:
	explicit TextureLoaderWebP(nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
