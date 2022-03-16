#include "GLShaderUniforms.h"
#include "GLShaderProgram.h"
#include "GLUniformCache.h"
#include "RenderResources.h"
#include <nctl/StaticHashMapIterator.h>
#include <nctl/algorithms.h>
#include <nctl/CString.h>

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderUniforms::GLShaderUniforms()
    : shaderProgram_(nullptr)
{
}

GLShaderUniforms::GLShaderUniforms(GLShaderProgram *shaderProgram)
    : GLShaderUniforms(shaderProgram, nullptr, nullptr)
{
	setProgram(shaderProgram);
}

GLShaderUniforms::GLShaderUniforms(GLShaderProgram *shaderProgram, const char *includeOnly, const char *exclude)
    : GLShaderUniforms()
{
	setProgram(shaderProgram, includeOnly, exclude);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniforms::setProgram(GLShaderProgram *shaderProgram, const char *includeOnly, const char *exclude)
{
	ASSERT(shaderProgram);

	shaderProgram_ = shaderProgram;
	shaderProgram_->deferredQueries();
	uniformCaches_.clear();

	if (shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		importUniforms(includeOnly, exclude);
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

void GLShaderUniforms::setDirty(bool isDirty)
{
	if (shaderProgram_->status() != GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		return;

	forEach(uniformCaches_.begin(), uniformCaches_.end(), [isDirty](GLUniformCache &uniform) { uniform.setDirty(isDirty); });
}

GLUniformCache *GLShaderUniforms::uniform(const char *name)
{
	ASSERT(name);
	GLUniformCache *uniformCache = nullptr;

	if (shaderProgram_)
		uniformCache = uniformCaches_.find(name);
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

void GLShaderUniforms::importUniforms(const char *includeOnly, const char *exclude)
{
	const unsigned int MaxUniformName = 128;

	unsigned int importedCount = 0;
	for (const GLUniform &uniform : shaderProgram_->uniforms_)
	{
		const char *uniformName = uniform.name();
		const char *currentIncludeOnly = includeOnly;
		const char *currentExclude = exclude;
		bool shouldImport = true;

		if (includeOnly != nullptr)
		{
			shouldImport = false;
			while (currentIncludeOnly != nullptr && currentIncludeOnly[0] != '\0')
			{
				if (strncmp(currentIncludeOnly, uniformName, MaxUniformName) == 0)
				{
					shouldImport = true;
					break;
				}
				currentIncludeOnly += nctl::strnlen(currentIncludeOnly, MaxUniformName) + 1;
			}
		}

		if (exclude != nullptr)
		{
			while (currentExclude != nullptr && currentExclude[0] != '\0')
			{
				if (strncmp(currentExclude, uniformName, MaxUniformName) == 0)
				{
					shouldImport = false;
					break;
				}
				currentExclude += nctl::strnlen(currentExclude, MaxUniformName) + 1;
			}
		}

		if (shouldImport)
		{
			GLUniformCache uniformCache(&uniform);
			uniformCaches_[uniformName] = uniformCache;
			importedCount++;
		}
	}

	if (importedCount > UniformCachesHashSize)
		LOGW_X("More imported uniform blocks (%d) than hashmap buckets (%d)", importedCount, UniformCachesHashSize);
}

}
