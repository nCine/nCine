#include <cstdio> // for NULL
#include "common_macros.h"
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
	ASSERT(attribute);
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
	if (attribute_ == NULL)
		return;

	glVertexAttribPointer(attribute_->location(), attribute_->numComponents(), attribute_->basicType(), GL_FALSE, vboStride_, vboPointer_);
}

void GLVertexAttribute::enable()
{
	if (attribute_ == NULL)
		return;

	glEnableVertexAttribArray(attribute_->location());
}

void GLVertexAttribute::disable()
{
	if (attribute_ == NULL)
		return;

	glDisableVertexAttribArray(attribute_->location());
}

}
