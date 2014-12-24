#ifndef CLASS_NCINE_TEXTUREFORMAT
#define CLASS_NCINE_TEXTUREFORMAT

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

namespace ncine {

/// Texture format class
class TextureFormat
{
  public:
	TextureFormat()
		: internalFormat_(-1), format_(-1), type_(-1), isCompressed_(false) { }
	TextureFormat(GLenum internalFormat);
	TextureFormat(GLenum internalFormat, GLenum type);

	/// Returns the specified internal format
	inline GLenum internalFormat() const { return internalFormat_; }
	/// Returns the corresponding format
	inline GLenum format() const { return format_; }
	/// Returns the corresponding pixel data type
	inline GLenum type() const { return type_; }
	/// Returns true if the format holds compressed data
	inline bool isCompressed() const { return isCompressed_; }
	// Returns true if the format provides an alpha channel
	bool hasAlpha() const;

#ifndef __ANDROID__
	// Converts the external format to the corresponding BGR one
	void bgrFormat();
#endif

	// Calculates the pixel data size for each MIP map level
	static long int calculateMipSizes(GLenum internalFormat, int width, int height, int mipMapCount, long int *mipDataOffsets, long int *mipDataSizes);

  private:
	GLenum internalFormat_;
	GLenum format_;
	GLenum type_;
	bool isCompressed_;

#ifndef __ANDROID__
	// Searches a match between an integer internal format and an external one
	bool integerFormat();
	// Searches a match between a floating point internal format and an external one
	bool floatFormat();
	// Searches a match between a compressed internal format and an external one
	bool compressedFormat();
#else
	// Searches a match between an OpenGL ES internal format and an external one
	bool oesFormat();
	// Searches a match between a OpenGL ES compressed internal format and an external one
	bool oesCompressedFormat();
#endif

	// Tries to find an external format corresponding to the internal one
	void findExternalFmt();
};

/// Returns true if the format provides an alpha channel
inline bool TextureFormat::hasAlpha() const
{
	return (format_ == GL_RGBA ||
#ifndef __ANDROID__
			format_ == GL_BGRA ||
#endif
			format_ == GL_LUMINANCE_ALPHA ||
			format_ == GL_ALPHA);
}

}

#endif
