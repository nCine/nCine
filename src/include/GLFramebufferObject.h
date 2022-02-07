#ifndef CLASS_NCINE_GLFRAMEBUFFEROBJECT
#define CLASS_NCINE_GLFRAMEBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include <nctl/StaticArray.h>
#include <nctl/UniquePtr.h>

namespace ncine {

class GLRenderbuffer;
class GLTexture;

/// A class to handle OpenGL framebuffer objects
class GLFramebufferObject
{
  public:
	static const unsigned int MaxRenderbuffers = 4;

	explicit GLFramebufferObject();
	~GLFramebufferObject();

	inline GLuint glHandle() const { return glHandle_; }

	bool bind() const;
	static bool unbind();

	bool bind(GLenum target) const;
	static bool unbind(GLenum target);

	inline unsigned int numRenderbuffers() const { return attachedRenderbuffers_.size(); }
	bool attachRenderbuffer(const char *label, GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment);
	bool attachRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment);

	void attachTexture(GLTexture &texture, GLenum attachment);
	void invalidate(GLsizei numAttachments, const GLenum *attachments);

	bool isStatusComplete();

	void setObjectLabel(const char *label);

  private:
	static unsigned int readBoundBuffer_;
	static unsigned int drawBoundBuffer_;

	nctl::StaticArray<nctl::UniquePtr<GLRenderbuffer>, MaxRenderbuffers> attachedRenderbuffers_;

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
