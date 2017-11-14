#include <cstring> // for memcpy()
#include "common_macros.h"
#include "GLUniform.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLUniform::GLUniform()
	: location_(-1), size_(0), type_(GL_FLOAT)
{
	name_[0] = '\0';
}

GLUniform::GLUniform(GLuint program, GLuint index)
	: location_(-1), size_(0), type_(GL_FLOAT)
{
	GLsizei length;
	glGetActiveUniform(program, index, MaxNameLength, &length, &size_, &type_, name_);
	location_ = glGetUniformLocation(program, name_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

GLenum GLUniform::basicType() const
{
	switch (type_)
	{
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
			return GL_FLOAT;
		case GL_INT:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
			return GL_INT;
		case GL_BOOL:
		case GL_BOOL_VEC2:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
			return GL_BOOL;
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT4:
			return GL_FLOAT;
		case GL_SAMPLER_2D:
			return GL_INT;
		case GL_SAMPLER_CUBE:
			return GL_INT;
		default:
			LOGW_X("No available case to handle type: %u", type_);
			return type_;
	}
}

unsigned int GLUniform::numComponents() const
{
	switch (type_)
	{
		case GL_FLOAT:
		case GL_INT:
		case GL_BOOL:
			return 1;
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
			return 2;
		case GL_FLOAT_VEC3:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
			return 3;
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
			return 4;
		case GL_FLOAT_MAT2:
			return 4;
		case GL_FLOAT_MAT3:
			return 9;
		case GL_FLOAT_MAT4:
			return 16;
		case GL_SAMPLER_2D:
			return 1;
		case GL_SAMPLER_CUBE:
			return 1;
		default:
			LOGW_X("No available case to handle type: %u", type_);
			return 0;
	}
}

}
