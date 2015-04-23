#ifndef CLASS_NCINE_GLVERTEXATTRIBUTE
#define CLASS_NCINE_GLVERTEXATTRIBUTE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

class GLAttribute;

/// A class to wrap all the parameters needed by a `GLVertexAttribPointer()` call
class GLVertexAttribute
{
  public:
	GLVertexAttribute();
	GLVertexAttribute(const GLAttribute *shaderAttribute);

	inline const GLAttribute* shaderAttribute() const { return attribute_; }
	inline GLsizei vboStride() const { return vboStride_; }
	inline const GLvoid* vboPointer() const { return vboPointer_; }
	void setVboParameters(GLsizei vboStride, const GLvoid *vboPointer);

  private:
	const GLAttribute *attribute_;
	GLsizei vboStride_;
	const GLvoid *vboPointer_;

	void vertexAttribPointer();
	void enable();
	void disable();

	friend class GLShaderAttributes;
};

}

#endif
