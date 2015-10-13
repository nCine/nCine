#ifndef CLASS_NCTEXTURELOADERETC
#define CLASS_NCTEXTURELOADERETC

#include <stdint.h> // for header
#include "ITextureLoader.h"

namespace ncine {

/// ETC1 texture loader
class TextureLoaderEtc : public ITextureLoader
{
  public:
	explicit TextureLoaderEtc(const char *filename);
	explicit TextureLoaderEtc(IFile *fileHandle);
	~TextureLoaderEtc() { }

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

}

#endif
