#ifndef CLASS_NCINE_GLBLENDING
#define CLASS_NCINE_GLBLENDING

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL blending
class GLBlending
{
  public:
	static bool isEnabled() { return enabled_; }
	static void enable();
	static void disable();
	static void blendFunc(GLenum sfactor, GLenum dfactor);
	static void blendFunc(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha);

	static void pushState();
	static void popState();

  private:
	static bool enabled_;
	static GLenum srcRgb_;
	static GLenum dstRgb_;
	static GLenum srcAlpha_;
	static GLenum dstAlpha_;

	static bool stateSaved_;
	static bool wasEnabled_;
	static GLenum oldSrcRgb_;
	static GLenum oldDstRgb_;
	static GLenum oldSrcAlpha_;
	static GLenum oldDstAlpha_;

	GLBlending() = delete;
	~GLBlending() = delete;

	/// Deleted copy constructor
	GLBlending(const GLBlending &) = delete;
	/// Deleted assignment operator
	GLBlending &operator=(const GLBlending &) = delete;
};

}

#endif
