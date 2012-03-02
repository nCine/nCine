#ifndef CLASS_NCTEXTUREFORMAT
#define CLASS_NCTEXTUREFORMAT

#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif !defined(NO_GLEW)
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

/// Texture format class
class ncTextureFormat
{
private:
	GLenum m_eInternalFormat;
	GLenum m_eFormat;
	GLenum m_eType;
	bool m_bCompressed;

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

public:
	ncTextureFormat()
		: m_eInternalFormat(-1), m_eFormat(-1), m_eType(-1), m_bCompressed(false) { }
	ncTextureFormat(GLenum eInternalFormat);

	/// Returns the specified internal format
	inline GLenum Internal() const { return m_eInternalFormat; }
	/// Returns the corresponding format
	inline GLenum Format() const { return m_eFormat; }
	/// Returns the corresponding pixel data type
	inline GLenum Type() const { return m_eType; }
	/// Specifies wheter data is compressed or not
	inline bool isCompressed() const { return m_bCompressed; }

#ifndef __ANDROID__
	// Converts to the corresponding BGR format
	void BGRFormat();
#endif
};

#endif
