#ifndef CLASS_NCINE_TEXTURE
#define CLASS_NCINE_TEXTURE

#include "Object.h"
#include "Rect.h"

namespace ncine {

class ITextureLoader;
class GLTexture;

/// Texture class
class DLL_PUBLIC Texture : public Object
{
  public:
	/// Texture filtering modes
	enum class Filtering
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};

	/// Texture wrap modes
	enum class Wrap
	{
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		REPEAT
	};

	Texture(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	Texture(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize, int width, int height);
	Texture(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize, Vector2i size);

	explicit Texture(const char *filename);
	Texture(const char *filename, int width, int height);
	Texture(const char *filename, Vector2i size);

	~Texture() override;

	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture MIP map levels
	inline int mipMapLevels() const { return mipMapLevels_; }
	/// Returns texture size
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns texture rectangle
	inline Recti rect() const { return Recti(0, 0, width_, height_); }

	/// Returns true if the texture holds compressed data
	inline bool isCompressed() const { return isCompressed_; }
	/// Returns the number of color channels
	inline unsigned int numChannels() const { return numChannels_; }
	/// Returns the amount of video memory needed to load the texture
	inline unsigned long dataSize() const { return dataSize_; }

	/// Returns the texture filtering for minification
	inline Filtering minFiltering() const { return minFiltering_; }
	/// Returns the texture filtering for magnification
	inline Filtering magFiltering() const { return magFiltering_; }
	/// Returns texture wrap for both `s` and `t` coordinates
	inline Wrap wrap() const { return wrapMode_; }
	/// Sets the texture filtering for minification
	void setMinFiltering(Filtering filter);
	/// Sets the texture filtering for magnification
	void setMagFiltering(Filtering filter);
	/// Sets texture wrap for both `s` and `t` coordinates
	void setWrap(Wrap wrapMode);

	/// Returns the user data opaque pointer for ImGui's ImTextureID
	void *imguiTexId();

	inline static ObjectType sType() { return ObjectType::TEXTURE; }

  private:
	nctl::UniquePtr<GLTexture> glTexture_;
	int width_;
	int height_;
	int mipMapLevels_;
	bool isCompressed_;
	unsigned int numChannels_;
	unsigned long dataSize_;

	Filtering minFiltering_;
	Filtering magFiltering_;
	Wrap wrapMode_;

	/// Deleted copy constructor
	Texture(const Texture &) = delete;
	/// Deleted assignment operator
	Texture &operator=(const Texture &) = delete;

	/// Loads a texture overriding the size detected by the texture loader
	void load(const ITextureLoader &texLoader, int width, int height);

	/// Sets the OpenGL object label for the texture
	void setGLTextureLabel(const char *filename);

	friend class Material;
};

}

#endif
