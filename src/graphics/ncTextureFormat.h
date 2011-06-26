#ifndef CLASS_NCTEXTUREFORMAT
#define CLASS_NCTEXTUREFORMAT

#include <GL/gl.h>
#include <GL/glext.h>


/// Texture format class
class ncTextureFormat
{
private:
	GLenum m_eInternalFormat;
	GLenum m_eFormat;
	GLenum m_eType;

	bool IntegerFormat();
	bool FloatFormat();
	bool CompressedFormat();
	bool OESFormat();
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
};

#endif
