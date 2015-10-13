#ifndef CLASS_NCINE_GLBUFFEROBJECT
#define CLASS_NCINE_GLBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"

namespace ncine {

class GLBufferObject
{
  public:
	explicit GLBufferObject(GLenum type);
	~GLBufferObject();

	inline GLuint glHandle() const { return glHandle_; }
	inline GLenum target() const { return target_; }

	void bind() const;
	void unbind() const;

	void bufferData(GLsizeiptr size, const GLvoid *data, GLenum usage);
	void bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data);

#ifndef __ANDROID__
	void *map(GLenum access);
	GLboolean unmap();

#ifndef __APPLE__
	void bufferStorage(GLsizeiptr size, const GLvoid *data, GLbitfield flags);

	void bindBufferBase(GLenum target, GLuint index);
	void unbindBufferBase(GLenum target, GLuint index);

	void *mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access);

	void bindVertexBuffer(GLuint bindingIndex, GLintptr offset, GLsizei stride);
#endif

#endif
  private:
	static class GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> boundBuffers_;

	GLuint glHandle_;
	GLenum target_;

	/// Private copy constructor
	GLBufferObject(const GLBufferObject&);
	/// Private assignment operator
	GLBufferObject& operator=(const GLBufferObject&);
};

}

#endif
