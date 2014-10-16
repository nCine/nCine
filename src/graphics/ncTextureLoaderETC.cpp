#include <cstdlib> // for exit()
#include <cstring>
#include "ncTextureLoaderETC.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderETC::ncTextureLoaderETC(const char *filename)
	: ncITextureLoader(filename)
{
	init();
}

ncTextureLoaderETC::ncTextureLoaderETC(ncIFile *fileHandle)
	: ncITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderETC::init()
{
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::gfxCapabilities();

	fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);

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
		width_ = ncIFile::int16FromBE(header.width);
		height_ = ncIFile::int16FromBE(header.height);

		ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderETC::init - Header found: w:%d h:%d", width_, height_);
	}

	// Checking for extension availability before loading
	if (gfxCaps.oesCompressedETC1RGB8Texture() == false)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderETC::init - GL_OES_compressed_ETC1_RGB8_texture not available");
		exit(-1);
	}

	loadPixels(GL_ETC1_RGB8_OES);
}
