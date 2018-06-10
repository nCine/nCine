#ifndef CLASS_NCINE_GLVERTEXFORMAT
#define CLASS_NCINE_GLVERTEXFORMAT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include "nctl/StaticArray.h"

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
			: enabled_(false), vbo_(nullptr), index_(0), size_(-1), type_(GL_FLOAT), stride_(0), pointer_(nullptr) { }

		void init(unsigned int index, GLint size, GLenum type);
		bool operator==(const Attribute &other) const;
		bool operator!=(const Attribute &other) const;

		inline void setVboParameters(GLsizei stride, const GLvoid *pointer) { stride_ = stride; pointer_ = pointer; }
		inline void setVbo(const GLBufferObject *vbo) { vbo_ = vbo; }

	  private:
		bool enabled_;
		const GLBufferObject *vbo_;
		unsigned int index_;
		GLint size_;
		GLenum type_;
		GLsizei stride_;
		const GLvoid *pointer_;

		friend class GLVertexFormat;
	};

	GLVertexFormat();
	GLVertexFormat(const GLVertexFormat &other);

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
