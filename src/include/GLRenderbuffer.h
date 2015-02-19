#ifndef CLASS_NCINE_GLRENDERBUFFER
#define CLASS_NCINE_GLRENDERBUFFER

#define NCINE_INCLUDE_OPENGL
#include "common.h"

namespace ncine
{

class GLRenderbuffer
{
public:
    void bind();
    void unbind();

private:
    static GLuint boundBuffer_;

    GLuint glHandle_;

	GLRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height);
	~GLRenderbuffer();

	/// Private copy constructor
	GLRenderbuffer(const GLRenderbuffer&);
	/// Private assignment operator
	GLRenderbuffer& operator=(const GLRenderbuffer&);

    void storage(GLenum internalFormat, GLsizei width, GLsizei height);

	friend class GLFramebufferObject;
};

}

#endif
