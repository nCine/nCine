#ifndef CLASS_NCTEXTURELOADERPNG
#define CLASS_NCTEXTURELOADERPNG

#include <png.h>
#include "ncITextureLoader.h"

/// PNG texture loader
class ncTextureLoaderPNG : public ncITextureLoader
{
 public:
	ncTextureLoaderPNG(const char *pFilename);
	ncTextureLoaderPNG(ncIFile *pFileHandle);
	~ncTextureLoaderPNG() { }

 private:
	void Init();
	static void ReadFromFileHandle(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead);

};

#endif
