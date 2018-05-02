#include <cstring> // for memcpy()
#include "RenderBatcher.h"
#include "RenderResources.h" // TODO: Remove dependency?
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

unsigned int RenderBatcher::MaxUniformBlockSize = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderBatcher::RenderBatcher()
	: buffers_(1), freeCommandsPool_(16), usedCommandsPool_(16)
{
	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	MaxUniformBlockSize = gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE);

	// Create the first buffer right away
	createBuffer(MaxUniformBlockSize);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderBatcher::createBatches(const nctl::Array<RenderCommand *> &srcQueue, nctl::Array<RenderCommand *> &destQueue)
{
	const unsigned int minBatchSize = theApplication().renderingSettings().minBatchSize;
	const unsigned int maxBatchSize = theApplication().renderingSettings().maxBatchSize;

	unsigned int lastSplit = 0;

	for (unsigned int i = 1; i < srcQueue.size(); i++)
	{
		const RenderCommand *command = srcQueue[i];
		const Material::ShaderProgramType type = command->material().shaderProgramType();
		const GLTexture *texture = command->material().texture();
		const GLenum primitive = command->geometry().primitiveType();

		const RenderCommand *prevCommand = srcQueue[i - 1];
		const Material::ShaderProgramType prevType = prevCommand->material().shaderProgramType();
		const GLTexture *prevTexture = prevCommand->material().texture();
		const GLenum prevPrimitive = prevCommand->geometry().primitiveType();

		// Should split if the shader differs or if it's the same but texture or primitive type aren't
		const bool shouldSplit = prevType != type || prevTexture != texture || prevPrimitive != primitive;

		// Also collect the very last command if it can be batched with the previous one
		unsigned int endSplit = (i == srcQueue.size() - 1 && !shouldSplit) ? i + 1 : i;

		const unsigned int batchSize = endSplit - lastSplit;
		// Split point if last command or split condition
		if (i == srcQueue.size() - 1 || shouldSplit || batchSize > maxBatchSize)
		{
			if (prevType == Material::ShaderProgramType::SPRITE && batchSize >= minBatchSize)
			{
				nctl::Array<RenderCommand *>::ConstIterator start = srcQueue.cBegin() + lastSplit;
				nctl::Array<RenderCommand *>::ConstIterator end = srcQueue.cBegin() + endSplit;
				while (start != end)
				{
					// Handling early splits while collecting (not enough UBO free space)
					RenderCommand *batchCommand = collectCommands(start, end, start);
					destQueue.pushBack(batchCommand);
				}

				// If the very last command can't be part of this batch, it has to passthrough now
				if (i == srcQueue.size() - 1 && shouldSplit)
					destQueue.pushBack(srcQueue[i]);
			}
			else
			{
				// Also collect the very last command
				endSplit = (i == srcQueue.size() - 1) ? i + 1 : i;

				// Passthrough for unsupported types
				for (unsigned int j = lastSplit; j < endSplit; j++)
					destQueue.pushBack(srcQueue[j]);
			}
			lastSplit = endSplit;
		}
	}

	// If the queue has only one command the for loop didn't execute, the command has to passthrough
	if (srcQueue.size() == 1)
		destQueue.pushBack(srcQueue[0]);
}

void RenderBatcher::reset()
{
	for (nctl::UniquePtr<RenderCommand> &command : usedCommandsPool_)
		freeCommandsPool_.pushBack(nctl::move(command));
	usedCommandsPool_.clear();

	// Reset managed buffers
	for (ManagedBuffer &buffer : buffers_)
		buffer.freeSpace = buffer.size;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

RenderCommand *RenderBatcher::collectCommands(
	nctl::Array<RenderCommand *>::ConstIterator start,
	nctl::Array<RenderCommand *>::ConstIterator end,
	nctl::Array<RenderCommand *>::ConstIterator &nextStart)
{
	ASSERT(end > start);

	const RenderCommand *refCommand = *start;
	RenderCommand *batchCommand = nullptr;
	GLUniformBlockCache *instancesBlock = nullptr;
	int spriteBlockSize = 0;

	// Tracking the amount of memory required by the instances uniform blocks
	unsigned long instancesBlockSize = 0;

	if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::SPRITE)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_SPRITES);
		instancesBlock = batchCommand->material().uniformBlock("InstancesBlock");
		instancesBlockSize += batchCommand->material().shaderProgram()->uniformsSize();

		batchCommand->setType(RenderCommand::CommandTypes::SPRITE);
		spriteBlockSize = (*start)->material().uniformBlock("SpriteBlock")->size();
	}
	else
		FATAL_MSG("Unsupported shader for batch element");

	nctl::Array<RenderCommand *>::ConstIterator it = start;
	while (it != end)
	{
		// Don't request more bytes than a UBO can hold
		if (instancesBlockSize + spriteBlockSize > MaxUniformBlockSize)
			break;

		instancesBlockSize += spriteBlockSize;
		++it;
	}
	nextStart = it;

	batchCommand->material().setUniformsDataPointer(acquireMemory(instancesBlockSize));
	batchCommand->material().uniform("uTexture")->setIntValue(0); // GL_TEXTURE0
	batchCommand->material().uniform("projection")->setFloatVector(RenderResources::projectionMatrix().data());

	it = start;
	unsigned int instancesBlockOffset = 0;
	while (it != nextStart)
	{
		RenderCommand *command = *it;
		command->commitTransformation();

		const GLUniformBlockCache *spriteBlock = command->material().uniformBlock("SpriteBlock");
		memcpy(instancesBlock->dataPointer() + instancesBlockOffset, spriteBlock->dataPointer(), spriteBlockSize);
		instancesBlockOffset += spriteBlockSize;

		++it;
	}

	batchCommand->material().setTexture(refCommand->material().texture());
	batchCommand->material().setTransparent(refCommand->material().isTransparent());
	batchCommand->geometry().setDrawParameters(GL_TRIANGLES, 0, 6 * (nextStart - start));
	batchCommand->setBatchSize(nextStart - start);
	batchCommand->material().uniformBlock("InstancesBlock")->setUsedSize(instancesBlockOffset);

	return batchCommand;
}

RenderCommand *RenderBatcher::retrieveCommandFromPool(Material::ShaderProgramType shaderProgramType)
{
	RenderCommand *retrievedCommand = nullptr;

	for (unsigned int i = 0; i < freeCommandsPool_.size(); i++)
	{
		const unsigned int poolSize = freeCommandsPool_.size();
		nctl::UniquePtr<RenderCommand> &command = freeCommandsPool_[i];
		if (command && command->material().shaderProgramType() == shaderProgramType)
		{
			retrievedCommand = command.get();
			usedCommandsPool_.pushBack(nctl::move(command));
			command = nctl::move(freeCommandsPool_[poolSize - 1]);
			freeCommandsPool_.setSize(poolSize - 1);
			break;
		}
	}

	if (retrievedCommand == nullptr)
	{
		nctl::UniquePtr<RenderCommand> newCommand = nctl::makeUnique<RenderCommand>();
		newCommand->material().setShaderProgramType(shaderProgramType);
		retrievedCommand = newCommand.get();
		usedCommandsPool_.pushBack(nctl::move(newCommand));
	}

	return retrievedCommand;
}

unsigned char *RenderBatcher::acquireMemory(unsigned int bytes)
{
	FATAL_ASSERT(bytes <= MaxUniformBlockSize);

	unsigned char *ptr = nullptr;

	for (ManagedBuffer &buffer : buffers_)
	{
		if (buffer.freeSpace >= bytes)
		{
			const unsigned int offset = buffer.size - buffer.freeSpace;
			ptr = buffer.buffer.get() + offset;
			buffer.freeSpace -= bytes;
			break;
		}
	}

	if (ptr == nullptr)
	{
		createBuffer(MaxUniformBlockSize);
		ptr = buffers_.back().buffer.get();
		buffers_.back().freeSpace -= bytes;
	}

	return ptr;
}

void RenderBatcher::createBuffer(unsigned int size)
{
	ManagedBuffer managedBuffer;
	managedBuffer.size = size;
	managedBuffer.freeSpace = size;
	managedBuffer.buffer = nctl::makeUnique<unsigned char []>(size);

	buffers_.pushBack(nctl::move(managedBuffer));
}

}
