#ifndef CLASS_NCINE_GLBUFFEROBJECT
#define CLASS_NCINE_GLBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common.h"

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
	static class BoundBuffers
	{
	  public:
		static const unsigned int Size = 1;
		BoundBuffers() { for (unsigned int i = 0; i < Size; i++) map[i] = 0; }
		GLuint& operator[](GLenum target);

	  private:
		GLuint map[Size];
	} boundBuffers_;

    GLuint glHandle_;
	GLenum target_;

	/// Private copy constructor
	GLBufferObject(const GLBufferObject&);
	/// Private assignment operator
	GLBufferObject& operator=(const GLBufferObject&);
};

inline GLuint& GLBufferObject::BoundBuffers::operator[](GLenum target)
{
	switch(target)
	{
		default:
		case GL_ARRAY_BUFFER:
			return map[0];
	}
}

inline GLenum GLBufferObject::target() const
{
	return target_;
}

}

#endif
