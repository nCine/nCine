#include "common_macros.h"
#include "TextureLoaderKtx.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

uint8_t TextureLoaderKtx::fileIdentifier_[] =
{ 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }; // "«KTX 11»\r\n\x1A\n"};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderKtx::TextureLoaderKtx(const char *filename)
	: ITextureLoader(filename)
{
	init();
}

TextureLoaderKtx::TextureLoaderKtx(IFile *fileHandle)
	: ITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderKtx::init()
{
	KtxHeader header;

	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);
	readHeader(header);
	parseFormat(header);
}

void TextureLoaderKtx::readHeader(KtxHeader &header)
{
	bool checkPassed = true;

	// KTX header is 64 bytes long
	fileHandle_->read(&header, 64);

	for (int i = 0; i < KtxIdentifierLength; i++)
	{
		if (header.identifier[i] != fileIdentifier_[i])
			checkPassed = false;
	}

	FATAL_ASSERT_MSG(checkPassed, "Not a KTX file");
	// Checking for the header identifier
	FATAL_ASSERT_MSG(header.endianess != 0x01020304, "File endianess doesn't match machine one");

	// Accounting for key-value data and `UInt32 imageSize` from first MIP level
	headerSize_ = 64 + IFile::int32FromLE(header.bytesOfKeyValueData) + 4;
	width_ = IFile::int32FromLE(header.pixelWidth);
	height_ = IFile::int32FromLE(header.pixelHeight);
	mipMapCount_ = IFile::int32FromLE(header.numberOfMipmapLevels);
}

void TextureLoaderKtx::parseFormat(const KtxHeader &header)
{
	const GLenum internalFormat = IFile::int32FromLE(header.glInternalFormat);
	const GLenum type = IFile::int32FromLE(header.glType);

	loadPixels(internalFormat, type);

	if (mipMapCount_ > 1)
	{
		LOGI_X("MIP Maps: %d", mipMapCount_);
		mipDataOffsets_ = new long[mipMapCount_];
		mipDataSizes_ = new long[mipMapCount_];
		long int dataSizesSum = TextureFormat::calculateMipSizes(internalFormat, width_, height_, mipMapCount_, mipDataOffsets_, mipDataSizes_);

		// HACK: accounting for `UInt32 imageSize` on top of each MIP level
		// Excluding the first one, already taken into account in header size
		for (int i = 1; i < mipMapCount_; i++)
			mipDataOffsets_[i] += 4 * i;
		dataSizesSum += 4 * mipMapCount_;

		if (dataSizesSum != dataSize_)
			LOGW_X("The sum of MIP maps size (%ld) is different than texture total data (%ld)", dataSizesSum, dataSize_);
	}
}

}
