#ifndef CLASS_NCTEXTURELOADERETC
#define CLASS_NCTEXTURELOADERETC

#include <stdint.h> // for header
#include "ncITextureLoader.h"

/// ETC1 texture loader
class ncTextureLoaderETC : public ncITextureLoader
{
  public:
	ncTextureLoaderETC(const char *filename);
	ncTextureLoaderETC(ncIFile *fileHandle);
	~ncTextureLoaderETC() { }

  private:
	/// Structure for ETC1 magic number
	typedef struct Etc1Magic
	{
		char magicId[6];
		uint16_t zero;
	} Etc1Magic;

	/// Header for the ETC1 header
	/*! The extended width and height are the dimensions rounded up to a multiple of 4.
	 *  The total data size in bytes is (extendedWidth / 4) * (extendedHeight / 4) * 8
	 */
	typedef struct Etc1Header
	{
		uint16_t extendedWidth;
		uint16_t extendedHeight;
		uint16_t width;
		uint16_t height;
	} Etc1Header;

	void init();
};

#endif
