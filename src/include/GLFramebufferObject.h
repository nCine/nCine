#ifndef CLASS_NCINE_GLFRAMEBUFFEROBJECT
#define CLASS_NCINE_GLFRAMEBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include <nctl/Array.h>

namespace ncine {

class GLRenderbuffer;
class GLTexture;

/// A class to handle OpenGL framebuffer objects
class GLFramebufferObject
{
  public:
	explicit GLFramebufferObject();
	~GLFramebufferObject();

	inline GLuint glHandle() const { return glHandle_; }

	bool bind() const;
	static bool unbind();

	bool bind(GLenum target) const;
	static bool unbind(GLenum target);

	void attachRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment);
	void attachTexture(GLTexture &texture, GLenum attachment);
	void invalidate(GLsizei numAttachments, const GLenum *attachments);

	bool isStatusComplete();

  private:
	static unsigned int readBoundBuffer_;
	static unsigned int drawBoundBuffer_;

	nctl::Array<GLRenderbuffer *> attachedRenderbuffers_;

	GLuint glHandle_;

	/// Deleted copy constructor
	GLFramebufferObject(const GLFramebufferObject &) = delete;
	/// Deleted assignment operator
	GLFramebufferObject &operator=(const GLFramebufferObject &) = delete;

	static bool bindHandle(GLenum target, GLuint glHandle);

	friend class Qt5GfxDevice;
};

}

#endif
