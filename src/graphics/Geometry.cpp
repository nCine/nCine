#include <cstring> // for memcpy()
#include "Geometry.h"
#include "RenderResources.h"
#include "RenderStatistics.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Geometry::Geometry()
    : primitiveType_(GL_TRIANGLES), firstVertex_(0), numVertices_(0),
      numElementsPerVertex_(2), firstIndex_(0), numIndices_(0),
      hostVertexPointer_(nullptr), hostIndexPointer_(nullptr),
      vboUsageFlags_(0), sharedVboParams_(nullptr),
      iboUsageFlags_(0), sharedIboParams_(nullptr)
{
}

Geometry::~Geometry()
{
	if (vbo_)
		RenderStatistics::removeCustomVbo(vbo_->size());

	if (ibo_)
		RenderStatistics::removeCustomIbo(ibo_->size());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::setDrawParameters(GLenum primitiveType, GLint firstVertex, GLsizei numVertices)
{
	primitiveType_ = primitiveType;
	firstVertex_ = firstVertex;
	numVertices_ = numVertices;
}

void Geometry::createCustomVbo(unsigned int numFloats, GLenum usage)
{
	vbo_ = nctl::makeUnique<GLBufferObject>(GL_ARRAY_BUFFER);
	vbo_->bufferData(numFloats * sizeof(GLfloat), nullptr, usage);

	vboUsageFlags_ = usage;
	vboParams_.object = vbo_.get();
	vboParams_.size = vbo_->size();
	vboParams_.offset = 0;
	vboParams_.mapBase = nullptr;

	RenderStatistics::addCustomVbo(vbo_->size());
}

GLfloat *Geometry::acquireVertexPointer(unsigned int numFloats, unsigned int numFloatsAlignment)
{
	ASSERT(vbo_ == nullptr);

	if (sharedVboParams_)
		vboParams_ = *sharedVboParams_;
	else
	{
		const RenderBuffersManager::BufferTypes::Enum bufferType = RenderBuffersManager::BufferTypes::ARRAY;
		if (vboParams_.mapBase == nullptr)
			vboParams_ = RenderResources::buffersManager().acquireMemory(bufferType, numFloats * sizeof(GLfloat), numFloatsAlignment * sizeof(GLfloat));
	}

	return reinterpret_cast<GLfloat *>(vboParams_.mapBase + vboParams_.offset);
}

/*! This method can only be used when mapping of OpenGL buffers is available */
GLfloat *Geometry::acquireVertexPointer()
{
	ASSERT(vbo_);

	if (vboParams_.mapBase == nullptr)
	{
		const GLenum mapFlags = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ARRAY).mapFlags;
		FATAL_ASSERT_MSG(mapFlags, "Mapping of OpenGL buffers is not available");
		vboParams_.mapBase = static_cast<GLubyte *>(vbo_->mapBufferRange(0, vbo_->size(), mapFlags));
	}

	return reinterpret_cast<GLfloat *>(vboParams_.mapBase);
}

void Geometry::releaseVertexPointer()
{
	// Don't flush and unmap if the VBO is not custom
	if (vbo_ && vboParams_.mapBase != nullptr)
	{
		vboParams_.object->flushMappedBufferRange(vboParams_.offset, vboParams_.size);
		vboParams_.object->unmap();
	}
	vboParams_.mapBase = nullptr;
}

void Geometry::shareVbo(const Geometry *geometry)
{
	if (geometry == nullptr)
		sharedVboParams_ = nullptr;
	else if (geometry != this)
	{
		vbo_.reset(nullptr);
		sharedVboParams_ = &geometry->vboParams_;
	}
}

void Geometry::createCustomIbo(unsigned int numIndices, GLenum usage)
{
	ibo_ = nctl::makeUnique<GLBufferObject>(GL_ELEMENT_ARRAY_BUFFER);
	ibo_->bufferData(numIndices * sizeof(GLushort), nullptr, usage);

	iboUsageFlags_ = usage;
	iboParams_.object = ibo_.get();
	iboParams_.size = ibo_->size();
	iboParams_.offset = 0;
	iboParams_.mapBase = nullptr;

	RenderStatistics::addCustomIbo(ibo_->size());
}

GLushort *Geometry::acquireIndexPointer(unsigned int numIndices)
{
	ASSERT(ibo_ == nullptr);

	if (sharedIboParams_)
		iboParams_ = *sharedIboParams_;
	else
	{
		const RenderBuffersManager::BufferTypes::Enum bufferType = RenderBuffersManager::BufferTypes::ELEMENT_ARRAY;
		if (iboParams_.mapBase == nullptr)
			iboParams_ = RenderResources::buffersManager().acquireMemory(bufferType, numIndices * sizeof(GLushort));
	}

	return reinterpret_cast<GLushort *>(iboParams_.mapBase + iboParams_.offset);
}

/*! This method can only be used when mapping of OpenGL buffers is available */
GLushort *Geometry::acquireIndexPointer()
{
	ASSERT(ibo_);

	if (iboParams_.mapBase == nullptr)
	{
		const GLenum mapFlags = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY).mapFlags;
		FATAL_ASSERT_MSG(mapFlags, "Mapping of OpenGL buffers is not available");
		iboParams_.mapBase = static_cast<GLubyte *>(ibo_->mapBufferRange(0, ibo_->size(), mapFlags));
	}

	return reinterpret_cast<GLushort *>(iboParams_.mapBase);
}

void Geometry::releaseIndexPointer()
{
	// Don't flush and unmap if the IBO is not custom
	if (ibo_ && iboParams_.mapBase != nullptr)
	{
		iboParams_.object->flushMappedBufferRange(iboParams_.offset, iboParams_.size);
		iboParams_.object->unmap();
	}
	iboParams_.mapBase = nullptr;
}

void Geometry::shareIbo(const Geometry *geometry)
{
	if (geometry == nullptr)
		sharedIboParams_ = nullptr;
	else if (geometry != this)
	{
		ibo_.reset(nullptr);
		sharedIboParams_ = &geometry->iboParams_;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::bind()
{
	if (vboParams_.object)
		vboParams_.object->bind();
}

void Geometry::draw(GLsizei numInstances)
{
	const GLint vboOffset = static_cast<GLint>(vboParams().offset / numElementsPerVertex_ / sizeof(GLfloat)) + firstVertex_;

	void *iboOffsetPtr = nullptr;
	if (numIndices_ > 0)
		iboOffsetPtr = reinterpret_cast<void *>(iboParams().offset + firstIndex_ * sizeof(GLushort));

	if (numInstances == 0)
	{
		if (numIndices_ > 0)
#if (defined(__ANDROID__) && !GL_ES_VERSION_3_2) || defined(__EMSCRIPTEN__)
			glDrawElements(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr);
#else
			glDrawElementsBaseVertex(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr, vboOffset);
#endif
		else
			glDrawArrays(primitiveType_, vboOffset, numVertices_);
	}
	else if (numInstances > 0)
	{
		if (numIndices_ > 0)
#if (defined(__ANDROID__) && !GL_ES_VERSION_3_2) || defined(__EMSCRIPTEN__)
			glDrawElementsInstanced(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr, numInstances);
#else
			glDrawElementsInstancedBaseVertex(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr, numInstances, vboOffset);
#endif
		else
			glDrawArraysInstanced(primitiveType_, vboOffset, numVertices_, numInstances);
	}
}

void Geometry::commitVertices()
{
	if (hostVertexPointer_)
	{
		// Checking if the common VBO is allowed to use mapping and do the same for the custom one
		const GLenum mapFlags = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ARRAY).mapFlags;
		const unsigned int numFloats = numVertices_ * numElementsPerVertex_;

		if (mapFlags == 0 && vbo_)
		{
			// Using buffer orphaning + `glBufferSubData()` when having a custom VBO with no mapping available
			vbo_->bufferData(vboParams_.size, nullptr, vboUsageFlags_);
			vbo_->bufferSubData(vboParams_.offset, vboParams_.size, hostVertexPointer_);
		}
		else
		{
			GLfloat *vertices = vbo_ ? acquireVertexPointer() : acquireVertexPointer(numFloats, numElementsPerVertex_);
			memcpy(vertices, hostVertexPointer_, numFloats * sizeof(GLfloat));
			releaseVertexPointer();
		}
	}
}

void Geometry::commitIndices()
{
	if (hostIndexPointer_)
	{
		// Checking if the common IBO is allowed to use mapping and do the same for the custom one
		const GLenum mapFlags = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY).mapFlags;

		if (mapFlags == 0 && ibo_)
		{
			// Using buffer orphaning + `glBufferSubData()` when having a custom IBO with no mapping available
			ibo_->bufferData(iboParams_.size, nullptr, iboUsageFlags_);
			ibo_->bufferSubData(iboParams_.offset, iboParams_.size, hostIndexPointer_);
		}
		else
		{
			GLushort *indices = ibo_ ? acquireIndexPointer() : acquireIndexPointer(numIndices_);
			memcpy(indices, hostIndexPointer_, numIndices_ * sizeof(GLushort));
			releaseIndexPointer();
		}
	}
}

}
