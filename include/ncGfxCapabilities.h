#ifndef CLASS_NCGFXCAPABILITIES
#define CLASS_NCGFXCAPABILITIES

/// A class to query and store OpenGL device runtime capabilities
class ncGfxCapabilities
{
private:
	int m_iGLMajor;
	int m_iGLMinor;
#ifndef __ANDROID__
	int m_iGLRelease;
#endif

	int m_iMaxTextureSize;
	int m_iMaxTextureUnits;

#ifndef __ANDROID__
	bool m_bEXTTextureCompressionS3TC;
#else
	bool m_bOESCompressedETC1RGB8Texture;
	bool m_bAMDCompressedATCTexture;
#endif

	// Queries the device about its capabilities
	void Init();

	friend class ncSDLGfxDevice;
	friend class ncEGLGfxDevice;

public:
	/// Default constructor
	ncGfxCapabilities();
	~ncGfxCapabilities() { }

	// Logs OpenGL device info
	void LogGLInfo();
	// Logs OpenGL extensions
	void LogGLExtensions();
	// Logs OpenGL device capabilites
	void LogGLCaps() const;

	// Check for an OpenGL extension
	bool CheckGLExtension(const char *pExtensionName) const;

	/// Returns OpenGL major version number
	inline int GLMajor() const { return m_iGLMajor; }
	/// Returns OpenGL minor version number
	inline int GLMinor() const { return m_iGLMinor; }
#ifndef __ANDROID__
	/// Returns OpenGL release version number
	inline int GLRelease() const { return m_iGLRelease; }
#endif

	/// Returns a rough estimate of the largest texture that the device can handle
	inline int MaxTextureSize() const { return m_iMaxTextureSize; }
	/// Returns the number of texture units supported
	inline int MaxTextureUnits() const { return m_iMaxTextureUnits; }

#ifndef __ANDROID__
	/// Returns true if the extension GL_EXT_texture_compression_s3tc is available
	inline bool EXTTextureCompressionS3TC() const { return m_bEXTTextureCompressionS3TC; }
#else
	/// Returns true if the extension GL_OES_compressed_ETC1_RGB8_texture is available
	inline bool OESCompressedETC1RGB8Texture() const { return m_bOESCompressedETC1RGB8Texture; }
	/// Returns true if the extension GL_AMD_compressed_ATC_texture is available
	inline bool AMDCompressedATCTexture() const { return m_bAMDCompressedATCTexture; }
#endif
};

#endif
