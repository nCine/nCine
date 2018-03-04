#ifndef CLASS_NCINE_GLSHADERATTRIBUTES
#define CLASS_NCINE_GLSHADERATTRIBUTES

#include "GLVertexAttribute.h"
#include "nctl/StaticArray.h"
#include "nctl/HashMap.h"
#include "nctl/String.h"

namespace ncine {

class GLShaderProgram;
class GLBufferObject;

/// A class to handle all the attributes of a shader program using a hashmap
class GLShaderAttributes
{
  public:
	GLShaderAttributes();
	explicit GLShaderAttributes(GLShaderProgram *shaderProgram);
	void setProgram(GLShaderProgram *shaderProgram);

	GLVertexAttribute *attribute(const char *name);
	void defineVertexPointers(const GLBufferObject *vbo);

  private:
	/// This structure stores the OpenGL state regarding a defined vertex attribute pointer
	struct GLVertexAttribPointerState
	{
		GLVertexAttribPointerState()
		    : enabled(false), boundVbo(0), size(-1), type(GL_FLOAT), vboStride(0), vboPointer(nullptr) { }

		bool enabled;
		GLuint boundVbo;
		GLint size;
		GLenum type;
		GLsizei vboStride;
		const GLvoid *vboPointer;

		bool operator==(const GLVertexAttribute &attribute) const;
		bool operator!=(const GLVertexAttribute &attribute) const;
	};

	static const unsigned int MaxDefinedVertexAttribPointers = 16;
	/// An array to cache the state of `glVertexAttribPointer()` calls
	static nctl::StaticArray<GLVertexAttribPointerState, MaxDefinedVertexAttribPointers> definedPointers_;

	GLShaderProgram *shaderProgram_;

	static const unsigned int VertexAttributesHashSize = 8;
	nctl::StringHashMap<GLVertexAttribute> vertexAttributes_;
	/// A dummy attribute returned when a vertex attribute is not found in the hashmap
	static GLVertexAttribute attributeNotFound_;

	void importAttributes();
};

}

#endif
