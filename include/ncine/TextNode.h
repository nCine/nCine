#ifndef CLASS_NCINE_TEXTNODE
#define CLASS_NCINE_TEXTNODE

#include "DrawableNode.h"
#include "Font.h"
#include "Color.h"
#include <nctl/Array.h>
#include <nctl/String.h>

namespace ncine {

class FontGlyph;

class GLUniformBlockCache;

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

	TextNode(SceneNode *parent, Font *font);
	TextNode(SceneNode *parent, Font *font, unsigned int maxStringLength);

	/// Returns the width of rendered text
	float width() const override;
	/// Returns the height of rendered text
	float height() const override;
	/// Returns the absolute width of rendered text
	float absWidth() const override;
	/// Returns the absolute height of rendered text
	float absHeight() const override;

	/// Returns true if kerning is enabled for this node rendering
	inline bool withKerning() const { return withKerning_; }
	/// Sets the kerning flag for this node rendering
	void enableKerning(bool withKerning);
	/// Gets the horizontal text alignment of multiple lines
	inline Alignment alignment() const { return alignment_; }
	/// Sets the horizontal text alignment of multiple lines
	void setAlignment(Alignment alignment);

	/// Gets the font base scaled by the vertical scale factor
	inline float fontBase() const { return font_->base() * scaleFactor_.y; }
	/// Gets the font line height scaled by the vertical scale factor
	inline float fontLineHeight() const { return font_->lineHeight() * scaleFactor_.y; }

	/// Gets the string to render
	inline const nctl::String &string() const { return string_; }
	/// Sets the string to render
	void setString(const nctl::String &string);

	void transform() override;
	void draw(RenderQueue &renderQueue) override;

	inline static ObjectType sType() { return ObjectType::TEXTNODE; }

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

	GLUniformBlockCache *textnodeBlock_;

	/// Calculates rectangle boundaries for the rendered text
	void calculateBoundaries() const;
	/// Calculates align offset for a particular line
	float calculateAlignment(unsigned int lineIndex) const;
	/// Fills the batch draw command with data from a glyph
	void processGlyph(const FontGlyph *glyph, Degenerate degen);

	void updateRenderCommand() override;
};

}

#endif
