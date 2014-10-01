#ifndef CLASS_NCTEXTUREFORMAT
#define CLASS_NCTEXTUREFORMAT

#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif defined(WITH_GLEW)
	#include <GL/glew.h>
#elif defined(__APPLE__)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

/// Texture format class
class ncTextureFormat
{
 public:
	ncTextureFormat()
		: m_eInternalFormat(-1), m_eFormat(-1), m_eType(-1), m_bCompressed(false) { }
	ncTextureFormat(GLenum eInternalFormat);
	ncTextureFormat(GLenum eInternalFormat, GLenum eType);

	/// Returns the specified internal format
	inline GLenum Internal() const { return m_eInternalFormat; }
	/// Returns the corresponding format
	inline GLenum Format() const { return m_eFormat; }
	/// Returns the corresponding pixel data type
	inline GLenum Type() const { return m_eType; }
	/// Returns true if the format holds compressed data
	inline bool isCompressed() const { return m_bCompressed; }
	// Returns true if the format provides an alpha channel
	bool hasAlpha() const;

#ifndef __ANDROID__
	// Converts the external format to the corresponding BGR one
	void BGRFormat();
#endif

	// Calculates the pixel data size for each MIP map level
	static long int CalculateMipSizes(GLenum eInternalFormat, int iWidth, int iHeight, int iMipMapCount, long int *pMipDataOffsets, long int *pMipDataSizes);

 private:
#ifndef __ANDROID__
	// Searches a match between an integer internal format and an external one
	bool IntegerFormat();
	// Searches a match between a floating point internal format and an external one
	bool FloatFormat();
	// Searches a match between a compressed internal format and an external one
	bool CompressedFormat();
#else
	// Searches a match between an OpenGL ES internal format and an external one
	bool OESFormat();
	// Searches a match between a OpenGL ES compressed internal format and an external one
	bool OESCompressedFormat();
#endif

	// Tries to find an external format corresponding to the internal one
	void FindExternalFmt();

	GLenum m_eInternalFormat;
	GLenum m_eFormat;
	GLenum m_eType;
	bool m_bCompressed;
};

/// Returns true if the format provides an alpha channel
inline bool ncTextureFormat::hasAlpha() const
{
	return (m_eFormat == GL_RGBA ||
	#ifndef __ANDROID__
			m_eFormat == GL_BGRA ||
	#endif
			m_eFormat == GL_LUMINANCE_ALPHA ||
			m_eFormat == GL_ALPHA);
}

#endif
