#ifndef CLASS_NCINE_GLSHADERPROGRAM
#define CLASS_NCINE_GLSHADERPROGRAM

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "Array.h"

namespace ncine
{

class GLShader;

class GLShaderProgram
{
public:
	GLShaderProgram();
	GLShaderProgram(const char *vertexFile, const char *fragmentFile);
	~GLShaderProgram();

    void attachShader(GLenum type, const char *filename);
	bool link();
	void use();

    GLint getUniformLocation(const GLchar *name);

	void bindAttribLocation(GLuint index, const GLchar *name);
	GLuint getUniformBlockIndex(const GLchar *uniformBlockName);
	void uniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding);

private:
    static GLuint boundProgram_;

	Array<GLShader *> attachedShaders_;
    GLuint glHandle_;

	/// Private copy constructor
	GLShaderProgram(const GLShaderProgram&);
	/// Private assignment operator
	GLShaderProgram& operator=(const GLShaderProgram&);
};

}

#endif
