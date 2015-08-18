#include <cstdio> // for NULL
#include "GLVertexAttribute.h"
#include "GLAttribute.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLVertexAttribute::GLVertexAttribute()
	: attribute_(NULL), vboStride_(0), vboPointer_(NULL)
{

}

GLVertexAttribute::GLVertexAttribute(const GLAttribute *attribute)
	: attribute_(attribute), vboStride_(0), vboPointer_(NULL)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLVertexAttribute::setVboParameters(GLsizei vboStride, const GLvoid *vboPointer)
{
	vboStride_ = vboStride;
	vboPointer_ = vboPointer;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLVertexAttribute::vertexAttribPointer()
{
	glVertexAttribPointer(attribute_->location(), attribute_->numComponents(), attribute_->basicType(), GL_FALSE, vboStride_, vboPointer_);
}

void GLVertexAttribute::enable()
{
	glEnableVertexAttribArray(attribute_->location());
}

void GLVertexAttribute::disable()
{
	glDisableVertexAttribArray(attribute_->location());
}

}
