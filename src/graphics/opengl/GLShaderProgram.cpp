#include "GLShaderProgram.h"
#include "GLShader.h"
#include "nctl/ncString.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLuint GLShaderProgram::boundProgram_ = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderProgram::GLShaderProgram()
	: glHandle_(0),
	  attachedShaders_(AttachedShadersInitialSize),
	  uniforms_(UniformsInitialSize), attributes_(AttributesInitialSize)
{
	glHandle_ = glCreateProgram();
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile)
	: glHandle_(0),
	  attachedShaders_(AttachedShadersInitialSize),
	  uniforms_(UniformsInitialSize), attributes_(AttributesInitialSize)
{
	glHandle_ = glCreateProgram();
	attachShader(GL_VERTEX_SHADER, vertexFile);
	attachShader(GL_FRAGMENT_SHADER, fragmentFile);
	link();
}

GLShaderProgram::~GLShaderProgram()
{
	if (boundProgram_ == glHandle_)
		glUseProgram(0);

	glDeleteProgram(glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderProgram::attachShader(GLenum type, const char *filename)
{
	GLShader *shader = new GLShader(type, filename);
	glAttachShader(glHandle_, shader->glHandle_);
	shader->compile();

	attachedShaders_.pushBack(shader);
}

void GLShaderProgram::attachShaderFromString(GLenum type, const char *string)
{
	GLShader *shader = new GLShader(type);
	shader->loadFromString(string);
	glAttachShader(glHandle_, shader->glHandle_);
	shader->compile();

	attachedShaders_.pushBack(shader);
}

bool GLShaderProgram::link()
{
	glLinkProgram(glHandle_);

	GLint status;
	glGetProgramiv(glHandle_, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

		if (length > 0)
		{
			nctl::String infoLog(length);
			glGetProgramInfoLog(glHandle_, length, &length, infoLog.data());
			LOGW_X("%s", infoLog.data());
		}

		return false;
	}

	// After linking, shader objects are not needed anymore
	for (GLShader *attachedShader : attachedShaders_)
		delete attachedShader;

	discoverUniforms();
	discoverAttributes();

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

GLint GLShaderProgram::getUniformLocation(const GLchar *name) const
{
	GLint location = glGetUniformLocation(glHandle_, name);

	if (location == -1)
		LOGW_X("Location not found for uniform \"%s\" in shader program %u", name, glHandle_);

	return location;
}

void GLShaderProgram::bindAttribLocation(GLuint index, const GLchar *name)
{
	// TODO: Add a `isLinked` flag and check it before binding an attribute location?
	glBindAttribLocation(glHandle_, index, name);
}

#if !defined(__ANDROID__) && !defined(__APPLE__)
GLuint GLShaderProgram::getUniformBlockIndex(const GLchar *uniformBlockName) const
{
	GLuint index = glGetUniformBlockIndex(glHandle_, uniformBlockName);

	if (index == GL_INVALID_INDEX)
		LOGW_X("Index not found for uniform block \"%s\" in shader program %u", uniformBlockName, glHandle_);

	return index;
}

void GLShaderProgram::uniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
	glUniformBlockBinding(glHandle_, uniformBlockIndex, uniformBlockBinding);
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderProgram::discoverUniforms()
{
	GLint count;
	glGetProgramiv(glHandle_, GL_ACTIVE_UNIFORMS, &count);

	for (int i = 0; i < count; i++)
	{
		GLUniform uniform(glHandle_, i);
		uniforms_.pushBack(uniform);

		LOGD_X("Shader %u - uniform %d : \"%s\"", glHandle_, uniform.location(), uniform.name());
	}
}

void GLShaderProgram::discoverAttributes()
{
	GLint count;
	glGetProgramiv(glHandle_, GL_ACTIVE_ATTRIBUTES, &count);

	for (int i = 0; i < count; i++)
	{
		GLAttribute attribute(glHandle_, i);
		attributes_.pushBack(attribute);

		LOGD_X("Shader %u - attribute %d : \"%s\"", glHandle_, attribute.location(), attribute.name());
	}
}

}
