#include "GLShaderProgram.h"
#include "GLShader.h"
#include "ncString.h"

namespace ncine {

GLuint GLShaderProgram::boundProgram_ = 0;

GLShaderProgram::GLShaderProgram()
	: attachedShaders_(4), glHandle_(0)
{
    glHandle_ = glCreateProgram();
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile)
	: attachedShaders_(4), glHandle_(0)
{
	glHandle_ = glCreateProgram();
    attachShader(GL_VERTEX_SHADER, vertexFile);
    attachShader(GL_FRAGMENT_SHADER, fragmentFile);
    link();
}

GLShaderProgram::~GLShaderProgram()
{
	if (boundProgram_ == glHandle_)
	{
		glUseProgram(0);
	}

    glDeleteProgram(glHandle_);
}

void GLShaderProgram::attachShader(GLenum type, const char *filename)
{
	GLShader *shader = new GLShader(type, filename);
    glAttachShader(glHandle_, shader->glHandle_);
    shader->compile();

	attachedShaders_.insertBack(shader);
}

bool GLShaderProgram::link()
{
    glLinkProgram(glHandle_);

	GLint status;
    glGetProgramiv(glHandle_, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length = 0;
		glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

		String infoLog(length);
		glGetProgramInfoLog(glHandle_, length, &length, infoLog.data());
		LOGW_X("%s", infoLog.data());

		return false;
	}

	for(unsigned int i =0; i < attachedShaders_.size(); i++)
    {
		delete attachedShaders_[i];
    }

	return true;
}

void GLShaderProgram::use()
{
    if (boundProgram_ != glHandle_)
	{
        glUseProgram(glHandle_);
        boundProgram_ = glHandle_;
	}
}

GLint GLShaderProgram::getUniformLocation(const GLchar *name)
{
    return glGetUniformLocation(glHandle_, name);
}

void GLShaderProgram::bindAttribLocation(GLuint index, const GLchar *name)
{
	// TODO: Add a isLinked flag and check it before binding an attribute location?
    glBindAttribLocation(glHandle_, index, name);
}

GLuint GLShaderProgram::getUniformBlockIndex(const GLchar *uniformBlockName)
{
    return glGetUniformBlockIndex(glHandle_, uniformBlockName);
}

void GLShaderProgram::uniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    glUniformBlockBinding(glHandle_, uniformBlockIndex, uniformBlockBinding);
}

}
