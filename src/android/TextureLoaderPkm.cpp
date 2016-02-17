#include <cstdlib> // for exit()
#include <cstring>
#include "TextureLoaderPkm.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderPkm::TextureLoaderPkm(const char *filename)
	: ITextureLoader(filename)
{
	init();
}

TextureLoaderPkm::TextureLoaderPkm(IFile* fileHandle)
	: ITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderPkm::init()
{
	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);

	PkmHeader header;
	// PKM header is 16 bytes long
	fileHandle_->read(&header, 16);

	// Checking for the header presence
	if (IFile::int32FromBE(header.magicId) == 0x504B4D20) // "PKM 10"
	{
		if (IFile::int16FromBE(header.version) != 0x3130) // "10"
		{
			LOGF_X("PKM version not supported: 0x%04x", header.version);
			exit(EXIT_FAILURE);
		}

		if (IFile::int16FromBE(header.dataType) != 0)
		{
			LOGF_X("PKM data type not supported: 0x%04x", header.dataType);
			exit(EXIT_FAILURE);
		}

		headerSize_ = 16;
		width_ = IFile::int16FromBE(header.width);
		height_ = IFile::int16FromBE(header.height);

		int extWidth = IFile::int16FromBE(header.extendedWidth);
		int extHeight = IFile::int16FromBE(header.extendedHeight);

		LOGI_X("Header found: w:%d h:%d, xw:%d xh:%d", width_, height_, extWidth, extHeight);
	}

	loadPixels(GL_ETC1_RGB8_OES);
}

}
