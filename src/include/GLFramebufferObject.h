#ifndef CLASS_NCINE_GLFRAMEBUFFEROBJECT
#define CLASS_NCINE_GLFRAMEBUFFEROBJECT

#include "GLHashMap.h"
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
	bool unbind() const;

	bool bind(GLenum target) const;
	bool unbind(GLenum target) const;

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
};

}

#endif
