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

	bool IntegerFormat();
	bool FloatFormat();
	bool CompressedFormat();
	bool OESFormat();
	bool OESCompressedFormat();
public:
	ncTextureFormat(GLenum eInternalFormat);

	/// Return the specified internal format
	inline GLenum Internal() const { return m_eInternalFormat; }
	/// Return the corresponding format
	inline GLenum Format() const { return m_eFormat; }
	// Return the corresponding BGR format
	GLenum BGRFormat() const;
	/// Return the corresponding pixel data type
	inline GLenum Type() const { return m_eType; }
	/// Specify wheter data is compressed or not
	inline bool isCompressed() const { return m_bCompressed; }
};

#endif
