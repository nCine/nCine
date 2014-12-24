#ifndef CLASS_NCINE_GFXCAPABILITIES
#define CLASS_NCINE_GFXCAPABILITIES

namespace ncine {

/// A class to query and store OpenGL device runtime capabilities
class GfxCapabilities
{
  public:
	/// Default constructor
	GfxCapabilities();

	// Logs OpenGL device info
	void logGLInfo();
	// Logs OpenGL extensions
	void logGLExtensions();
	// Logs OpenGL device capabilites
	void logGLCaps() const;

	// Check for an OpenGL extension
	bool checkGLExtension(const char *extensionName) const;

	/// Returns OpenGL major version number
	inline int majorGL() const { return majorGL_; }
	/// Returns OpenGL minor version number
	inline int minorGL() const { return minorGL_; }
#ifndef __ANDROID__
	/// Returns OpenGL release version number
	inline int releaseGL() const { return releaseGL_; }
#endif

	/// Returns a rough estimate of the largest texture that the device can handle
	inline int maxTextureSize() const { return maxTextureSize_; }
	/// Returns the number of texture units supported
	inline int maxTextureUnits() const { return maxTextureUnits_; }

#ifndef __ANDROID__
	/// Returns true if the extension GL_EXT_texture_compression_s3tc is available
	inline bool extTextureCompressionS3TC() const { return extTextureCompressionS3TC_; }
#else
	/// Returns true if the extension GL_OES_compressed_ETC1_RGB8_texture is available
	inline bool oesCompressedETC1RGB8Texture() const { return oesCompressedETC1RGB8Texture_; }
	/// Returns true if the extension GL_AMD_compressed_ATC_texture is available
	inline bool amdCompressedATCTexture() const { return amdCompressedATCTexture_; }
	/// Returns true if the extension GL_IMG_texture_compression_pvrtc is available
	inline bool imgTextureCompressionPVRTC() const { return imgTextureCompressionPVRTC_; }
#endif

  private:
	int majorGL_;
	int minorGL_;
#ifndef __ANDROID__
	int releaseGL_;
#endif

	int maxTextureSize_;
	int maxTextureUnits_;

#ifndef __ANDROID__
	bool extTextureCompressionS3TC_;
#else
	bool oesCompressedETC1RGB8Texture_;
	bool amdCompressedATCTexture_;
	bool imgTextureCompressionPVRTC_;
#endif

	// Queries the device about its capabilities
	void init();

	friend class IGfxDevice;
};

}

#endif
