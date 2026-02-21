#ifndef CLASS_NCINE_IIMAGELOADER
#define CLASS_NCINE_IIMAGELOADER

#include <nctl/UniquePtr.h>
#include "Vector2.h"

namespace ncine {

class IFile;

/// Image loader interface class
class IImageLoader
{
  public:
	enum class Format
	{
		R8,
		RG8,
		RGB8,
		RGBA8
	};

	virtual ~IImageLoader();

	/// Returns true if the texture has been correctly loaded
	inline bool hasLoaded() const { return hasLoaded_; }

	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture size as a `Vector2<int>` class
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns texture data size in bytes
	inline unsigned long dataSize() const { return dataSize_; }
	/// Returns the texture format object
	inline const Format &format() const { return format_; }
	/// Returns the number of color channels based on the image format
	unsigned int numChannels() const;
	/// Returns the pointer to pixel data
	inline const unsigned char *pixels() const { return pixels_.get(); }

	/// Returns the proper texture loader according to the memory buffer name extension
	static nctl::UniquePtr<IImageLoader> createFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	/// Returns the proper texture loader according to the file extension
	static nctl::UniquePtr<IImageLoader> createFromFile(const char *filename);

  protected:
	/// A flag indicating if the loading process has been successful
	bool hasLoaded_;
	/// Texture file handle
	nctl::UniquePtr<IFile> fileHandle_;

	int width_;
	int height_;
	unsigned long dataSize_;
	Format format_;
	nctl::UniquePtr<unsigned char[]> pixels_;

	explicit IImageLoader(nctl::UniquePtr<IFile> fileHandle);

	static nctl::UniquePtr<IImageLoader> createLoader(nctl::UniquePtr<IFile> fileHandle, const char *filename);

	friend class ITextureLoader;
};

/// A class created when the image file extension is not recognized
class InvalidImageLoader : IImageLoader
{
  public:
	explicit InvalidImageLoader(nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
