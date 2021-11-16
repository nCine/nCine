#ifndef CLASS_NCINE_GLVIEWPORT
#define CLASS_NCINE_GLVIEWPORT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include "Rect.h"

namespace ncine {

/// A class to handle OpenGL viewport
class GLViewport
{
  public:
	struct State
	{
		Recti rect = Recti(0, 0, 0, 0);
	};

	static Recti rect() { return state_.rect; }
	static void setRect(const Recti &rect);
	static void setRect(GLint x, GLint y, GLsizei width, GLsizei height);

	static State state() { return state_; }
	static void setState(State newState);

  private:
	static State state_;

	static void initRect(GLint x, GLint y, GLsizei width, GLsizei height);

	/// Deleted default constructor
	GLViewport() = delete;
	/// Deleted destructor
	~GLViewport() = delete;

	/// Deleted copy constructor
	GLViewport(const GLViewport &) = delete;
	/// Deleted assignment operator
	GLViewport &operator=(const GLViewport &) = delete;

	friend class IGfxDevice; // for `initRect()`
};

}

#endif
