#include <cstdlib> // for exit()
#include <cstring>
#include "ncTextureLoaderETC.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderETC::ncTextureLoaderETC(const char *pFilename)
	: ncITextureLoader(pFilename)
{
	Init();
}

ncTextureLoaderETC::ncTextureLoaderETC(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderETC::Init()
{
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::GfxCapabilities();

	m_pFileHandle->Open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);

	// ETC1 header is 16 bytes long
	// It have been splitted in two structures to avoid compiler-specific alignment pragmas
	ETC1_magic magic;
	ETC1_header header;
	m_pFileHandle->Read(&magic, 8);

	// Checking for the header presence
	if (strncmp(magic.cMagicId, "PKM 10", 6) == 0)
	{
		m_pFileHandle->Read(&header, 8);
		m_iHeaderSize = 16;
		m_iWidth = ncIFile::Int16FromBE(header.uWidth);
		m_iHeight = ncIFile::Int16FromBE(header.uHeight);

		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderETC::ncTextureLoaderETC - Header found: w:%d h:%d", m_iWidth, m_iHeight);
	}

	// Checking for extension availability before loading
	if (gfxCaps.OESCompressedETC1RGB8Texture() == false)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderETC::ncTextureLoaderETC - GL_OES_compressed_ETC1_RGB8_texture not available");
		exit(-1);
	}

	LoadPixels(GL_ETC1_RGB8_OES);
}
