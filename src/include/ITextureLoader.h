#ifndef CLASS_NCINE_ITEXTURELOADER
#define CLASS_NCINE_ITEXTURELOADER

#include "TextureFormat.h"
#include "Vector2.h"
#include "IFile.h"

namespace ncine {

class IFile;

/// Texture loader interface class
class ITextureLoader
{
  public:
	virtual ~ITextureLoader() {}

	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture size as a `Vector2<int>` class
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns texture bytes per pixel
	inline int bpp() const { return bpp_; }
	/// Returns the number of MIP maps stored in the texture file
	inline int mipMapCount() const { return mipMapCount_; }
	/// Returns texture data size in bytes
	inline unsigned long dataSize() const { return dataSize_; }
	/// Returns the texture data size in bytes for the specified MIP map level
	long dataSize(unsigned int mipMapLevel) const;
	/// Returns the texture format object
	inline const TextureFormat &texFormat() const { return texFormat_; }
	/// Returns the pointer to pixel data
	inline const GLubyte *pixels() const { return pixels_.get(); }
	/// Returns the pointer to pixel data for the specified MIP map level
	const GLubyte *pixels(unsigned int mipMapLevel) const;

	/// Returns the proper texture loader according to the file extension
	static nctl::UniquePtr<ITextureLoader> createFromFile(const char *filename);

  protected:
	/// Texture file handle
	nctl::UniquePtr<IFile> fileHandle_;

	int width_;
	int height_;
	int bpp_;
	int headerSize_;
	unsigned long dataSize_;
	int mipMapCount_;
	nctl::UniquePtr<unsigned long[]> mipDataOffsets_;
	nctl::UniquePtr<unsigned long[]> mipDataSizes_;
	TextureFormat texFormat_;
	nctl::UniquePtr<GLubyte[]> pixels_;

	explicit ITextureLoader(const char *filename);
	explicit ITextureLoader(nctl::UniquePtr<IFile> fileHandle);

	/// Loads pixel data from a texture file holding either compressed or uncompressed data
	void loadPixels(GLenum internalFormat);
	/// Loads pixel data from a texture file holding either compressed or uncompressed data, overriding pixel type
	void loadPixels(GLenum internalFormat, GLenum type);
};

}

#endif
