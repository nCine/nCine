#include "GLShaderProgram.h"
#include "GLShader.h"
#include "GLDebug.h"
#include "RenderResources.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLuint GLShaderProgram::boundProgram_ = 0;
char GLShaderProgram::infoLogString_[MaxInfoLogLength];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderProgram::GLShaderProgram()
    : GLShaderProgram(QueryPhase::IMMEDIATE)
{
}

GLShaderProgram::GLShaderProgram(QueryPhase queryPhase)
    : glHandle_(0), attachedShaders_(AttachedShadersInitialSize),
      status_(Status::NOT_LINKED), queryPhase_(queryPhase), shouldLogOnErrors_(true),
      uniformsSize_(0), uniformBlocksSize_(0), uniforms_(UniformsInitialSize),
      uniformBlocks_(UniformBlocksInitialSize), attributes_(AttributesInitialSize)
{
	glHandle_ = glCreateProgram();
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection, QueryPhase queryPhase)
    : GLShaderProgram(queryPhase)
{
	const bool hasCompiledVS = attachShader(GL_VERTEX_SHADER, vertexFile);
	const bool hasCompiledFS = attachShader(GL_FRAGMENT_SHADER, fragmentFile);

	if (hasCompiledVS && hasCompiledFS)
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

	RenderResources::removeCameraUniformData(this);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLShaderProgram::isLinked() const
{
	return (status_ == Status::LINKED ||
	        status_ == Status::LINKED_WITH_DEFERRED_QUERIES ||
	        status_ == Status::LINKED_WITH_INTROSPECTION);
}

unsigned int GLShaderProgram::retrieveInfoLogLength() const
{
	GLint length = 0;
	glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

	return static_cast<unsigned int>(length);
}

void GLShaderProgram::retrieveInfoLog(nctl::String &infoLog) const
{
	GLint length = 0;
	glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

	if (length > 0 && infoLog.capacity() > 0)
	{
		const unsigned int capacity = infoLog.capacity();
		glGetProgramInfoLog(glHandle_, capacity, &length, infoLog.data());
		infoLog.setLength(static_cast<unsigned int>(length) < capacity - 1 ? static_cast<unsigned int>(length) : capacity - 1);
	}
}

bool GLShaderProgram::attachShader(GLenum type, const char *filename)
{
	nctl::UniquePtr<GLShader> shader = nctl::makeUnique<GLShader>(type, filename);
	glAttachShader(glHandle_, shader->glHandle());

	const GLShader::ErrorChecking errorChecking = (queryPhase_ == GLShaderProgram::QueryPhase::IMMEDIATE)
	                                                  ? GLShader::ErrorChecking::IMMEDIATE
	                                                  : GLShader::ErrorChecking::DEFERRED;
	const bool hasCompiled = shader->compile(errorChecking, shouldLogOnErrors_);

	if (hasCompiled)
	{
		GLDebug::objectLabel(GLDebug::LabelTypes::SHADER, shader->glHandle(), filename);
		attachedShaders_.pushBack(nctl::move(shader));
	}
	else
		status_ = Status::COMPILATION_FAILED;

	return hasCompiled;
}

bool GLShaderProgram::attachShaderFromString(GLenum type, const char *string)
{
	nctl::UniquePtr<GLShader> shader = nctl::makeUnique<GLShader>(type);
	shader->loadFromString(string);
	glAttachShader(glHandle_, shader->glHandle());

	const GLShader::ErrorChecking errorChecking = (queryPhase_ == GLShaderProgram::QueryPhase::IMMEDIATE)
	                                                  ? GLShader::ErrorChecking::IMMEDIATE
	                                                  : GLShader::ErrorChecking::DEFERRED;
	const bool hasCompiled = shader->compile(errorChecking, shouldLogOnErrors_);

	if (hasCompiled)
		attachedShaders_.pushBack(nctl::move(shader));
	else
		status_ = Status::COMPILATION_FAILED;

	return hasCompiled;
}

bool GLShaderProgram::link(Introspection introspection)
{
	introspection_ = introspection;
	glLinkProgram(glHandle_);

	if (queryPhase_ == QueryPhase::IMMEDIATE)
	{
		const bool linkCheck = checkLinking();
		if (linkCheck == false)
			return false;

		// After linking, shader objects are not needed anymore
		attachedShaders_.clear();

		performIntrospection();
		return linkCheck;
	}
	else
	{
		status_ = GLShaderProgram::Status::LINKED_WITH_DEFERRED_QUERIES;
		return true;
	}
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

bool GLShaderProgram::validate()
{
	glValidateProgram(glHandle_);
	GLint status;
	glGetProgramiv(glHandle_, GL_VALIDATE_STATUS, &status);
	return (status == GL_TRUE);
}

void GLShaderProgram::reset()
{
	if (status_ != Status::NOT_LINKED && status_ != Status::COMPILATION_FAILED)
	{
		uniforms_.clear();
		uniformBlocks_.clear();
		attributes_.clear();

		if (boundProgram_ == glHandle_)
			glUseProgram(0);
		attachedShaders_.clear();
		glDeleteProgram(glHandle_);

		RenderResources::removeCameraUniformData(this);

		glHandle_ = glCreateProgram();
	}

	status_ = Status::NOT_LINKED;
}

void GLShaderProgram::setObjectLabel(const char *label)
{
	GLDebug::objectLabel(GLDebug::LabelTypes::PROGRAM, glHandle_, label);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GLShaderProgram::deferredQueries()
{
	if (status_ == GLShaderProgram::Status::LINKED_WITH_DEFERRED_QUERIES)
	{
		for (nctl::UniquePtr<GLShader> &attachedShader : attachedShaders_)
		{
			const bool compileCheck = attachedShader->checkCompilation(shouldLogOnErrors_);
			if (compileCheck == false)
				return false;
		}

		const bool linkCheck = checkLinking();
		if (linkCheck == false)
			return false;

		// After linking, shader objects are not needed anymore
		attachedShaders_.clear();

		performIntrospection();
	}
	return true;
}

bool GLShaderProgram::checkLinking()
{
	if (status_ == Status::LINKED || status_ == Status::LINKED_WITH_INTROSPECTION)
		return true;

	GLint status;
	glGetProgramiv(glHandle_, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		if (shouldLogOnErrors_)
		{
			GLint length = 0;
			glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

			if (length > 0)
			{
				glGetProgramInfoLog(glHandle_, MaxInfoLogLength, &length, infoLogString_);
				LOGW_X("%s", infoLogString_);
			}
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
		if (uniformsOutsideBlocks > MaxNumUniforms - 1)
			break;
	}

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
