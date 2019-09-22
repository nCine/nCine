#ifndef CLASS_NCINE_GLVERTEXFORMAT
#define CLASS_NCINE_GLVERTEXFORMAT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include <nctl/StaticArray.h>

namespace ncine {

class GLBufferObject;

/// A class that collects all the state that specifies the format of a vertex
class GLVertexFormat
{
  public:
	/// The minimum value for `GL_MAX_VERTEX_ATTRIBS`
	static const unsigned int MaxAttributes = 16;

	class Attribute
	{
	  public:
		Attribute()
		    : enabled_(false), vbo_(nullptr), index_(0), size_(-1), type_(GL_FLOAT), stride_(0), pointer_(nullptr), baseOffset_(0) {}

		void init(unsigned int index, GLint size, GLenum type);
		bool operator==(const Attribute &other) const;
		bool operator!=(const Attribute &other) const;

		inline void setVboParameters(GLsizei stride, const GLvoid *pointer)
		{
			stride_ = stride;
			pointer_ = pointer;
		}
		inline void setVbo(const GLBufferObject *vbo) { vbo_ = vbo; }
		inline void setBaseOffset(unsigned int baseOffset) { baseOffset_ = baseOffset; }

		inline void setSize(GLint size) { size_ = size; }
		inline void setType(GLenum type) { type_ = type; }
		inline void setNormalized(bool normalized) { normalized_ = normalized; }

	  private:
		bool enabled_;
		const GLBufferObject *vbo_;
		unsigned int index_;
		GLint size_;
		GLenum type_;
		GLboolean normalized_;
		GLsizei stride_;
		const GLvoid *pointer_;
		/// Used to simulate missing `glDrawElementsBaseVertex()` on OpenGL ES 3.0
		unsigned int baseOffset_;

		friend class GLVertexFormat;
	};

	GLVertexFormat();
	GLVertexFormat(const GLVertexFormat &other) = default;
	GLVertexFormat &operator=(const ncine::GLVertexFormat &other) = default;

	inline const GLBufferObject *ibo() const { return ibo_; }
	inline void setIbo(const GLBufferObject *ibo) { ibo_ = ibo; }
	void define();
	void reset();

	inline Attribute &operator[](unsigned int index) { return attributes_[index]; }
	inline const Attribute &operator[](unsigned int index) const { return attributes_[index]; }

	bool operator==(const GLVertexFormat &other) const;
	bool operator!=(const GLVertexFormat &other) const;

  private:
	nctl::StaticArray<Attribute, MaxAttributes> attributes_;
	const GLBufferObject *ibo_;
};

}

#endif
