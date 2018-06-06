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
	  numElementsPerVertex_(2), hostVertexPointer_(nullptr), sharedVboParams_(nullptr)
{

}

Geometry::~Geometry()
{
	if (vbo_)
		RenderStatistics::removeCustomVbo(vbo_->size());
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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::bind()
{
	if (ibo_)
		ibo_->bind();

	if (vboParams_.object)
		vboParams_.object->bind();
}

void Geometry::draw(GLsizei numInstances)
{
	if (sharedVboParams_)
		vboParams_ = *sharedVboParams_;

	const unsigned int offset = (vboParams_.offset / numElementsPerVertex_) / sizeof(GLfloat);
	const void *offsetPtr = reinterpret_cast<const void *>(offset);

	if (numInstances == 0)
	{
		if (ibo_)
			glDrawElements(primitiveType_, numVertices_, GL_UNSIGNED_SHORT, offsetPtr);
		else
			glDrawArrays(primitiveType_, offset + firstVertex_, numVertices_);
	}
	else if (numInstances > 0)
	{
		if (ibo_)
			glDrawElementsInstanced(primitiveType_, numVertices_, GL_UNSIGNED_SHORT, offsetPtr, numInstances);
		else
			glDrawArraysInstanced(primitiveType_, offset + firstVertex_, numVertices_, numInstances);
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

}
