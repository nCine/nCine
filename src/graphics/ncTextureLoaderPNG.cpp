#include <cstdlib> // for exit()
#include <cstring>
#include "ncTextureLoaderPNG.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderPNG::ncTextureLoaderPNG(const char *pFilename)
	: ncITextureLoader(pFilename)
{
	Init();
}

ncTextureLoaderPNG::ncTextureLoaderPNG(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderPNG::Init()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPNG::Init - Loading \"%s\"", m_pFileHandle->Filename());

	const int iSignatureLength = 8;
	unsigned char ubSignature[iSignatureLength];
	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);
	m_pFileHandle->Read(ubSignature, iSignatureLength);

	// Checking PNG signature
	if(!png_check_sig(ubSignature, iSignatureLength))
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::Init - PNG signature check failed");
		exit(-1);
	}

	// Get PNG file info struct (memory is allocated by libpng)
	png_structp png_ptr = NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(png_ptr == NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::Init - Cannot create png read structure");
		exit(-1);
	}

	// Get PNG image data info struct (memory is allocated by libpng)
	png_infop info_ptr = NULL;
	info_ptr = png_create_info_struct(png_ptr);

	if(info_ptr == NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::Init - Cannot create png info structure");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		exit(-1);
	}

	// Setting custom read function that uses an ncIFile as input
	png_set_read_fn(png_ptr, m_pFileHandle, ReadFromFileHandle);
	// Telling libpng the signature has already be read
	png_set_sig_bytes(png_ptr, iSignatureLength);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 uWidth = 0;
	png_uint_32 uHeight = 0;
	int iBitDepth = 0;
	int iColorType = -1;
	png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &uWidth, &uHeight, &iBitDepth, &iColorType, NULL, NULL, NULL);

	if(retval != 1)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::Init - Cannot create png info structure");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		exit(-1);
	}

	m_iWidth = uWidth;
	m_iHeight = uHeight;
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPNG::Init - Header found: w:%d h:%d", m_iWidth, m_iHeight);

	switch(iColorType)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			m_texFormat = ncTextureFormat(GL_RGBA);
			break;
		case PNG_COLOR_TYPE_RGB:
			m_texFormat = ncTextureFormat(GL_RGB);
			break;
		case PNG_COLOR_TYPE_GRAY:
			m_texFormat = ncTextureFormat(GL_ALPHA);;
			break;
		default:
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPNG::Init - Color type not supported: %d", iColorType);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			exit(-1);
			break;
	}

	// Row size in bytes
	const png_uint_32 uBytesPerRow = png_get_rowbytes(png_ptr, info_ptr);

	m_uPixels = new unsigned char[uBytesPerRow * m_iHeight];

	png_bytep *pRowPointers = new png_bytep[m_iHeight];
	for (int i = 0; i < m_iHeight; i++)
		pRowPointers[i] = m_uPixels + i * uBytesPerRow;

	// Decoding png data through row pointers
	png_read_image(png_ptr, pRowPointers);

	delete[] pRowPointers;

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

void ncTextureLoaderPNG::ReadFromFileHandle(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
	ncIFile *pFileHandle = reinterpret_cast<ncIFile *>(png_get_io_ptr(png_ptr));

	unsigned long int ulBytesRead = pFileHandle->Read(outBytes, byteCountToRead);
	if (ulBytesRead != byteCountToRead)
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncTextureLoaderPNG::ReadFromFileHandle - Read %l bytes instead of %u: %d",
			ulBytesRead, byteCountToRead);
}
