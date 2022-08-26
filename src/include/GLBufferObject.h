#ifndef CLASS_NCINE_GLBUFFEROBJECT
#define CLASS_NCINE_GLBUFFEROBJECT

#include "GLHashMap.h"

namespace ncine {

/// A class to handle OpenGL buffer objects of different kinds
class GLBufferObject
{
  public:
	explicit GLBufferObject(GLenum target);
	~GLBufferObject();

	inline GLuint glHandle() const { return glHandle_; }
	inline GLenum target() const { return target_; }
	inline GLsizeiptr size() const { return size_; }

	bool bind() const;
	bool unbind() const;

	void bufferData(GLsizeiptr size, const GLvoid *data, GLenum usage);
	void bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data);
#if !defined(WITH_OPENGLES)
	void bufferStorage(GLsizeiptr size, const GLvoid *data, GLbitfield flags);
#endif

	void bindBufferBase(GLuint index);
	void bindBufferRange(GLuint index, GLintptr offset, GLsizei ptrsize);
	void *mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access);
	void flushMappedBufferRange(GLintptr offset, GLsizeiptr length);
	GLboolean unmap();
#if !defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_2)
	void texBuffer(GLenum internalformat);
#endif

	void setObjectLabel(const char *label);

  private:
	static class GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> boundBuffers_;

	struct BufferRange
	{
		BufferRange()
		    : glHandle(0), offset(0), ptrsize(0) {}

		GLuint glHandle;
		GLintptr offset;
		GLsizei ptrsize;
	};

	GLuint glHandle_;
	GLenum target_;
	GLsizeiptr size_;
	bool mapped_;

	static const int MaxIndexBufferRange = 128;
	/// Current bound index for buffer base. Negative if not bound.
	static GLuint boundIndexBase_[MaxIndexBufferRange];
	/// Current range and offset for buffer range index
	static BufferRange boundBufferRange_[MaxIndexBufferRange];

	/// Deleted copy constructor
	GLBufferObject(const GLBufferObject &) = delete;
	/// Deleted assignment operator
	GLBufferObject &operator=(const GLBufferObject &) = delete;

	inline static void setBoundHandle(GLenum target, GLuint glHandle) { boundBuffers_[target] = glHandle; }
	static bool bindHandle(GLenum target, GLuint glHandle);
	friend class RenderVaoPool;
};

}

#endif
