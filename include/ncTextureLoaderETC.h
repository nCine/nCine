#ifndef CLASS_NCTEXTURELOADERETC
#define CLASS_NCTEXTURELOADERETC

#include <stdint.h> // for header
#include "ncITextureLoader.h"

/// ETC1 texture loader
class ncTextureLoaderETC : public ncITextureLoader
{
private:
	/// Structure for ETC1 magic number
	typedef struct ETC1_magic {
		char cMagicId[6];
		uint16_t uZero;
	} ETC1_magic;

	/// Header for the ETC1 header
	typedef struct ETC1_header {
		uint16_t uWidth;
		uint16_t uHeight;
		uint16_t uWidth2;
		uint16_t uHeight2;
	} ETC1_header;

	void Init();

public:
	ncTextureLoaderETC(const char *pFilename);
	ncTextureLoaderETC(ncIFile *pFileHandle);
	~ncTextureLoaderETC() { }
};

#endif
