#ifndef CLASS_NCINE_GLTEXTURE
#define CLASS_NCINE_GLTEXTURE

#include "GLHashMap.h"

namespace ncine {

/// A class to handle OpenGL 2D textures
class GLTexture
{
  public:
	explicit GLTexture(GLenum target_);
	~GLTexture();

	inline GLuint glHandle() const { return glHandle_; }
	inline GLenum target() const { return target_; }

	bool bind(unsigned int textureUnit) const;
	inline bool bind() const { return bind(0); }
	bool unbind() const;

	void texImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
	void texSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
	void compressedTexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei imageSize, const void *data);
	void compressedTexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
	void texStorage2D(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height);

	void getTexImage(GLint level, GLenum format, GLenum type, void *pixels);

	void texParameterf(GLenum pname, GLfloat param);
	void texParameteri(GLenum pname, GLint param);

	void setObjectLabel(const char *label);

  private:
	static const unsigned int MaxTextureUnits = 4;
	static class GLHashMap<GLTextureMappingFunc::Size, GLTextureMappingFunc> boundTextures_[MaxTextureUnits];
	static unsigned int boundUnit_;

	GLuint glHandle_;
	GLenum target_;
	/// The texture unit is mutable in order for constant texture objects to be bound
	/*! A texture can be bound to a specific texture unit. */
	mutable unsigned int textureUnit_;

	/// Deleted copy constructor
	GLTexture(const GLTexture &) = delete;
	/// Deleted assignment operator
	GLTexture &operator=(const GLTexture &) = delete;

	static bool bindHandle(GLenum target, GLuint glHandle, unsigned int textureUnit);
	static bool bindHandle(GLenum target, GLuint glHandle) { return bindHandle(target, glHandle, 0); }

	friend class GLFramebufferObject;
	friend class Qt5GfxDevice;
	friend class ImGuiDrawing;
	friend class NuklearDrawing;
};

}

#endif
