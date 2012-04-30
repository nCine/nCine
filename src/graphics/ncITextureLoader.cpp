#include <cstdlib> // for exit()
#include <cstring>
#include "ncITextureLoader.h"
#include "ncTextureLoaderDDS.h"
#include "ncTextureLoaderPVR.h"
#ifdef WITH_SDLIMAGE
#include "ncTextureLoaderSDL.h"
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
	: m_pFileHandle(NULL), m_iWidth(0), m_iHeight(0),
	  m_iBpp(0), m_iHeaderSize(0), m_lFileSize(0), m_uPixels(NULL)
{
	m_pFileHandle = ncIFile::CreateFileHandle(pFilename);
}

ncITextureLoader::ncITextureLoader(ncIFile *pFileHandle)
	: m_pFileHandle(pFileHandle), m_iWidth(0), m_iHeight(0),
	  m_iBpp(0), m_iHeaderSize(0), m_lFileSize(0), m_uPixels(NULL)
{

}

ncITextureLoader::~ncITextureLoader()
{
	if (m_uPixels)
		delete[] m_uPixels;

	if (m_pFileHandle)
		delete m_pFileHandle;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns the proper texture loader according to the file extension
ncITextureLoader* ncITextureLoader::CreateFromFile(const char *pFilename)
{
	// Creating a handle from ncIFile static method to detect assets file
	ncIFile *pFileHandle = ncIFile::CreateFileHandle(pFilename);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncITextureLoader::CreateFromFile - Loading file: \"%s\"", pFileHandle->Filename());

	if (pFileHandle->HasExtension("dds"))
		return new ncTextureLoaderDDS(pFileHandle);
	if (pFileHandle->HasExtension("pvr"))
		return new ncTextureLoaderPVR(pFileHandle);
#ifdef WITH_SDLIMAGE
	else if (pFileHandle->HasExtension("png") || pFileHandle->HasExtension("jpg"))
		return new ncTextureLoaderSDL(pFileHandle);
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

	m_lFileSize = m_pFileHandle->Size() - m_iHeaderSize;
	m_pFileHandle->Seek(m_iHeaderSize, SEEK_SET);

	m_uPixels =  new unsigned char[m_lFileSize];
	m_pFileHandle->Read(m_uPixels, m_lFileSize);
}
