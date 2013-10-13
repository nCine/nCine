#include <cstdlib> // for exit()
#include <cstring>
#include "ncITextureLoader.h"
#include "ncTextureLoaderDDS.h"
#include "ncTextureLoaderPVR.h"
#include "ncTextureLoaderKTX.h"
#if defined(WITH_SDLIMAGE)
	#include "ncTextureLoaderSDL.h"
#elif defined(WITH_PNG)
	#include "ncTextureLoaderPNG.h"
#endif
#ifdef WITH_WEBP
	#include "ncTextureLoaderWebP.h"
#endif
#ifdef __ANDROID__
	#include "ncTextureLoaderETC.h"
#endif
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncITextureLoader::ncITextureLoader(const char *pFilename)
	: m_pFileHandle(NULL), m_iWidth(0), m_iHeight(0), m_iBpp(0), m_iHeaderSize(0), m_lDataSize(0),
	  m_iMipMapCount(1), m_lMipDataOffsets(NULL), m_lMipDataSizes(NULL), m_uPixels(NULL)
{
	m_pFileHandle = ncIFile::CreateFileHandle(pFilename);
}

ncITextureLoader::ncITextureLoader(ncIFile *pFileHandle)
	: m_pFileHandle(pFileHandle), m_iWidth(0), m_iHeight(0), m_iBpp(0), m_iHeaderSize(0), m_lDataSize(0),
	  m_iMipMapCount(1), m_lMipDataOffsets(NULL), m_lMipDataSizes(NULL), m_uPixels(NULL)
{

}

ncITextureLoader::~ncITextureLoader()
{
	if (m_uPixels)
		delete[] m_uPixels;

	if (m_lMipDataSizes)
		delete[] m_lMipDataSizes;

	if (m_lMipDataOffsets)
		delete[] m_lMipDataOffsets;

	if (m_pFileHandle)
		delete m_pFileHandle;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the texture data size in bytes for the specified MIP map level
long ncITextureLoader::DataSize(unsigned int uMipMapLevel) const
{
	long int lDataSize = 0;

	if (m_iMipMapCount > 1)
	{
		if (int(uMipMapLevel) < m_iMipMapCount)
			lDataSize = m_lMipDataSizes[uMipMapLevel];
	}
	else if (uMipMapLevel == 0)
		lDataSize = m_lDataSize;

	return lDataSize;
}

/// Returns the pointer to pixel data for the specified MIP map level
const GLubyte* ncITextureLoader::Pixels(unsigned int uMipMapLevel) const
{
	GLubyte *pPixels = NULL;

	if (m_iMipMapCount > 1)
	{
		if (int(uMipMapLevel) < m_iMipMapCount)
			pPixels = m_uPixels + m_lMipDataOffsets[uMipMapLevel];
	}
	else if (uMipMapLevel == 0)
		pPixels = m_uPixels;

	return pPixels;
}

/// Returns the proper texture loader according to the file extension
ncITextureLoader* ncITextureLoader::CreateFromFile(const char *pFilename)
{
	// Creating a handle from ncIFile static method to detect assets file
	ncIFile *pFileHandle = ncIFile::CreateFileHandle(pFilename);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncITextureLoader::CreateFromFile - Loading file: \"%s\"", pFileHandle->Filename());

	if (pFileHandle->HasExtension("dds"))
		return new ncTextureLoaderDDS(pFileHandle);
	else if (pFileHandle->HasExtension("pvr"))
		return new ncTextureLoaderPVR(pFileHandle);
	else if (pFileHandle->HasExtension("ktx"))
		return new ncTextureLoaderKTX(pFileHandle);
#if defined(WITH_SDLIMAGE)
	else if (pFileHandle->HasExtension("png") || pFileHandle->HasExtension("jpg"))
		return new ncTextureLoaderSDL(pFileHandle);
#elif defined(WITH_PNG)
	else if (pFileHandle->HasExtension("png"))
		return new ncTextureLoaderPNG(pFileHandle);
#endif
#ifdef WITH_WEBP
	else if (pFileHandle->HasExtension("webp"))
		return new ncTextureLoaderWebP(pFileHandle);
#endif
#ifdef __ANDROID__
	else if (pFileHandle->HasExtension("pkm"))
		return new ncTextureLoaderETC(pFileHandle);
#endif
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncITextureLoader::CreateFromFile - Extension unknown: \"%s\"", pFileHandle->Extension());
		delete pFileHandle;
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads pixel data from a texture file holding either compressed or uncompressed data
void ncITextureLoader::LoadPixels(GLenum eInternalFormat)
{
	LoadPixels(eInternalFormat, 0);
}

/// Loads pixel data from a texture file holding either compressed or uncompressed data, overriding pixel type
void ncITextureLoader::LoadPixels(GLenum eInternalFormat, GLenum eType)
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncITextureLoader::LoadPixels - Loading \"%s\"", m_pFileHandle->Filename());
	if (eType) // overriding pixel type
		m_texFormat = ncTextureFormat(eInternalFormat, eType);
	else
		m_texFormat = ncTextureFormat(eInternalFormat);

	// If the file has not been already opened by a header reader method
	if (m_pFileHandle->IsOpened() == false)
		m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);

	m_lDataSize = m_pFileHandle->Size() - m_iHeaderSize;
	m_pFileHandle->Seek(m_iHeaderSize, SEEK_SET);

	m_uPixels =  new unsigned char[m_lDataSize];
	m_pFileHandle->Read(m_uPixels, m_lDataSize);
}
