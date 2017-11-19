#include "common_macros.h"
#include "GLVertexAttribute.h"
#include "GLAttribute.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLVertexAttribute::GLVertexAttribute()
	: attribute_(nullptr), vboStride_(0), vboPointer_(nullptr)
{

}

GLVertexAttribute::GLVertexAttribute(const GLAttribute *attribute)
	: attribute_(attribute), vboStride_(0), vboPointer_(nullptr)
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
	if (attribute_ == nullptr)
		return;

	glVertexAttribPointer(attribute_->location(), attribute_->numComponents(), attribute_->basicType(), GL_FALSE, vboStride_, vboPointer_);
}

void GLVertexAttribute::enable()
{
	if (attribute_ == nullptr)
		return;

	glEnableVertexAttribArray(attribute_->location());
}

void GLVertexAttribute::disable()
{
	if (attribute_ == nullptr)
		return;

	glDisableVertexAttribArray(attribute_->location());
}

}
