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
	if (attribute_)
	{
		const GLuint index = static_cast<unsigned int>(attribute_->location());
		const GLint size = attribute_->numComponents();
		const GLenum type = attribute_->basicType();
		glVertexAttribPointer(index, size, type, GL_FALSE, vboStride_, vboPointer_);
	}
}

void GLVertexAttribute::enable()
{
	if (attribute_)
	{
		const GLuint index = static_cast<unsigned int>(attribute_->location());
		glEnableVertexAttribArray(index);
	}
}

void GLVertexAttribute::disable()
{
	if (attribute_)
	{
		const GLuint index = static_cast<unsigned int>(attribute_->location());
		glDisableVertexAttribArray(index);
	}
}

}
