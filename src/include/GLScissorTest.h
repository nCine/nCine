#ifndef CLASS_NCINE_GLSCISSORTEST
#define CLASS_NCINE_GLSCISSORTEST

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL scissor test
class GLScissorTest
{
  public:
	static void enable(GLint x, GLint y, GLsizei width, GLsizei height);
	static void enable();
	static void disable();

  private:
	static bool enabled_;
	static GLint x_;
	static GLint y_;
	static GLsizei width_;
	static GLsizei height_;

	GLScissorTest() = delete;
	~GLScissorTest() = delete;

	/// Deleted copy constructor
	GLScissorTest(const GLScissorTest &) = delete;
	/// Deleted assignment operator
	GLScissorTest &operator=(const GLScissorTest &) = delete;
};

}

#endif
