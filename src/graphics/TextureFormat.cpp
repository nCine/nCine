#include <cstdlib> // for exit()
#include "TextureFormat.h"
#include "../ServiceLocator.h"


///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureFormat::TextureFormat(GLenum eInternalFormat)
	: m_eInternalFormat(eInternalFormat), m_eFormat(-1), m_eType(-1)
{
	bool bFound = IntegerFormat();
	if (bFound == false)
		bFound = FloatFormat();
	if (bFound == false)
		bFound = CompressedFormat();
	if (bFound == false)
		bFound = OESFormat();

	if (bFound == false) {
		ServiceLocator::GetLogger().Write(2, (char *)"TextureFormat::TextureFormat - Unknown internal format: %d", eInternalFormat);
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Return the corresponding BGR format
GLenum TextureFormat::BGRFormat() const
{
	if (m_eFormat == GL_RGBA)
		return GL_BGRA;
	if (m_eFormat == GL_RGB)
		return GL_BGR;
	else
		return m_eFormat;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Search a match between an integer internal format and an external one
bool TextureFormat::IntegerFormat()
{
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
		case GL_LUMINANCE8:
		case GL_LUMINANCE:
		case 1:
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

/// Search a match between a floating point internal format and an external one
bool TextureFormat::FloatFormat()
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

/// Search a match between a compressed internal format and an external one
bool TextureFormat::CompressedFormat()
{
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
		m_eType = GL_UNSIGNED_BYTE;

	return bFound;
}

/// Search a match between an OpenGL ES internal format and an external one
bool TextureFormat::OESFormat()
{
	bool bFound = true;
/*
	switch(m_eInternalFormat)
	{
		case GL_RGBA8_OES:
			m_eFormat = GL_RGBA;
			break;
		case GL_RGB8_OES:
			m_eFormat = GL_RGB;
			break;
		default:
			bFound = false;
			break;
	}

	if (bFound)
		m_eType = GL_UNSIGNED_BYTE;

	return bFound;
*/
	return false;
}
