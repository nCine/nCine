#ifndef CLASS_NCINE_FONT
#define CLASS_NCINE_FONT

#include "Object.h"
#include "Vector2.h"
#include <nctl/HashMap.h>

namespace ncine {

class FntParser;
class FontGlyph;
class Texture;

/// A class holding the font and glyphs information needed to render text
class DLL_PUBLIC Font : public Object
{
  public:
	/// The available modes for text nodes rendering
	/*! \note Depending on the glyph channel a different shader will be used. */
	enum RenderMode
	{
		/// Glyph data is in the alpha channel
		GLYPH_IN_ALPHA,
		/// Glyph data is in the red channel
		GLYPH_IN_RED,
		/// Glyph data is in all four channels (glyphs are colored)
		GLYPH_SPRITE
	};

	/// Constructs an empty font object with no glyphs nor kerning pairs
	Font();

	/// Constructs the object from an AngelCode's `FNT` memory buffer and a texture
	Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename);
	/// Constructs the object from an AngelCode's `FNT` memory buffer and a texture memory buffer
	Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize,
	     const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize);
	/// Constructs the object from an AngelCode's `FNT` memory buffer and a shared texture object
	Font(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, Texture *texture);

	/// Constructs the object from an AngelCode's `FNT` file
	explicit Font(const char *fntFilename);
	/// Constructs the object from an AngelCode's `FNT` file and a texture file
	Font(const char *fntFilename, const char *texFilename);
	/// Constructs the object from an AngelCode's `FNT` file and a shared texture object
	Font(const char *fntFilename, Texture *texture);

	~Font() override;

	/// Default move constructor
	Font(Font &&);
	/// Default move assignment operator
	Font &operator=(Font &&);

	bool loadFromMemory(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, const char *texFilename);
	bool loadFromMemory(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize,
	                    const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize);
	bool loadFromMemory(const char *fntBufferName, const unsigned char *fntBufferPtr, unsigned long int fntBufferSize, Texture *texture);
	bool loadFromFile(const char *fntFilename);
	bool loadFromFile(const char *fntFilename, const char *texFilename);
	bool loadFromFile(const char *fntFilename, Texture *texture);

	/// Returns the constant texture object in use by the font
	inline const Texture *texture() const { return (texture_ != nullptr) ? texture_.get() : texturePtr_; }
	/// Returns the texture object in use by the font
	inline Texture *texture() { return (texture_ != nullptr) ? texture_.get() : texturePtr_; }
	/// Sets a new shared texture object without modifying any glyphs or kerning data
	bool setTexture(Texture *texture);

	/// Returns the font line height
	inline unsigned int lineHeight() const { return lineHeight_; }
	/// Returns the font base
	inline unsigned int base() const { return base_; }
	/// Returns the texture atlas size
	inline Vector2i textureSize() const { return Vector2i(width_, height_); }
	/// Returns the number of glyphs
	inline unsigned int numGlyphs() const { return numGlyphs_; }
	/// Returns the number of kerning pairs
	inline unsigned int numKernings() const { return numKernings_; }
	/// Returns a constant pointer to a glyph
	const FontGlyph *glyph(unsigned int glyphId) const;

	/// Returns the mode detected by the font to render text nodes
	inline RenderMode renderMode() const { return renderMode_; }

	inline static ObjectType sType() { return ObjectType::FONT; }

  private:
	/// The font texture
	nctl::UniquePtr<Texture> texture_;
	/// The pointer to the optional shared texture object
	Texture *texturePtr_;
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

	/// Maximum number of font glyphs stored in the array (single byte UTF-8 characters)
	static const unsigned int GlyphArraySize = 256;
	/// Array of font glyphs encoded in a single UTF-8 code unit
	nctl::UniquePtr<FontGlyph[]> glyphArray_;
	/// Maximum number of font glyphs stored in the hashmap (multi-byte UTF-8 characters)
	static const unsigned int GlyphHashmapSize = 1024;
	/// Hashmap of font glyphs encoded in more than one UTF-8 code unit
	nctl::HashMap<unsigned short int, FontGlyph> glyphHashMap_;

	RenderMode renderMode_;

	/// Deleted copy constructor
	Font(const Font &) = delete;
	/// Deleted assignment operator
	Font &operator=(const Font &) = delete;

	bool loadTextureFromMemory(const char *texBufferName, const unsigned char *texBufferPtr, unsigned long int texBufferSize);
	bool loadTextureFromFile(const char *texFilename);

	/// Checks whether the FNT information are compatible with rendering or not
	bool checkFntInformation(const FntParser &fntParser);

	/// Determines the render mode based on the FNT information
	void determineRenderMode(const FntParser &fntParser);
	/// Retrieves font information from the FNT parser
	void retrieveInfoFromFnt(const FntParser &fntParser);
};

}

#endif
