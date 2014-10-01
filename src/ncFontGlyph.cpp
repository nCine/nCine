#include <cstring>
#include "ncFontGlyph.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncFontGlyph::ncFontGlyph()
	: m_uX(0), m_uY(0), m_uWidth(0), m_uHeight(0), m_iXOffset(0),
	  m_iYOffset(0), m_uXAdvance(0), m_vKernings(4)
{

}

ncFontGlyph::ncFontGlyph(unsigned short uX, unsigned short uY, unsigned short uWidth, unsigned short uHeight,
	short iXOffset, short iYOffset, unsigned short uXAdvance)
	: m_uX(uX), m_uY(uY), m_uWidth(uWidth), m_uHeight(uHeight),
	  m_iXOffset(iXOffset), m_iYOffset(iYOffset), m_uXAdvance(uXAdvance),
	  m_vKernings(4)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Return the kerning amount for a subsequent glyph
int ncFontGlyph::Kerning(int iSecondGlyph) const
{
	int iKerningAmount = 0;

	for (unsigned int i = 0; i < m_vKernings.Size(); i++)
	{
		if (iSecondGlyph == m_vKernings[i].m_iSecondGlyph)
		{
			iKerningAmount = m_vKernings[i].m_iAmount;
			break;
		}
	}

	return iKerningAmount;
}
