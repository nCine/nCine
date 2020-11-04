#ifndef CLASS_NCINE_TEXTURELOADERPKM
#define CLASS_NCINE_TEXTURELOADERPKM

#include <cstdint> // for header
#include "ITextureLoader.h"

namespace ncine {

/// PKM texture loader
class TextureLoaderPkm : public ITextureLoader
{
  public:
	explicit TextureLoaderPkm(nctl::UniquePtr<IFile> fileHandle);

  private:
	/// Header for the PKM header
	/*! The extended width and height are the dimensions rounded up to a multiple of 4.
	 *  The total data size in bytes is (extendedWidth / 4) * (extendedHeight / 4) * 8
	 */
	struct PkmHeader
	{
		uint32_t magicId;
		uint16_t version;
		uint16_t dataType;
		uint16_t extendedWidth;
		uint16_t extendedHeight;
		uint16_t width;
		uint16_t height;
	};
};

}

#endif
