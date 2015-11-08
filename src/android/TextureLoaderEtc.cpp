#include <cstdlib> // for exit()
#include <cstring>
#include "TextureLoaderEtc.h"
#include "GfxCapabilities.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderEtc::TextureLoaderEtc(const char *filename)
	: ITextureLoader(filename)
{
	init();
}

TextureLoaderEtc::TextureLoaderEtc(IFile* fileHandle)
	: ITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderEtc::init()
{
	const GfxCapabilities& gfxCaps = theServiceLocator().gfxCapabilities();

	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);

	// ETC1 header is 16 bytes long
	// It have been splitted in two structures to avoid compiler-specific alignment pragmas
	Etc1Magic magic;
	Etc1Header header;
	fileHandle_->read(&magic, 8);

	// Checking for the header presence
	if (strncmp(magic.magicId, "PKM 10", 6) == 0)
	{
		fileHandle_->read(&header, 8);
		headerSize_ = 16;
		width_ = IFile::int16FromBE(header.width);
		height_ = IFile::int16FromBE(header.height);

		LOGI_X("Header found: w:%d h:%d", width_, height_);
	}

	// Checking for extension availability before loading
	if (gfxCaps.oesCompressedEtc1Rgb8Texture() == false)
	{
		LOGF("GL_OES_compressed_ETC1_RGB8_texture not available");
		exit(EXIT_FAILURE);
	}

	loadPixels(GL_ETC1_RGB8_OES);
}

}
