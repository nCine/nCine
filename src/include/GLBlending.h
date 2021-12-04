#ifndef CLASS_NCINE_GLBLENDING
#define CLASS_NCINE_GLBLENDING

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL blending
class GLBlending
{
  public:
	struct State
	{
		bool enabled = false;
		GLenum srcRgb = GL_ONE;
		GLenum dstRgb = GL_ZERO;
		GLenum srcAlpha = GL_ONE;
		GLenum dstAlpha = GL_ZERO;
	};

	static bool isEnabled() { return state_.enabled; }
	static void enable();
	static void disable();
	static void setBlendFunc(GLenum sfactor, GLenum dfactor);
	static void setBlendFunc(GLenum srcRgb, GLenum dstRgb, GLenum srcAlpha, GLenum dstAlpha);

	static State state() { return state_; }
	static void setState(State newState);

  private:
	static State state_;

	/// Deleted default constructor
	GLBlending() = delete;
	/// Deleted destructor
	~GLBlending() = delete;

	/// Deleted copy constructor
	GLBlending(const GLBlending &) = delete;
	/// Deleted assignment operator
	GLBlending &operator=(const GLBlending &) = delete;
};

}

#endif
