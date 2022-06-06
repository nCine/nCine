#ifndef CLASS_NCINE_GLRENDERBUFFER
#define CLASS_NCINE_GLRENDERBUFFER

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL renderbuffer objects
class GLRenderbuffer
{
  public:
	GLRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height);
	~GLRenderbuffer();

	inline GLuint glHandle() const { return glHandle_; }

	inline void setAttachment(GLenum attachment) { attachment_ = attachment; }
	inline GLenum attachment() const { return attachment_; }

	bool bind() const;
	static bool unbind();

	void setObjectLabel(const char *label);

  private:
	static GLuint boundBuffer_;

	GLuint glHandle_;
	GLenum attachment_;

	/// Deleted copy constructor
	GLRenderbuffer(const GLRenderbuffer &) = delete;
	/// Deleted assignment operator
	GLRenderbuffer &operator=(const GLRenderbuffer &) = delete;

	void storage(GLenum internalFormat, GLsizei width, GLsizei height);

	friend class GLFramebufferObject;
};

}

#endif
