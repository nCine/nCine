#include "ncRenderCommand.h"
#include "ncTexture.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncRenderMaterial::Bind() const
{
	if (m_fColor[3] < 1.0f)
		glEnable(GL_BLEND);

	glColor4fv(m_fColor);
	glBindTexture(GL_TEXTURE_2D, m_uTextureGLId);
}

void ncRenderTransformation::Apply() const
{

}

void ncRenderGeometry::Draw() const
{
	if (m_fTexCoords)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, m_fTexCoords);
	}

	if (m_fVertices) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, m_fVertices);
		glDrawArrays(m_eDrawType, 0, m_iNumVertices);
	}

	if (m_fTexCoords)
		glDisableClientState(GL_VERTEX_ARRAY);
	if (m_fTexCoords)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ncRenderCommand::CalculateSortKey()
{
	m_uSortKey = m_material.TextureGLId();
}

void ncRenderCommand::Issue() const
{
	m_material.Bind();
	m_transformation.Apply();
	m_geometry.Draw();
}
