#ifndef CLASS_NCITEXTURELOADER
#define CLASS_NCITEXTURELOADER

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

#include "ncTextureFormat.h"
#include "ncPoint.h"
#include "ncIFile.h"

/// Texture loader interface
class ncITextureLoader
{
  public:
	virtual ~ncITextureLoader();

	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture size as a ncPoint class
	inline ncPoint size() const { return ncPoint(width_, height_); }
	/// Returns texture bytes per pixel
	inline int bpp() const { return bpp_; }
	/// Returns the number of MIP maps stored in the texture file
	inline int mipMapCount() const { return mipMapCount_; }
	/// Returns texture data size in bytes
	inline long dataSize() const { return dataSize_; }
	// Returns the texture data size in bytes for the specified MIP map level
	long dataSize(unsigned int mipMapLevel) const;
	/// Returns the texture format class
	inline const ncTextureFormat& texFormat() const { return texFormat_; }
	/// Returns the pointer to pixel data
	inline const GLubyte* pixels() const { return pixels_; }
	// Returns the pointer to pixel data for the specified MIP map level
	const GLubyte* pixels(unsigned int mipMapLevel) const;

	// Returns the proper texture loader according to the file extension
	static ncITextureLoader* createFromFile(const char *filename);

  protected:
	/// Texture file handle
	ncIFile *fileHandle_;

	int width_;
	int height_;
	int bpp_;
	int headerSize_;
	long dataSize_;
	int mipMapCount_;
	long *mipDataOffsets_;
	long *mipDataSizes_;
	ncTextureFormat texFormat_;
	GLubyte* pixels_;

	ncITextureLoader(const char *filename);
	ncITextureLoader(ncIFile *fileHandle);

	// Loads pixel data from a texture file holding either compressed or uncompressed data
	void loadPixels(GLenum internalFormat);
	// Loads pixel data from a texture file holding either compressed or uncompressed data, overriding pixel type
	void loadPixels(GLenum internalFormat, GLenum type);

	/// Load the texture data from file
	virtual void init() = 0;
};

#endif
