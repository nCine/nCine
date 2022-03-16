#include "GLShaderUniformBlocks.h"
#include "GLShaderProgram.h"
#include "RenderResources.h"
#include <nctl/StaticHashMapIterator.h>
#include <nctl/CString.h>
#include <cstring> // for memcpy()

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderUniformBlocks::GLShaderUniformBlocks()
    : shaderProgram_(nullptr), dataPointer_(nullptr)
{
}

GLShaderUniformBlocks::GLShaderUniformBlocks(GLShaderProgram *shaderProgram)
    : GLShaderUniformBlocks(shaderProgram, nullptr, nullptr)
{
}

GLShaderUniformBlocks::GLShaderUniformBlocks(GLShaderProgram *shaderProgram, const char *includeOnly, const char *exclude)
    : GLShaderUniformBlocks()
{
	setProgram(shaderProgram, includeOnly, exclude);
}

void GLShaderUniformBlocks::bind()
{
	if (uboParams_.object)
	{
		uboParams_.object->bind();

		GLintptr moreOffset = 0;
		for (GLUniformBlockCache &uniformBlockCache : uniformBlockCaches_)
		{
			uniformBlockCache.setBlockBinding(uniformBlockCache.index());
			uboParams_.object->bindBufferRange(uniformBlockCache.bindingIndex(), uboParams_.offset + moreOffset, uniformBlockCache.usedSize());
			moreOffset += uniformBlockCache.usedSize();
		}
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniformBlocks::setProgram(GLShaderProgram *shaderProgram, const char *includeOnly, const char *exclude)
{
	ASSERT(shaderProgram);

	shaderProgram_ = shaderProgram;
	shaderProgram_->deferredQueries();
	uniformBlockCaches_.clear();

	if (shaderProgram->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		importUniformBlocks(includeOnly, exclude);
}

void GLShaderUniformBlocks::setUniformsDataPointer(GLubyte *dataPointer)
{
	ASSERT(dataPointer);

	if (shaderProgram_->status() != GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		return;

	dataPointer_ = dataPointer;
	int offset = 0;
	for (GLUniformBlockCache &uniformBlockCache : uniformBlockCaches_)
	{
		uniformBlockCache.setDataPointer(dataPointer + offset);
		offset += uniformBlockCache.uniformBlock()->size();
	}
}

GLUniformBlockCache *GLShaderUniformBlocks::uniformBlock(const char *name)
{
	ASSERT(name);
	GLUniformBlockCache *uniformBlockCache = nullptr;

	if (shaderProgram_)
		uniformBlockCache = uniformBlockCaches_.find(name);
	else
		LOGE_X("Cannot find uniform block \"%s\", no shader program associated", name);

	return uniformBlockCache;
}

void GLShaderUniformBlocks::commitUniformBlocks()
{
	if (shaderProgram_)
	{
		if (shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		{
			int totalUsedSize = 0;
			for (GLUniformBlockCache &uniformBlockCache : uniformBlockCaches_)
				totalUsedSize += uniformBlockCache.usedSize();

			if (totalUsedSize > 0)
			{
				const RenderBuffersManager::BufferTypes::Enum bufferType = RenderBuffersManager::BufferTypes::UNIFORM;
				uboParams_ = RenderResources::buffersManager().acquireMemory(bufferType, totalUsedSize);
				if (uboParams_.mapBase)
					memcpy(uboParams_.mapBase + uboParams_.offset, dataPointer_, totalUsedSize);
			}
		}
	}
	else
		LOGE("No shader program associated");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniformBlocks::importUniformBlocks(const char *includeOnly, const char *exclude)
{
	const unsigned int MaxUniformBlockName = 128;

	unsigned int importedCount = 0;
	for (GLUniformBlock &uniformBlock : shaderProgram_->uniformBlocks_)
	{
		const char *uniformBlockName = uniformBlock.name();
		const char *currentIncludeOnly = includeOnly;
		const char *currentExclude = exclude;
		bool shouldImport = true;

		if (includeOnly != nullptr)
		{
			shouldImport = false;
			while (currentIncludeOnly != nullptr && currentIncludeOnly[0] != '\0')
			{
				if (strncmp(currentIncludeOnly, uniformBlockName, MaxUniformBlockName) == 0)
				{
					shouldImport = true;
					break;
				}
				currentIncludeOnly += nctl::strnlen(currentIncludeOnly, MaxUniformBlockName) + 1;
			}
		}

		if (exclude != nullptr)
		{
			while (currentExclude != nullptr && currentExclude[0] != '\0')
			{
				if (strncmp(currentExclude, uniformBlockName, MaxUniformBlockName) == 0)
				{
					shouldImport = false;
					break;
				}
				currentExclude += nctl::strnlen(currentExclude, MaxUniformBlockName) + 1;
			}
		}

		if (shouldImport)
		{
			GLUniformBlockCache uniformBlockCache(&uniformBlock);
			uniformBlockCaches_[uniformBlockName] = uniformBlockCache;
			importedCount++;
		}
	}

	if (importedCount > UniformBlockCachesHashSize)
		LOGW_X("More imported uniform blocks (%d) than hashmap buckets (%d)", importedCount, UniformBlockCachesHashSize);
}

}
