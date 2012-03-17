#include "ncRenderCommand.h"
#include "ncTexture.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Binds the material state
void ncRenderMaterial::Bind() const
{
//	if (m_color.B() < 255)
//		glEnable(GL_BLEND);

//	glColor4ubv(m_color.Vector()); // Not available on GLES
	glColor4ub(m_color.R(), m_color.G(), m_color.B(), m_color.A());
	glBindTexture(GL_TEXTURE_2D, m_uTextureGLId);
}

/// Applies the transformation
void ncRenderTransformation::Apply() const
{

}

/// Draws the geometry
void ncRenderGeometry::Draw() const
{
	if (m_ubColors)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_ubColors);
	}

	if (m_fTexCoords)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, m_fTexCoords);
	}

	if (m_fVertices) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, m_fVertices);
		glDrawArrays(m_eDrawType, m_iFirstVertex, m_iNumVertices);
	}

	if (m_fVertices)
		glDisableClientState(GL_VERTEX_ARRAY);
	if (m_fTexCoords)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (m_ubColors)
		glDisableClientState(GL_COLOR_ARRAY);
}

/// Calculates a sort key for the queue
void ncRenderCommand::CalculateSortKey()
{
	unsigned long int upper = m_iPriority << 16;
	unsigned int lower = m_material.TextureGLId();
	m_uSortKey = upper + lower;
//	m_bDirtyKey = false;
}

/// Issues the render command
void ncRenderCommand::Issue() const
{
	m_material.Bind();
	m_transformation.Apply();
	m_geometry.Draw();
}
