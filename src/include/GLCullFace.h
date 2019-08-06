#ifndef CLASS_NCINE_GLCULLFACE
#define CLASS_NCINE_GLCULLFACE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL face culling
class GLCullFace
{
  public:
	static bool isEnabled() { return enabled_; }
	static void enable();
	static void disable();
	static void set(GLenum set);

	static void pushState();
	static void popState();

  private:
	static bool enabled_;
	static GLenum mode_;

	static bool stateSaved_;
	static bool wasEnabled_;
	static GLenum oldMode_;

	GLCullFace() = delete;
	~GLCullFace() = delete;

	/// Deleted copy constructor
	GLCullFace(const GLCullFace &) = delete;
	/// Deleted assignment operator
	GLCullFace &operator=(const GLCullFace &) = delete;
};

}

#endif
