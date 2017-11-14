#include "common_macros.h"
#include "GLAttribute.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLAttribute::GLAttribute()
	: location_(-1), size_(0), type_(GL_FLOAT)
{
	name_[0] = '\0';
}

GLAttribute::GLAttribute(GLuint program, GLuint index)
	: location_(-1), size_(0), type_(GL_FLOAT)
{
	GLsizei length;
	glGetActiveAttrib(program, index, MaxNameLength, &length, &size_, &type_, name_);
	location_ = glGetAttribLocation(program, name_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

GLenum GLAttribute::basicType() const
{
	switch (type_)
	{
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
			return GL_FLOAT;
			break;
		default:
			LOGW_X("No available case to handle type: %u", type_);
			return type_;
			break;
	}
}

int GLAttribute::numComponents() const
{
	switch (type_)
	{
		case GL_FLOAT:
			return 1;
			break;
		case GL_FLOAT_VEC2:
			return 2;
			break;
		case GL_FLOAT_VEC3:
			return 3;
			break;
		case GL_FLOAT_VEC4:
			return 4;
			break;
		default:
			LOGW_X("No available case to handle type: %u", type_);
			return 0;
			break;
	}
}

}
