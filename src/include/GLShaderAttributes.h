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
	/// This class stores the OpenGL state regarding a defined vertex attribute pointer
	class GLVertexAttribPointerState
	{
	  private:
		bool enabled_;
		GLuint boundVboHandle_;
		GLint size_;
		GLenum type_;
		GLsizei vboStride_;
		const GLvoid *vboPointer_;

	  public:
		GLVertexAttribPointerState();
		inline bool isEnabled() const { return enabled_; }
		inline void setEnabled(bool enabled) { enabled_ = enabled; }
		inline GLuint boundVbo() const { return boundVboHandle_; }
		inline void setBoundVbo(GLuint boundVboHandle) { boundVboHandle_ = boundVboHandle; }
		bool operator==(const GLVertexAttribute &attribute) const;
		bool operator!=(const GLVertexAttribute &attribute) const;
		GLVertexAttribPointerState &operator=(const GLVertexAttribute &attribute);
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
