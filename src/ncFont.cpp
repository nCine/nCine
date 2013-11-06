#include <cstring>
#include "ncFont.h"
#include "ncIFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
ncFont::ncFont(const char *pTexFilename, const char *pFntFilename)
	: m_pTexture(NULL), m_uLineHeight(0), m_uBase(0), m_uWidth(0), m_uHeight(0), m_uNumGlyphs (0), m_uNumKernings(0)
{
	m_pTexture = new ncTexture(pTexFilename);

	ncIFile *pFileHandle = ncIFile::CreateFileHandle(pFntFilename);
	pFileHandle->Open(ncIFile::MODE_READ);
	ParseFNTFile(pFileHandle);
	delete pFileHandle;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads an AngelCode's FNT file in a RAM buffer then parses it
void ncFont::ParseFNTFile(ncIFile *pFileHandle)
{
	int iFileLine = 0;

	int iGlyphId;
	unsigned int uX, uY;
	unsigned int uWidth, uHeight;
	int iXOffset, iYOffset;
	unsigned int uXAdvance;
	int iSecondGlyphId;
	int iKerningAmount;

	char *pFileBuffer = new char[pFileHandle->Size()];
	pFileHandle->Read(pFileBuffer, pFileHandle->Size());

	char *pBuffer = pFileBuffer;
	do
	{
		iFileLine++;

		// skipping entirely the "info" line
		if (strncmp(pBuffer, "info", 4) == 0)
			continue;
		else if (strncmp(pBuffer, "common", 6) == 0)
		{
			sscanf(pBuffer, "common lineHeight=%u base=%u scaleW=%u scaleH=%u", &m_uLineHeight, &m_uBase, &m_uWidth, &m_uHeight);
			if ((int)m_uWidth != m_pTexture->Width() || (int)m_uHeight != m_pTexture->Height())
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncFont::ParseFNTFile - FNT texture has a different size: (%u, %u)", m_uWidth, m_uHeight);
				exit(EXIT_FAILURE);
			}
		}
		// skipping entirely the "page" line
		else if (strncmp(pBuffer, "page", 4) == 0)
			continue;
		else if (strncmp(pBuffer, "chars", 5) == 0)
			sscanf(pBuffer, "chars count=%u", &m_uNumGlyphs);
		else if (strncmp(pBuffer, "char", 4) == 0)
		{
			sscanf(pBuffer, "char id=%d x=%u y=%u width=%u height=%u xoffset=%d yoffset=%d xadvance=%u", &iGlyphId, &uX, &uY, &uWidth, &uHeight, &iXOffset, &iYOffset, &uXAdvance);
			m_vGlyphs[iGlyphId].Set(uX, uY, uWidth, uHeight, iXOffset, iYOffset, uXAdvance);
		}
		else if (strncmp(pBuffer, "kernings", 8) == 0)
			sscanf(pBuffer, "kernings count=%u", &m_uNumKernings);
		else if (strncmp(pBuffer, "kerning", 7) == 0)
		{
			sscanf(pBuffer, "kerning first=%d second=%d amount=%d ", &iGlyphId, &iSecondGlyphId, &iKerningAmount);
			m_vGlyphs[iGlyphId].AddKerning(iSecondGlyphId, iKerningAmount);
		}
	} while (strchr(pBuffer, '\n') && (pBuffer = strchr(pBuffer, '\n')+1) < pFileBuffer + pFileHandle->Size());

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncFont::ParseFNTFile - FNT file parsed: %u glyphs and %u kernings", m_uNumGlyphs, m_uNumKernings);
	delete[] pFileBuffer;
}
