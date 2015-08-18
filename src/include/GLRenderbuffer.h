#ifndef CLASS_NCINE_GLRENDERBUFFER
#define CLASS_NCINE_GLRENDERBUFFER

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

class GLRenderbuffer
{
  public:
	inline GLuint glHandle() const { return glHandle_; }

	void bind() const;
	void unbind() const;

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
