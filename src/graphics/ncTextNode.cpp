#include "ncTextNode.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextNode::ncTextNode(ncSceneNode* pParent, ncFont* pFont)
	: ncDrawableNode(pParent, 0, 0), m_bDirty(true), m_bWithKerning(true), m_pFont(pFont),
	  m_fScaleFactor(1.0f), m_vVertices(16), m_vTexCoords(16), m_uXAdvance(0), m_uXAdvanceSum(0)
{
	m_eType = TEXT_TYPE;
	memset(m_vString, 0, MAX_LENGHT);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Return the total sum of advances for a string (its width)
unsigned int ncTextNode::XAdvanceSum() const
{
	if (m_bDirty)
	{
		m_uXAdvance = 0;
		m_uXAdvanceSum = 0;
		for (int i = 0; i < strnlen(m_vString, MAX_LENGHT); i++)
		{
			const ncFontGlyph *pGlyph = m_pFont->Glyph(int(m_vString[i]));
			if (pGlyph)
			{
				m_uXAdvance += pGlyph->XAdvance()*m_fScaleFactor;
				if (m_bWithKerning)
				{
					// font kerning
					if (i < strnlen(m_vString, MAX_LENGHT) - 1)
						m_uXAdvance += pGlyph->Kerning(int(m_vString[i+1]) * m_fScaleFactor);
				}
			}
		}
		m_uXAdvanceSum = m_uXAdvance;
	}

	return m_uXAdvanceSum;
}

/// Set the string to render
void ncTextNode::SetString(const char *pString)
{
	// Setting the dirty flag and updating the string only if different
	if (strncmp(m_vString, pString, MAX_LENGHT))
	{
		strncpy(m_vString, pString, MAX_LENGHT);
		m_bDirty = true;
	}
}

void ncTextNode::Visit(ncRenderQueue& rRenderQueue)
{
	// early return if a node is invisible
	if(!bShouldDraw)
		return;

	Transform();

	Draw(rRenderQueue);
}

void ncTextNode::Draw(ncRenderQueue& rRenderQueue)
{
	if (m_bDirty)
	{
		// Clear every previous sprite data before drawing
		m_vVertices.Clear();
		m_vTexCoords.Clear();

		m_uXAdvance = 0;
		for (int i = 0; i < strnlen(m_vString, MAX_LENGHT); i++)
		{	
			const ncFontGlyph *pGlyph = m_pFont->Glyph(int(m_vString[i]));	
			if (pGlyph)
			{
				ProcessGlyph(pGlyph);
				if (m_bWithKerning)
				{
					// font kerning
					if (i < strnlen(m_vString, MAX_LENGHT) - 1)
						m_uXAdvance += pGlyph->Kerning(int(m_vString[i+1]));
				}
			}
		}

		m_uXAdvanceSum = m_uXAdvance;
		m_bDirty = false;
	}

	ncDrawableNode::Draw(rRenderQueue);
}

ncTextNode* ncTextNode::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncTextNode *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncTextNode::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncTextNode::FromId - Object not found");
		return NULL;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextNode::ProcessGlyph(const ncFontGlyph* pGlyph)
{
	ncPoint size = pGlyph->Size();
	ncPoint offset = pGlyph->Offset();
	ncVector2f pos;
	// Parent transformation
	pos.x = m_absX + m_uXAdvance*m_fScaleFactor;
	pos.y = m_absY;

	float leftPos = pos.x + offset.x*m_fScaleFactor;
	float rightPos = leftPos + size.x*m_fScaleFactor;
	float bottomPos = pos.y;
	float topPos = bottomPos + size.y*m_fScaleFactor;

	m_vVertices.InsertBack(leftPos);	m_vVertices.InsertBack(bottomPos);
	m_vVertices.InsertBack(leftPos);	m_vVertices.InsertBack(topPos);
	m_vVertices.InsertBack(rightPos);	m_vVertices.InsertBack(bottomPos);

	m_vVertices.InsertBack(rightPos);	m_vVertices.InsertBack(bottomPos);
	m_vVertices.InsertBack(rightPos);	m_vVertices.InsertBack(topPos);
	m_vVertices.InsertBack(leftPos);	m_vVertices.InsertBack(topPos);


	ncPoint texSize = m_pFont->Texture()->Size();
	ncRect texRect = pGlyph->TexRect();

	float leftCoord = float(texRect.x)/float(texSize.x);
	float rightCoord = float(texRect.x+texRect.w)/float(texSize.x);
	float bottomCoord = float(texRect.y+texRect.h)/float(texSize.y);
	float topCoord = float(texRect.y)/float(texSize.y);

	m_vTexCoords.InsertBack(leftCoord);		m_vTexCoords.InsertBack(bottomCoord);
	m_vTexCoords.InsertBack(leftCoord);		m_vTexCoords.InsertBack(topCoord);
	m_vTexCoords.InsertBack(rightCoord);	m_vTexCoords.InsertBack(bottomCoord);

	m_vTexCoords.InsertBack(rightCoord);	m_vTexCoords.InsertBack(bottomCoord);
	m_vTexCoords.InsertBack(rightCoord);	m_vTexCoords.InsertBack(topCoord);
	m_vTexCoords.InsertBack(leftCoord);		m_vTexCoords.InsertBack(topCoord);

	m_uXAdvance += pGlyph->XAdvance();
}

void ncTextNode::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pFont->Texture()->GLId());
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_renderCmd.Geometry().SetData(GL_TRIANGLES, 0, m_vVertices.Size()/2, m_vVertices.Pointer(), m_vTexCoords.Pointer(), NULL);
	m_renderCmd.CalculateSortKey();
}
