#include "return_macros.h"
#include "TextureLoaderKtx.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

uint8_t TextureLoaderKtx::fileIdentifier_[] = {
	0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
}; // "«KTX 11»\r\n\x1A\n"};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderKtx::TextureLoaderKtx(nctl::UniquePtr<IFile> fileHandle)
    : ITextureLoader(nctl::move(fileHandle))
{
	KtxHeader header;

	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());
	const bool headerRead = readHeader(header);
	RETURN_ASSERT_MSG(headerRead, "KTX header cannot be read");
	const bool formatParsed = parseFormat(header);
	RETURN_ASSERT_MSG(formatParsed, "KTX format cannot be parsed");

	hasLoaded_ = true;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool TextureLoaderKtx::readHeader(KtxHeader &header)
{
	bool checkPassed = true;

	// KTX header is 64 bytes long
	fileHandle_->read(&header, 64);

	for (int i = 0; i < KtxIdentifierLength; i++)
	{
		if (header.identifier[i] != fileIdentifier_[i])
			checkPassed = false;
	}

	RETURNF_ASSERT_MSG(checkPassed, "Not a KTX file");
	// Checking for the header identifier
	RETURNF_ASSERT_MSG(header.endianess != 0x01020304, "File endianess doesn't match machine one");

	// Accounting for key-value data and `UInt32 imageSize` from first MIP level
	headerSize_ = 64 + IFile::int32FromLE(header.bytesOfKeyValueData) + 4;
	width_ = IFile::int32FromLE(header.pixelWidth);
	height_ = IFile::int32FromLE(header.pixelHeight);
	mipMapCount_ = IFile::int32FromLE(header.numberOfMipmapLevels);

	return true;
}

bool TextureLoaderKtx::parseFormat(const KtxHeader &header)
{
	const GLenum internalFormat = IFile::int32FromLE(header.glInternalFormat);
	const GLenum type = IFile::int32FromLE(header.glType);

	loadPixels(internalFormat, type);

	if (mipMapCount_ > 1)
	{
		LOGI_X("MIP Maps: %d", mipMapCount_);
		mipDataOffsets_ = nctl::makeUnique<unsigned long[]>(mipMapCount_);
		mipDataSizes_ = nctl::makeUnique<unsigned long[]>(mipMapCount_);
		unsigned long dataSizesSum = TextureFormat::calculateMipSizes(internalFormat, width_, height_, mipMapCount_, mipDataOffsets_.get(), mipDataSizes_.get());

		// HACK: accounting for `UInt32 imageSize` on top of each MIP level
		// Excluding the first one, already taken into account in header size
		for (int i = 1; i < mipMapCount_; i++)
			mipDataOffsets_[i] += 4 * i;
		dataSizesSum += 4 * mipMapCount_;

		if (dataSizesSum != dataSize_)
			LOGW_X("The sum of MIP maps size (%ld) is different than texture total data (%ld)", dataSizesSum, dataSize_);
	}

	return true;
}

}
