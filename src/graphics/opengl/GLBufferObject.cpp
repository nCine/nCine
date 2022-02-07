#include "GLBufferObject.h"
#include "GLDebug.h"
#include "tracy_opengl.h"

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
    : glHandle_(0), target_(target), size_(0), mapped_(false)
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

bool GLBufferObject::bind() const
{
	if (boundBuffers_[target_] != glHandle_)
	{
		glBindBuffer(target_, glHandle_);
		boundBuffers_[target_] = glHandle_;
		return true;
	}
	return false;
}

bool GLBufferObject::unbind() const
{
	if (boundBuffers_[target_] != 0)
	{
		glBindBuffer(target_, 0);
		boundBuffers_[target_] = 0;
		return true;
	}
	return false;
}

void GLBufferObject::bufferData(GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	TracyGpuZone("glBufferData");
	bind();
	glBufferData(target_, size, data, usage);
	size_ = size;
}

void GLBufferObject::bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
	TracyGpuZone("glBufferSubData");
	bind();
	glBufferSubData(target_, offset, size, data);
}

#if !defined(WITH_OPENGLES)
void GLBufferObject::bufferStorage(GLsizeiptr size, const GLvoid *data, GLbitfield flags)
{
	TracyGpuZone("glBufferStorage");
	bind();
	glBufferStorage(target_, size, data, flags);
	size_ = size;
}
#endif

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
	FATAL_ASSERT(mapped_ == false);
	mapped_ = true;
	bind();
	return glMapBufferRange(target_, offset, length, access);
}

void GLBufferObject::flushMappedBufferRange(GLintptr offset, GLsizeiptr length)
{
	FATAL_ASSERT(mapped_ == true);
	bind();
	glFlushMappedBufferRange(target_, offset, length);
}

GLboolean GLBufferObject::unmap()
{
	FATAL_ASSERT(mapped_ == true);
	mapped_ = false;
	bind();
	return glUnmapBuffer(target_);
}

#if !defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_2)
void GLBufferObject::texBuffer(GLenum internalformat)
{
	FATAL_ASSERT(target_ == GL_TEXTURE_BUFFER);
	glTexBuffer(GL_TEXTURE_BUFFER, internalformat, glHandle_);
}
#endif

void GLBufferObject::setObjectLabel(const char *label)
{
	GLDebug::objectLabel(GLDebug::LabelTypes::BUFFER, glHandle_, label);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GLBufferObject::bindHandle(GLenum target, GLuint glHandle)
{
	if (boundBuffers_[target] != glHandle)
	{
		glBindBuffer(target, glHandle);
		boundBuffers_[target] = glHandle;
		return true;
	}
	return false;
}

}
