#ifndef CLASS_NCINE_GLSHADERPROGRAM
#define CLASS_NCINE_GLSHADERPROGRAM

#include "Array.h"
#include "GLUniform.h"
#include "GLAttribute.h"

namespace ncine {

class GLShader;

class GLShaderProgram
{
  public:
	GLShaderProgram();
	GLShaderProgram(const char *vertexFile, const char *fragmentFile);
	~GLShaderProgram();

	inline GLuint glHandle() const { return glHandle_; }

	void attachShader(GLenum type, const char *filename);
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
	Array<GLShader *> attachedShaders_;

	static const int UniformsInitialSize = 8;
	Array<GLUniform> uniforms_;
	static const int AttributesInitialSize = 4;
	Array<GLAttribute> attributes_;

	void discoverUniforms();
	void discoverAttributes();

	/// Private copy constructor
	GLShaderProgram(const GLShaderProgram&);
	/// Private assignment operator
	GLShaderProgram& operator=(const GLShaderProgram&);

	friend class GLShaderUniforms;
	friend class GLShaderAttributes;
};

}

#endif
