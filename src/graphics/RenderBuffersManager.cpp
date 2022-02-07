#include <nctl/StaticString.h>
#include "RenderBuffersManager.h"
#include "RenderStatistics.h"
#include "GLDebug.h"
#include "tracy.h"

namespace ncine {

namespace {
	/// The string used to output OpenGL debug group information
	static nctl::StaticString<64> debugString;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderBuffersManager::RenderBuffersManager(bool useBufferMapping, unsigned long vboMaxSize, unsigned long iboMaxSize)
    : buffers_(4)
{
	BufferSpecifications &vboSpecs = specs_[BufferTypes::ARRAY];
	vboSpecs.type = BufferTypes::ARRAY;
	vboSpecs.target = GL_ARRAY_BUFFER;
	vboSpecs.mapFlags = useBufferMapping ? GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT : 0;
	vboSpecs.usageFlags = GL_STREAM_DRAW;
	vboSpecs.maxSize = vboMaxSize;
	vboSpecs.alignment = sizeof(GLfloat);

	BufferSpecifications &iboSpecs = specs_[BufferTypes::ELEMENT_ARRAY];
	iboSpecs.type = BufferTypes::ELEMENT_ARRAY;
	iboSpecs.target = GL_ELEMENT_ARRAY_BUFFER;
	iboSpecs.mapFlags = useBufferMapping ? GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT : 0;
	iboSpecs.usageFlags = GL_STREAM_DRAW;
	iboSpecs.maxSize = iboMaxSize;
	iboSpecs.alignment = sizeof(GLushort);

	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	const int maxUniformBlockSize = gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE);
	const int offsetAlignment = gfxCaps.value(IGfxCapabilities::GLIntValues::UNIFORM_BUFFER_OFFSET_ALIGNMENT);

	// Clamping the value as some drivers report a maximum size similar to SSBO one
	const int uboMaxSize = maxUniformBlockSize <= 64 * 1024 ? maxUniformBlockSize : 64 * 1024;

	BufferSpecifications &uboSpecs = specs_[BufferTypes::UNIFORM];
	uboSpecs.type = BufferTypes::UNIFORM;
	uboSpecs.target = GL_UNIFORM_BUFFER;
	uboSpecs.mapFlags = useBufferMapping ? GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT : 0;
	uboSpecs.usageFlags = GL_STREAM_DRAW;
	uboSpecs.maxSize = static_cast<unsigned long>(uboMaxSize);
	uboSpecs.alignment = static_cast<unsigned int>(offsetAlignment);

	// Create the first buffer for each type right away
	for (unsigned int i = 0; i < BufferTypes::COUNT; i++)
		createBuffer(specs_[i]);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	const char *bufferTypeToString(RenderBuffersManager::BufferTypes::Enum type)
	{
		switch (type)
		{
			case RenderBuffersManager::BufferTypes::Enum::ARRAY: return "Array";
			case RenderBuffersManager::BufferTypes::Enum::ELEMENT_ARRAY: return "Element Array";
			case RenderBuffersManager::BufferTypes::Enum::UNIFORM: return "Uniform";
			case RenderBuffersManager::BufferTypes::Enum::COUNT: return "";
		}

		return "";
	}

}

RenderBuffersManager::Parameters RenderBuffersManager::acquireMemory(BufferTypes::Enum type, unsigned long bytes, unsigned int alignment)
{
	FATAL_ASSERT_MSG_X(bytes <= specs_[type].maxSize, "Trying to acquire %lu bytes when the maximum for buffer type \"%s\" is %lu",
	                   bytes, bufferTypeToString(type), specs_[type].maxSize);

	// Accepting a custom alignment only if it is a multiple of the specification one
	if (alignment % specs_[type].alignment != 0)
		alignment = specs_[type].alignment;

	Parameters params;

	for (ManagedBuffer &buffer : buffers_)
	{
		if (buffer.type == type)
		{
			const unsigned long offset = buffer.size - buffer.freeSpace;
			const unsigned int alignAmount = (alignment - offset % alignment) % alignment;

			if (buffer.freeSpace >= bytes + alignAmount)
			{
				params.object = buffer.object.get();
				params.offset = offset + alignAmount;
				params.size = bytes;
				buffer.freeSpace -= bytes + alignAmount;
				params.mapBase = buffer.mapBase;
				break;
			}
		}
	}

	if (params.object == nullptr)
	{
		createBuffer(specs_[type]);
		params.object = buffers_.back().object.get();
		params.offset = 0;
		params.size = bytes;
		buffers_.back().freeSpace -= bytes;
		params.mapBase = buffers_.back().mapBase;
	}

	return params;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderBuffersManager::flushUnmap()
{
	ZoneScoped;
	GLDebug::ScopedGroup scoped("RenderBuffersManager::flushUnmap()");

	for (ManagedBuffer &buffer : buffers_)
	{
		RenderStatistics::gatherStatistics(buffer);
		const unsigned long usedSize = buffer.size - buffer.freeSpace;
		FATAL_ASSERT(usedSize <= specs_[buffer.type].maxSize);
		buffer.freeSpace = buffer.size;

		if (specs_[buffer.type].mapFlags == 0)
		{
			if (usedSize > 0)
				buffer.object->bufferSubData(0, usedSize, buffer.hostBuffer.get());
		}
		else
		{
			if (usedSize > 0)
				buffer.object->flushMappedBufferRange(0, usedSize);
			buffer.object->unmap();
		}

		buffer.mapBase = nullptr;
	}
}

void RenderBuffersManager::remap()
{
	ZoneScoped;
	GLDebug::ScopedGroup scoped("RenderBuffersManager::remap()");

	for (ManagedBuffer &buffer : buffers_)
	{
		ASSERT(buffer.freeSpace == buffer.size);
		ASSERT(buffer.mapBase == nullptr);

		if (specs_[buffer.type].mapFlags == 0)
		{
			buffer.object->bufferData(buffer.size, nullptr, specs_[buffer.type].usageFlags);
			buffer.mapBase = buffer.hostBuffer.get();
		}
		else
			buffer.mapBase = static_cast<GLubyte *>(buffer.object->mapBufferRange(0, buffer.size, specs_[buffer.type].mapFlags));

		FATAL_ASSERT(buffer.mapBase != nullptr);
	}
}

void RenderBuffersManager::createBuffer(const BufferSpecifications &specs)
{
	ZoneScoped;

	ManagedBuffer managedBuffer;
	managedBuffer.type = specs.type;
	managedBuffer.size = specs.maxSize;
	managedBuffer.object = nctl::makeUnique<GLBufferObject>(specs.target);
	managedBuffer.object->bufferData(managedBuffer.size, nullptr, specs.usageFlags);
	managedBuffer.freeSpace = managedBuffer.size;

	switch (managedBuffer.type)
	{
		default:
		case BufferTypes::Enum::ARRAY:
			managedBuffer.object->setObjectLabel("Vertex_ManagedBuffer");
			break;
		case BufferTypes::Enum::ELEMENT_ARRAY:
			managedBuffer.object->setObjectLabel("Index_ManagedBuffer");
			break;
		case BufferTypes::Enum::UNIFORM:
			managedBuffer.object->setObjectLabel("Uniform_ManagedBuffer");
			break;
	}

	if (specs.mapFlags == 0)
	{
		managedBuffer.hostBuffer = nctl::makeUnique<GLubyte[]>(specs.maxSize);
		managedBuffer.mapBase = managedBuffer.hostBuffer.get();
	}
	else
		managedBuffer.mapBase = static_cast<GLubyte *>(managedBuffer.object->mapBufferRange(0, managedBuffer.size, specs.mapFlags));

	FATAL_ASSERT(managedBuffer.mapBase != nullptr);

	buffers_.pushBack(nctl::move(managedBuffer));

	debugString.format("Create %s buffer 0x%lx", bufferTypeToString(specs.type), uintptr_t(buffers_.back().object.get()));
	GLDebug::messageInsert(debugString.data());
}

}
