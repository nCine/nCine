#include "GLBufferObject.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> GLBufferObject::boundBuffers_;
GLuint GLBufferObject::boundIndexBase_[MaxIndexBufferRange];
GLBufferObject::BufferRange GLBufferObject::boundBufferRange_[MaxIndexBufferRange];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLBufferObject::GLBufferObject(GLenum target)
	: glHandle_(0), target_(target), size_(0)
{
	for (unsigned int i = 0; i < MaxIndexBufferRange; i++)
		boundIndexBase_[i] = 0;

	glGenBuffers(1, &glHandle_);
}

GLBufferObject::~GLBufferObject()
{
	if (boundBuffers_[target_] == glHandle_)
		unbind();

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
	size_ = size;
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
	size_ = size;
}

void GLBufferObject::bindBufferBase(GLuint index)
{
	ASSERT(target_ == GL_UNIFORM_BUFFER);
	ASSERT(index < MaxIndexBufferRange);

	if (index >= MaxIndexBufferRange)
		glBindBufferBase(target_, index, glHandle_);
	else if (boundIndexBase_[index] != glHandle_)
	{
		boundBufferRange_[index].glHandle = -1;
		boundBufferRange_[index].offset = 0;
		boundBufferRange_[index].ptrsize = 0;
		boundIndexBase_[index] = glHandle_;
		glBindBufferBase(target_, index, glHandle_);
	}
}

void GLBufferObject::bindBufferRange(GLuint index, GLintptr offset, GLsizei ptrsize)
{
	ASSERT(target_ == GL_UNIFORM_BUFFER);
	ASSERT(index < MaxIndexBufferRange);

	if (index >= MaxIndexBufferRange)
		glBindBufferRange(target_, index, glHandle_, offset, ptrsize);
	else if (boundBufferRange_[index].glHandle != glHandle_ ||
	         boundBufferRange_[index].offset != offset ||
	         boundBufferRange_[index].ptrsize != ptrsize)
	{
		boundIndexBase_[index] = -1;
		boundBufferRange_[index].glHandle = glHandle_;
		boundBufferRange_[index].offset = offset;
		boundBufferRange_[index].ptrsize = ptrsize;
		glBindBufferRange(target_, index, glHandle_, offset, ptrsize);
	}
}

void *GLBufferObject::mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	bind();
	return glMapBufferRange(target_, offset, length, access);
}

void GLBufferObject::flushMappedBufferRange(GLintptr offset, GLsizeiptr length)
{
	bind();
	glFlushMappedBufferRange(target_, offset, length);
}

void GLBufferObject::bindVertexBuffer(GLuint bindingIndex, GLintptr offset, GLsizei stride)
{
	bind();
	glBindVertexBuffer(bindingIndex, glHandle_, offset, stride);
}
#endif

#endif

}
