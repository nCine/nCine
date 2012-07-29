#include "ncTextNode.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextNode::ncTextNode(ncSceneNode* pParent, ncFont* pFont)
	: ncDrawableNode(pParent, 0, 0), m_bDirtyDraw(true), m_bDirtyBoundaries(true),
	  m_bWithKerning(true), m_pFont(pFont), m_vVertices(16), m_vTexCoords(16),
	  m_fXAdvance(0.0f), m_fXAdvanceSum(0.0f), m_fYAdvance(0.0f), m_fYAdvanceSum(0.0f), m_vLineLengths(4), m_alignment(ALIGN_LEFT)
{
	m_eType = TEXT_TYPE;
	SetPriority(ncDrawableNode::HUD_PRIORITY);
	m_renderCmd.SetType(ncRenderCommand::TEXT_TYPE);
	m_renderCmd.Material().SetAlwaysTransparent(true);
	memset(m_vString, 0, s_uMaxStringLength);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns rendered text width
float ncTextNode::Width() const
{
	CalculateBoundaries();

	return m_fXAdvanceSum*CurrentAbsScale();
}

/// Returns rendered text height
float ncTextNode::Height() const
{
	CalculateBoundaries();

	return m_fYAdvanceSum*CurrentAbsScale();
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
	if (strncmp(m_vString, pString, s_uMaxStringLength))
	{
		strncpy(m_vString, pString, s_uMaxStringLength);
		// Preventing unterminated string by forcing termination
		m_vString[s_uMaxStringLength-1] = '\0';
		m_bDirtyDraw = true;
		m_bDirtyBoundaries = true;
	}
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
		for (size_t i = 0; i < strlen(m_vString); i++)
		{	
			if (m_vString[i] == '\n')
			{
				uCurrentLine++;
				m_fXAdvance = CalculateAlignment(uCurrentLine);
				m_fYAdvance += m_pFont->Base();;
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
						if (i < strlen(m_vString) - 1)
							m_fXAdvance += pGlyph->Kerning(int(m_vString[i+1]));
					}
				}
			}
		}
	}

	ncDrawableNode::Draw(rRenderQueue);
	m_bDirtyDraw = false;
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
		for (size_t i = 0; i < strlen(m_vString); i++)
		{
			if (m_vString[i] == '\n')
			{
				m_vLineLengths.InsertBack(m_fXAdvance);
				if (m_fXAdvance > fXAdvanceMax)
					fXAdvanceMax = m_fXAdvance;
				m_fXAdvance = 0.0f;
				m_fYAdvance += m_pFont->Base();
			}
			else
			{
				const ncFontGlyph *pGlyph = m_pFont->Glyph(int(m_vString[i]));
				if (pGlyph)
				{
					m_fXAdvance += pGlyph->XAdvance();
					if (m_bWithKerning)
					{
						// font kerning
						if (i < strlen(m_vString) - 1)
							m_fXAdvance += pGlyph->Kerning(int(m_vString[i+1]));
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

/// Calculates absolute scale factor on the fly
float ncTextNode::CurrentAbsScale() const
{
	float fAbsScaleFactor = m_fScaleFactor;
	if (m_pParent)
		fAbsScaleFactor *= m_pParent->AbsScale();

	return fAbsScaleFactor;
}

/// Fills the batch draw command with data from a glyph
void ncTextNode::ProcessGlyph(const ncFontGlyph* pGlyph)
{
	ncPoint size = pGlyph->Size();
	ncPoint offset = pGlyph->Offset();

	float leftPos = m_fXAdvance + offset.x;
	float rightPos = leftPos + size.x;
	float topPos = -m_fYAdvance - offset.y;
	float bottomPos = topPos - size.y;

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

	m_fXAdvance += pGlyph->XAdvance();
}

void ncTextNode::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pFont->Texture()->GLId());
	m_renderCmd.Material().SetColor(m_absColor);
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_renderCmd.Geometry().SetData(GL_TRIANGLES, 0, m_vVertices.Size()/2, m_vVertices.Pointer(), m_vTexCoords.Pointer(), NULL);
	m_renderCmd.CalculateSortKey();

	if (m_bDirtyDraw)
		ApplyTransformations();
}
