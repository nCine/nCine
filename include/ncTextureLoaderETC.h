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
	/*! The extended width and height are the dimensions rounded up to a multiple of 4.
	 *  The total data size in bytes is (uExtendedWidth/4)*(uExtendedHeight/4)*8
	 */
	typedef struct ETC1_header {
		uint16_t uExtendedWidth;
		uint16_t uExtendedHeight;
		uint16_t uWidth;
		uint16_t uHeight;
	} ETC1_header;

	void Init();

public:
	ncTextureLoaderETC(const char *pFilename);
	ncTextureLoaderETC(ncIFile *pFileHandle);
	~ncTextureLoaderETC() { }
};

#endif
