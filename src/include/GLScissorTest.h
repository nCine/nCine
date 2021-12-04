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
	struct State
	{
		bool enabled = false;
		Recti rect = Recti(0, 0, 0, 0);
	};

	static bool isEnabled() { return state_.enabled; }
	static Recti rect() { return state_.rect; }
	static void enable(const Recti &rect);
	static void enable(GLint x, GLint y, GLsizei width, GLsizei height);
	static void enable();
	static void disable();

	static State state() { return state_; }
	static void setState(State newState);

  private:
	static State state_;

	/// Deleted default constructor
	GLScissorTest() = delete;
	/// Deleted destructor
	~GLScissorTest() = delete;

	/// Deleted copy constructor
	GLScissorTest(const GLScissorTest &) = delete;
	/// Deleted assignment operator
	GLScissorTest &operator=(const GLScissorTest &) = delete;
};

}

#endif
