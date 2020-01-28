#include "GLFramebufferObject.h"

#include "GLRenderbuffer.h"
#include "GLTexture.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

unsigned int GLFramebufferObject::readBoundBuffer_ = 0;
unsigned int GLFramebufferObject::drawBoundBuffer_ = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLFramebufferObject::GLFramebufferObject()
    : attachedRenderbuffers_(4), glHandle_(0)
{
	glGenFramebuffers(1, &glHandle_);
}

GLFramebufferObject::~GLFramebufferObject()
{
	if (readBoundBuffer_ == glHandle_)
		unbind(GL_READ_FRAMEBUFFER);
	if (drawBoundBuffer_ == glHandle_)
		unbind(GL_DRAW_FRAMEBUFFER);

	for (GLRenderbuffer *attachedRenderbuffer : attachedRenderbuffers_)
		delete attachedRenderbuffer;

	glDeleteFramebuffers(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLFramebufferObject::bind() const
{
	return bind(GL_FRAMEBUFFER);
}

bool GLFramebufferObject::unbind() const
{
	return unbind(GL_FRAMEBUFFER);
}

bool GLFramebufferObject::bind(GLenum target) const
{
	FATAL_ASSERT(target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);

	if (target == GL_FRAMEBUFFER &&
	    (readBoundBuffer_ != glHandle_ || drawBoundBuffer_ != glHandle_))
	{
		glBindFramebuffer(target, glHandle_);
		readBoundBuffer_ = glHandle_;
		drawBoundBuffer_ = glHandle_;
		return true;
	}
	else if (target == GL_READ_FRAMEBUFFER && readBoundBuffer_ != glHandle_)
	{
		glBindFramebuffer(target, glHandle_);
		readBoundBuffer_ = glHandle_;
		return true;
	}
	else if (target == GL_DRAW_FRAMEBUFFER && drawBoundBuffer_ != glHandle_)
	{
		glBindFramebuffer(target, glHandle_);
		drawBoundBuffer_ = glHandle_;
		return true;
	}
	return false;
}

bool GLFramebufferObject::unbind(GLenum target) const
{
	FATAL_ASSERT(target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);

	if (target == GL_FRAMEBUFFER &&
	    (readBoundBuffer_ != 0 || drawBoundBuffer_ != 0))
	{
		glBindFramebuffer(target, 0);
		readBoundBuffer_ = 0;
		drawBoundBuffer_ = 0;
		return true;
	}
	else if (target == GL_READ_FRAMEBUFFER && readBoundBuffer_ != 0)
	{
		glBindFramebuffer(target, 0);
		readBoundBuffer_ = 0;
		return true;
	}
	else if (target == GL_DRAW_FRAMEBUFFER && drawBoundBuffer_ != 0)
	{
		glBindFramebuffer(target, 0);
		drawBoundBuffer_ = 0;
		return true;
	}
	return false;
}

void GLFramebufferObject::attachRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment)
{
	GLRenderbuffer *renderBuffer = new GLRenderbuffer(internalFormat, width, height);
	attachedRenderbuffers_.pushBack(renderBuffer);
	renderBuffer->bind();

	bind(GL_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderBuffer->glHandle_);
}

void GLFramebufferObject::attachTexture(GLTexture &texture, GLenum attachment)
{
	bind(GL_FRAMEBUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.target_, texture.glHandle_, 0);
}

void GLFramebufferObject::invalidate(GLsizei numAttachments, const GLenum *attachments)
{
	bind(GL_FRAMEBUFFER);
	glInvalidateFramebuffer(GL_FRAMEBUFFER, numAttachments, attachments);
}

bool GLFramebufferObject::isStatusComplete()
{
	bind(GL_FRAMEBUFFER);
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	return (status == GL_FRAMEBUFFER_COMPLETE);
}

}
