#ifndef CLASS_NCTEXTURELOADERWEBP
#define CLASS_NCTEXTURELOADERWEBP

#include <webp/decode.h>
#include "ncITextureLoader.h"

/// WebP texture loader
class ncTextureLoaderWebP : public ncITextureLoader
{
private:
	void Init();

public:
	ncTextureLoaderWebP(const char *pFilename);
	ncTextureLoaderWebP(ncIFile *pFileHandle);
	~ncTextureLoaderWebP() { }
};

#endif
