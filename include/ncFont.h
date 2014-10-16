#ifndef CLASS_NCFONT
#define CLASS_NCFONT

#include "ncTexture.h"
#include "ncFontGlyph.h"

/// A class holding every information needed to correctly render text
class ncFont
{
  public:
	// Constructs a font class from a texture and a FNT file (from AngelCode's Bitmap Font Generator)
	ncFont(const char* texFilename, const char *fntFilename);
	virtual ~ncFont() { delete texture_; }

	/// Gets the texture object
	inline const ncTexture* texture() const { return texture_; }
	/// Sets the texture object
	inline void setTexture(ncTexture *texture) { texture_ = texture; }

	/// Returns font line height
	inline unsigned short lineHeight() const { return lineHeight_; }
	/// Returns font base
	inline unsigned short base() const { return base_; }
	/// Returns texture atlas size
	inline ncPoint textureSize() const { return ncPoint(width_, height_); }
	/// Returns number of glyphs
	inline unsigned short numGlyphs() const { return numGlyphs_; }
	/// Reurns a constant pointer to a glyph
	const ncFontGlyph* glyph(unsigned int glyphId) const;

  private:
	/// The font texture
	ncTexture *texture_;
	/// Font line height
	unsigned int lineHeight_;
	/// Font base
	unsigned int base_;
	/// Texture atlas width
	unsigned int width_;
	/// Texture atlas height
	unsigned int height_;
	/// Number of glyphs for this font
	unsigned int numGlyphs_;
	/// Number of kernings for this font
	unsigned int numKernings_;

	/// Maximum number of glyphs in a font
	static const unsigned int MaxGlyphs = 256;
	/// Array of font glyphs
	ncFontGlyph glyphs_[MaxGlyphs];

	/// Preventing construction by copy
	ncFont(const ncFont& other);

	// Loads an AngelCode's FNT file in a RAM buffer then parses it
	void parseFntFile(ncIFile *fileHandle);
};

/// Returns a constant pointer to a glyph
inline const ncFontGlyph* ncFont::glyph(unsigned int glyphId) const
{
	if (glyphId < MaxGlyphs)
	{
		return &glyphs_[glyphId];
	}
	else
	{
		return NULL;
	}
}

#endif
