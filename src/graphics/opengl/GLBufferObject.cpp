#include "GLBufferObject.h"

namespace ncine {

GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> GLBufferObject::boundBuffers_;

GLBufferObject::GLBufferObject(GLenum target)
    : glHandle_(0)
	, target_(target)
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

void GLBufferObject::bind()
{
    if (boundBuffers_[target_] != glHandle_)
	{
        glBindBuffer(target_, glHandle_);
        boundBuffers_[target_] = glHandle_;
	}
}

void GLBufferObject::unbind()
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

void *GLBufferObject::mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    bind();
    return glMapBufferRange(target_, offset, length, access);
}

 void GLBufferObject::bindVertexBuffer(GLuint bindingIndex, GLintptr offset, GLintptr stride)
 {
     bind();
     glBindVertexBuffer(bindingIndex, glHandle_, offset, stride);
 }

}
