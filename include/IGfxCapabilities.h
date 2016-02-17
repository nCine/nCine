#ifndef CLASS_NCINE_IGFXCAPABILITIES
#define CLASS_NCINE_IGFXCAPABILITIES

#include "common_defines.h"

namespace ncine {

/// The interface to query runtime OpenGL device capabilities
class DLL_PUBLIC IGfxCapabilities
{
  public:
	enum GLVersion
	{
		MAJOR = 0,
		MINOR,
		RELEASE,

		NUM_VERSIONS
	};

	enum GLIntValues
	{
		MAX_TEXTURE_SIZE = 0,
		MAX_TEXTURE_IMAGE_UNITS,

		NUM_INTVALUES
	};

	enum GLExtensions
	{
		EXT_TEXTURE_COMPRESSION_S3TC = 0,
		OES_COMPRESSED_ETC1_RGB8_TEXTURE,
		AMD_COMPRESSED_ATC_TEXTURE,
		IMG_TEXTURE_COMPRESSION_PVRTC,
		KHR_TEXTURE_COMPRESSION_ASTC_LDR,

		NUM_EXTENSIONS
	};

	virtual ~IGfxCapabilities() = 0;

	/// Returns OpenGL version numbers
	virtual int glVersion(GLVersion version) const = 0;
	/// Returns the value of a runtime OpenGL integer value
	virtual int value(GLIntValues valueName) const = 0;
	/// Returns true if the specified OpenGL extension is available
	virtual bool hasExtension(GLExtensions extensionName) const = 0;
};

inline IGfxCapabilities::~IGfxCapabilities() { }


/// A fake graphics capabilities class that reports no available capability
class DLL_PUBLIC NullGfxCapabilities : public IGfxCapabilities
{
  public:
	virtual int glVersion(GLVersion version) const { return 0; }
	virtual int value(GLIntValues valueName) const { return 0; }
	virtual bool hasExtension(GLExtensions extensionName) const { return false; }
};

}

#endif
