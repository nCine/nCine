#include "GLShaderProgram.h"
#include "GLShader.h"
#include "GLDebug.h"
#include <nctl/String.h>
#include <cstring> // for strnlen()
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLuint GLShaderProgram::boundProgram_ = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderProgram::GLShaderProgram()
	: glHandle_(0), attachedShaders_(AttachedShadersInitialSize),
	  status_(Status::NOT_LINKED), uniformsSize_(0), uniformBlocksSize_(0),
	  uniforms_(UniformsInitialSize), uniformBlocks_(UniformBlocksInitialSize),
	  attributes_(AttributesInitialSize)
{
	glHandle_ = glCreateProgram();
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection)
	: GLShaderProgram()
{
	glHandle_ = glCreateProgram();
	attachShader(GL_VERTEX_SHADER, vertexFile);
	attachShader(GL_FRAGMENT_SHADER, fragmentFile);
	link(introspection);
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile)
	: GLShaderProgram(vertexFile, fragmentFile, Introspection::ENABLED)
{

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

	const size_t length = strnlen(filename, GLDebug::maxLabelLength());
	GLDebug::objectLabel(GLDebug::LabelTypes::SHADER, shader->glHandle_, length, filename);

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

bool GLShaderProgram::link(Introspection introspection)
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

		status_ = Status::LINKING_FAILED;
		return false;
	}

	// After linking, shader objects are not needed anymore
	for (GLShader *attachedShader : attachedShaders_)
		delete attachedShader;

	if (introspection != Introspection::DISABLED)
	{
		const GLUniformBlock::DiscoverUniforms discover = (introspection == Introspection::NO_UNIFORMS_IN_BLOCKS) ?
			GLUniformBlock::DiscoverUniforms::DISBLED : GLUniformBlock::DiscoverUniforms::ENABLED;

		discoverUniforms();
		discoverUniformBlocks(discover);
		discoverAttributes();
		status_ = Status::LINKED_WITH_INTROSPECTION;
	}
	else
		status_ = Status::LINKED;

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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderProgram::discoverUniforms()
{
	ZoneScoped;
	GLint count;
	glGetProgramiv(glHandle_, GL_ACTIVE_UNIFORMS, &count);

	unsigned int uniformsOutsideBlocks = 0;
	GLuint indices[MaxNumUniforms];
	GLint blockIndex;
	for (unsigned int i = 0; i < static_cast<unsigned int>(count); i++)
	{
		glGetActiveUniformsiv(glHandle_, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIndex);
		if (blockIndex == -1)
		{
			indices[uniformsOutsideBlocks] = i;
			uniformsOutsideBlocks++;
		}
	}

	ASSERT(uniformsOutsideBlocks < MaxNumUniforms);

	for (unsigned int i = 0; i < uniformsOutsideBlocks; i++)
	{
		GLUniform uniform(glHandle_, indices[i]);
		uniformsSize_ += uniform.memorySize();
		uniforms_.pushBack(uniform);

		LOGD_X("Shader %u - uniform %d : \"%s\"", glHandle_, uniform.location(), uniform.name());
	}
}

void GLShaderProgram::discoverUniformBlocks(GLUniformBlock::DiscoverUniforms discover)
{
	ZoneScoped;
	GLint count;
	glGetProgramiv(glHandle_, GL_ACTIVE_UNIFORM_BLOCKS, &count);

	for (int i = 0; i < count; i++)
	{
		GLUniformBlock uniformBlock(glHandle_, i, discover);
		uniformBlocksSize_ += uniformBlock.size();
		uniformBlocks_.pushBack(uniformBlock);

		LOGD_X("Shader %u - uniform block %u : \"%s\"", glHandle_, uniformBlock.index(), uniformBlock.name());
	}
}

void GLShaderProgram::discoverAttributes()
{
	ZoneScoped;
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
