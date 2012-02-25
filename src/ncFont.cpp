#include <cstring>
#include "ncFont.h"
#include "ncFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
ncFont::ncFont(const char *pTexFilename, const char *pFntFilename)
	: m_pTexture(NULL)
{
	m_pTexture = new ncTexture(pTexFilename);

	ncFile fileHandle(pFntFilename);
	fileHandle.FOpen("r");
	ParseFNTFile(fileHandle.Ptr());
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Parse an AngelCode's FNT file
void ncFont::ParseFNTFile(FILE *pFile)
{
	const unsigned int uBufferLength = 256;
	char vBuffer[uBufferLength];
	int iFileLine = 0;

	int iGlyphId;
	unsigned int uX, uY;
	unsigned int uWidth, uHeight;
	int iXOffset, iYOffset;
	unsigned int uXAdvance;
	int iSecondGlyphId;
	int iKerningAmount;

	while(fgets(vBuffer, uBufferLength, pFile ) != NULL)
	{
		iFileLine++;

		// skipping entirely the "info" line
		if (strncmp(vBuffer, "info", 4) == 0)
			continue;
		else if (strncmp(vBuffer, "common", 6) == 0)
		{
			sscanf(vBuffer, "common lineHeight=%u base=%u scaleW=%u scaleH=%u", &m_uLineHeight, &m_uBase, &m_uWidth, &m_uHeight);
			if (m_uWidth != m_pTexture->Width() || m_uHeight != m_pTexture->Height())
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncFont::ParseFNTFile - FNT texture has a different size: (%u, %u)", m_uWidth, m_uHeight);
				exit(-1);
			}
		}
		// skipping entirely the "page" line
		else if (strncmp(vBuffer, "page", 4) == 0)
			continue;
		else if (strncmp(vBuffer, "chars", 5) == 0)
			sscanf(vBuffer, "chars count=%u", &m_uNumGlyphs);
		else if (strncmp(vBuffer, "char", 4) == 0)
		{
			sscanf(vBuffer, "char id=%d x=%u y=%u width=%u height=%u xoffset=%d yoffset=%d xadvance=%u", &iGlyphId, &uX, &uY, &uWidth, &uHeight, &iXOffset, &iYOffset, &uXAdvance);
			m_vGlyphs[iGlyphId].Set(uX, uY, uWidth, uHeight, iXOffset, iYOffset, uXAdvance);
		}
		else if (strncmp(vBuffer, "kernings", 8) == 0)
			sscanf(vBuffer, "kernings count=%u", &m_uNumKernings);
		else if (strncmp(vBuffer, "kerning", 7) == 0)
		{
			sscanf(vBuffer, "kerning first=%d second=%d amount=%d ", &iGlyphId, &iSecondGlyphId, &iKerningAmount);
			m_vGlyphs[iGlyphId].AddKerning(iSecondGlyphId, iKerningAmount);
		}
	}

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncFont::ParseFNTFile - FNT file parsed: %u glyphs and %u kernings", m_uNumGlyphs, m_uNumKernings);
}
