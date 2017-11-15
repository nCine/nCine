#include "GLBufferObject.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> GLBufferObject::boundBuffers_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLBufferObject::GLBufferObject(GLenum target)
	: glHandle_(0),
	  target_(target)
{
	glGenBuffers(1, &glHandle_);
}

GLBufferObject::~GLBufferObject()
{
	if (boundBuffers_[target_] == glHandle_)
	{
		unbind();
	}

	glDeleteBuffers(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLBufferObject::bind() const
{
	if (boundBuffers_[target_] != glHandle_)
	{
		glBindBuffer(target_, glHandle_);
		boundBuffers_[target_] = glHandle_;
	}
}

void GLBufferObject::unbind() const
{
	if (boundBuffers_[target_] != 0)
	{
		glBindBuffer(target_, 0);
		boundBuffers_[target_] = 0;
	}
}

void GLBufferObject::bufferData(GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	bind();
	glBufferData(target_, size, data, usage);
}

void GLBufferObject::bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
	bind();
	glBufferSubData(target_, offset, size, data);
}

#ifndef __ANDROID__
void *GLBufferObject::map(GLenum access)
{
	bind();
	return glMapBuffer(target_, access);
}

GLboolean GLBufferObject::unmap()
{
	bind();
	return glUnmapBuffer(target_);
}

#ifndef __APPLE__
void GLBufferObject::bufferStorage(GLsizeiptr size, const GLvoid *data, GLbitfield flags)
{
	bind();
	glBufferStorage(target_, size, data, flags);
}

void GLBufferObject::bindBufferBase(GLenum target, GLuint index)
{
	glBindBufferBase(target, index, glHandle_);
}

void GLBufferObject::unbindBufferBase(GLenum target, GLuint index)
{
	glBindBufferBase(target, index, 0);
}

void *GLBufferObject::mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	bind();
	return glMapBufferRange(target_, offset, length, access);
}

void GLBufferObject::bindVertexBuffer(GLuint bindingIndex, GLintptr offset, GLsizei stride)
{
	bind();
	glBindVertexBuffer(bindingIndex, glHandle_, offset, stride);
}
#endif

#endif

}
