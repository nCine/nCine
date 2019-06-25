#include "GLShaderUniforms.h"
#include "GLShaderProgram.h"
#include "GLUniformCache.h"
#include "RenderResources.h"
#include <nctl/StaticHashMapIterator.h>
#include <nctl/algorithms.h>

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLUniformCache GLShaderUniforms::uniformNotFound_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderUniforms::GLShaderUniforms()
    : shaderProgram_(nullptr)
{
}

GLShaderUniforms::GLShaderUniforms(GLShaderProgram *shaderProgram)
    : GLShaderUniforms()
{
	setProgram(shaderProgram);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniforms::setProgram(GLShaderProgram *shaderProgram)
{
	ASSERT(shaderProgram);

	shaderProgram_ = shaderProgram;
	shaderProgram_->deferredQueries();
	uniformCaches_.clear();

	if (shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		importUniforms();
}

void GLShaderUniforms::setUniformsDataPointer(GLubyte *dataPointer)
{
	ASSERT(dataPointer);

	if (shaderProgram_->status() != GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		return;

	unsigned int offset = 0;
	for (GLUniformCache &uniformCache : uniformCaches_)
	{
		uniformCache.setDataPointer(dataPointer + offset);
		offset += uniformCache.uniform()->memorySize();
	}
}

GLUniformCache *GLShaderUniforms::uniform(const char *name)
{
	ASSERT(name);
	GLUniformCache *uniformCache = nullptr;

	if (shaderProgram_)
	{
		uniformCache = uniformCaches_.find(name);

		if (uniformCache == nullptr)
		{
			// Returning the dummy uniform cache to prevent the application from crashing
			uniformCache = &uniformNotFound_;
			LOGW_X("Uniform \"%s\" not found in shader program %u", name, shaderProgram_->glHandle());
		}
	}
	else
		LOGE_X("Cannot find uniform \"%s\", no shader program associated", name);

	return uniformCache;
}

void GLShaderUniforms::commitUniforms()
{
	if (shaderProgram_)
	{
		if (shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		{
			shaderProgram_->use();
			forEach(uniformCaches_.begin(), uniformCaches_.end(), [](GLUniformCache &uniform) { uniform.commitValue(); });
		}
	}
	else
		LOGE("No shader program associated");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniforms::importUniforms()
{
	const unsigned int count = shaderProgram_->uniforms_.size();
	if (count > UniformCachesHashSize)
		LOGW_X("More active uniforms (%d) than hashmap buckets (%d)", count, UniformCachesHashSize);

	for (const GLUniform &uniform : shaderProgram_->uniforms_)
	{
		GLUniformCache uniformCache(&uniform);
		uniformCaches_[uniform.name()] = uniformCache;
	}
}

}
