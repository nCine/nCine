#include <cstring>
#include "ncFontGlyph.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Return the kerning amount for a subsequent glyph
int ncFontGlyph::Kerning(int iSecondGlyph) const
{
	int iKerningAmount = 0;

	for (int i = 0; i < m_vKernings.Size(); i++)
	{
		if (iSecondGlyph == m_vKernings[i].m_iSecondGlyph)
		{
			iKerningAmount = m_vKernings[i].m_iAmount;
			break;
		}
	}

	return iKerningAmount;
}
