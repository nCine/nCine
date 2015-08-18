#ifndef CLASS_NCINE_GLFRAMEBUFFEROBJECT
#define CLASS_NCINE_GLFRAMEBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"
#include "Array.h"

namespace ncine {

class GLRenderbuffer;
class GLTexture;

class GLFramebufferObject
{
  public:
	explicit GLFramebufferObject();
	~GLFramebufferObject();

	inline GLuint glHandle() const { return glHandle_; }
	inline GLenum target() const { return target_; }

	void bind(GLenum target) const;
	void unbind() const;

	void attachRenderbuffer(GLenum internalFormat, GLsizei width, GLsizei height, GLenum attachment);
	void attachTexture(GLTexture &texture, GLenum attachment);

	bool isStatusComplete();

  private:
	static class GLHashMap<GLFramebufferMappingFunc::Size, GLFramebufferMappingFunc> boundBuffers_;

	Array<GLRenderbuffer *> attachedRenderbuffers_;

	GLuint glHandle_;
	/// The target is mutable in order for constant FBO objects to be bound
	/*! The FBO binding can change to become the target for read, write or both operations */
	mutable GLenum target_;

	/// Private copy constructor
	GLFramebufferObject(const GLFramebufferObject&);
	/// Private assignment operator
	GLFramebufferObject& operator=(const GLFramebufferObject&);
};

}

#endif
