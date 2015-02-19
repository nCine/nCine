#ifndef CLASS_NCINE_GLTEXTURE
#define CLASS_NCINE_GLTEXTURE

#define NCINE_INCLUDE_OPENGL
#include "common.h"

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
	static class BoundTextures
	{
	  public:
		static const unsigned int Size = 3;
		BoundTextures() {for (unsigned int i = 0; i < Size; i++) map[i] = 0; }
		GLuint& operator[](GLenum target);

	  private:
		GLuint map[Size];
	} boundTextures_[MaxTextureUnits];
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

inline GLuint& GLTexture::BoundTextures::operator[](GLenum target)
{
	switch(target)
	{
		default:
		case GL_TEXTURE_1D:
			return map[0];
		case GL_TEXTURE_2D:
			return map[1];
		case GL_TEXTURE_3D:
			return map[2];
	}
}

inline GLenum GLTexture::target() const
{
    return target_;
}

}

#endif
