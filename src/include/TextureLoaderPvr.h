#ifndef CLASS_NCINE_TEXTURELOADERPVR
#define CLASS_NCINE_TEXTURELOADERPVR

#include <stdint.h> // for header
#include "ITextureLoader.h"

namespace ncine {

/// PVR texture loader
class TextureLoaderPvr : public ITextureLoader
{
  public:
	TextureLoaderPvr(const char *filename);
	TextureLoaderPvr(IFile *fileHandle);

  private:
	/// Header for the PVR3 format
	typedef struct Pvr3Header
	{
		uint32_t    version;
		uint32_t    flags;
		uint64_t    pixelFormat;
		uint32_t    colourSpace;
		uint32_t    channelType;
		uint32_t    height;
		uint32_t    width;
		uint32_t    depth;
		uint32_t    numSurfaces;
		uint32_t    numFaces;
		uint32_t    numMipmaps;
		uint32_t    metaDataSize;
	} Pvr3Header;

	static const uint64_t FMT_PVRTC_2BPP_RGB = 0;
	static const uint64_t FMT_PVRTC_2BPP_RGBA = 1;
	static const uint64_t FMT_PVRTC_4BPP_RGB = 2;
	static const uint64_t FMT_PVRTC_4BPP_RGBA = 3;
	static const uint64_t FMT_PVRTCII_2BPP = 4;
	static const uint64_t FMT_PVRTCII_4BPP = 5;
	static const uint64_t FMT_ETC1 = 6;
	static const uint64_t FMT_DXT1 = 7;
	static const uint64_t FMT_DXT3 = 9;
	static const uint64_t FMT_DXT5 = 11;
	static const uint64_t FMT_RGB_888 = 0x0008080800626772ULL; // rgb08880
	static const uint64_t FMT_RGBA_8888 = 0x0808080861626772ULL; // rgba8888
	static const uint64_t FMT_RGB_565 = 0x0005060500626772ULL; // rgb05650
	static const uint64_t FMT_RGBA_5551 = 0x0105050561626772ULL; // rgba5551
	static const uint64_t FMT_RGBA_4444 = 0x0404040461626772ULL; // rgba4444

	void init();

	// Reads the PVR3 header and fills the corresponding structure
	void readHeader(Pvr3Header& header);
	// Parses the PVR3 header to determine its format
	void parseFormat(const Pvr3Header& header);
};

}

#endif
