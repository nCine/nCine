#ifndef CLASS_NCINE_TEXTURELOADERDDS
#define CLASS_NCINE_TEXTURELOADERDDS

#include <cstdint> // for header
#include "ITextureLoader.h"

namespace ncine {

/// DDS texture loader
class TextureLoaderDds : public ITextureLoader
{
  public:
	explicit TextureLoaderDds(nctl::UniquePtr<IFile> fileHandle);

  private:
	/// Header for the DDS pixel format
	struct DdsPixelformat
	{
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwABitMask;
	};

	/// Header for the DDS format
	struct DdsHeader
	{
		uint32_t dwMagic;
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwHeight;
		uint32_t dwWidth;
		uint32_t dwPitchOrLinearSize;
		uint32_t dwDepth;
		uint32_t dwMipMapCount;
		uint32_t dwReserved1[11];
		DdsPixelformat ddspf;
		uint32_t dwCaps;
		uint32_t dwCaps2;
		uint32_t dwCaps3;
		uint32_t dwCaps4;
		uint32_t dwReserved2;
	};

	static const uint32_t DDPF_ALPHAPIXELS = 0x1;
	static const uint32_t DDPF_ALPHA = 0x2;
	static const uint32_t DDPF_FOURCC = 0x4;
	static const uint32_t DDPF_RGB = 0x40;
	static const uint32_t DDPF_YUV = 0x200;
	static const uint32_t DDPF_LUMINANCE = 0x20000;

	static const uint32_t DDS_R8G8B8 = 20;
	static const uint32_t DDS_A8R8G8B8 = 21;
	static const uint32_t DDS_R5G6B5 = 23;
	static const uint32_t DDS_A1R5G5B5 = 25;
	static const uint32_t DDS_A4R4G4B4 = 26;
	static const uint32_t DDS_DXT1 = 0x31545844; // "DXT1"
	static const uint32_t DDS_DXT3 = 0x33545844; // "DXT3"
	static const uint32_t DDS_DXT5 = 0x35545844; // "DXT5"
	static const uint32_t DDS_ETC1 = 0x31435445; // "ETC1"
	static const uint32_t DDS_ATC = 0x20435441; // "ATC "
	static const uint32_t DDS_ATCA = 0x41435441; // "ATCA"
	static const uint32_t DDS_ATCI = 0x49435441; // "ATCI"

	/// Reads the DDS header and fills the corresponding structure
	bool readHeader(DdsHeader &header);
	/// Parses the DDS header to determine its format
	bool parseFormat(const DdsHeader &header);
};

}

#endif
