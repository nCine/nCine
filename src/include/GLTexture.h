#ifndef CLASS_NCINE_GLTEXTURE
#define CLASS_NCINE_GLTEXTURE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"
#include "GLDebug.h"

namespace ncine {

/// A class to handle OpenGL 2D textures
class GLTexture
{
  public:
	explicit GLTexture(GLenum target_);
	~GLTexture();

	inline GLuint glHandle() const { return glHandle_; }
	inline GLenum target() const { return target_; }

	inline void getObjectLabel(int bufSize, int *length, char *label) const { GLDebug::getObjectLabel(GL_TEXTURE, glHandle_, bufSize, length, label); }
	inline void setObjectLabel(const char *label) { GLDebug::objectLabel(GL_TEXTURE, glHandle_, label);}

	void bind(unsigned int textureUnit) const;
	void bind() const;
	void unbind() const;

	void texImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
	void texSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
	void compressedTexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei imageSize, const void *data);

#if !defined(__ANDROID__) && !defined(__APPLE__)
	void texStorage2D(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height);
	void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
	void unbindImageTexture(GLuint unit);
#endif

	void texParameterf(GLenum pname, GLfloat param);
	void texParameteri(GLenum pname, GLint param);

  private:
	static const unsigned int MaxTextureUnits = 4;
	static class GLHashMap<GLTextureMappingFunc::Size, GLTextureMappingFunc> boundTextures_[MaxTextureUnits];
	static unsigned int boundUnit_;

	GLuint glHandle_;
	GLenum target_;
	/// The texture unit is mutable in order for constant texture objects to be bound
	/*! A texture can be bound to a specific texture unit. */
	mutable unsigned int textureUnit_;
	GLenum imageFormat_;

	/// Deleted copy constructor
	GLTexture(const GLTexture &) = delete;
	/// Deleted assignment operator
	GLTexture &operator=(const GLTexture &) = delete;

	friend class GLFramebufferObject;
};

}

#endif
