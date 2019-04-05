#ifndef CLASS_NCINE_FONT
#define CLASS_NCINE_FONT

#include "Object.h"
#include "Vector2.h"

namespace ncine {

class FontGlyph;
class Texture;
class IFile;

/// A class holding every information needed to correctly render text
class DLL_PUBLIC Font : public Object
{
  public:
	/// Constructs the object from a texture and a `FNT` file (from <em>AngelCode's Bitmap Font Generator</em>)
	Font(const char *texFilename, const char *fntFilename);
	~Font() override;

	/// Gets the texture object
	inline const Texture *texture() const { return texture_.get(); }

	/// Returns font line height
	inline unsigned int lineHeight() const { return lineHeight_; }
	/// Returns font base
	inline unsigned int base() const { return base_; }
	/// Returns texture atlas size
	inline Vector2i textureSize() const { return Vector2i(width_, height_); }
	/// Returns number of glyphs
	inline unsigned int numGlyphs() const { return numGlyphs_; }
	/// Returns a constant pointer to a glyph
	const FontGlyph *glyph(unsigned int glyphId) const;

	inline static ObjectType sType() { return ObjectType::FONT; }

  private:
	/// The font texture
	nctl::UniquePtr<Texture> texture_;
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
	nctl::UniquePtr<FontGlyph []> glyphs_;

	/// Deleted copy constructor
	Font(const Font &) = delete;
	/// Deleted assignment operator
	Font &operator=(const Font &) = delete;

	/// Loads an <em>AngelCode's</em> `FNT` file in a memory buffer then parses it
	void parseFntFile(IFile *fileHandle);
};

}

#endif
