#include <cstring> // for memcpy()
#include "GLShaderProgram.h"
#include "RenderBatcher.h"
#include "RenderCommand.h"
#include "RenderCommandPool.h"
#include "RenderResources.h"
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
    : buffers_(1)
{
	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	const unsigned int maxUniformBlockSize = static_cast<unsigned int>(gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE));

	// Clamping the value as some drivers report a maximum size similar to SSBO one
	UboMaxSize = maxUniformBlockSize <= 64 * 1024 ? maxUniformBlockSize : 64 * 1024;

	// Create the first buffer right away
	createBuffer(UboMaxSize);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderBatcher::createBatches(const nctl::Array<RenderCommand *> &srcQueue, nctl::Array<RenderCommand *> &destQueue)
{
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	const unsigned int fixedBatchSize = theApplication().appConfiguration().fixedBatchSize;
	const unsigned int minBatchSize = (fixedBatchSize > 0) ? fixedBatchSize : theApplication().renderingSettings().minBatchSize;
	const unsigned int maxBatchSize = (fixedBatchSize > 0) ? fixedBatchSize : theApplication().renderingSettings().maxBatchSize;
#else
	const unsigned int minBatchSize = theApplication().renderingSettings().minBatchSize;
	const unsigned int maxBatchSize = theApplication().renderingSettings().maxBatchSize;
#endif
	ASSERT(minBatchSize > 1);
	ASSERT(maxBatchSize >= minBatchSize);

	unsigned int lastSplit = 0;

	for (unsigned int i = 1; i < srcQueue.size(); i++)
	{
		const RenderCommand *command = srcQueue[i];
		const GLShaderProgram *shaderProgram = command->material().shaderProgram();
		const GLTexture *texture = command->material().texture();
		const bool isBlendingEnabled = command->material().isBlendingEnabled();
		const GLenum srcBlendingFactor = command->material().srcBlendingFactor();
		const GLenum destBlendingFactor = command->material().destBlendingFactor();
		const GLenum primitive = command->geometry().primitiveType();

		const RenderCommand *prevCommand = srcQueue[i - 1];
		const GLShaderProgram *prevShaderProgram = prevCommand->material().shaderProgram();
		const GLTexture *prevTexture = prevCommand->material().texture();
		const bool prevIsBlendingEnabled = prevCommand->material().isBlendingEnabled();
		const GLenum prevSrcBlendingFactor = prevCommand->material().srcBlendingFactor();
		const GLenum prevDestBlendingFactor = prevCommand->material().destBlendingFactor();
		const GLenum prevPrimitive = prevCommand->geometry().primitiveType();

		// Always false for the opaque queue as blending is not enabled for any of the commands
		const bool blendingDiffers = isBlendingEnabled && prevIsBlendingEnabled &&
		                             (prevSrcBlendingFactor != srcBlendingFactor || prevDestBlendingFactor != destBlendingFactor);

		// Should split if the shader differs or if it's the same but texture, blending or primitive type aren't
		const bool shouldSplit = prevShaderProgram != shaderProgram || prevTexture != texture || prevPrimitive != primitive || blendingDiffers;

		// Also collect the very last command if it can be batched with the previous one
		unsigned int endSplit = (i == srcQueue.size() - 1 && !shouldSplit) ? i + 1 : i;

		// Split point if last command or split condition
		if (i == srcQueue.size() - 1 || shouldSplit)
		{
			const GLShaderProgram *batchedShader = RenderResources::batchedShader(prevCommand->material().shaderProgram());
			if (batchedShader && (endSplit - lastSplit) >= minBatchSize)
			{
				// Split point for the maximum batch size
				while (lastSplit < endSplit)
				{
					const unsigned int batchSize = endSplit - lastSplit;
					unsigned int nextSplit = endSplit;
					if (batchSize > maxBatchSize)
						nextSplit = lastSplit + maxBatchSize;
					else if (batchSize < minBatchSize)
						break;

					nctl::Array<RenderCommand *>::ConstIterator start = srcQueue.cBegin() + lastSplit;
					nctl::Array<RenderCommand *>::ConstIterator end = srcQueue.cBegin() + nextSplit;

					// Handling early splits while collecting (not enough UBO free space)
					RenderCommand *batchCommand = collectCommands(start, end, start);
					destQueue.pushBack(batchCommand);
					lastSplit = start - srcQueue.cBegin();
				}
			}

			// Also collect the very last command
			endSplit = (i == srcQueue.size() - 1) ? i + 1 : i;

			// Passthrough for unsupported command types and for the last few commands that are less than the minimum batch size
			for (unsigned int j = lastSplit; j < endSplit; j++)
				destQueue.pushBack(srcQueue[j]);

			lastSplit = endSplit;
		}
	}

	// If the queue has only one command the for loop didn't execute, the command has to passthrough
	if (srcQueue.size() == 1)
		destQueue.pushBack(srcQueue[0]);
}

void RenderBatcher::reset()
{
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

	GLShaderProgram *batchedShader = RenderResources::batchedShader(refCommand->material().shaderProgram());
	FATAL_ASSERT_MSG(batchedShader != nullptr, "Unsupported shader for batch element");
	bool commandAdded = false;
	batchCommand = RenderResources::renderCommandPool().retrieveOrAdd(batchedShader, commandAdded);

	const GLShaderProgram *refShader = refCommand->material().shaderProgram();
	const bool isSprite = (refShader->hasAttribute(Material::PositionAttributeName) == false);
	const bool isMeshSpriteNoTexture = refShader->hasAttribute(Material::PositionAttributeName) && (refShader->hasAttribute(Material::TexCoordsAttributeName) == false);
	const bool isBatchedSprite = (batchedShader->hasAttribute(Material::PositionAttributeName) == false) && batchCommand->material().uniformBlock(Material::InstancesBlockName);

	singleInstanceBlockSize = (*start)->material().uniformBlock(Material::InstanceBlockName)->size();

	if (commandAdded)
		batchCommand->setType(refCommand->type());
	instancesBlock = batchCommand->material().uniformBlock(Material::InstancesBlockName);
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

		if (isSprite == false)
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
	const unsigned long twoVerticesDataSize = 2 * (refCommand->geometry().numElementsPerVertex() + 1) * sizeof(GLfloat);
	if (instancesIndicesAmount >= 2)
		instancesIndicesAmount -= 2;
	else if (instancesVertexDataSize >= twoVerticesDataSize)
		instancesVertexDataSize -= twoVerticesDataSize;

	batchCommand->material().setUniformsDataPointer(acquireMemory(instancesBlockSize));
	if (commandAdded && refShader->hasAttribute(Material::TexCoordsAttributeName))
		batchCommand->material().uniform(Material::TextureUniformName)->setIntValue(0); // GL_TEXTURE0

	const unsigned int SizeVertexFormat = ((isMeshSpriteNoTexture == false)
	                                           ? sizeof(RenderResources::VertexFormatPos2Tex2)
	                                           : sizeof(RenderResources::VertexFormatPos2));
	const unsigned int SizeVertexFormatAndIndex = SizeVertexFormat + sizeof(int);
	const unsigned int NumFloatsVertexFormat = SizeVertexFormat / sizeof(GLfloat);
	const unsigned int NumFloatsVertexFormatAndIndex = NumFloatsVertexFormat + 1; // index is an `int`, same size as a `float`

	float *destVtx = nullptr;
	GLushort *destIdx = nullptr;

	if (isBatchedSprite == false)
	{
		const unsigned int numFloats = instancesVertexDataSize / sizeof(GLfloat);
		destVtx = batchCommand->geometry().acquireVertexPointer(numFloats, NumFloatsVertexFormat + 1); // aligned to vertex format with index

		if (instancesIndicesAmount > 0)
			destIdx = batchCommand->geometry().acquireIndexPointer(instancesIndicesAmount);
	}

	it = start;
	unsigned int instancesBlockOffset = 0;
	unsigned short batchFirstVertexId = 0;
	while (it != nextStart)
	{
		RenderCommand *command = *it;
		command->commitNodeTransformation();

		if (isBatchedSprite)
		{
			const GLUniformBlockCache *singleInstanceBlock = command->material().uniformBlock(Material::InstanceBlockName);
			memcpy(instancesBlock->dataPointer() + instancesBlockOffset, singleInstanceBlock->dataPointer(), singleInstanceBlockSize);
			instancesBlockOffset += singleInstanceBlockSize;
		}
		else
		{
			GLUniformBlockCache *singleInstanceBlock = nullptr;
			singleInstanceBlock = command->material().uniformBlock(Material::InstanceBlockName);
			memcpy(instancesBlock->dataPointer() + instancesBlockOffset, singleInstanceBlock->dataPointer(), singleInstanceBlockSize);
			instancesBlockOffset += singleInstanceBlockSize;

			const unsigned int numVertices = command->geometry().numVertices();
			const int meshIndex = it - start;
			const float *srcVtx = command->geometry().hostVertexPointer();
			FATAL_ASSERT(srcVtx != nullptr);

			// Vertex of a degenerate triangle, if not a starting element and there are more than one in the batch
			if (it != start && nextStart - start > 1 && !batchingWithIndices)
			{
				memcpy(destVtx, srcVtx, SizeVertexFormat);
				*reinterpret_cast<int *>(static_cast<void *>(&destVtx[NumFloatsVertexFormat])) = meshIndex; // last element is the index
				destVtx += NumFloatsVertexFormatAndIndex;
			}
			for (unsigned int i = 0; i < numVertices; i++)
			{
				memcpy(destVtx, srcVtx, SizeVertexFormat);
				*reinterpret_cast<int *>(static_cast<void *>(&destVtx[NumFloatsVertexFormat])) = meshIndex; // last element is the index
				destVtx += NumFloatsVertexFormatAndIndex;
				srcVtx += NumFloatsVertexFormat; // source format does not include an index
			}
			// Vertex of a degenerate triangle, if not an ending element and there are more than one in the batch
			if (it != nextStart - 1 && nextStart - start > 1 && !batchingWithIndices)
			{
				srcVtx -= NumFloatsVertexFormat;
				memcpy(destVtx, srcVtx, SizeVertexFormat);
				*reinterpret_cast<int *>(static_cast<void *>(&destVtx[NumFloatsVertexFormat])) = meshIndex; // last element is the index
				destVtx += NumFloatsVertexFormatAndIndex;
			}

			if (instancesIndicesAmount > 0)
			{
				unsigned short vertexId = 0;
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

	if (isBatchedSprite == false)
	{
		batchCommand->geometry().releaseVertexPointer();
		if (destIdx)
			batchCommand->geometry().releaseIndexPointer();
	}

	batchCommand->material().setTexture(refCommand->material().texture());
	batchCommand->material().setBlendingEnabled(refCommand->material().isBlendingEnabled());
	batchCommand->material().setBlendingFactors(refCommand->material().srcBlendingFactor(), refCommand->material().destBlendingFactor());
	batchCommand->setBatchSize(nextStart - start);
	batchCommand->material().uniformBlock(Material::InstancesBlockName)->setUsedSize(instancesBlockOffset);
	batchCommand->setLayer(refCommand->layer());
	batchCommand->setVisitOrder(refCommand->visitOrder());

	if (isBatchedSprite)
		batchCommand->geometry().setDrawParameters(GL_TRIANGLES, 0, 6 * (nextStart - start));
	else
	{
		const unsigned int totalVertices = instancesVertexDataSize / SizeVertexFormatAndIndex;
		batchCommand->geometry().setDrawParameters(refCommand->geometry().primitiveType(), 0, totalVertices);
		batchCommand->geometry().setNumElementsPerVertex(NumFloatsVertexFormatAndIndex);
		batchCommand->geometry().setNumIndices(instancesIndicesAmount);
	}

	return batchCommand;
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
