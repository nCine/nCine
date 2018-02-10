#ifndef CLASS_NCINE_GLSHADERPROGRAM
#define CLASS_NCINE_GLSHADERPROGRAM

#include "nctl/Array.h"
#include "GLUniform.h"
#include "GLAttribute.h"

namespace ncine {

class GLShader;

/// A class to handle OpenGL shader programs
class GLShaderProgram
{
  public:
	GLShaderProgram();
	GLShaderProgram(const char *vertexFile, const char *fragmentFile);
	~GLShaderProgram();

	inline GLuint glHandle() const { return glHandle_; }

	void attachShader(GLenum type, const char *filename);
	void attachShaderFromString(GLenum type, const char *string);
	bool link();
	void use();

	GLint getUniformLocation(const GLchar *name) const;
	void bindAttribLocation(GLuint index, const GLchar *name);

#if !defined(__ANDROID__) && !defined(__APPLE__)
	GLuint getUniformBlockIndex(const GLchar *uniformBlockName) const;
	void uniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding);
#endif

  private:
	static GLuint boundProgram_;

	GLuint glHandle_;
	static const int AttachedShadersInitialSize = 4;
	nctl::Array<GLShader *> attachedShaders_;

	static const int UniformsInitialSize = 8;
	nctl::Array<GLUniform> uniforms_;
	static const int AttributesInitialSize = 4;
	nctl::Array<GLAttribute> attributes_;

	void discoverUniforms();
	void discoverAttributes();

	/// Deleted copy constructor
	GLShaderProgram(const GLShaderProgram &) = delete;
	/// Deleted assignment operator
	GLShaderProgram &operator=(const GLShaderProgram &) = delete;

	friend class GLShaderUniforms;
	friend class GLShaderAttributes;
};

}

#endif
