#ifndef CLASS_NCINE_GLBUFFEROBJECT
#define CLASS_NCINE_GLBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"

namespace ncine
{

class GLBufferObject
{
public:
	explicit GLBufferObject(GLenum type);
	~GLBufferObject();

	void bind();
	void unbind();

	void bufferData(GLsizeiptr size, const GLvoid *data, GLenum usage);
	void bufferStorage(GLsizeiptr size, const GLvoid *data, GLbitfield flags);

    void bindBufferBase(GLenum target, GLuint index);
    void unbindBufferBase(GLenum target, GLuint index);

    void *map(GLenum access);
    GLboolean unmap();
    void *mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access);

    void bindVertexBuffer(GLuint bindingIndex, GLintptr offset, GLintptr stride);

	GLenum target() const;

private:
	static class GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> boundBuffers_;

    GLuint glHandle_;
	GLenum target_;

	/// Private copy constructor
	GLBufferObject(const GLBufferObject&);
	/// Private assignment operator
	GLBufferObject& operator=(const GLBufferObject&);
};

inline GLenum GLBufferObject::target() const
{
	return target_;
}

}

#endif
