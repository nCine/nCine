#include "ncTextNode.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextNode::ncTextNode(ncSceneNode* pParent, ncFont* pFont)
	: ncDrawableNode(pParent, 0, 0), m_bDirtyDraw(true), m_bDirtyBoundaries(true),
	  m_bWithKerning(true), m_pFont(pFont), m_fScaleFactor(1.0f), m_vVertices(16), m_vTexCoords(16),
	  m_fXAdvance(0.0f), m_fXAdvanceSum(0.0f), m_fYAdvance(0.0f), m_fYAdvanceSum(0.0f), m_vLineLengths(4), m_alignment(ALIGN_LEFT)
{
	m_eType = TEXT_TYPE;
	SetPriority(ncDrawableNode::HUD_PRIORITY);
	memset(m_vString, 0, MAX_LENGHT);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns rendered text width
float ncTextNode::Width() const
{
	CalculateBoundaries();

	return m_fXAdvanceSum;
}

/// Returns rendered text height
float ncTextNode::Height() const
{
	CalculateBoundaries();

	return m_fYAdvanceSum;
}

/// Sets the kerning flag for this node renderin
void ncTextNode::EnableKerning(bool bWithKerning)
{
	if (bWithKerning != m_bWithKerning)
	{
		m_bWithKerning = bWithKerning;
		m_bDirtyDraw = true;
		m_bDirtyBoundaries = true;
	}
}

// Sets the horizontal text alignment
void ncTextNode::SetAlignment(eAlignment alignment)
{
	if (alignment != m_alignment)
	{
		m_alignment = alignment;
		m_bDirtyDraw = true;
		m_bDirtyBoundaries = true;
	}
}

/// Sets the string to render
void ncTextNode::SetString(const char *pString)
{
	// Setting the dirty flag and updating the string only if different
	if (strncmp(m_vString, pString, MAX_LENGHT))
	{
		strncpy(m_vString, pString, MAX_LENGHT);
		m_bDirtyDraw = true;
		m_bDirtyBoundaries = true;
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
	// Precalculate boundaries for horizontal alignment
	CalculateBoundaries();

	if (m_bDirtyDraw)
	{
		// Clear every previous quad before drawing again
		m_vVertices.Clear();
		m_vTexCoords.Clear();

		unsigned int uCurrentLine = 0;
		m_fXAdvance = CalculateAlignment(uCurrentLine);
		m_fYAdvance = 0.0f;
		for (int i = 0; i < strnlen(m_vString, MAX_LENGHT); i++)
		{	
			if (m_vString[i] == '\n')
			{
				uCurrentLine++;
				m_fXAdvance = CalculateAlignment(uCurrentLine);
				m_fYAdvance += m_pFont->Base() * m_fScaleFactor;
			}
			else
			{
				const ncFontGlyph *pGlyph = m_pFont->Glyph(int(m_vString[i]));
				if (pGlyph)
				{
					ProcessGlyph(pGlyph);
					if (m_bWithKerning)
					{
						// font kerning
						if (i < strnlen(m_vString, MAX_LENGHT) - 1)
							m_fXAdvance += pGlyph->Kerning(int(m_vString[i+1])) * m_fScaleFactor;
					}
				}
			}
		}
		m_bDirtyDraw = false;
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

/// Calculates rectangle boundaries for the rendered text
void ncTextNode::CalculateBoundaries() const
{
	if (m_bDirtyBoundaries)
	{
		m_vLineLengths.Clear();
		float fXAdvanceMax = 0.0f; // longest line
		m_fXAdvance = 0.0f;
		m_fYAdvance = 0.0f;
		for (int i = 0; i < strnlen(m_vString, MAX_LENGHT); i++)
		{
			if (m_vString[i] == '\n')
			{
				m_vLineLengths.InsertBack(m_fXAdvance);
				if (m_fXAdvance > fXAdvanceMax)
					fXAdvanceMax = m_fXAdvance;
				m_fXAdvance = 0.0f;
				m_fYAdvance += m_pFont->Base() * m_fScaleFactor;
			}
			else
			{
				const ncFontGlyph *pGlyph = m_pFont->Glyph(int(m_vString[i]));
				if (pGlyph)
				{
					m_fXAdvance += pGlyph->XAdvance() * m_fScaleFactor;
					if (m_bWithKerning)
					{
						// font kerning
						if (i < strnlen(m_vString, MAX_LENGHT) - 1)
							m_fXAdvance += pGlyph->Kerning(int(m_vString[i+1])) * m_fScaleFactor;
					}
				}
			}
		}
		m_vLineLengths.InsertBack(m_fXAdvance);
		if (m_fXAdvance > fXAdvanceMax)
			fXAdvanceMax = m_fXAdvance;

		m_fXAdvanceSum = fXAdvanceMax;
		m_fYAdvanceSum = m_fYAdvance;

		m_bDirtyBoundaries = false;
	}
}

/// Calculates align offset for a particular line
float ncTextNode::CalculateAlignment(unsigned int uLineIndex) const
{
	float fAlignOffset = 0.0f;

	switch(m_alignment)
	{
		case ALIGN_LEFT:
			fAlignOffset = 0.0f;
			break;
		case ALIGN_CENTER:
			fAlignOffset = (m_fXAdvanceSum - m_vLineLengths[uLineIndex]) * 0.5f;
			break;
		case ALIGN_RIGHT:
			fAlignOffset = m_fXAdvanceSum - m_vLineLengths[uLineIndex];
			break;
	}

	return fAlignOffset;
}

/// Fills the batch draw command with data from a glyph
void ncTextNode::ProcessGlyph(const ncFontGlyph* pGlyph)
{
	ncPoint size = pGlyph->Size();
	ncPoint offset = pGlyph->Offset();
	ncVector2f pos;
	// Parent transformation (advances are both already scaled)
	pos.x = m_absX + m_fXAdvance;
	pos.y = m_absY - m_fYAdvance;

	float leftPos = pos.x + offset.x*m_fScaleFactor;
	float rightPos = leftPos + size.x*m_fScaleFactor;
	float topPos = pos.y - offset.y*m_fScaleFactor;
	float bottomPos = topPos - size.y*m_fScaleFactor;

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

	m_fXAdvance += pGlyph->XAdvance() * m_fScaleFactor;
}

void ncTextNode::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pFont->Texture()->GLId());
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_renderCmd.Geometry().SetData(GL_TRIANGLES, 0, m_vVertices.Size()/2, m_vVertices.Pointer(), m_vTexCoords.Pointer(), NULL);
	m_renderCmd.CalculateSortKey();
}
