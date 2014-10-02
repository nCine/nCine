#ifndef CLASS_NCTEXTURELOADERKTX
#define CLASS_NCTEXTURELOADERKTX

#include <stdint.h> // for header
#include "ncITextureLoader.h"

/// KTX texture loader
class ncTextureLoaderKTX : public ncITextureLoader
{
  public:
	ncTextureLoaderKTX(const char *pFilename);
	ncTextureLoaderKTX(ncIFile *pFileHandle);
	~ncTextureLoaderKTX() { }

  private:
	static const int KTX_IDENTIFIER_LENGTH = 12;
	static uint8_t m_uFileIdentifier[KTX_IDENTIFIER_LENGTH];

	/// Header for the KTX format
	typedef struct KTX_header
	{
		uint8_t		identifier[KTX_IDENTIFIER_LENGTH];
		uint32_t    endianess;
		uint32_t    glType;
		uint32_t    glTypeSize;
		uint32_t    glFormat;
		uint32_t    glInternalFormat;
		uint32_t    glBaseInternalFormat;
		uint32_t    pixelWidth;
		uint32_t    pixelHeight;
		uint32_t    pixelDepth;
		uint32_t    numberOfArrayElements;
		uint32_t    numberOfFaces;
		uint32_t    numberOfMipmapLevels;
		uint32_t	bytesOfKeyValueData;
	} KTX_header;

	void Init();

	// Reads the KTX header and fills the corresponding structure
	void ReadHeader(KTX_header& header);
	// Parses the KTX header to determine its format
	void ParseFormat(const KTX_header& header);
};

#endif
