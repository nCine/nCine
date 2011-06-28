#ifdef __ANDROID__
	#include <stdlib.h> // for exit()
#else
	#include <cstdio> // for NULL
#endif

#include "ncSpriteBatch.h"
#include "ncServiceLocator.h"


///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSpriteBatch::ncSpriteBatch(unsigned int uBufferSize)
	: m_uBufferSize(uBufferSize), m_uMaxBufferCounter(0), m_uMaxRenderCallsCounter(0),
	  m_fVertices(0), m_fTexCoords(0), m_pLastTexture(0), m_bDrawing(false)
{
	if (uBufferSize > 0)
	{
		m_fVertices = new GLfloat[uBufferSize*QUAD_ELEMENTS];
		m_fTexCoords = new GLfloat[uBufferSize*QUAD_ELEMENTS];
	}
	else
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncSpriteBatch::ncSpriteBatch - Invalid value for buffer size: %d", uBufferSize);
		exit(-1);
	}
}

ncSpriteBatch::~ncSpriteBatch()
{
	if (m_fVertices)
		delete[] m_fVertices;
	if (m_fTexCoords)
		delete[] m_fTexCoords;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// To be called before drawing
void ncSpriteBatch::Begin()
{
	if (m_bDrawing)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncSpriteBatch::Begin - Already drawing");
		exit(-1);
	}

	m_bDrawing = true;
	m_uBufferTop = 0;
	m_pLastTexture = NULL;

	m_uMaxBufferCounter = 0;
	m_uMaxRenderCallsCounter = 0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, m_fVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, m_fTexCoords);
}

/// Draw the source rectangle from the texture to the destination rectangle on screen
void ncSpriteBatch::Draw(ncTexture *pTexture, ncRect dstRect, ncRect srcRect)
{
	if (!m_bDrawing)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncSpriteBatch::Draw - Begin has not been called first");
		exit(-1);
	}

	if (pTexture == NULL)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncSpriteBatch::Draw - Texture pointer is NULL");
		exit(-1);
	}

	if (m_uBufferTop >= m_uBufferSize)
		FlushBuffer();

	if (m_pLastTexture == NULL ||
		m_pLastTexture->m_uId != pTexture->m_uId)
	{
		FlushBuffer();
		pTexture->Bind();
		m_uMaxRenderCallsCounter++;
		m_pLastTexture = pTexture;
	}

	int iOffset = QUAD_ELEMENTS * m_uBufferTop;
	SetVertices(m_fVertices + iOffset, dstRect);
	if (srcRect.w == 0 || srcRect.h == 0) // blitting the whole texture
		SetTexCoords(m_fTexCoords + iOffset); // optimized function
	else
		SetTexCoords(m_fTexCoords + iOffset, srcRect);

	m_uBufferTop++;
	if (m_uBufferTop > m_uMaxBufferCounter)
		m_uMaxBufferCounter = m_uBufferTop;
}

/// Draw the entire texture to the destination rectangle on screen
void ncSpriteBatch::Draw(ncTexture *pTexture, ncRect dstRect)
{
//	ncRect srcRect(0, 0, pTexture->m_iWidth, pTexture->m_iHeight);
	ncRect srcRect(0, 0, 0, 0); // to call the optimized SetTexXoords()
	Draw(pTexture, dstRect, srcRect);
}

/// Draw the entire texture to the screen, centered at pos parameter
void ncSpriteBatch::Draw(ncTexture *pTexture, ncPoint pos)
{
	ncRect dstRect(pos.x - pTexture->m_iWidth/2, pos.y - pTexture->m_iHeight/2,
				   pTexture->m_iWidth/2, pTexture->m_iHeight/2);
//	ncRect srcRect(0, 0, pTexture->m_iWidth, pTexture->m_iHeight);
	ncRect srcRect(0, 0, 0, 0); // to call the optimized SetTexXoords()
	Draw(pTexture, dstRect, srcRect);
}

/// To be called after drawing
void ncSpriteBatch::End()
{
	if (!m_bDrawing)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncSpriteBatch::End - Begin has not been called first");
		exit(-1);
	}

	m_bDrawing = false;
	FlushBuffer();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

//	ncServiceLocator::GetLogger().Write(2, (char *)"ncSpriteBatch::End - Render calls: %u", m_uMaxRenderCallsCounter);
//	ncServiceLocator::GetLogger().Write(2, (char *)"ncSpriteBatch::End - Max buffered: %u", m_uMaxBufferCounter);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////


/// Flush the sprite buffers issuing rendering commands
void ncSpriteBatch::FlushBuffer()
{
	if (m_uBufferTop)
	{
		glDrawArrays(GL_TRIANGLES, 0, m_uBufferTop*6); // Six vertices per sprite
		m_uMaxRenderCallsCounter++;
		m_uBufferTop = 0;
	}
}

/// Fill the buffer with two triangles vertices
void ncSpriteBatch::SetVertices(GLfloat *fVertices, ncRect rect)
{
	fVertices[0] = rect.x;
	fVertices[1] = rect.y;
	fVertices[2] = rect.x;
	fVertices[3] = rect.y + rect.h;
	fVertices[4] = rect.x + rect.w;
	fVertices[5] = rect.y + rect.h;

	fVertices[6] = rect.x + rect.w;
	fVertices[7] = rect.y + rect.h;
	fVertices[8] = rect.x + rect.w;
	fVertices[9] = rect.y;
	fVertices[10] = rect.x;
	fVertices[11] = rect.y;
}

/// Fill the buffer with two triangles UVs
void ncSpriteBatch::SetTexCoords(GLfloat *fTexCoords, ncRect rect)
{
	float x = float(rect.x)/float(m_pLastTexture->m_iWidth);
	float y = float(rect.y)/float(m_pLastTexture->m_iHeight);
	float w = float(rect.w)/float(m_pLastTexture->m_iWidth);
	float h = float(rect.h)/float(m_pLastTexture->m_iHeight);

	fTexCoords[0] = x;
	fTexCoords[1] = y + h;
	fTexCoords[2] = x;
	fTexCoords[3] = y;
	fTexCoords[4] = x + w;
	fTexCoords[5] = y;

	fTexCoords[6] = x + w;
	fTexCoords[7] = y;
	fTexCoords[8] = x + w;
	fTexCoords[9] = y + h;
	fTexCoords[10] = x;
	fTexCoords[11] = y + h;
}

/// Fill the buffer with two triangles UVs that cover the entire texture
void ncSpriteBatch::SetTexCoords(GLfloat *fTexCoords)
{
	fTexCoords[0] = 0.0f;
	fTexCoords[1] = 1.0f;
	fTexCoords[2] = 0.0f;
	fTexCoords[3] = 0.0f;
	fTexCoords[4] = 1.0f;
	fTexCoords[5] = 0.0f;

	fTexCoords[6] = 1.0f;
	fTexCoords[7] = 0.0f;
	fTexCoords[8] = 1.0f;
	fTexCoords[9] = 1.0f;
	fTexCoords[10] = 0.0f;
	fTexCoords[11] = 1.0f;
}

