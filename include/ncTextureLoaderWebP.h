#ifndef CLASS_NCTEXTURELOADERWEBP
#define CLASS_NCTEXTURELOADERWEBP

#include <webp/decode.h>
#include "ncITextureLoader.h"

/// WebP texture loader
class ncTextureLoaderWebP : public ncITextureLoader
{
  public:
	ncTextureLoaderWebP(const char *filename);
	ncTextureLoaderWebP(ncIFile *fileHandle);
	~ncTextureLoaderWebP() { }

  private:
	void init();
};

#endif
