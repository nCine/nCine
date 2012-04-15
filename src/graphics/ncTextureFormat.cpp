#include <cstdlib> // for exit()
#include "ncTextureFormat.h"
#include "ncServiceLocator.h"


///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureFormat::ncTextureFormat(GLenum eInternalFormat)
	: m_eInternalFormat(eInternalFormat), m_eFormat(-1),
	  m_eType(-1), m_bCompressed(false)
{
	FindExternalFmt();
}

ncTextureFormat::ncTextureFormat(GLenum eInternalFormat, GLenum eType)
	: m_eInternalFormat(eInternalFormat), m_eFormat(-1),
	  m_eType(-1), m_bCompressed(false)
{
	FindExternalFmt();
	// Overriding the type found by FindExternalFmt()
	m_eType = eType;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
/// Converts the external format to the corresponding BGR one
void ncTextureFormat::BGRFormat()
{
	if (m_eFormat == GL_RGBA)
		m_eFormat = GL_BGRA;
	else if (m_eFormat == GL_RGB)
		m_eFormat = GL_BGR;
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to find an external format corresponding to the internal one
void ncTextureFormat::FindExternalFmt()
{
	bool bFound = false;

#ifndef __ANDROID__
	if (bFound == false)
		bFound = IntegerFormat();
	if (bFound == false)
		bFound = FloatFormat();
	if (bFound == false)
		bFound = CompressedFormat();
#else
	if (bFound == false)
		bFound = OESFormat();
	if (bFound == false)
		bFound = OESCompressedFormat();
#endif

	if (bFound == false)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureFormat::FindExternalFmt - Unknown internal format: %d", m_eInternalFormat);
		exit(-1);
	}
}

#ifndef __ANDROID__
/// Searches a match between an integer internal format and an external one
bool ncTextureFormat::IntegerFormat()
{
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_RGBA:
		case GL_RGBA8:
		case 4:
			m_eFormat = GL_RGBA;
			break;
		case GL_RGB:
		case GL_RGB8:
		case 3:
			m_eFormat = GL_RGB;
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE8_ALPHA8:
		case 2:
			m_eFormat = GL_LUMINANCE_ALPHA;
			break;
		case GL_ALPHA:
		case GL_ALPHA8:
		case 1:
			m_eFormat = GL_ALPHA;
			break;
		case GL_LUMINANCE:
		case GL_LUMINANCE8:
			m_eFormat = GL_LUMINANCE;
			break;
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_COMPONENT16_ARB:
		case GL_DEPTH_COMPONENT24_ARB:
		case GL_DEPTH_COMPONENT32_ARB:
			m_eFormat = GL_DEPTH_COMPONENT;
			break;
		default:
			bFound = false;
			break;
	}

	if (bFound)
		m_eType = GL_UNSIGNED_BYTE;

	return bFound;
}

/// Searches a match between a floating point internal format and an external one
bool ncTextureFormat::FloatFormat()
{
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_RGBA16F_ARB:
		case GL_RGBA32F_ARB:
			m_eFormat = GL_RGBA;
			break;
		case GL_RGB16F_ARB:
		case GL_RGB32F_ARB:
			m_eFormat = GL_RGB;
			break;
		default:
			bFound = false;
			break;
	}

	if (bFound)
		m_eType = GL_FLOAT;

	return bFound;
}

/// Searches a match between a compressed internal format and an external one
bool ncTextureFormat::CompressedFormat()
{
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_COMPRESSED_RGBA:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			m_eFormat = GL_RGBA;
			break;
		case GL_COMPRESSED_RGB:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			m_eFormat = GL_RGB;
			break;
		default:
			bFound = false;
			break;
	}

	if (bFound)
	{
		m_eType = GL_UNSIGNED_BYTE;
		m_bCompressed = true;
	}

	return bFound;
}
#else
/// Searches a match between an OpenGL ES internal format and an external one
bool ncTextureFormat::OESFormat()
{
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_RGBA:
			m_eFormat = GL_RGBA;
			break;
		case GL_RGB:
			m_eFormat = GL_RGB;
			break;
		case GL_LUMINANCE_ALPHA:
			m_eFormat = GL_LUMINANCE_ALPHA;
			break;
		case GL_LUMINANCE:
			m_eFormat = GL_LUMINANCE;
			break;
		case GL_ALPHA:
			m_eFormat = GL_ALPHA;
			break;
		default:
			bFound = false;
			break;
	}

	if (bFound)
		m_eType = GL_UNSIGNED_BYTE;

	return bFound;
}

/// Searches a match between a OpenGL ES compressed internal format and an external one
bool ncTextureFormat::OESCompressedFormat()
{
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
			m_eFormat = GL_RGBA;
			break;
		case GL_ETC1_RGB8_OES:
		case GL_ATC_RGB_AMD:
			m_eFormat = GL_RGB;
			break;
		default:
			bFound = false;
			break;
	}

	if (bFound)
	{
		m_eType = GL_UNSIGNED_BYTE;
		m_bCompressed = true;
	}

	return bFound;
}
#endif
