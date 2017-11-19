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
	int value(GLIntValues valueName) const override;
	bool hasExtension(GLExtensions extensionName) const override;

  private:
	int glMajorVersion_;
	int glMinorVersion_;
	/// The OpenGL release version number (not available in OpenGL ES)
	int glReleaseVersion_;

	/// Array of OpenGL integer values
	int glIntValues_[IGfxCapabilities::NUM_INTVALUES];
	/// Array of OpenGL extension availability flags
	bool glExtensions_[IGfxCapabilities::NUM_EXTENSIONS];

	/// Queries the device about its runtime graphics capabilities
	void init();

	/// Logs OpenGL device info
	void logGLInfo();
	/// Logs OpenGL extensions
	void logGLExtensions();
	/// Logs OpenGL device capabilites
	void logGLCaps() const;

	/// Checks for an OpenGL extension
	bool checkGLExtension(const char *extensionName) const;
};

}

#endif
