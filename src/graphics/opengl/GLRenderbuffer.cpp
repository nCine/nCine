#include "GLRenderbuffer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLuint GLRenderbuffer::boundBuffer_ = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLRenderbuffer::GLRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height)
	: glHandle_(0)
{
	glGenRenderbuffers(1, &glHandle_);
	storage(internalFormat, width, height);
}

GLRenderbuffer::~GLRenderbuffer()
{
	if (boundBuffer_ == glHandle_)
		unbind();

	glDeleteRenderbuffers(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLRenderbuffer::bind() const
{
	if (boundBuffer_ != glHandle_)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, glHandle_);
		boundBuffer_ = glHandle_;
		return true;
	}
	return false;
}

bool GLRenderbuffer::unbind() const
{
	if (boundBuffer_ != 0)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		boundBuffer_ = 0;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLRenderbuffer::storage(GLenum internalFormat, GLsizei width, GLsizei height)
{
	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
}

}
