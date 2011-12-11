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
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureFormat::ncTextureFormat - Unknown internal format: %d", eInternalFormat);
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Converts to the corresponding BGR format
void ncTextureFormat::BGRFormat()
{
#ifndef __ANDROID__
	if (m_eFormat == GL_RGBA)
		m_eFormat = GL_BGRA;
	else if (m_eFormat == GL_RGB)
		m_eFormat = GL_BGR;
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Searches a match between an integer internal format and an external one
bool ncTextureFormat::IntegerFormat()
{
#ifndef __ANDROID__
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_RGBA8:
		case 4:
			m_eFormat = GL_RGBA;
			break;
		case GL_RGB8:
		case 3:
			m_eFormat = GL_RGB;
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE8_ALPHA8:
		case 2:
			m_eFormat = GL_LUMINANCE_ALPHA;
			break;
		case GL_ALPHA8:
		case GL_ALPHA:
		case 1:
			m_eFormat = GL_ALPHA;
			break;
		case GL_LUMINANCE8:
		case GL_LUMINANCE:
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
#endif
}

/// Searches a match between a floating point internal format and an external one
bool ncTextureFormat::FloatFormat()
{
#ifndef __ANDROID__
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
#endif
}

/// Searches a match between a compressed internal format and an external one
bool ncTextureFormat::CompressedFormat()
{
#ifndef __ANDROID__
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_COMPRESSED_RGBA:
			m_eFormat = GL_RGBA;
			break;
		case GL_COMPRESSED_RGB:
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
#endif
}

/// Searches a match between an OpenGL ES internal format and an external one
bool ncTextureFormat::OESFormat()
{
#ifdef __ANDROID__
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_RGBA8_OES:
			m_eFormat = GL_RGBA;
			break;
		case GL_RGB8_OES:
			m_eFormat = GL_RGB;
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
#endif
}

/// Searches a match between a OpenGL ES compressed internal format and an external one
bool ncTextureFormat::OESCompressedFormat()
{
#ifdef __ANDROID__
	bool bFound = true;

	switch(m_eInternalFormat)
	{
		case GL_ETC1_RGB8_OES:
			m_eFormat = GL_RGB;
			break;
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
			m_eFormat = GL_RGBA;
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
#endif
}
