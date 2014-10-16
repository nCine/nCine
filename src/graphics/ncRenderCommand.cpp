#include "ncRenderCommand.h"
#include "ncTexture.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncRenderGeometry::ncRenderGeometry(GLenum drawType, GLint firstVertex, GLsizei numVertices, GLfloat *vertices, GLfloat *texCoords)
	: drawType_(drawType),
	  firstVertex_(firstVertex), numVertices_(numVertices),
	  vertices_(vertices), texCoords_(texCoords), colors_(NULL)
{

}

ncRenderGeometry::ncRenderGeometry(GLsizei numVertices, GLfloat *vertices, GLfloat *texCoords)
	: drawType_(GL_TRIANGLES),
	  firstVertex_(0), numVertices_(numVertices),
	  vertices_(vertices), texCoords_(texCoords), colors_(NULL)
{

}

ncRenderGeometry::ncRenderGeometry()
	: drawType_(GL_TRIANGLES),
	  firstVertex_(0), numVertices_(0),
	  vertices_(NULL), texCoords_(NULL), colors_(NULL)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Binds the material state
void ncRenderMaterial::bind() const
{
//	glColor4ubv(color.vector()); // Not available on GLES
	glColor4ub(color_.r(), color_.g(), color_.b(), color_.a());
	glBindTexture(GL_TEXTURE_2D, textureGLId_);
}

/// Applies the transformation
void ncRenderTransformation::apply() const
{

}

/// Draws the geometry
void ncRenderGeometry::draw() const
{
	if (colors_)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors_);
	}

	if (texCoords_)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords_);
	}

	if (vertices_)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices_);
		glDrawArrays(drawType_, firstVertex_, numVertices_);
	}

	if (vertices_)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if (texCoords_)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if (colors_)
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
}

/// Calculates a sort key for the queue
void ncRenderCommand::calculateSortKey()
{
	unsigned long int upper = priority_ << 16;
	unsigned int lower = material_.textureGLId();
	sortKey_ = upper + lower;
}

/// Issues the render command
void ncRenderCommand::issue() const
{
#ifdef WITH_DEPTH_TEST
	glPolygonOffset(0.0f, -1.0f * priority_);
#endif

	material_.bind();
	transformation_.apply();
	geometry_.draw();
}
