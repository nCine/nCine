#include "GLShaderUniformBlocks.h"
#include "RenderResources.h"
#include <nctl/StaticHashMapIterator.h>
#include <cstring> // for memcpy()

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLUniformBlockCache GLShaderUniformBlocks::uniformBlockNotFound_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderUniformBlocks::GLShaderUniformBlocks()
    : shaderProgram_(nullptr), dataPointer_(nullptr)
{
}

GLShaderUniformBlocks::GLShaderUniformBlocks(GLShaderProgram *shaderProgram)
    : GLShaderUniformBlocks()
{
	setProgram(shaderProgram);
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

void GLShaderUniformBlocks::setProgram(GLShaderProgram *shaderProgram)
{
	ASSERT(shaderProgram);

	shaderProgram_ = shaderProgram;
	shaderProgram_->deferredQueries();
	uniformBlockCaches_.clear();

	if (shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
		importUniformBlocks();
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
	{
		uniformBlockCache = uniformBlockCaches_.find(name);

		if (uniformBlockCache == nullptr)
		{
			// Returning the dummy uniform cache to prevent the application from crashing
			uniformBlockCache = &uniformBlockNotFound_;
			LOGW_X("Uniform block \"%s\" not found in shader program %u", name, shaderProgram_->glHandle());
		}
	}
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

void GLShaderUniformBlocks::importUniformBlocks()
{
	const unsigned int count = shaderProgram_->uniformBlocks_.size();
	if (count > UniformBlockCachesHashSize)
		LOGW_X("More uniform blocks (%d) than hashmap buckets (%d)", count, UniformBlockCachesHashSize);

	for (GLUniformBlock &uniformBlock : shaderProgram_->uniformBlocks_)
	{
		GLUniformBlockCache uniformBlockCache(&uniformBlock);
		uniformBlockCaches_[uniformBlock.name()] = uniformBlockCache;
	}
}

}
