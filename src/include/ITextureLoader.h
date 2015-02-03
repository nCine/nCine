#ifndef CLASS_NCINE_ITEXTURELOADER
#define CLASS_NCINE_ITEXTURELOADER

#define NCINE_INCLUDE_OPENGL
#include "common.h"

#include "TextureFormat.h"
#include "Point.h"
#include "IFile.h"

namespace ncine {

/// Texture loader interface
class ITextureLoader
{
  public:
	virtual ~ITextureLoader();

	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture size as a Point class
	inline Point size() const { return Point(width_, height_); }
	/// Returns texture bytes per pixel
	inline int bpp() const { return bpp_; }
	/// Returns the number of MIP maps stored in the texture file
	inline int mipMapCount() const { return mipMapCount_; }
	/// Returns texture data size in bytes
	inline long dataSize() const { return dataSize_; }
	// Returns the texture data size in bytes for the specified MIP map level
	long dataSize(unsigned int mipMapLevel) const;
	/// Returns the texture format class
	inline const TextureFormat& texFormat() const { return texFormat_; }
	/// Returns the pointer to pixel data
	inline const GLubyte* pixels() const { return pixels_; }
	// Returns the pointer to pixel data for the specified MIP map level
	const GLubyte* pixels(unsigned int mipMapLevel) const;

	// Returns the proper texture loader according to the file extension
	static ITextureLoader* createFromFile(const char *filename);

  protected:
	/// Texture file handle
	IFile *fileHandle_;

	int width_;
	int height_;
	int bpp_;
	int headerSize_;
	long dataSize_;
	int mipMapCount_;
	long *mipDataOffsets_;
	long *mipDataSizes_;
	TextureFormat texFormat_;
	GLubyte* pixels_;

	ITextureLoader(const char *filename);
	ITextureLoader(IFile *fileHandle);

	// Loads pixel data from a texture file holding either compressed or uncompressed data
	void loadPixels(GLenum internalFormat);
	// Loads pixel data from a texture file holding either compressed or uncompressed data, overriding pixel type
	void loadPixels(GLenum internalFormat, GLenum type);

	/// Load the texture data from file
	virtual void init() = 0;
};

}

#endif
