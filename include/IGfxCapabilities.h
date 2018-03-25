#ifndef CLASS_NCINE_IGFXCAPABILITIES
#define CLASS_NCINE_IGFXCAPABILITIES

#include "common_defines.h"

namespace ncine {

/// The interface class to query runtime OpenGL device capabilities
class DLL_PUBLIC IGfxCapabilities
{
  public:
	/// OpenGL version components
	enum class GLVersion
	{
		MAJOR,
		MINOR,
		RELEASE
	};

	/// OpenGL queryable runtime integer values
	struct GLIntValues
	{
		enum Enum
		{
			MAX_TEXTURE_SIZE = 0,
			MAX_TEXTURE_IMAGE_UNITS,
			MAX_UNIFORM_BLOCK_SIZE,
			MAX_UNIFORM_BUFFER_BINDINGS,
			MAX_VERTEX_UNIFORM_BLOCKS,
			MAX_FRAGMENT_UNIFORM_BLOCKS,
			UNIFORM_BUFFER_OFFSET_ALIGNMENT,

			COUNT
		};
	};

	/// OpenGL queryable extensions
	struct GLExtensions
	{
		enum Enum
		{
			KHR_DEBUG = 0,
			EXT_TEXTURE_COMPRESSION_S3TC,
			OES_COMPRESSED_ETC1_RGB8_TEXTURE,
			AMD_COMPRESSED_ATC_TEXTURE,
			IMG_TEXTURE_COMPRESSION_PVRTC,
			KHR_TEXTURE_COMPRESSION_ASTC_LDR,

			COUNT
		};
	};

	virtual ~IGfxCapabilities() = 0;

	/// Returns the OpenGL version numbers
	virtual int glVersion(GLVersion version) const = 0;
	/// Returns the value of a runtime OpenGL integer value
	virtual int value(GLIntValues::Enum valueName) const = 0;
	/// Returns true if the specified OpenGL extension is available
	virtual bool hasExtension(GLExtensions::Enum extensionName) const = 0;
};

inline IGfxCapabilities::~IGfxCapabilities() { }


/// A fake graphics capabilities class that reports no available capabilities
class DLL_PUBLIC NullGfxCapabilities : public IGfxCapabilities
{
  public:
	int glVersion(GLVersion version) const override { return 0; }
	int value(GLIntValues::Enum valueName) const override { return 0; }
	bool hasExtension(GLExtensions::Enum extensionName) const override { return false; }
};

}

#endif
