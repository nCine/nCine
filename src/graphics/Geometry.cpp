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
	  numElementsPerVertex_(2), numIndices_(0),
	  hostVertexPointer_(nullptr), hostIndexPointer_(nullptr),
	  sharedVboParams_(nullptr), sharedIboParams_(nullptr)
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

GLfloat *Geometry::acquireVertexPointer()
{
	ASSERT(vbo_);

	if (vboParams_.mapBase == nullptr)
	{
		const GLenum mapFlags = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ARRAY).mapFlags;
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

void Geometry::shareVbo(const Geometry &geometry)
{
	vbo_.reset(nullptr);
	sharedVboParams_ = &geometry.vboParams_;
}

void Geometry::createCustomIbo(unsigned int numIndices, GLenum usage)
{
	ibo_ = nctl::makeUnique<GLBufferObject>(GL_ELEMENT_ARRAY_BUFFER);
	ibo_->bufferData(numIndices * sizeof(GLushort), nullptr, usage);

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

GLushort *Geometry::acquireIndexPointer()
{
	ASSERT(ibo_);

	if (iboParams_.mapBase == nullptr)
	{
		const GLenum mapFlags = RenderResources::buffersManager().specs(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY).mapFlags;
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

void Geometry::shareIbo(const Geometry &geometry)
{
	ibo_.reset(nullptr);
	sharedIboParams_ = &geometry.iboParams_;
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
	if (sharedVboParams_)
		vboParams_ = *sharedVboParams_;
	const GLint vboOffset = static_cast<GLint>(vboParams_.offset / numElementsPerVertex_ / sizeof(GLfloat));

	void *iboOffsetPtr = nullptr;
	if (numIndices_ > 0)
	{
		if (sharedIboParams_)
			iboParams_ = *sharedIboParams_;
		iboOffsetPtr = reinterpret_cast<void *>(iboParams_.offset);
	}

	if (numInstances == 0)
	{
		if (numIndices_ > 0)
#if defined(__ANDROID__) && !GL_ES_VERSION_3_2
			glDrawElements(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr);
#else
			glDrawElementsBaseVertex(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr, vboOffset + firstVertex_);
#endif
		else
			glDrawArrays(primitiveType_, vboOffset + firstVertex_, numVertices_);
	}
	else if (numInstances > 0)
	{
		if (numIndices_ > 0)
#if defined(__ANDROID__) && !GL_ES_VERSION_3_2
			glDrawElementsInstanced(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr, numInstances);
#else
			glDrawElementsInstancedBaseVertex(primitiveType_, numIndices_, GL_UNSIGNED_SHORT, iboOffsetPtr, numInstances, vboOffset + firstVertex_);
#endif
		else
			glDrawArraysInstanced(primitiveType_, vboOffset + firstVertex_, numVertices_, numInstances);
	}
}

void Geometry::commitVertices()
{
	if (hostVertexPointer_)
	{
		const unsigned int numFloats = numVertices_ * numElementsPerVertex_;
		GLfloat *vertices = acquireVertexPointer(numFloats, numElementsPerVertex_);
		memcpy(vertices, hostVertexPointer_, numFloats * sizeof(GLfloat));
		releaseVertexPointer();
	}
}

void Geometry::commitIndices()
{
	if (hostIndexPointer_)
	{
		GLushort *indices = acquireIndexPointer(numIndices_);
		memcpy(indices, hostIndexPointer_, numIndices_ * sizeof(GLushort));
		releaseIndexPointer();
	}
}


}
