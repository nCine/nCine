#include <cstring> // for memcpy()
#include "GLUniformCache.h"
#include "GLUniform.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLUniformCache::GLUniformCache()
	: uniform_(NULL), dataPointer_(NULL)
{

}

GLUniformCache::GLUniformCache(const GLUniform* uniform)
	: uniform_(uniform), dataPointer_(NULL)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLUniformCache::setFloatVector(const GLfloat *vec)
{
	checkFloat();
	memcpy(dataPointer_, vec, sizeof(GLfloat) * uniform_->numComponents());
}

void GLUniformCache::setFloatValue(GLfloat v0)
{
	checkFloat();
	checkComponents(1);

	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
}

void GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1)
{
	checkFloat();
	checkComponents(2);

	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
}

void GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2)
{
	checkFloat();
	checkComponents(3);

	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
}

void GLUniformCache::setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	checkFloat();
	checkComponents(4);

	GLfloat *data = reinterpret_cast<GLfloat *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
}

void GLUniformCache::setIntVector(const GLint *vec)
{
	checkInt();
	memcpy(dataPointer_, vec, sizeof(GLint) * uniform_->numComponents());
}

void GLUniformCache::setIntValue(GLint v0)
{
	checkInt();
	checkComponents(1);

	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
}

void GLUniformCache::setIntValue(GLint v0, GLint v1)
{
	checkInt();
	checkComponents(2);

	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
}

void GLUniformCache::setIntValue(GLint v0, GLint v1, GLint v2)
{
	checkInt();
	checkComponents(3);

	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
}

void GLUniformCache::setIntValue(GLint v0, GLint v1, GLint v2, GLint v3)
{
	checkInt();
	checkComponents(4);

	GLint *data = reinterpret_cast<GLint *>(dataPointer_);
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
}

void GLUniformCache::commitValue()
{
	GLint location = uniform_->location();
	switch(uniform_->type())
	{
		case GL_FLOAT:
			glUniform1fv(location, 1, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(location, 1, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(location, 1, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_INT:
			glUniform1iv(location, 1, reinterpret_cast<const GLint*>(dataPointer_));
			break;
		case GL_INT_VEC2:
			glUniform2iv(location, 1, reinterpret_cast<const GLint*>(dataPointer_));
			break;
		case GL_INT_VEC3:
			glUniform3iv(location, 1, reinterpret_cast<const GLint*>(dataPointer_));
			break;
		case GL_INT_VEC4:
			glUniform4iv(location, 1, reinterpret_cast<const GLint*>(dataPointer_));
			break;
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(dataPointer_));
			break;
		case GL_SAMPLER_2D:
			glUniform1iv(location, 1, reinterpret_cast<const GLint*>(dataPointer_));
			break;
		case GL_SAMPLER_CUBE:
			glUniform1iv(location, 1, reinterpret_cast<const GLint*>(dataPointer_));
			break;
		default:
			LOGW_X("No available case to handle type: %u", uniform_->type());
			break;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLUniformCache::checkFloat()
{
	if (uniform_->basicType() != GL_FLOAT)
	{
		LOGE_X("Uniform \"%s\" is not floating point", uniform_->name());
	}
}

void GLUniformCache::checkInt()
{
	if (uniform_->basicType() != GL_INT && uniform_->basicType() != GL_BOOL &&
		uniform_->basicType() != GL_SAMPLER_2D && uniform_->basicType() != GL_SAMPLER_CUBE)
	{
		LOGE_X("Uniform \"%s\" is not integer", uniform_->name());
	}
}

void GLUniformCache::checkComponents(unsigned int requiredComponents)
{
	if (uniform_->numComponents() != requiredComponents)
	{
		LOGE_X("Uniform \"%s\" has %u components, not %u", uniform_->name(), uniform_->numComponents(), requiredComponents);
	}
}

}
