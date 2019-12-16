#include <cstring> // for memcpy()
#include "RenderBatcher.h"
#include "RenderResources.h" // TODO: Remove dependency?
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

unsigned int RenderBatcher::UboMaxSize = 0;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderBatcher::RenderBatcher()
    : buffers_(1), freeCommandsPool_(16), usedCommandsPool_(16)
{
	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	const int maxUniformBlockSize = gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE);

	// Clamping the value as some drivers report a maximum size similar to SSBO one
	UboMaxSize = maxUniformBlockSize <= 64 * 1024 ? maxUniformBlockSize : 64 * 1024;

	// Create the first buffer right away
	createBuffer(UboMaxSize);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	bool isSupportedType(Material::ShaderProgramType type)
	{
		return (type == Material::ShaderProgramType::SPRITE ||
		        type == Material::ShaderProgramType::SPRITE_GRAY ||
		        type == Material::ShaderProgramType::MESH_SPRITE ||
		        type == Material::ShaderProgramType::MESH_SPRITE_GRAY ||
		        type == Material::ShaderProgramType::TEXTNODE_ALPHA ||
		        type == Material::ShaderProgramType::TEXTNODE_RED);
	}

	bool isBatchedSprite(Material::ShaderProgramType type)
	{
		return (type == Material::ShaderProgramType::BATCHED_SPRITES ||
		        type == Material::ShaderProgramType::BATCHED_SPRITES_GRAY);
	}

	bool isBatchedMeshSprite(Material::ShaderProgramType type)
	{
		return (type == Material::ShaderProgramType::BATCHED_MESH_SPRITES ||
		        type == Material::ShaderProgramType::BATCHED_MESH_SPRITES_GRAY);
	}

	bool isBatchedTextnode(Material::ShaderProgramType type)
	{
		return (type == Material::ShaderProgramType::BATCHED_TEXTNODES_ALPHA ||
		        type == Material::ShaderProgramType::BATCHED_TEXTNODES_RED);
	}

}

void RenderBatcher::createBatches(const nctl::Array<RenderCommand *> &srcQueue, nctl::Array<RenderCommand *> &destQueue)
{
#ifdef __EMSCRIPTEN__
	const unsigned int minBatchSize = theApplication().appConfiguration().fixedBatchSize;
	const unsigned int maxBatchSize = theApplication().appConfiguration().fixedBatchSize;
#else
	const unsigned int minBatchSize = theApplication().renderingSettings().minBatchSize;
	const unsigned int maxBatchSize = theApplication().renderingSettings().maxBatchSize;
#endif

	unsigned int lastSplit = 0;

	for (unsigned int i = 1; i < srcQueue.size(); i++)
	{
		const RenderCommand *command = srcQueue[i];
		const Material::ShaderProgramType type = command->material().shaderProgramType();
		const GLTexture *texture = command->material().texture();
		const bool isBlendingEnabled = command->material().isBlendingEnabled();
		const GLenum srcBlendingFactor = command->material().srcBlendingFactor();
		const GLenum destBlendingFactor = command->material().destBlendingFactor();
		const GLenum primitive = command->geometry().primitiveType();

		const RenderCommand *prevCommand = srcQueue[i - 1];
		const Material::ShaderProgramType prevType = prevCommand->material().shaderProgramType();
		const GLTexture *prevTexture = prevCommand->material().texture();
		const bool prevIsBlendingEnabled = prevCommand->material().isBlendingEnabled();
		const GLenum prevSrcBlendingFactor = prevCommand->material().srcBlendingFactor();
		const GLenum prevDestBlendingFactor = prevCommand->material().destBlendingFactor();
		const GLenum prevPrimitive = prevCommand->geometry().primitiveType();

		// Always false for the opaque queue as blending is not enabled for any of the commands
		const bool blendingDiffers = isBlendingEnabled && prevIsBlendingEnabled &&
		                             (prevSrcBlendingFactor != srcBlendingFactor || prevDestBlendingFactor != destBlendingFactor);

		// Should split if the shader differs or if it's the same but texture, blending or primitive type aren't
		const bool shouldSplit = prevType != type || prevTexture != texture || prevPrimitive != primitive || blendingDiffers;

		// Also collect the very last command if it can be batched with the previous one
		unsigned int endSplit = (i == srcQueue.size() - 1 && !shouldSplit) ? i + 1 : i;

		const unsigned int batchSize = endSplit - lastSplit;
		// Split point if last command or split condition
		if (i == srcQueue.size() - 1 || shouldSplit || batchSize > maxBatchSize - 1)
		{
			if (isSupportedType(prevType) && batchSize >= minBatchSize)
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
	int singleInstanceBlockSize = 0;

	// Tracking the amount of memory required by uniform blocks, vertices and indices of all instances
	unsigned long instancesBlockSize = 0;
	unsigned long instancesVertexDataSize = 0;
	unsigned int instancesIndicesAmount = 0;

	if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::SPRITE)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_SPRITES);
		singleInstanceBlockSize = (*start)->material().uniformBlock("SpriteBlock")->size();
	}
	else if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::SPRITE_GRAY)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_SPRITES_GRAY);
		singleInstanceBlockSize = (*start)->material().uniformBlock("SpriteBlock")->size();
	}
	else if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::MESH_SPRITE)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_MESH_SPRITES);
		singleInstanceBlockSize = (*start)->material().uniformBlock("MeshSpriteBlock")->size();
	}
	else if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::MESH_SPRITE_GRAY)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_MESH_SPRITES_GRAY);
		singleInstanceBlockSize = (*start)->material().uniformBlock("MeshSpriteBlock")->size();
	}
	else if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::TEXTNODE_ALPHA)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_TEXTNODES_ALPHA);
		singleInstanceBlockSize = (*start)->material().uniformBlock("TextnodeBlock")->size();
	}
	else if (refCommand->material().shaderProgramType() == Material::ShaderProgramType::TEXTNODE_RED)
	{
		batchCommand = retrieveCommandFromPool(Material::ShaderProgramType::BATCHED_TEXTNODES_RED);
		singleInstanceBlockSize = (*start)->material().uniformBlock("TextnodeBlock")->size();
	}
	else
		FATAL_MSG("Unsupported shader for batch element");

	batchCommand->setType(refCommand->type());
	instancesBlock = batchCommand->material().uniformBlock("InstancesBlock");
	instancesBlockSize += batchCommand->material().shaderProgram()->uniformsSize();

	// Set to true if at least one command in the batch has indices or forced by a rendering settings
	bool batchingWithIndices = theApplication().renderingSettings().batchingWithIndices;
	// Sum the amount of UBO memory required by the batch and determine if indices are needed
	nctl::Array<RenderCommand *>::ConstIterator it = start;
	while (it != end)
	{
		if ((*it)->geometry().numIndices() > 0)
			batchingWithIndices = true;

		// Don't request more bytes than a UBO can hold
		if (instancesBlockSize + singleInstanceBlockSize > UboMaxSize)
			break;
		else
			instancesBlockSize += singleInstanceBlockSize;

		++it;
	}
	nextStart = it;

	const unsigned long maxVertexDataSize = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ARRAY).maxSize;
	const unsigned long maxIndexDataSize = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY).maxSize;
	// Sum the amount of VBO and IBO memory required by the batch
	it = start;
	while (it != nextStart)
	{
		unsigned int vertexDataSize = 0;
		unsigned int numIndices = (*it)->geometry().numIndices();

		if (refCommand->material().shaderProgramType() != Material::ShaderProgramType::SPRITE)
		{
			unsigned int numVertices = (*it)->geometry().numVertices();
			if (batchingWithIndices == false)
				numVertices += 2; // plus two degenerates if indices are not used
			const unsigned int numElementsPerVertex = (*it)->geometry().numElementsPerVertex() + 1; // plus the mesh index
			vertexDataSize = numVertices * numElementsPerVertex * sizeof(GLfloat);

			if (batchingWithIndices)
				numIndices = (numIndices > 0) ? numIndices + 2 : numVertices + 2;
		}

		// Don't request more bytes than a common VBO or IBO can hold
		if (instancesVertexDataSize + vertexDataSize > maxVertexDataSize ||
		    (instancesIndicesAmount + numIndices) * sizeof(GLushort) > maxIndexDataSize ||
		    instancesIndicesAmount + numIndices > 65535)
			break;
		else
		{
			instancesVertexDataSize += vertexDataSize;
			instancesIndicesAmount += numIndices;
		}

		++it;
	}
	nextStart = it;

	// Remove the two missing degenerate vertices or indices from first and last elements
	if (instancesIndicesAmount > 0)
		instancesIndicesAmount -= 2;
	else
		instancesVertexDataSize -= 2 * (refCommand->geometry().numElementsPerVertex() + 1) * sizeof(GLfloat);

	batchCommand->material().setUniformsDataPointer(acquireMemory(instancesBlockSize));
	batchCommand->material().uniform("uTexture")->setIntValue(0); // GL_TEXTURE0
	batchCommand->material().uniform("projection")->setFloatVector(RenderResources::projectionMatrix().data());

	RenderResources::VertexFormatPos2Tex2Index *destVtx = nullptr;
	GLushort *destIdx = nullptr;
	if (isBatchedSprite(batchCommand->material().shaderProgramType()) == false)
	{
		const unsigned int numFloats = instancesVertexDataSize / sizeof(GLfloat);
		const unsigned int numFloatsAlignment = sizeof(RenderResources::VertexFormatPos2Tex2Index) / sizeof(GLfloat);
		destVtx = reinterpret_cast<RenderResources::VertexFormatPos2Tex2Index *>(batchCommand->geometry().acquireVertexPointer(numFloats, numFloatsAlignment));

		if (instancesIndicesAmount > 0)
			destIdx = batchCommand->geometry().acquireIndexPointer(instancesIndicesAmount);
	}

	it = start;
	unsigned int instancesBlockOffset = 0;
	unsigned short batchFirstVertexId = 0;
	while (it != nextStart)
	{
		RenderCommand *command = *it;
		command->commitTransformation();

		if (isBatchedSprite(batchCommand->material().shaderProgramType()))
		{
			const GLUniformBlockCache *singleInstanceBlock = command->material().uniformBlock("SpriteBlock");
			memcpy(instancesBlock->dataPointer() + instancesBlockOffset, singleInstanceBlock->dataPointer(), singleInstanceBlockSize);
			instancesBlockOffset += singleInstanceBlockSize;
		}
		else
		{
			GLUniformBlockCache *singleInstanceBlock = nullptr;
			if (isBatchedMeshSprite(batchCommand->material().shaderProgramType()))
				singleInstanceBlock = command->material().uniformBlock("MeshSpriteBlock");
			else if (isBatchedTextnode(batchCommand->material().shaderProgramType()))
				singleInstanceBlock = command->material().uniformBlock("TextnodeBlock");

			memcpy(instancesBlock->dataPointer() + instancesBlockOffset, singleInstanceBlock->dataPointer(), singleInstanceBlockSize);
			instancesBlockOffset += singleInstanceBlockSize;

			const unsigned int numVertices = command->geometry().numVertices();
			const int meshIndex = it - start;
			const RenderResources::VertexFormatPos2Tex2 *srcVtx = reinterpret_cast<const RenderResources::VertexFormatPos2Tex2 *>(command->geometry().hostVertexPointer());
			FATAL_ASSERT(srcVtx != nullptr);

			// Vertex of a degenerate triangle, if not a starting element and there are more than one in the batch
			if (it != start && nextStart - start > 1 && !batchingWithIndices)
			{
				memcpy(destVtx, srcVtx, sizeof(RenderResources::VertexFormatPos2Tex2));
				destVtx->drawindex = meshIndex;
				destVtx++;
			}
			for (unsigned int i = 0; i < numVertices; i++)
			{
				memcpy(destVtx, srcVtx, sizeof(RenderResources::VertexFormatPos2Tex2));
				destVtx->drawindex = meshIndex;
				destVtx++;
				srcVtx++;
			}
			// Vertex of a degenerate triangle, if not an ending element and there are more than one in the batch
			if (it != nextStart - 1 && nextStart - start > 1 && !batchingWithIndices)
			{
				srcVtx--;
				memcpy(destVtx, srcVtx, sizeof(RenderResources::VertexFormatPos2Tex2));
				destVtx->drawindex = meshIndex;
				destVtx++;
			}

			unsigned short vertexId = 0;
			if (instancesIndicesAmount > 0)
			{
				const unsigned int numIndices = command->geometry().numIndices() ? command->geometry().numIndices() : numVertices;
				const GLushort *srcIdx = command->geometry().hostIndexPointer();

				// Index of a degenerate triangle, if not a starting element and there are more than one in the batch
				if (it != start && nextStart - start > 1)
				{
					*destIdx = batchFirstVertexId + (srcIdx ? *srcIdx : vertexId);
					destIdx++;
				}
				for (unsigned int i = 0; i < numIndices; i++)
				{
					*destIdx = batchFirstVertexId + (srcIdx ? *srcIdx : vertexId);
					destIdx++;
					vertexId++;
					if (srcIdx)
						srcIdx++;
				}
				// Index of a degenerate triangle, if not an ending element and there are more than one in the batch
				if (it != nextStart - 1 && nextStart - start > 1)
				{
					if (srcIdx)
						srcIdx--;
					*destIdx = batchFirstVertexId + (srcIdx ? *srcIdx : vertexId - 1);
					destIdx++;
				}

				batchFirstVertexId += srcIdx ? numVertices : vertexId;
			}
		}

		++it;
	}

	if (isBatchedSprite(batchCommand->material().shaderProgramType()) == false)
	{
		batchCommand->geometry().releaseVertexPointer();
		if (destIdx)
			batchCommand->geometry().releaseIndexPointer();
	}

	batchCommand->material().setTexture(refCommand->material().texture());
	batchCommand->material().setBlendingEnabled(refCommand->material().isBlendingEnabled());
	batchCommand->material().setBlendingFactors(refCommand->material().srcBlendingFactor(), refCommand->material().destBlendingFactor());
	batchCommand->setBatchSize(nextStart - start);
	batchCommand->material().uniformBlock("InstancesBlock")->setUsedSize(instancesBlockOffset);

	if (isBatchedSprite(batchCommand->material().shaderProgramType()))
		batchCommand->geometry().setDrawParameters(GL_TRIANGLES, 0, 6 * (nextStart - start));
	else
	{
		const unsigned int totalVertices = instancesVertexDataSize / sizeof(RenderResources::VertexFormatPos2Tex2Index);
		batchCommand->geometry().setDrawParameters(refCommand->geometry().primitiveType(), 0, totalVertices);
		batchCommand->geometry().setNumElementsPerVertex(sizeof(RenderResources::VertexFormatPos2Tex2Index) / sizeof(GLfloat));
		batchCommand->geometry().setNumIndices(instancesIndicesAmount);
	}

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
	FATAL_ASSERT(bytes <= UboMaxSize);

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
		createBuffer(UboMaxSize);
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
	managedBuffer.buffer = nctl::makeUnique<unsigned char[]>(size);

	buffers_.pushBack(nctl::move(managedBuffer));
}

}
