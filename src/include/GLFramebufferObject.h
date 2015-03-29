#ifndef CLASS_NCINE_GLFRAMEBUFFEROBJECT
#define CLASS_NCINE_GLFRAMEBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"
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
	static class GLHashMap<GLFramebufferMappingFunc::Size, GLFramebufferMappingFunc> boundBuffers_;

	Array<GLRenderbuffer *> attachedRenderbuffers_;

    GLuint glHandle_;
    GLenum target_;

	/// Private copy constructor
	GLFramebufferObject(const GLFramebufferObject&);
	/// Private assignment operator
	GLFramebufferObject& operator=(const GLFramebufferObject&);
};

inline GLenum GLFramebufferObject::target() const
{
    return target_;
}

}

#endif
