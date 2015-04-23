#include <cstring> // for memcpy()
#include "GLUniform.h"
#include "ServiceLocator.h"

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
	switch(type_)
	{
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
			return GL_FLOAT;
			break;
		case GL_INT:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
			return GL_INT;
			break;
		case GL_BOOL:
		case GL_BOOL_VEC2:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
			return GL_BOOL;
			break;
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT4:
			return GL_FLOAT;
			break;
		case GL_SAMPLER_2D:
			return GL_INT;
			break;
		case GL_SAMPLER_CUBE:
			return GL_INT;
			break;
		default:
			LOGW_X("No available case to handle type: %u", type_);
			return type_;
			break;
	}
}

unsigned int GLUniform::numComponents() const
{
	switch(type_)
	{
		case GL_FLOAT:
		case GL_INT:
		case GL_BOOL:
			return 1;
			break;
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
			return 2;
			break;
		case GL_FLOAT_VEC3:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
			return 3;
			break;
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
			return 4;
			break;
		case GL_FLOAT_MAT2:
			return 4;
			break;
		case GL_FLOAT_MAT3:
			return 9;
			break;
		case GL_FLOAT_MAT4:
			return 16;
			break;
		case GL_SAMPLER_2D:
			return 1;
			break;
		case GL_SAMPLER_CUBE:
			return 1;
			break;
		default:
			LOGW_X("No available case to handle type: %u", type_);
			return 0;
			break;
	}
}

}
