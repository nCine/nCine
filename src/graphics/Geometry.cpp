#include "Geometry.h"
#include "RenderResources.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Empty constructor
Geometry::Geometry()
	: vboSharingType_(SHARED),
	  drawType_(GL_TRIANGLES), firstVertex_(0), numVertices_(0),
	  vbo_(NULL), ibo_(NULL)
{

}

Geometry::~Geometry()
{
	if (vboSharingType_ == UNIQUE)
	{
		delete vbo_;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::setDrawParameters(GLenum drawType, GLint firstVertex, GLsizei numVertices)
{
	drawType_ = drawType;
	firstVertex_ = firstVertex;
	numVertices_ = numVertices;
}

void Geometry::createCustomVbo(unsigned int numFloats, GLenum usage)
{
	if (vboSharingType_ != UNIQUE)
	{
		// VBO is now a custom one
		vboSharingType_ = UNIQUE;
	}
	else
	{
		// There is already a custom VBO, it needs to be deleted
		delete vbo_;
	}

	vbo_ = new GLBufferObject(GL_ARRAY_BUFFER);
	vbo_->bufferData(numFloats * sizeof(GLfloat), NULL, usage);
}

void Geometry::updateVboData(unsigned int floatOffset, unsigned int floatSize, const GLfloat *data)
{
	// Common resources cannot be altered
	if (vboSharingType_ != COMMON_RESOURCE)
	{
		vbo_->bufferSubData(floatOffset * sizeof(GLfloat), floatSize * sizeof(GLfloat), data);
	}
}

void Geometry::shareVbo(const Geometry& geometry)
{
	if (vboSharingType_ == UNIQUE)
	{
		delete vbo_;
	}

	vboSharingType_ = SHARED;
	vbo_ = geometry.vbo_;
}

void Geometry::makeSharedQuad()
{
	if (vboSharingType_ == UNIQUE)
	{
		delete vbo_;
	}

	vboSharingType_ = COMMON_RESOURCE;
	vbo_ = const_cast<GLBufferObject *>(RenderResources::quadVbo());
	setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::bind()
{
	if (ibo_)
	{
		ibo_->bind();
	}

	if (vbo_)
	{
		vbo_->bind();
	}
}

GLuint Geometry::vboHandle()
{
	GLuint glHandle = 0;

	if (vbo_)
	{
		glHandle = vbo_->glHandle();
	}

	return glHandle;
}

}
