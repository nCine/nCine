#include <cstring> // for memcpy()
#include "common_macros.h"
#include "GLUniformCache.h"
#include "GLUniform.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLUniformCache::GLUniformCache()
    : uniform_(nullptr), dataPointer_(nullptr), isDirty_(false)
{
}

GLUniformCache::GLUniformCache(const GLUniform *uniform)
    : uniform_(uniform), dataPointer_(nullptr), isDirty_(false)
{
	ASSERT(uniform);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLUniformCache::setFloatVector(const GLfloat *vec)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkFloat();

	isDirty_ = true;
	memcpy(dataPointer_, vec, sizeof(GLfloat) * uniform_->numComponents());
}

void GLUniformCache::setFloatValue(GLfloat v0)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkFloat();
	checkComponents(1);

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
}

void GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkFloat();
	checkComponents(2);

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
}

void GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkFloat();
	checkComponents(3);

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
}

void GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkFloat();
	checkComponents(4);

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
}

void GLUniformCache::setIntVector(const GLint *vec)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkInt();

	isDirty_ = true;
	memcpy(dataPointer_, vec, sizeof(GLint) * uniform_->numComponents());
}

void GLUniformCache::setIntValue(GLint v0)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkInt();
	checkComponents(1);

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
}

void GLUniformCache::setIntValue(GLint v0, GLint v1)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkInt();
	checkComponents(2);

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
}

void GLUniformCache::setIntValue(GLint v0, GLint v1, GLint v2)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkInt();
	checkComponents(3);

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
}

void GLUniformCache::setIntValue(GLint v0, GLint v1, GLint v2, GLint v3)
{
	if (uniform_ == nullptr)
		return;

	ASSERT(dataPointer_ != nullptr);
	checkInt();
	checkComponents(4);

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
}

void GLUniformCache::commitValue()
{
	if (uniform_ == nullptr || isDirty_ == false)
		return;

	ASSERT(dataPointer_ != nullptr);
	// The uniform must not belong to any uniform block
	ASSERT(uniform_->blockIndex() == -1);

	const GLint location = uniform_->location();
	switch (uniform_->type())
	{
		case GL_FLOAT:
			glUniform1fv(location, 1, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(location, 1, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(location, 1, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(location, 1, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
		case GL_INT:
			glUniform1iv(location, 1, reinterpret_cast<const GLint *>(dataPointer_));
			break;
		case GL_INT_VEC2:
			glUniform2iv(location, 1, reinterpret_cast<const GLint *>(dataPointer_));
			break;
		case GL_INT_VEC3:
			glUniform3iv(location, 1, reinterpret_cast<const GLint *>(dataPointer_));
			break;
		case GL_INT_VEC4:
			glUniform4iv(location, 1, reinterpret_cast<const GLint *>(dataPointer_));
			break;
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(dataPointer_));
			break;
#if !defined(__ANDROID__) && !defined(WITH_ANGLE) // not available in OpenGL ES
		case GL_SAMPLER_1D:
#endif
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
#if (!defined(__ANDROID__) && !defined(WITH_ANGLE)) || (defined(__ANDROID__) && GL_ES_VERSION_3_2)
		case GL_SAMPLER_BUFFER:
#endif
			glUniform1iv(location, 1, reinterpret_cast<const GLint *>(dataPointer_));
			break;
		default:
			LOGW_X("No available case to handle type: %u", uniform_->type());
			break;
	}

	isDirty_ = false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLUniformCache::checkFloat()
{
	if (uniform_->basicType() != GL_FLOAT)
		LOGE_X("Uniform \"%s\" is not floating point", uniform_->name());
}

void GLUniformCache::checkInt()
{
	if (uniform_->basicType() != GL_INT &&
	    uniform_->basicType() != GL_BOOL &&
#if !defined(__ANDROID__) && !defined(WITH_ANGLE) // not available in OpenGL ES
	    uniform_->basicType() != GL_SAMPLER_1D &&
#endif
	    uniform_->basicType() != GL_SAMPLER_2D &&
	    uniform_->basicType() != GL_SAMPLER_3D &&
	    uniform_->basicType() != GL_SAMPLER_CUBE
#if (!defined(__ANDROID__) && !defined(WITH_ANGLE)) || (defined(__ANDROID__) && GL_ES_VERSION_3_2)
	    && uniform_->basicType() != GL_SAMPLER_BUFFER
#endif
	)
	{
		LOGE_X("Uniform \"%s\" is not integer", uniform_->name());
	}
}

void GLUniformCache::checkComponents(unsigned int requiredComponents)
{
	if (uniform_->numComponents() != requiredComponents)
		LOGE_X("Uniform \"%s\" has %u components, not %u", uniform_->name(), uniform_->numComponents(), requiredComponents);
}

}
