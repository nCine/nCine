#include "TextNode.h"
#include "FontGlyph.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "GLDebug.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextNode::TextNode(SceneNode *parent, Font *font)
    : TextNode(parent, font, DefaultStringLength)
{
}

TextNode::TextNode(SceneNode *parent, Font *font, unsigned int maxStringLength)
    : DrawableNode(parent, 0.0f, 0.0f), string_(maxStringLength), dirtyDraw_(true),
      dirtyBoundaries_(true), withKerning_(true), font_(font),
      interleavedVertices_(maxStringLength * 4 + (maxStringLength - 1) * 2),
      xAdvance_(0.0f), yAdvance_(0.0f), lineLengths_(4), alignment_(Alignment::LEFT),
      lineHeight_(font ? font->lineHeight() : 0.0f), textnodeBlock_(nullptr)
{
	ASSERT(font);
	ASSERT(maxStringLength > 0);

	type_ = ObjectType::TEXTNODE;
	setLayer(DrawableNode::LayerBase::HUD);
	renderCommand_->setType(RenderCommand::CommandTypes::TEXT);
	renderCommand_->material().setBlendingEnabled(true);
	const Material::ShaderProgramType shaderProgramType = font_->renderMode() == Font::RenderMode::GLYPH_IN_RED
	                                                          ? Material::ShaderProgramType::TEXTNODE_RED
	                                                          : Material::ShaderProgramType::TEXTNODE_ALPHA;
	renderCommand_->material().setShaderProgramType(shaderProgramType);
	textnodeBlock_ = renderCommand_->material().uniformBlock("TextnodeBlock");
	renderCommand_->material().setTexture(*font_->texture());
	renderCommand_->geometry().setPrimitiveType(GL_TRIANGLE_STRIP);
	renderCommand_->geometry().setNumElementsPerVertex(sizeof(Vertex) / sizeof(float));
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

float TextNode::width() const
{
	calculateBoundaries();
	return width_ * scaleFactor_.x;
}

float TextNode::height() const
{
	calculateBoundaries();
	return height_ * scaleFactor_.y;
}

float TextNode::absWidth() const
{
	calculateBoundaries();
	return width_ * absScaleFactor_.x;
}

float TextNode::absHeight() const
{
	calculateBoundaries();
	return height_ * absScaleFactor_.y;
}

void TextNode::setFont(Font *font)
{
	if (font == nullptr)
		return;

	// Allow self-assignment to take into account the case where the font stays the same but it loads new data

	// Keep the ratio between text node lineHeight and font one
	lineHeight_ = (lineHeight_ / font_->lineHeight()) * font->lineHeight();

	font_ = font;
	const Material::ShaderProgramType shaderProgramType = font_->renderMode() == Font::RenderMode::GLYPH_IN_RED
	                                                          ? Material::ShaderProgramType::TEXTNODE_RED
	                                                          : Material::ShaderProgramType::TEXTNODE_ALPHA;
	renderCommand_->material().setShaderProgramType(shaderProgramType);
	renderCommand_->material().setTexture(*font_->texture());

	dirtyDraw_ = true;
	dirtyBoundaries_ = true;
}

void TextNode::enableKerning(bool withKerning)
{
	if (withKerning != withKerning_)
	{
		withKerning_ = withKerning;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::setAlignment(Alignment alignment)
{
	if (alignment != alignment_)
	{
		alignment_ = alignment;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::setString(const nctl::String &string)
{
	if (string_ != string)
	{
		string_ = string;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::transform()
{
	// Precalculate boundaries for horizontal alignment
	calculateBoundaries();
	SceneNode::transform();
}

void TextNode::draw(RenderQueue &renderQueue)
{
	// Early-out if the string is empty
	if (string_.isEmpty())
		return;

	if (dirtyDraw_)
	{
		GLDebug::ScopedGroup scoped("Processing TextNode glyphs");

		// Clear every previous quad before drawing again
		interleavedVertices_.clear();

		unsigned int currentLine = 0;
		xAdvance_ = calculateAlignment(currentLine) - width_ * 0.5f;
		yAdvance_ = 0.0f - height_ * 0.5f;
		const unsigned int length = string_.length();
		for (unsigned int i = 0; i < length;) // increments handled by UTF-8 decoding
		{
			if (string_[i] == '\n')
			{
				currentLine++;
				xAdvance_ = calculateAlignment(currentLine) - width_ * 0.5f;
				yAdvance_ += lineHeight_;
				i++; // manual increment as newline character is not decoded
			}
			else
			{
				unsigned int codepoint = nctl::String::InvalidUnicode;
				const int codePointLength = string_.utf8ToCodePoint(i, codepoint);
				const FontGlyph *glyph = (codepoint != nctl::String::InvalidUnicode) ? font_->glyph(codepoint) : nullptr;
				if (glyph)
				{
					Degenerate degen = Degenerate::NONE;
					if (length > 1)
					{
						if (i == 0)
							degen = Degenerate::END;
						else if (i == length - 1)
							degen = Degenerate::START;
						else
							degen = Degenerate::START_END;
					}
					processGlyph(glyph, degen);

					if (withKerning_)
					{
						// font kerning
						if (i + codePointLength < length)
						{
							unsigned int nextCodepoint = nctl::String::InvalidUnicode;
							string_.utf8ToCodePoint(i + codePointLength, nextCodepoint);
							xAdvance_ += glyph->kerning(nextCodepoint);
						}
					}
				}
				i += codePointLength; // manual increment to next codepoint
			}
		}

		// Vertices are updated only if the string changes
		renderCommand_->geometry().setNumVertices(interleavedVertices_.size());
		renderCommand_->geometry().setHostVertexPointer(reinterpret_cast<const float *>(interleavedVertices_.data()));
		dirtyDraw_ = false;
	}

	DrawableNode::draw(renderQueue);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextNode::calculateBoundaries() const
{
	if (dirtyBoundaries_)
	{
		ZoneScoped;
		const float oldWidth = width_;
		const float oldHeight = height_;

		lineLengths_.clear();
		float xAdvanceMax = 0.0f; // longest line
		xAdvance_ = 0.0f;
		yAdvance_ = 0.0f;
		const unsigned int length = string_.length();
		for (unsigned int i = 0; i < length;) // increments handled by UTF-8 decoding
		{
			if (string_[i] == '\n')
			{
				lineLengths_.pushBack(xAdvance_);
				if (xAdvance_ > xAdvanceMax)
					xAdvanceMax = xAdvance_;
				xAdvance_ = 0.0f;
				yAdvance_ += lineHeight_;
				i++; // manual increment as newline character is not decoded
			}
			else
			{
				unsigned int codepoint = nctl::String::InvalidUnicode;
				const int codePointLength = string_.utf8ToCodePoint(i, codepoint);
				const FontGlyph *glyph = (codepoint != nctl::String::InvalidUnicode) ? font_->glyph(codepoint) : nullptr;
				if (glyph)
				{
					xAdvance_ += glyph->xAdvance();
					if (withKerning_)
					{
						// font kerning
						if (i + codePointLength < length)
						{
							unsigned int nextCodepoint = nctl::String::InvalidUnicode;
							string_.utf8ToCodePoint(i + codePointLength, nextCodepoint);
							xAdvance_ += glyph->kerning(nextCodepoint);
						}
					}
				}
				i += codePointLength; // manual increment to next codepoint
			}
		}

		// If the string does not end with a new line character,
		// last line height has not been taken into account before
		if (!string_.isEmpty() && string_[string_.length() - 1] != '\n')
			yAdvance_ += lineHeight_;

		lineLengths_.pushBack(xAdvance_);
		if (xAdvance_ > xAdvanceMax)
			xAdvanceMax = xAdvance_;

		// Update node size and anchor points
		TextNode *mutableNode = const_cast<TextNode *>(this);
		// Total advance on the X-axis for the longest line (horizontal boundary)
		mutableNode->width_ = xAdvanceMax;
		// Total advance on the Y-axis for the entire string (vertical boundary)
		mutableNode->height_ = yAdvance_;

		if (oldWidth > 0.0f && oldHeight > 0.0f)
		{
			mutableNode->anchorPoint_.x = (anchorPoint_.x / oldWidth) * width_;
			mutableNode->anchorPoint_.y = (anchorPoint_.y / oldHeight) * height_;
		}

		dirtyBoundaries_ = false;
	}
}

float TextNode::calculateAlignment(unsigned int lineIndex) const
{
	float alignOffset = 0.0f;

	switch (alignment_)
	{
		case Alignment::LEFT:
			alignOffset = 0.0f;
			break;
		case Alignment::CENTER:
			alignOffset = (width_ - lineLengths_[lineIndex]) * 0.5f;
			break;
		case Alignment::RIGHT:
			alignOffset = width_ - lineLengths_[lineIndex];
			break;
	}

	return alignOffset;
}

void TextNode::processGlyph(const FontGlyph *glyph, Degenerate degen)
{
	const Vector2i size = glyph->size();
	const Vector2i offset = glyph->offset();

	const float leftPos = xAdvance_ + offset.x;
	const float rightPos = leftPos + size.x;
	const float topPos = -yAdvance_ - offset.y;
	const float bottomPos = topPos - size.y;

	const Vector2i texSize = font_->texture()->size();
	const Recti texRect = glyph->texRect();

	const float leftCoord = float(texRect.x) / float(texSize.x);
	const float rightCoord = float(texRect.x + texRect.w) / float(texSize.x);
	const float bottomCoord = float(texRect.y + texRect.h) / float(texSize.y);
	const float topCoord = float(texRect.y) / float(texSize.y);

	if (degen == Degenerate::START || degen == Degenerate::START_END)
		interleavedVertices_.pushBack(Vertex(leftPos, bottomPos, leftCoord, bottomCoord));

	interleavedVertices_.pushBack(Vertex(leftPos, bottomPos, leftCoord, bottomCoord));
	interleavedVertices_.pushBack(Vertex(leftPos, topPos, leftCoord, topCoord));
	interleavedVertices_.pushBack(Vertex(rightPos, bottomPos, rightCoord, bottomCoord));
	interleavedVertices_.pushBack(Vertex(rightPos, topPos, rightCoord, topCoord));

	if (degen == Degenerate::START_END || degen == Degenerate::END)
		interleavedVertices_.pushBack(Vertex(rightPos, topPos, rightCoord, topCoord));

	xAdvance_ += glyph->xAdvance();
}

void TextNode::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;
	textnodeBlock_->uniform("color")->setFloatVector(Colorf(absColor()).data());
}

}
