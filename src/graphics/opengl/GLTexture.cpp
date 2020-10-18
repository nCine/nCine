#include "GLTexture.h"
#include "tracy_opengl.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLHashMap<GLTextureMappingFunc::Size, GLTextureMappingFunc> GLTexture::boundTextures_[MaxTextureUnits];
unsigned int GLTexture::boundUnit_ = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

GLTexture::GLTexture(GLenum target)
    : glHandle_(0), target_(target), textureUnit_(0)
{
	glGenTextures(1, &glHandle_);
}

GLTexture::~GLTexture()
{
	if (boundTextures_[boundUnit_][target_] == glHandle_)
		unbind();

	glDeleteTextures(1, &glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLTexture::bind(unsigned int textureUnit) const
{
	FATAL_ASSERT(textureUnit < MaxTextureUnits);

	if (boundUnit_ != textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		boundUnit_ = textureUnit;
		textureUnit_ = textureUnit;
	}

	if (boundTextures_[textureUnit][target_] != glHandle_)
	{
		glBindTexture(target_, glHandle_);
		boundTextures_[textureUnit][target_] = glHandle_;
	}
}

void GLTexture::unbind() const
{
	if (boundUnit_ != textureUnit_)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit_);
		boundUnit_ = textureUnit_;
	}

	if (boundTextures_[textureUnit_][target_] != 0)
	{
		glBindTexture(target_, 0);
		boundTextures_[textureUnit_][target_] = 0;
	}
}

void GLTexture::texImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data)
{
	TracyGpuZone("glTexImage2D");
	bind();
	glTexImage2D(target_, level, internalFormat, width, height, 0, format, type, data);
}

void GLTexture::texSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data)
{
	TracyGpuZone("glTexSubImage2D");
	bind();
	glTexSubImage2D(target_, level, xoffset, yoffset, width, height, format, type, data);
}

void GLTexture::compressedTexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei imageSize, const void *data)
{
	TracyGpuZone("glCompressedTexImage2D");
	bind();
	glCompressedTexImage2D(target_, level, internalFormat, width, height, 0, imageSize, data);
}

void GLTexture::compressedTexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
	TracyGpuZone("glCompressedTexSubImage2D");
	bind();
	glCompressedTexSubImage2D(target_, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void GLTexture::texStorage2D(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height)
{
	TracyGpuZone("glTexStorage2D");
	bind();
	glTexStorage2D(target_, levels, internalFormat, width, height);
}

#if !defined(__ANDROID__) && !defined(WITH_ANGLE)
void GLTexture::getTexImage(GLint level, GLenum format, GLenum type, void *pixels)
{
	TracyGpuZone("glGetTexImage");
	bind();
	glGetTexImage(target_, level, format, type, pixels);
}
#endif

void GLTexture::texParameterf(GLenum pname, GLfloat param)
{
	bind();
	glTexParameterf(target_, pname, param);
}

void GLTexture::texParameteri(GLenum pname, GLint param)
{
	bind();
	glTexParameteri(target_, pname, param);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GLTexture::bindHandle(GLenum target, GLuint glHandle, unsigned int textureUnit)
{
	FATAL_ASSERT(textureUnit < MaxTextureUnits);

	if (boundUnit_ != textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		boundUnit_ = textureUnit;
	}

	if (boundTextures_[textureUnit][target] != glHandle)
	{
		glBindTexture(target, glHandle);
		boundTextures_[textureUnit][target] = glHandle;
		return true;
	}
	return false;
}

}
