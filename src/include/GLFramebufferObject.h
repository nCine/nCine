#ifndef CLASS_NCINE_GLFRAMEBUFFEROBJECT
#define CLASS_NCINE_GLFRAMEBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "Array.h"

namespace ncine
{

class GLRenderbuffer;
class GLTexture;

class GLFramebufferObject
{
public:
	explicit GLFramebufferObject();
	~GLFramebufferObject();

    void bind(GLenum target);
	void unbind();

    void attachRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment);
	void attachTexture(GLTexture &texture, GLenum attachment);

    bool isStatusComplete();

    GLenum target() const;

private:
	static class BoundBuffers
	{
	  public:
		static const unsigned int Size = 1;
		BoundBuffers() { for (unsigned int i = 0; i < Size; i++) map[i] = 0; }
		GLuint& operator[](GLenum target);

	  private:
		GLuint map[Size];
	} boundBuffers_;

	Array<GLRenderbuffer *> attachedRenderbuffers_;

    GLuint glHandle_;
    GLenum target_;

	/// Private copy constructor
	GLFramebufferObject(const GLFramebufferObject&);
	/// Private assignment operator
	GLFramebufferObject& operator=(const GLFramebufferObject&);
};

inline GLuint& GLFramebufferObject::BoundBuffers::operator[](GLenum target)
{
	switch(target)
	{
		default:
		case GL_FRAMEBUFFER:
			return map[0];
	}
}

inline GLenum GLFramebufferObject::target() const
{
    return target_;
}

}

#endif
