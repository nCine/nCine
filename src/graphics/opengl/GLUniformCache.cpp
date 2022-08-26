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

const GLfloat *GLUniformCache::floatVector() const
{
	ASSERT(uniform_ == nullptr || (dataPointer_ != nullptr && checkFloat()));
	const GLfloat *vec = nullptr;

	if (dataPointer_)
		vec = reinterpret_cast<GLfloat *>(dataPointer_);
	return vec;
}

GLfloat GLUniformCache::floatValue(unsigned int index) const
{
	ASSERT(uniform_ == nullptr || (dataPointer_ != nullptr && checkFloat() && uniform_->numComponents() > index));

	GLfloat value = 0.0f;

	if (dataPointer_)
		value = static_cast<GLfloat>(*dataPointer_);
	return value;
}

const GLint *GLUniformCache::intVector() const
{
	ASSERT(uniform_ == nullptr || (dataPointer_ != nullptr && checkInt()));
	const GLint *vec = nullptr;

	if (dataPointer_)
		vec = reinterpret_cast<GLint *>(dataPointer_);
	return vec;
}

GLint GLUniformCache::intValue(unsigned int index) const
{
	ASSERT(uniform_ == nullptr || (dataPointer_ != nullptr && checkInt() && uniform_->numComponents() > index));
	GLint value = 0;

	if (dataPointer_)
		value = static_cast<GLint>(*dataPointer_);
	return value;
}

bool GLUniformCache::setFloatVector(const GLfloat *vec)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr || checkFloat() == false)
		return false;

	isDirty_ = true;
	memcpy(dataPointer_, vec, sizeof(GLfloat) * uniform_->numComponents());
	return true;
}

bool GLUniformCache::setFloatValue(GLfloat v0)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkFloat() == false || checkComponents(1) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	return true;
}

bool GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkFloat() == false || checkComponents(2) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	return true;
}

bool GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkFloat() == false || checkComponents(3) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	return true;
}

bool GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkFloat() == false || checkComponents(4) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
	return true;
}

bool GLUniformCache::setIntVector(const GLint *vec)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr || checkInt() == false)
	{
		return false;
	}

	isDirty_ = true;
	memcpy(dataPointer_, vec, sizeof(GLint) * uniform_->numComponents());
	return true;
}

bool GLUniformCache::setIntValue(GLint v0)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkInt() == false || checkComponents(1) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	return true;
}

bool GLUniformCache::setIntValue(GLint v0, GLint v1)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkInt() == false || checkComponents(2) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	return true;
}

bool GLUniformCache::setIntValue(GLint v0, GLint v1, GLint v2)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkInt() == false || checkComponents(3) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	return true;
}

bool GLUniformCache::setIntValue(GLint v0, GLint v1, GLint v2, GLint v3)
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr ||
	    checkInt() == false || checkComponents(4) == false)
	{
		return false;
	}

	isDirty_ = true;
	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
	return true;
}

bool GLUniformCache::commitValue()
{
	ASSERT(uniform_ == nullptr || dataPointer_ != nullptr);
	if (uniform_ == nullptr || dataPointer_ == nullptr || isDirty_ == false)
		return false;

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
#if !defined(WITH_OPENGLES) // not available in OpenGL ES
		case GL_SAMPLER_1D:
#endif
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
#if !defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_2)
		case GL_SAMPLER_BUFFER:
#endif
			glUniform1iv(location, 1, reinterpret_cast<const GLint *>(dataPointer_));
			break;
		default:
			LOGW_X("No available case to handle type: %u", uniform_->type());
			break;
	}

	isDirty_ = false;
	return true;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GLUniformCache::checkFloat() const
{
	if (uniform_->basicType() != GL_FLOAT)
	{
		LOGE_X("Uniform \"%s\" is not floating point", uniform_->name());
		return false;
	}
	else
		return true;
}

bool GLUniformCache::checkInt() const
{
	if (uniform_->basicType() != GL_INT &&
	    uniform_->basicType() != GL_BOOL &&
#if !defined(WITH_OPENGLES) // not available in OpenGL ES
	    uniform_->basicType() != GL_SAMPLER_1D &&
#endif
	    uniform_->basicType() != GL_SAMPLER_2D &&
	    uniform_->basicType() != GL_SAMPLER_3D &&
	    uniform_->basicType() != GL_SAMPLER_CUBE
#if !defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_2)
	    && uniform_->basicType() != GL_SAMPLER_BUFFER
#endif
	)
	{
		LOGE_X("Uniform \"%s\" is not integer", uniform_->name());
		return false;
	}
	else
		return true;
}

bool GLUniformCache::checkComponents(unsigned int requiredComponents) const
{
	if (uniform_->numComponents() != requiredComponents)
	{
		LOGE_X("Uniform \"%s\" has %u components, not %u", uniform_->name(), uniform_->numComponents(), requiredComponents);
		return false;
	}
	else
		return true;
}

}
