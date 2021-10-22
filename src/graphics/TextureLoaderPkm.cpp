#include "return_macros.h"
#include <cstring>
#include "TextureLoaderPkm.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderPkm::TextureLoaderPkm(nctl::UniquePtr<IFile> fileHandle)
    : ITextureLoader(nctl::move(fileHandle))
{
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());

	PkmHeader header;
	// PKM header is 16 bytes long
	fileHandle_->read(&header, 16);

	// Checking for the header presence
	if (IFile::int32FromBE(header.magicId) == 0x504B4D20) // "PKM 10"
	{
		if (IFile::int16FromBE(header.version) != 0x3130) // "10"
			RETURN_MSG_X("PKM version not supported: 0x%04x", header.version);

		if (IFile::int16FromBE(header.dataType) != 0)
			RETURN_MSG_X("PKM data type not supported: 0x%04x", header.dataType);

		headerSize_ = 16;
		width_ = IFile::int16FromBE(header.width);
		height_ = IFile::int16FromBE(header.height);

		const int extWidth = IFile::int16FromBE(header.extendedWidth);
		const int extHeight = IFile::int16FromBE(header.extendedHeight);

		LOGI_X("Header found: w:%d h:%d, xw:%d xh:%d", width_, height_, extWidth, extHeight);
	}

	loadPixels(GL_ETC1_RGB8_OES);
	hasLoaded_ = true;
}

}
