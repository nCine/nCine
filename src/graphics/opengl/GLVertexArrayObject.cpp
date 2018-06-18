#include "GLVertexArrayObject.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

unsigned int GLVertexArrayObject::boundVAO_ = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

GLVertexArrayObject::GLVertexArrayObject()
	: glHandle_(0)
{
	glGenVertexArrays(1, &glHandle_);
}

GLVertexArrayObject::~GLVertexArrayObject()
{
	if (boundVAO_ == glHandle_)
		unbind();

	glDeleteVertexArrays(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLVertexArrayObject::bind() const
{
	if (boundVAO_ != glHandle_)
	{
		glBindVertexArray(glHandle_);
		boundVAO_ = glHandle_;
	}
}

void GLVertexArrayObject::unbind() const
{
	if (boundVAO_ != 0)
	{
		glBindVertexArray(0);
		boundVAO_ = 0;
	}
}

}
