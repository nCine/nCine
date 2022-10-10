#include <nctl/StaticHashMapIterator.h>
#include "GLShaderProgram.h"
#include "GLShader.h"
#include "GLDebug.h"
#include "RenderResources.h"
#include "RenderVaoPool.h"
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
		for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
			glDetachShader(glHandle_, shader->glHandle());
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

GLVertexFormat::Attribute *GLShaderProgram::attribute(const char *name)
{
	ASSERT(name);
	GLVertexFormat::Attribute *vertexAttribute = nullptr;

	int location = -1;
	const bool attributeFound = attributeLocations_.contains(name, location);

	if (attributeFound)
		vertexAttribute = &vertexFormat_[location];

	return vertexAttribute;
}

void GLShaderProgram::defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset)
{
	if (vbo)
	{
		for (int location : attributeLocations_)
		{
			vertexFormat_[location].setVbo(vbo);
			vertexFormat_[location].setBaseOffset(vboOffset);
		}
		vertexFormat_.setIbo(ibo);

		RenderResources::vaoPool().bindVao(vertexFormat_);
	}
}

void GLShaderProgram::reset()
{
	if (status_ != Status::NOT_LINKED && status_ != Status::COMPILATION_FAILED)
	{
		uniforms_.clear();
		uniformBlocks_.clear();
		attributes_.clear();

		attributeLocations_.clear();
		vertexFormat_.reset();

		if (boundProgram_ == glHandle_)
			glUseProgram(0);

		for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
			glDetachShader(glHandle_, shader->glHandle());
		attachedShaders_.clear();

		glDeleteProgram(glHandle_);

		RenderResources::removeCameraUniformData(this);
		RenderResources::unregisterBatchedShader(this);

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
		for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
			glDetachShader(glHandle_, shader->glHandle());
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
		                                                      ? GLUniformBlock::DiscoverUniforms::DISABLED
		                                                      : GLUniformBlock::DiscoverUniforms::ENABLED;

		discoverUniforms();
		discoverUniformBlocks(discover);
		discoverAttributes();
		initVertexFormat();
		status_ = Status::LINKED_WITH_INTROSPECTION;
	}
}

void GLShaderProgram::discoverUniforms()
{
	static const unsigned int NumIndices = 512;
	static GLuint uniformIndices[NumIndices];
	static GLint blockIndices[NumIndices];

	ZoneScoped;
	GLint uniformCount = 0;
	glGetProgramiv(glHandle_, GL_ACTIVE_UNIFORMS, &uniformCount);

	if (uniformCount > 0)
	{
		unsigned int uniformsOutsideBlocks = 0;
		GLuint indices[MaxNumUniforms];
		unsigned int remainingIndices = static_cast<unsigned int>(uniformCount);

		while (remainingIndices > 0)
		{
			const unsigned int uniformCountStep = (remainingIndices > NumIndices) ? NumIndices : remainingIndices;
			const unsigned int startIndex = static_cast<unsigned int>(uniformCount) - remainingIndices;

			for (unsigned int i = 0; i < uniformCountStep; i++)
				uniformIndices[i] = startIndex + i;

			glGetActiveUniformsiv(glHandle_, uniformCountStep, uniformIndices, GL_UNIFORM_BLOCK_INDEX, blockIndices);

			for (unsigned int i = 0; i < uniformCountStep; i++)
			{
				if (blockIndices[i] == -1)
				{
					indices[uniformsOutsideBlocks] = startIndex + i;
					uniformsOutsideBlocks++;
				}
				if (uniformsOutsideBlocks > MaxNumUniforms - 1)
					break;
			}

			remainingIndices -= uniformCountStep;
		}

		for (unsigned int i = 0; i < uniformsOutsideBlocks; i++)
		{
			GLUniform uniform(glHandle_, indices[i]);
			uniformsSize_ += uniform.memorySize();
			uniforms_.pushBack(uniform);

			LOGD_X("Shader %u - uniform %d : \"%s\"", glHandle_, uniform.location(), uniform.name());
		}
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

void GLShaderProgram::initVertexFormat()
{
	ZoneScoped;
	const unsigned int count = attributes_.size();
	if (count > GLVertexFormat::MaxAttributes)
		LOGW_X("More active attributes (%d) than supported by the vertex format class (%d)", count, GLVertexFormat::MaxAttributes);

	for (unsigned int i = 0; i < attributes_.size(); i++)
	{
		const GLAttribute &attribute = attributes_[i];
		const int location = attribute.location();
		if (location < 0)
			continue;

		attributeLocations_[attribute.name()] = location;
		vertexFormat_[location].init(attribute.location(), attribute.numComponents(), attribute.basicType());
	}
}

}
