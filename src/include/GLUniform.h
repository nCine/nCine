#ifndef CLASS_NCINE_GLUNIFORM
#define CLASS_NCINE_GLUNIFORM

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to store information about an active OpenGL shader uniform
/*! Its only purpose is to initialize a `GLUniformCache` class. */
class GLUniform
{
  public:
	GLUniform();
	GLUniform(GLuint program, GLuint index);

	inline GLint location() const { return location_; }
	inline GLint size() const { return size_; }
	inline GLenum type() const { return type_; }
	inline const char *name() const { return name_; }
	GLenum basicType() const;
	unsigned int numComponents() const;

  private:
	GLint location_;
	GLint size_;
	GLenum type_;
	static const unsigned int MaxNameLength = 32;
	char name_[MaxNameLength];
};

}

#endif
