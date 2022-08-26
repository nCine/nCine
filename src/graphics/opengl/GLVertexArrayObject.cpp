#include "GLVertexArrayObject.h"
#include "GLDebug.h"

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

bool GLVertexArrayObject::bind() const
{
	if (boundVAO_ != glHandle_)
	{
		glBindVertexArray(glHandle_);
		boundVAO_ = glHandle_;
		return true;
	}
	return false;
}

bool GLVertexArrayObject::unbind()
{
	if (boundVAO_ != 0)
	{
		glBindVertexArray(0);
		boundVAO_ = 0;
		return true;
	}
	return false;
}

void GLVertexArrayObject::setObjectLabel(const char *label)
{
	GLDebug::objectLabel(GLDebug::LabelTypes::VERTEX_ARRAY, glHandle_, label);
}

}
