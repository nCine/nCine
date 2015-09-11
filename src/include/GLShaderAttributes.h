#ifndef CLASS_NCINE_GLSHADERATTRIBUTES
#define CLASS_NCINE_GLSHADERATTRIBUTES

#include "GLVertexAttribute.h"
#include "Array.h"
#include "StaticArray.h"
#include "HashMap.h"
#include "ncString.h"

namespace ncine {

class GLShaderProgram;

/// A class to handle all the attributes of a shader program using a hashmap
class GLShaderAttributes
{
  public:
	GLShaderAttributes();
	GLShaderAttributes(GLShaderProgram *shaderProgram);
	void setProgram(GLShaderProgram *shaderProgram);

	GLVertexAttribute* attribute(const char *name);
	void defineVertexPointers(GLuint buondVboHandle);

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
		bool operator==(const GLVertexAttribute& attribute) const;
		bool operator!=(const GLVertexAttribute& attribute) const;
		GLVertexAttribPointerState& operator=(const GLVertexAttribute& attribute);
	};

	static const int MaxDefinedVertexAttribPointers = 16;
	/// An array to cache the state of `glVertexAttribPointer()` calls
	static StaticArray<GLVertexAttribPointerState, MaxDefinedVertexAttribPointers> definedPointers_;

	GLShaderProgram* shaderProgram_;

	static const int VertexAttributesHashSize = 8;
	StringHashMap<GLVertexAttribute>::Type vertexAttributes_;

	void importAttributes();
};

}

#endif
