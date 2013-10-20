#ifndef CLASS_NCFONT
#define CLASS_NCFONT

#include "ncTexture.h"
#include "ncFontGlyph.h"

/// A class holding every information needed to correctly render text
class ncFont
{
private:
	/// The font texture
	ncTexture *m_pTexture;
	/// Font line height
	unsigned int m_uLineHeight;
	/// Font base
	unsigned int m_uBase;
	/// Texture atlas width
	unsigned int m_uWidth;
	/// Texture atlas height
	unsigned int m_uHeight;
	/// Number of glyphs for this font
	unsigned int m_uNumGlyphs;
	/// Number of kernings for this font
	unsigned int m_uNumKernings;

	/// Maximum number of glyphs in a font
	static const unsigned int s_uMaxGlyphs = 256;
	/// Array of font glyphs
	ncFontGlyph m_vGlyphs[s_uMaxGlyphs];

	// Loads an AngelCode's FNT file in a RAM buffer then parses it
	void ParseFNTFile(ncIFile *pFileHandle);

	/// Preventing construction by copy
	ncFont(const ncFont& rOther);
public:
	// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
	ncFont(const char* pTexFilename, const char *pFntFilename);
	virtual ~ncFont() { delete m_pTexture; }

	/// Gets the texture object
	inline const ncTexture* Texture() const { return m_pTexture; }
	/// Sets the texture object
	inline void SetTexture(ncTexture *pTexture) { m_pTexture = pTexture; }

	/// Returns font line height
	inline unsigned short LineHeight() const { return m_uLineHeight; }
	/// Returns font base
	inline unsigned short Base() const { return m_uBase; }
	/// Returns texture atlas size
	inline ncPoint TexSize() const
	{
		return ncPoint(m_uWidth, m_uHeight);
	}
	/// Returns number of glyphs
	inline unsigned short NumGlyphs() const { return m_uNumGlyphs; }
	/// Returns a constant pointer to a glyph
	inline const ncFontGlyph* Glyph(unsigned int uGlyphId) const
	{
		if (uGlyphId < s_uMaxGlyphs)
			return &m_vGlyphs[uGlyphId];
		else
			return NULL;
	}
};

#endif
