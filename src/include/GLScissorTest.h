#ifndef CLASS_NCINE_GLSCISSORTEST
#define CLASS_NCINE_GLSCISSORTEST

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include "Rect.h"

namespace ncine {

/// A class to handle OpenGL scissor test
class GLScissorTest
{
  public:
	static bool isEnabled() { return enabled_; }
	static Recti rect() { return Recti(x_, y_, width_, height_); }
	static void enable(GLint x, GLint y, GLsizei width, GLsizei height);
	static void enable(const Recti &rect) { enable(rect.x, rect.y, rect.w, rect.h); }
	static void enable();
	static void disable();

	static void pushState();
	static void popState();

  private:
	static bool enabled_;
	static GLint x_;
	static GLint y_;
	static GLsizei width_;
	static GLsizei height_;

	static bool stateSaved_;
	static bool wasEnabled_;
	static GLint oldX_;
	static GLint oldY_;
	static GLsizei oldWidth_;
	static GLsizei oldHeight_;

	GLScissorTest() = delete;
	~GLScissorTest() = delete;

	/// Deleted copy constructor
	GLScissorTest(const GLScissorTest &) = delete;
	/// Deleted assignment operator
	GLScissorTest &operator=(const GLScissorTest &) = delete;
};

}

#endif
