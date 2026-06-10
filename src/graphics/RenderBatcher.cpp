#include <cstring> // for memcpy()
#include "GLShaderProgram.h"
#include "RenderBatcher.h"
#include "RenderCommand.h"
#include "RenderCommandPool.h"
#include "RenderResources.h"
#include "Application.h"
#include <nctl/StaticHashMapIterator.h>

#include "tracy.h"

#ifdef WITH_TRACY
	#include <nctl/StaticString.h>
	nctl::StaticString<64> zoneTextString;
#endif

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
	// Clamped between 16 KB and 64 KB
	UboMaxSize = static_cast<unsigned int>(gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE));

	// Create the first buffer right away
	createBuffer(UboMaxSize);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderBatcher::createBatches(const nctl::Array<RenderCommand *> &srcQueue, nctl::Array<RenderCommand *> &destQueue)
{
	ZoneScoped;

	unsigned int &maxBatchSize = theApplication().renderingSettings().maxBatchSize;
	unsigned int &minBatchSize = theApplication().renderingSettings().minBatchSize;
	const bool instancingEnabled = theApplication().renderingSettings().instancingEnabled;
	unsigned int &maxInstancedBatchSize = theApplication().renderingSettings().maxInstancedBatchSize;
	unsigned int &minInstancedBatchSize = theApplication().renderingSettings().minInstancedBatchSize;

#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	const unsigned int fixedBatchSize = theApplication().appConfiguration().graphics.opengl.fixedBatchSize;
	if (fixedBatchSize > 0)
		maxBatchSize = fixedBatchSize;
#endif

#if defined(__EMSCRIPTEN__)
	// On Emscripten (but not on ANGLE), batch size needs to be fixed: min and max must be the same
	minBatchSize = maxBatchSize;
#endif

	// Validating batch size values
	maxBatchSize = (maxBatchSize == 0) ? 1 : maxBatchSize;
	minBatchSize = (minBatchSize == 0) ? 1 : minBatchSize;
	minBatchSize = (minBatchSize > maxBatchSize) ? maxBatchSize : minBatchSize;
	// Validating instanced batch size values
	maxInstancedBatchSize = (maxInstancedBatchSize == 0) ? 1 : maxInstancedBatchSize;
	minInstancedBatchSize = (minInstancedBatchSize == 0) ? 1 : minInstancedBatchSize;
	minInstancedBatchSize = (minInstancedBatchSize > maxInstancedBatchSize) ? maxInstancedBatchSize : minInstancedBatchSize;

	unsigned int lastSplit = 0;
	for (unsigned int i = 1; i < srcQueue.size(); i++)
	{
		const RenderCommand *command = srcQueue[i];
		const GLenum primitive = command->geometry().primitiveType();

		const RenderCommand *prevCommand = srcQueue[i - 1];
		const GLenum prevPrimitive = prevCommand->geometry().primitiveType();

		// Should split if the lower part of a material's sort key or the primitive type differ
		const bool shouldSplit = command->lowerMaterialSortKey() != prevCommand->lowerMaterialSortKey() || prevPrimitive != primitive;

		// Also collect the very last command if it can be batched with the previous one
		unsigned int endSplit = (i == srcQueue.size() - 1 && !shouldSplit) ? i + 1 : i;

		// Split point if last command or split condition
		if (i == srcQueue.size() - 1 || shouldSplit)
		{
			nctl::Array<RenderCommand *>::ConstIterator start = srcQueue.cBegin() + lastSplit;
			const bool canUseInstancing = ((*start)->type() == RenderCommand::CommandTypes::SPRITE &&
			                               (*start)->material().shaderProgramType() != Material::ShaderProgramType::CUSTOM);
			const bool useInstancing = (canUseInstancing && instancingEnabled);

			const unsigned int minBatchSizeValue = useInstancing ? minInstancedBatchSize : minBatchSize;
			const unsigned int maxBatchSizeValue = useInstancing ? maxInstancedBatchSize : maxBatchSize;

			const GLShaderProgram *batchedShader = RenderResources::batchedShader(prevCommand->material().shaderProgram());
			if (batchedShader && (endSplit - lastSplit) >= minBatchSizeValue)
			{
				// Split point for the maximum batch size
				while (lastSplit < endSplit)
				{
					const unsigned int batchSize = endSplit - lastSplit;
					unsigned int nextSplit = endSplit;
					if (batchSize > maxBatchSizeValue)
						nextSplit = lastSplit + maxBatchSizeValue;
					else if (batchSize < minBatchSizeValue)
						break;

					nctl::Array<RenderCommand *>::ConstIterator end = srcQueue.cBegin() + nextSplit;

					RenderCommand *batchCommand = nullptr;
					if (useInstancing)
						batchCommand = collectCommandsWithInstancing(start, end, start);
					else
					{
						// Handling early splits while collecting (not enough UBO free space)
						batchCommand = collectCommandsWithUniforms(start, end, start);
					}
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

RenderCommand *RenderBatcher::collectCommandsWithUniforms(
    nctl::Array<RenderCommand *>::ConstIterator start,
    nctl::Array<RenderCommand *>::ConstIterator end,
    nctl::Array<RenderCommand *>::ConstIterator &nextStart)
{
	ASSERT(end > start);
	ZoneScoped;
#ifdef WITH_TRACY
	zoneTextString.format("Batch size: %u", end - start);
	ZoneText(zoneTextString.data(), zoneTextString.length());
#endif

	const RenderCommand *refCommand = *start;
	RenderCommand *batchCommand = nullptr;
	GLUniformBlockCache *instancesBlock = nullptr;

	// Tracking the amount of memory required by uniform blocks, vertices and indices of all instances
	unsigned long instancesBlockSize = 0;
	unsigned long instancesVertexDataSize = 0;
	unsigned int instancesIndicesAmount = 0;

	const GLShaderProgram *refShader = refCommand->material().shaderProgram();
	GLShaderProgram *batchedShader = RenderResources::batchedShader(refShader);
	// The following check should never fail as it is already checked by the calling function
	FATAL_ASSERT_MSG(batchedShader != nullptr, "Unsupported shader for batch element");
	bool commandAdded = false;
	batchCommand = RenderResources::renderCommandPool().retrieveOrAdd(batchedShader, commandAdded);

	// Retrieving the original block instance size without the uniform buffer offset alignment
	const GLUniformBlockCache *singleInstanceBlock = (*start)->material().uniformBlock(Material::InstanceBlockName);
	const int singleInstanceBlockSizePacked = singleInstanceBlock->size() - singleInstanceBlock->alignAmount(); // remove the uniform buffer offset alignment
	const int singleInstanceBlockSize = singleInstanceBlockSizePacked + (16 - singleInstanceBlockSizePacked % 16) % 16; // but add the std140 vec4 layout alignment

	if (commandAdded)
		batchCommand->setType(refCommand->type());
	instancesBlock = batchCommand->material().uniformBlock(Material::InstancesBlockName);
	FATAL_ASSERT_MSG_X(instancesBlock != nullptr, "Batched shader does not have an \"%s\" uniform block", Material::InstancesBlockName);

	const unsigned long nonBlockUniformsSize = batchCommand->material().shaderProgram()->uniformsSize();
	nctl::StaticString<GLUniformBlock::MaxNameLength> uniformBlockName;
	// Determine how much memory is needed by uniform blocks that are not for instances
	unsigned long nonInstancesBlocksSize = 0;
	const GLShaderUniformBlocks::UniformHashMapType allUniformBlocks = refCommand->material().allUniformBlocks();
	for (const GLUniformBlockCache &uniformBlockCache : allUniformBlocks)
	{
		uniformBlockName = uniformBlockCache.uniformBlock()->name();
		if (uniformBlockName == Material::InstanceBlockName)
			continue;

		GLUniformBlockCache *batchBlock = batchCommand->material().uniformBlock(uniformBlockName.data());
		ASSERT(batchBlock);
		if (batchBlock)
			nonInstancesBlocksSize += uniformBlockCache.size() - uniformBlockCache.alignAmount();
	}

	// Set to true if at least one command in the batch has indices or forced by a rendering settings
	bool batchingWithIndices = theApplication().renderingSettings().batchingWithIndices;
	// Sum the amount of UBO memory required by the batch and determine if indices are needed
	nctl::Array<RenderCommand *>::ConstIterator it = start;
	while (it != end)
	{
		if ((*it)->geometry().numIndices() > 0)
			batchingWithIndices = true;

		// Don't request more bytes than an instances block or an UBO can hold (also protects against big `RenderingSettings::maxBatchSize` values)
		const unsigned long currentSize = nonBlockUniformsSize + nonInstancesBlocksSize + instancesBlockSize;
		if (instancesBlockSize + singleInstanceBlockSize > instancesBlock->size() || currentSize + singleInstanceBlockSize > UboMaxSize)
			break;
		else
			instancesBlockSize += singleInstanceBlockSize;

		++it;
	}
	nextStart = it;

	batchCommand->material().setUniformsDataPointer(acquireMemory(nonBlockUniformsSize + nonInstancesBlocksSize + instancesBlockSize));
	// Copying data for non-instances uniform blocks from the first command in the batch
	for (const GLUniformBlockCache &uniformBlockCache : allUniformBlocks)
	{
		uniformBlockName = uniformBlockCache.uniformBlock()->name();
		if (uniformBlockName == Material::InstanceBlockName)
			continue;

		GLUniformBlockCache *batchBlock = batchCommand->material().uniformBlock(uniformBlockName.data());
		const bool dataCopied = batchBlock->copyData(uniformBlockCache.dataPointer());
		ASSERT(dataCopied);
		batchBlock->setUsedSize(uniformBlockCache.usedSize());
	}

	// Setting sampler uniforms for GL_TEXTURE* units
	const GLShaderUniforms::UniformHashMapType allUniforms = refCommand->material().allUniforms();
	for (const GLUniformCache &uniformCache : allUniforms)
	{
		if (uniformCache.uniform()->type() == GL_SAMPLER_2D)
		{
			GLUniformCache *batchUniformCache = batchCommand->material().uniform(uniformCache.uniform()->name());
			const int refValue = uniformCache.intValue(0);
			const int batchValue = batchUniformCache->intValue(0);
			// Also checking if the command has just been added, as the memory at the
			// uniforms data pointer is not cleared and might contain the reference value
			if (batchValue != refValue || commandAdded)
				batchUniformCache->setIntValue(refValue);
		}
	}

	const unsigned long maxVertexDataSize = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ARRAY).maxSize;
	const unsigned long maxIndexDataSize = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY).maxSize;
	// Sum the amount of VBO and IBO memory required by the batch
	it = start;
	const bool refShaderHasAttributes = (refShader->numAttributes() > 0);
	while (it != nextStart)
	{
		unsigned int vertexDataSize = 0;
		unsigned int numIndices = (*it)->geometry().numIndices();

		if (refShaderHasAttributes)
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

	const unsigned int NumFloatsVertexFormat = refCommand->geometry().numElementsPerVertex();
	const unsigned int NumFloatsVertexFormatAndIndex = NumFloatsVertexFormat + 1; // index is an `int`, same size as a `float`
	const unsigned int SizeVertexFormat = NumFloatsVertexFormat * 4;
	const unsigned int SizeVertexFormatAndIndex = SizeVertexFormat + sizeof(int);

	float *destVtx = nullptr;
	GLushort *destIdx = nullptr;

	const bool batchedShaderHasAttributes = (batchedShader->numAttributes() > 1);
	if (batchedShaderHasAttributes)
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

		const GLUniformBlockCache *singleInstanceBlock = command->material().uniformBlock(Material::InstanceBlockName);
		const bool dataCopied = instancesBlock->copyData(instancesBlockOffset, singleInstanceBlock->dataPointer(), singleInstanceBlockSize);
		ASSERT(dataCopied);
		instancesBlockOffset += singleInstanceBlockSize;

		if (batchedShaderHasAttributes)
		{
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
	instancesBlock->setUsedSize(instancesBlockOffset);

	if (batchedShaderHasAttributes)
	{
		batchCommand->geometry().releaseVertexPointer();
		if (destIdx)
			batchCommand->geometry().releaseIndexPointer();
	}

	for (unsigned int i = 0; i < GLTexture::MaxTextureUnits; i++)
		batchCommand->material().setTexture(i, refCommand->material().texture(i));
	batchCommand->material().setBlendingEnabled(refCommand->material().isBlendingEnabled());
	batchCommand->material().setBlendingFactors(refCommand->material().srcBlendingFactor(), refCommand->material().destBlendingFactor());
	batchCommand->setBatchSize(nextStart - start);
	batchCommand->setLayer(refCommand->layer());
	batchCommand->setVisitOrder(refCommand->visitOrder());

	if (batchedShaderHasAttributes)
	{
		const unsigned int totalVertices = instancesVertexDataSize / SizeVertexFormatAndIndex;
		batchCommand->geometry().setDrawParameters(refCommand->geometry().primitiveType(), 0, totalVertices);
		batchCommand->geometry().setNumElementsPerVertex(NumFloatsVertexFormatAndIndex);
		batchCommand->geometry().setNumIndices(instancesIndicesAmount);
	}
	else
		batchCommand->geometry().setDrawParameters(GL_TRIANGLES, 0, 6 * (nextStart - start));

	return batchCommand;
}

namespace {

	struct SpriteInstanceBase
	{
		float transform[4];
		float translation[4];

		uint32_t color;
		uint32_t spriteSize;
	};

	struct SpriteInstance : public SpriteInstanceBase
	{
		uint32_t uvEndpointsU;
		uint32_t uvEndpointsV;
	};

	struct SpriteNoTextureInstance : public SpriteInstanceBase
	{
	};

}

RenderCommand *RenderBatcher::collectCommandsWithInstancing(
    nctl::Array<RenderCommand *>::ConstIterator start,
    nctl::Array<RenderCommand *>::ConstIterator end,
    nctl::Array<RenderCommand *>::ConstIterator &nextStart)
{
	ASSERT(end > start);
	ZoneScoped;
#ifdef WITH_TRACY
	zoneTextString.format("Batch size: %u", end - start);
	ZoneText(zoneTextString.data(), zoneTextString.length());
#endif

	const RenderCommand *refCommand = *start;
	RenderCommand *batchCommand = nullptr;

	bool commandAdded = false;
	const bool withTexture = (refCommand->material().shaderProgramType() != Material::ShaderProgramType::SPRITE_NO_TEXTURE);
	const Material::ShaderProgramType attribsShaderProgramType = withTexture
	    ? Material::ShaderProgramType::SPRITE_ATTRIBS
	    : Material::ShaderProgramType::SPRITE_NO_TEXTURE_ATTRIBS;
	batchCommand = RenderResources::renderCommandPool().retrieveOrAdd(RenderResources::shaderProgram(attribsShaderProgramType), commandAdded);

	const size_t instanceStructSize = withTexture ? sizeof(SpriteInstance) : sizeof(SpriteNoTextureInstance);
	const unsigned int numFloats = instanceStructSize / sizeof(GLfloat);
	const unsigned int numInstances = end - start;

	if (commandAdded)
	{
		batchCommand->setType(RenderCommand::CommandTypes::SPRITE);
		batchCommand->material().reserveUniformsDataMemory();

		GLShaderProgram &shaderProgram = *batchCommand->material().shaderProgram();
		GLVertexFormat::Attribute *transformAttribute = shaderProgram.attribute("aTransform");
		GLVertexFormat::Attribute *translationAttribute = shaderProgram.attribute("aTranslation");
		GLVertexFormat::Attribute *colorAttribute = shaderProgram.attribute("aColor");
		GLVertexFormat::Attribute *spriteSizeAttribute = shaderProgram.attribute("aSpriteSize");
		GLVertexFormat::Attribute *uvEndpointsUAttribute = withTexture ? shaderProgram.attribute("aUvEndpointsU") : nullptr;
		GLVertexFormat::Attribute *uvEndpointsVAttribute = withTexture ? shaderProgram.attribute("aUvEndpointsV") : nullptr;

		transformAttribute->setVboParameters(instanceStructSize, reinterpret_cast<void *>(0 * sizeof(GLfloat)));
		translationAttribute->setVboParameters(instanceStructSize, reinterpret_cast<void *>(4 * sizeof(GLfloat)));
		colorAttribute->setVboParameters(instanceStructSize, reinterpret_cast<void *>(8 * sizeof(GLfloat)));
		spriteSizeAttribute->setVboParameters(instanceStructSize, reinterpret_cast<void *>(9 * sizeof(GLfloat)));
		if (withTexture)
		{
			uvEndpointsUAttribute->setVboParameters(instanceStructSize, reinterpret_cast<void *>(10 * sizeof(GLfloat)));
			uvEndpointsVAttribute->setVboParameters(instanceStructSize, reinterpret_cast<void *>(11 * sizeof(GLfloat)));
		}

		transformAttribute->setDivisor(1);
		translationAttribute->setDivisor(1);
		colorAttribute->setDivisor(1);
		spriteSizeAttribute->setDivisor(1);
		if (withTexture)
		{
			uvEndpointsUAttribute->setDivisor(1);
			uvEndpointsVAttribute->setDivisor(1);
		}
	}

	// The base pointer should be a multiple of 4 for the vertex ID calculation to work in the shader
	GLfloat *base = batchCommand->geometry().acquireVertexPointer(numFloats * numInstances, 4);
	uint8_t *instanceData = reinterpret_cast<uint8_t *>(base);

	nctl::Array<RenderCommand *>::ConstIterator it = start;
	unsigned int instanceIndex = 0;
	while (it != end)
	{
		const RenderCommand *command = *it;
		ASSERT(command->type() == RenderCommand::CommandTypes::SPRITE);

		const Material &material = command->material();

		const GLUniformBlockCache *instanceBlock = material.uniformBlock(Material::InstanceBlockName);
		const int alignedInstanceStructSize = (instanceStructSize + 15) & ~size_t(15);
		ASSERT(instanceBlock->usedSize() - instanceBlock->alignAmount() == alignedInstanceStructSize);

		uint8_t *dstPtr = instanceData + instanceIndex * instanceStructSize;
		SpriteInstanceBase *dst = reinterpret_cast<SpriteInstanceBase *>(dstPtr);

		const float *matrix = command->transformation().data();
		dst->transform[0] = matrix[0];
		dst->transform[1] = matrix[4];
		dst->transform[2] = matrix[1];
		dst->transform[3] = matrix[5];

		dst->translation[0] = matrix[12];
		dst->translation[1] = matrix[13];
		dst->translation[2] = matrix[14];

		const size_t bytesToCopyAfterMatrix = (withTexture ? 4 : 2) * sizeof(uint32_t);
		memcpy(&dst->color, reinterpret_cast<const uint32_t *>(instanceBlock->dataPointer()) + 8, bytesToCopyAfterMatrix);

		instanceIndex++;
		++it;
	}
	nextStart = it;

	batchCommand->geometry().releaseVertexPointer();

	// Setting sampler uniforms for GL_TEXTURE* units
	const GLShaderUniforms::UniformHashMapType allUniforms = refCommand->material().allUniforms();
	for (const GLUniformCache &uniformCache : allUniforms)
	{
		if (uniformCache.uniform()->type() == GL_SAMPLER_2D)
		{
			GLUniformCache *batchUniformCache = batchCommand->material().uniform(uniformCache.uniform()->name());
			const int refValue = uniformCache.intValue(0);
			const int batchValue = batchUniformCache->intValue(0);
			// Also checking if the command has just been added, as the memory at the
			// uniforms data pointer is not cleared and might contain the reference value
			if (batchValue != refValue || commandAdded)
				batchUniformCache->setIntValue(refValue);
		}
	}

	for (unsigned int i = 0; i < GLTexture::MaxTextureUnits; i++)
		batchCommand->material().setTexture(i, refCommand->material().texture(i));
	batchCommand->material().setBlendingEnabled(refCommand->material().isBlendingEnabled());
	batchCommand->material().setBlendingFactors(refCommand->material().srcBlendingFactor(), refCommand->material().destBlendingFactor());
	batchCommand->setBatchSize(nextStart - start);
	batchCommand->setLayer(refCommand->layer());
	batchCommand->setVisitOrder(refCommand->visitOrder());

	batchCommand->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);
	batchCommand->setNumInstances(nextStart - start);

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
