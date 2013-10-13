#include <cstdlib> // for exit()
#include <cstring>
#include "ncTextureLoaderWebP.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderWebP::ncTextureLoaderWebP(const char *pFilename)
	: ncITextureLoader(pFilename)
{
	Init();
}

ncTextureLoaderWebP::ncTextureLoaderWebP(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderWebP::Init()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderWebP::Init - Loading \"%s\"", m_pFileHandle->Filename());

	// Loading the whole file in memory
	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);
	long int lFileSize = m_pFileHandle->Size();
	unsigned char *pFileBuffer = new unsigned char[lFileSize];
	m_pFileHandle->Read(pFileBuffer, lFileSize);

	if (WebPGetInfo(pFileBuffer, lFileSize, &m_iWidth, &m_iHeight) == 0)
	{
		delete[] pFileBuffer;
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderWebP::Init - Cannot read WebP header");
		exit(-1);
	}

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderWebP::Init - Header found: w:%d h:%d", m_iWidth, m_iHeight);

	m_iMipMapCount = 1; // No MIP Mapping
	// HACK: assuming WebP always decodes to RGBA
	m_texFormat = ncTextureFormat(GL_RGBA);
	long int lDecodedSize = m_iWidth * m_iHeight * 4;
	m_uPixels =  new unsigned char[lDecodedSize];

	if (WebPDecodeRGBAInto(pFileBuffer, lFileSize, m_uPixels, lDecodedSize, m_iWidth * 4) == NULL)
	{
		delete[] pFileBuffer;
		delete[] m_uPixels;
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderWebP::Init - Cannot decode WebP image");
		exit(-1);
	}

	delete[] pFileBuffer;
}
