#ifndef CLASS_NCINE_GLTEXTURE
#define CLASS_NCINE_GLTEXTURE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"

namespace ncine
{

class GLTexture
{
public:
	GLTexture(GLenum target_);
	~GLTexture();

    void bind(unsigned int textureUnit);
    void bind();
    void unbind();

    void texImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, void* data);
    void texStorage2D(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height);
    void texSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, void* data);

    void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
    void unbindImageTexture(GLuint unit);

	void texParameterf(GLenum pname, GLfloat param);
	void texParameteri(GLenum pname, GLint param);

    GLenum target() const;

private:
    static const int MaxTextureUnits = 4;
	static class GLHashMap<GLTextureMappingFunc::Size, GLTextureMappingFunc> boundTextures_[MaxTextureUnits];
    static unsigned int boundUnit_;

    GLuint glHandle_;
    GLenum target_;
    unsigned int textureUnit_;
    GLenum imageFormat_;

	/// Private copy constructor
	GLTexture(const GLTexture&);
	/// Private assignment operator
	GLTexture& operator=(const GLTexture&);

	friend class GLFramebufferObject;
};

inline GLenum GLTexture::target() const
{
    return target_;
}

}

#endif
