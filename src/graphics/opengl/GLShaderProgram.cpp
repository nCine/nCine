#include "GLShaderProgram.h"
#include "GLShader.h"
#include "GLDebug.h"
#include <nctl/CString.h>
#include <nctl/String.h>
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
    : GLShaderProgram(QueryPhase::IMMEDIATE)
{
}

GLShaderProgram::GLShaderProgram(QueryPhase queryPhase)
    : glHandle_(0), attachedShaders_(AttachedShadersInitialSize),
      status_(Status::NOT_LINKED), queryPhase_(queryPhase),
      uniformsSize_(0), uniformBlocksSize_(0), uniforms_(UniformsInitialSize),
      uniformBlocks_(UniformBlocksInitialSize), attributes_(AttributesInitialSize)
{
	glHandle_ = glCreateProgram();
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection, QueryPhase queryPhase)
    : GLShaderProgram(queryPhase)
{
	attachShader(GL_VERTEX_SHADER, vertexFile);
	attachShader(GL_FRAGMENT_SHADER, fragmentFile);
	link(introspection);
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection)
    : GLShaderProgram(vertexFile, fragmentFile, introspection, QueryPhase::IMMEDIATE)
{
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile)
    : GLShaderProgram(vertexFile, fragmentFile, Introspection::ENABLED, QueryPhase::IMMEDIATE)
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
	nctl::UniquePtr<GLShader> shader = nctl::makeUnique<GLShader>(type, filename);
	glAttachShader(glHandle_, shader->glHandle());

	const GLShader::ErrorChecking errorChecking = (queryPhase_ == GLShaderProgram::QueryPhase::IMMEDIATE)
	                                                  ? GLShader::ErrorChecking::IMMEDIATE
	                                                  : GLShader::ErrorChecking::DEFERRED;
	shader->compile(errorChecking);
	FATAL_ASSERT(shader->status() != GLShader::Status::COMPILATION_FAILED);

	const size_t length = nctl::strnlen(filename, GLDebug::maxLabelLength());
	GLDebug::objectLabel(GLDebug::LabelTypes::SHADER, shader->glHandle(), length, filename);

	attachedShaders_.pushBack(nctl::move(shader));
}

void GLShaderProgram::attachShaderFromString(GLenum type, const char *string)
{
	nctl::UniquePtr<GLShader> shader = nctl::makeUnique<GLShader>(type);
	shader->loadFromString(string);
	glAttachShader(glHandle_, shader->glHandle());

	const GLShader::ErrorChecking errorChecking = (queryPhase_ == GLShaderProgram::QueryPhase::IMMEDIATE)
	                                                  ? GLShader::ErrorChecking::IMMEDIATE
	                                                  : GLShader::ErrorChecking::DEFERRED;
	shader->compile(errorChecking);
	FATAL_ASSERT(shader->status() != GLShader::Status::COMPILATION_FAILED);

	attachedShaders_.pushBack(nctl::move(shader));
}

void GLShaderProgram::link(Introspection introspection)
{
	introspection_ = introspection;
	glLinkProgram(glHandle_);

	if (queryPhase_ == QueryPhase::IMMEDIATE)
	{
		checkLinking();

		// After linking, shader objects are not needed anymore
		for (nctl::UniquePtr<GLShader> &attachedShader : attachedShaders_)
			attachedShader.reset(nullptr);

		performIntrospection();
	}
	else
		status_ = GLShaderProgram::Status::LINKED_WITH_DEFERRED_QUERIES;
}

void GLShaderProgram::use()
{
	if (boundProgram_ != glHandle_)
	{
		deferredQueries();
		glUseProgram(glHandle_);
		boundProgram_ = glHandle_;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderProgram::deferredQueries()
{
	if (status_ == GLShaderProgram::Status::LINKED_WITH_DEFERRED_QUERIES)
	{
		for (nctl::UniquePtr<GLShader> &attachedShader : attachedShaders_)
			FATAL_ASSERT(attachedShader->checkCompilation());

		const bool linkCheck = checkLinking();
		FATAL_ASSERT(linkCheck);

		// After linking, shader objects are not needed anymore
		for (nctl::UniquePtr<GLShader> &attachedShader : attachedShaders_)
			attachedShader.reset(nullptr);

		if (introspection_ != GLShaderProgram::Introspection::DISABLED)
			performIntrospection();
	}
}

bool GLShaderProgram::checkLinking()
{
	if (status_ == Status::LINKED || status_ == Status::LINKED_WITH_INTROSPECTION)
		return true;

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

	status_ = Status::LINKED;
	return true;
}

void GLShaderProgram::performIntrospection()
{
	if (introspection_ != Introspection::DISABLED && status_ != Status::LINKED_WITH_INTROSPECTION)
	{
		const GLUniformBlock::DiscoverUniforms discover = (introspection_ == Introspection::NO_UNIFORMS_IN_BLOCKS)
		                                                      ? GLUniformBlock::DiscoverUniforms::DISBLED
		                                                      : GLUniformBlock::DiscoverUniforms::ENABLED;

		discoverUniforms();
		discoverUniformBlocks(discover);
		discoverAttributes();
		status_ = Status::LINKED_WITH_INTROSPECTION;
	}
}

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
