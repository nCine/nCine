#ifndef CLASS_NCINE_GLCULLFACE
#define CLASS_NCINE_GLCULLFACE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL face culling
class GLCullFace
{
  public:
	struct State
	{
		bool enabled = false;
		GLenum mode = GL_BACK;
	};

	static bool isEnabled() { return state_.enabled; }
	static void enable();
	static void disable();
	static void setMode(GLenum mode);

	static State state() { return state_; }
	static void setState(State newState);

  private:
	static State state_;

	/// Deleted default constructor
	GLCullFace() = delete;
	/// Deleted destructor
	~GLCullFace() = delete;

	/// Deleted copy constructor
	GLCullFace(const GLCullFace &) = delete;
	/// Deleted assignment operator
	GLCullFace &operator=(const GLCullFace &) = delete;
};

}

#endif
