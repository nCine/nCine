#include "GLFramebufferObject.h"
#include "GLRenderbuffer.h"
#include "GLTexture.h"
#include "GLDebug.h"

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
    : glHandle_(0)
{
	glGenFramebuffers(1, &glHandle_);
}

GLFramebufferObject::~GLFramebufferObject()
{
	if (readBoundBuffer_ == glHandle_)
		unbind(GL_READ_FRAMEBUFFER);
	if (drawBoundBuffer_ == glHandle_)
		unbind(GL_DRAW_FRAMEBUFFER);

	glDeleteFramebuffers(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLFramebufferObject::bind() const
{
	return bind(GL_FRAMEBUFFER);
}

bool GLFramebufferObject::unbind()
{
	return unbind(GL_FRAMEBUFFER);
}

bool GLFramebufferObject::bind(GLenum target) const
{
	return bindHandle(target, glHandle_);
}

bool GLFramebufferObject::unbind(GLenum target)
{
	return bindHandle(target, 0);
}

bool GLFramebufferObject::drawBuffers(unsigned int numDrawBuffers)
{
	static const GLenum drawBuffers[MaxDrawbuffers] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
		                                                GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };

	if (numDrawBuffers < MaxDrawbuffers && numDrawBuffers_ != numDrawBuffers)
	{
		glDrawBuffers(numDrawBuffers, drawBuffers);
		numDrawBuffers_ = numDrawBuffers;
		return true;
	}
	return false;
}

bool GLFramebufferObject::attachRenderbuffer(const char *label, GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment)
{
	if (attachedRenderbuffers_.size() >= MaxRenderbuffers - 1)
		return false;

	for (unsigned int i = 0; i < attachedRenderbuffers_.size(); i++)
	{
		if (attachedRenderbuffers_[i]->attachment() == attachment)
			return false;
	}

	attachedRenderbuffers_.pushBack(nctl::makeUnique<GLRenderbuffer>(internalFormat, width, height));
	attachedRenderbuffers_.back()->setObjectLabel(label);
	attachedRenderbuffers_.back()->setAttachment(attachment);

	bind(GL_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, attachedRenderbuffers_.back()->glHandle_);
	return true;
}

bool GLFramebufferObject::attachRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment)
{
	return attachRenderbuffer(nullptr, internalFormat, width, height, attachment);
}

bool GLFramebufferObject::detachRenderbuffer(GLenum attachment)
{
	for (unsigned int i = 0; i < attachedRenderbuffers_.size(); i++)
	{
		if (attachedRenderbuffers_[i]->attachment() == attachment)
		{
			bind(GL_FRAMEBUFFER);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, 0);
			attachedRenderbuffers_.removeAt(i);
			return true;
		}
	}
	return false;
}

void GLFramebufferObject::attachTexture(GLTexture &texture, GLenum attachment)
{
	bind(GL_FRAMEBUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.target_, texture.glHandle_, 0);
}

void GLFramebufferObject::detachTexture(GLenum attachment)
{
	bind(GL_FRAMEBUFFER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
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
	unbind(GL_FRAMEBUFFER);

	return (status == GL_FRAMEBUFFER_COMPLETE);
}

void GLFramebufferObject::setObjectLabel(const char *label)
{
	GLDebug::objectLabel(GLDebug::LabelTypes::FRAMEBUFFER, glHandle_, label);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GLFramebufferObject::bindHandle(GLenum target, GLuint glHandle)
{
	FATAL_ASSERT(target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER);

	if (target == GL_FRAMEBUFFER &&
	    (readBoundBuffer_ != glHandle || drawBoundBuffer_ != glHandle))
	{
		glBindFramebuffer(target, glHandle);
		readBoundBuffer_ = glHandle;
		drawBoundBuffer_ = glHandle;
		return true;
	}
	else if (target == GL_READ_FRAMEBUFFER && readBoundBuffer_ != glHandle)
	{
		glBindFramebuffer(target, glHandle);
		readBoundBuffer_ = glHandle;
		return true;
	}
	else if (target == GL_DRAW_FRAMEBUFFER && drawBoundBuffer_ != glHandle)
	{
		glBindFramebuffer(target, glHandle);
		drawBoundBuffer_ = glHandle;
		return true;
	}
	return false;
}

}
