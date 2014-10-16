#ifndef CLASS_NCTEXTURELOADERKTX
#define CLASS_NCTEXTURELOADERKTX

#include <stdint.h> // for header
#include "ncITextureLoader.h"

/// KTX texture loader
class ncTextureLoaderKTX : public ncITextureLoader
{
  public:
	ncTextureLoaderKTX(const char *filename);
	ncTextureLoaderKTX(ncIFile *fileHandle);
	~ncTextureLoaderKTX() { }

  private:
	static const int KtxIdentifierLength = 12;
	static uint8_t fileIdentifier_[KtxIdentifierLength];

	/// Header for the KTX format
	typedef struct KtxHeader
	{
		uint8_t		identifier[KtxIdentifierLength];
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
	} KtxHeader;

	void init();

	// Reads the KTX header and fills the corresponding structure
	void readHeader(KtxHeader& header);
	// Parses the KTX header to determine its format
	void parseFormat(const KtxHeader& header);
};

#endif
