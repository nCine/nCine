#include "GLFramebufferObject.h"

#include "GLRenderbuffer.h"
#include "GLTexture.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLHashMap<GLFramebufferMappingFunc::Size, GLFramebufferMappingFunc> GLFramebufferObject::boundBuffers_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLFramebufferObject::GLFramebufferObject()
	: attachedRenderbuffers_(4), glHandle_(0), target_(GL_FRAMEBUFFER)
{
	glGenFramebuffers(1, &glHandle_);
}

GLFramebufferObject::~GLFramebufferObject()
{
	if (boundBuffers_[target_] == glHandle_)
		unbind();

	for (GLRenderbuffer *attachedRenderbuffer : attachedRenderbuffers_)
		delete attachedRenderbuffer;

	glDeleteFramebuffers(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLFramebufferObject::bind(GLenum target) const
{
	if (boundBuffers_[target] != glHandle_)
	{
		target_ = target;
		glBindFramebuffer(target_, glHandle_);
		boundBuffers_[target_] = glHandle_;
		return true;
	}
	return false;
}

bool GLFramebufferObject::unbind() const
{
	if (boundBuffers_[target_] != 0)
	{
		glBindFramebuffer(target_, 0);
		boundBuffers_[target_] = 0;
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
