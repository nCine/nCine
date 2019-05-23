#ifndef CLASS_NCINE_GLDEPTHTEST
#define CLASS_NCINE_GLDEPTHTEST

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL depth test
class GLDepthTest
{
  public:
	static bool isEnabled() { return enabled_; }
	static void enable();
	static void disable();

	static bool isDepthMaskEnabled() { return depthMaskEnabled_; }
	static void enableDepthMask();
	static void disableDepthMask();

	static void pushState();
	static void popState();

  private:
	static bool enabled_;
	static bool depthMaskEnabled_;

	static bool stateSaved_;
	static bool wasEnabled_;
	static bool depthMaskWasEnabled_;

	GLDepthTest() = delete;
	~GLDepthTest() = delete;

	/// Deleted copy constructor
	GLDepthTest(const GLDepthTest &) = delete;
	/// Deleted assignment operator
	GLDepthTest &operator=(const GLDepthTest &) = delete;
};

}

#endif
