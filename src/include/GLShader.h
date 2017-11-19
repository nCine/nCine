#ifndef CLASS_NCINE_GLSHADER
#define CLASS_NCINE_GLSHADER

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL shader objects
class GLShader
{
  public:
	inline GLuint glHandle() const { return glHandle_; }

	void loadFromString(const char *string);
	void loadFromFile(const char *filename);
	bool compile();

  private:
	GLuint glHandle_;

	explicit GLShader(GLenum type);
	GLShader(GLenum type, const char *filename);
	~GLShader();

	/// Deleted copy constructor
	GLShader(const GLShader &) = delete;
	/// Deleted assignment operator
	GLShader &operator=(const GLShader &) = delete;

	friend class GLShaderProgram;
};

}

#endif
