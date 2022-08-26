#ifndef CLASS_NCINE_TEXTNODE
#define CLASS_NCINE_TEXTNODE

#include "DrawableNode.h"
#include "Font.h"
#include "Color.h"
#include <nctl/Array.h>
#include <nctl/String.h>

namespace ncine {

class GLUniformBlockCache;
class FontGlyph;

/// A scene node to draw a text label
class DLL_PUBLIC TextNode : public DrawableNode
{
  public:
	/// Default maximum length for a string to be rendered
	/*! This number affects both the size of the string container
	 * and the initial size of the vertex array in host memory. */
	static const unsigned int DefaultStringLength = 256;

	/// Horizontal alignment modes for text made of multiple lines
	/*! \note It does not change the node anchor point */
	enum class Alignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	TextNode();
	explicit TextNode(unsigned int maxStringLength);
	TextNode(SceneNode *parent, Font *font);
	TextNode(SceneNode *parent, Font *font, unsigned int maxStringLength);

	/// Default move constructor
	TextNode(TextNode &&) = default;
	/// Default move assignment operator
	TextNode &operator=(TextNode &&) = default;

	/// Returns a copy of this object
	inline TextNode clone() const { return TextNode(*this); }

	/// Returns the width of rendered text
	float width() const override;
	/// Returns the height of rendered text
	float height() const override;
	/// Returns the absolute width of rendered text
	float absWidth() const override;
	/// Returns the absolute height of rendered text
	float absHeight() const override;

	/// Gets the font object
	inline const Font *font() const { return font_; }
	/// Sets the font object
	void setFont(Font *font);

	/// Returns true if kerning is enabled for this node rendering
	inline bool withKerning() const { return withKerning_; }
	/// Sets the kerning flag for this node rendering
	void enableKerning(bool withKerning);
	/// Gets the horizontal text alignment of multiple lines
	inline Alignment alignment() const { return alignment_; }
	/// Sets the horizontal text alignment of multiple lines
	void setAlignment(Alignment alignment);

	/// Gets the text node line height
	inline float lineHeight() const { return lineHeight_; }
	/// Gets the text node line height multiplied by the absolute vertical scale factor
	inline float absLineHeight() const { return lineHeight_ * absScaleFactor_.y; }
	/// Sets the text node line height
	inline void setLineHeight(float lineHeight) { lineHeight_ = lineHeight; }

	/// Gets the constant string to render
	inline const nctl::String &string() const { return string_; }
	/// Gets the string to render
	inline nctl::String &string() { return string_; }
	/// Sets the string to render
	void setString(const nctl::String &string);

	/// Calculates the rectangle boundaries needed to render the provided string with the specified font
	static Vector2f calculateBoundaries(const Font &font, bool withKerning, const nctl::String &string);

	void transform() override;
	bool draw(RenderQueue &renderQueue) override;

	inline static ObjectType sType() { return ObjectType::TEXTNODE; }

  protected:
	/// Protected copy constructor used to clone objects
	TextNode(const TextNode &other);

  private:
	/// Vertex data for the glyphs
	struct Vertex
	{
		float x, y;
		float u, v;

		Vertex() {}
		Vertex(float xx, float yy, float uu, float vv)
		    : x(xx), y(yy), u(uu), v(vv) {}
	};

	/// Position of degenerate vertices in glyph quad
	enum class Degenerate
	{
		NONE,
		START,
		START_END,
		END
	};

	/// The string to be rendered
	nctl::String string_;
	/// Dirty flag for vertices and texture coordinates
	bool dirtyDraw_;
	/// Dirty flag for boundary rectangle
	mutable bool dirtyBoundaries_;
	/// Kerning flag for rendering
	bool withKerning_;
	/// The font class used to render text
	Font *font_;
	/// The array of vertex positions interleaved with texture coordinates for every glyph in the node
	nctl::Array<Vertex> interleavedVertices_;

	/// Advance on the X-axis for the next processed glyph
	mutable float xAdvance_;
	/// Advance on the Y-axis for the next processed glyph
	mutable float yAdvance_;
	/// Text width for each line of text
	mutable nctl::Array<float> lineLengths_;
	/// Horizontal text alignment of multiple lines
	Alignment alignment_;
	/// The line height for the text node
	float lineHeight_;

	GLUniformBlockCache *instanceBlock_;

	/// Deleted assignment operator
	TextNode &operator=(const TextNode &) = delete;

	/// Initializer method for constructors and the copy constructor
	void init();

	/// Calculates rectangle boundaries for the rendered text
	void calculateBoundaries() const;
	/// Calculates align offset for a particular line
	float calculateAlignment(unsigned int lineIndex) const;
	/// Fills the batch draw command with data from a glyph
	void processGlyph(const FontGlyph *glyph, Degenerate degen);

	void shaderHasChanged() override;

	void updateRenderCommand() override;
};

}

#endif
