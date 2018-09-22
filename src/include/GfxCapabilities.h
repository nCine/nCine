#ifndef CLASS_NCINE_GFXCAPABILITIES
#define CLASS_NCINE_GFXCAPABILITIES

#include "IGfxCapabilities.h"

namespace ncine {

/// A class that stores and retrieves runtime OpenGL device capabilities
class GfxCapabilities : public IGfxCapabilities
{
  public:
	GfxCapabilities();

	int glVersion(GLVersion version) const override;
	inline const GlInfoStrings &glInfoStrings() const override { return glInfoStrings_; }
	int value(GLIntValues::Enum valueName) const override;
	bool hasExtension(GLExtensions::Enum extensionName) const override;

  private:
	int glMajorVersion_;
	int glMinorVersion_;
	/// The OpenGL release version number (not available in OpenGL ES)
	int glReleaseVersion_;

	GlInfoStrings glInfoStrings_;

	/// Array of OpenGL integer values
	int glIntValues_[IGfxCapabilities::GLIntValues::COUNT];
	/// Array of OpenGL extension availability flags
	bool glExtensions_[IGfxCapabilities::GLExtensions::COUNT];

	/// Queries the device about its runtime graphics capabilities
	void init();

	/// Logs OpenGL device info
	void logGLInfo();
	/// Logs OpenGL extensions
	void logGLExtensions();
	/// Logs OpenGL device capabilites
	void logGLCaps() const;

	/// Checks for OpenGL extensions availability
	void checkGLExtensions(const char *extensionNames[], bool results[], unsigned int numExtensionsToCheck) const;
};

}

#endif
