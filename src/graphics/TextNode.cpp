#include "TextNode.h"
#include "FontGlyph.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "tracy.h"

namespace ncine {

const unsigned int SpaceCodePoint = 0x20;

Material::ShaderProgramType fontRenderModeToShaderProgram(const Font::RenderMode renderMode)
{
	switch (renderMode)
	{
		case Font::RenderMode::GLYPH_IN_ALPHA:
			return Material::ShaderProgramType::TEXTNODE_ALPHA;
		case Font::RenderMode::GLYPH_IN_RED:
			return Material::ShaderProgramType::TEXTNODE_RED;
		case Font::RenderMode::GLYPH_SPRITE:
			return Material::ShaderProgramType::TEXTNODE_SPRITE;
	}
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextNode::TextNode()
    : TextNode(nullptr, nullptr, DefaultStringLength)
{
}

TextNode::TextNode(unsigned int maxStringLength)
    : TextNode(nullptr, nullptr, maxStringLength)
{
}

TextNode::TextNode(SceneNode *parent, Font *font)
    : TextNode(parent, font, DefaultStringLength)
{
}

TextNode::TextNode(SceneNode *parent, Font *font, unsigned int maxStringLength)
    : DrawableNode(parent, 0.0f, 0.0f), string_(maxStringLength), dirtyDraw_(true),
      dirtyBoundaries_(true), withKerning_(true), font_(font),
      interleavedVertices_(maxStringLength * 4 + (maxStringLength - 1) * 2),
      xAdvance_(0.0f), yAdvance_(0.0f), lineLengths_(4), alignment_(Alignment::LEFT),
      lineHeight_(font ? font->lineHeight() : 0.0f), tabSize_(DefaultTabSize),
      instanceBlock_(nullptr)
{
	ASSERT(maxStringLength > 0);
	init();
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
	// Allow self-assignment to take into account the case where the font stays the same but it loads new data

	if (font_ && font)
	{
		// Keep the ratio between text node lineHeight and font one
		lineHeight_ = (lineHeight_ / font_->lineHeight()) * font->lineHeight();
	}
	else if (font_ == nullptr && font)
	{
		// Assigning a font when there wasn't any
		lineHeight_ = font->lineHeight();
	}

	font_ = font;
	if (font_)
	{
		const Material::ShaderProgramType shaderProgramType = font_
		                                                          ? fontRenderModeToShaderProgram(font_->renderMode())
		                                                          : Material::ShaderProgramType::TEXTNODE_ALPHA;
		const bool hasChanged = renderCommand_->material().setShaderProgramType(shaderProgramType);
		if (hasChanged)
			shaderHasChanged();
		if (font_->texture())
			renderCommand_->material().setTexture(*font_->texture());

		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
	else
	{
		renderCommand_->material().setTexture(nullptr);
		// Skip rendering for this node if no font is assigned
		width_ = 0;
		height_ = 0;
	}
}

Font::RenderMode TextNode::renderMode() const
{
	switch (renderCommand_->material().shaderProgramType())
	{
		default:
		case Material::ShaderProgramType::TEXTNODE_ALPHA:
			return Font::RenderMode::GLYPH_IN_ALPHA;
		case Material::ShaderProgramType::TEXTNODE_RED:
			return Font::RenderMode::GLYPH_IN_RED;
		case Material::ShaderProgramType::TEXTNODE_SPRITE:
			return Font::RenderMode::GLYPH_SPRITE;
	}
}

void TextNode::setRenderMode(Font::RenderMode renderMode)
{
	const Material::ShaderProgramType shaderProgramType = fontRenderModeToShaderProgram(renderMode);
	const bool hasChanged = renderCommand_->material().setShaderProgramType(shaderProgramType);
	if (hasChanged)
		shaderHasChanged();
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

void TextNode::setTabSize(unsigned int tabSize)
{
	if (tabSize > MaxTabSize)
		tabSize = MaxTabSize;

	if (tabSize != tabSize_)
	{
		tabSize_ = tabSize;
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

void TextNode::setString(const char *string)
{
	if (string != nullptr && string_.compare(string))
	{
		string_.assign(string);
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

Vector2f TextNode::calculateBoundaries(const Font &font, bool withKerning, unsigned int tabSize, const nctl::String &string)
{
	float xAdvanceMax = 0.0f; // longest line
	float xAdvance = 0.0f;
	float yAdvance = 0.0f;

	const float lineHeight = static_cast<float>(font.lineHeight());
	const unsigned int length = string.length();
	for (unsigned int i = 0; i < length;) // increments handled by UTF-8 decoding
	{
		if (string[i] == '\n')
		{
			if (xAdvance > xAdvanceMax)
				xAdvanceMax = xAdvance;
			xAdvance = 0.0f;
			yAdvance += lineHeight;
			i++; // manual increment as newline character is not decoded
		}
		else if (string[i] == '\t')
		{
			const FontGlyph *glyph = font.glyph(SpaceCodePoint);
			xAdvance += glyph->xAdvance() * tabSize;
			i++; // manual increment as tabular character is not decoded
		}
		else
		{
			unsigned int codepoint = nctl::Utf8::InvalidUnicode;
			const int codePointLength = string.utf8ToCodePoint(i, codepoint);
			const FontGlyph *glyph = (codepoint != nctl::Utf8::InvalidUnicode) ? font.glyph(codepoint) : nullptr;
			if (glyph)
			{
				xAdvance += glyph->xAdvance();
				if (withKerning)
				{
					// font kerning
					if (i + codePointLength < length)
					{
						unsigned int nextCodepoint = nctl::Utf8::InvalidUnicode;
						string.utf8ToCodePoint(i + codePointLength, nextCodepoint);
						xAdvance += glyph->kerning(nextCodepoint);
					}
				}
			}
			i += codePointLength; // manual increment to next codepoint
		}
	}

	// If the string does not end with a new line character,
	// last line height has not been taken into account before
	if (!string.isEmpty() && string[string.length() - 1] != '\n')
		yAdvance += lineHeight;

	if (xAdvance > xAdvanceMax)
		xAdvanceMax = xAdvance;

	return Vector2f(xAdvanceMax, yAdvance);
}

Vector2f TextNode::calculateBoundaries(const Font &font, bool withKerning, const nctl::String &string)
{
	return calculateBoundaries(font, withKerning, DefaultTabSize, string);
}

void TextNode::transform()
{
	// Precalculate boundaries for horizontal alignment
	calculateBoundaries();
	SceneNode::transform();
}

bool TextNode::draw(RenderQueue &renderQueue)
{
	// Early-out if the string is empty
	if (string_.isEmpty())
		return false;

	if (font_ && dirtyDraw_)
	{
		ZoneScoped;
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
			else if (string_[i] == '\t')
			{
				const FontGlyph *glyph = font_->glyph(SpaceCodePoint);
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
				for (unsigned int j = 0; j < tabSize_; j++)
					processGlyph(glyph, degen);
				i++; // manual increment as tabular character is not decoded
			}
			else
			{
				unsigned int codepoint = nctl::Utf8::InvalidUnicode;
				const int codePointLength = string_.utf8ToCodePoint(i, codepoint);
				const FontGlyph *glyph = (codepoint != nctl::Utf8::InvalidUnicode) ? font_->glyph(codepoint) : nullptr;
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
							unsigned int nextCodepoint = nctl::Utf8::InvalidUnicode;
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

	return DrawableNode::draw(renderQueue);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

TextNode::TextNode(const TextNode &other)
    : DrawableNode(other),
      string_(other.string_), dirtyDraw_(true), dirtyBoundaries_(true),
      withKerning_(other.withKerning_), font_(other.font_),
      interleavedVertices_(string_.capacity() * 4 + (string_.capacity() - 1) * 2),
      xAdvance_(0.0f), yAdvance_(0.0f), lineLengths_(4), alignment_(other.alignment_),
      lineHeight_(font_ ? font_->lineHeight() : 0.0f), instanceBlock_(nullptr)
{
	init();
	setBlendingEnabled(other.isBlendingEnabled());
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextNode::init()
{
	ZoneScoped;
	if (font_ && font_->texture() && font_->texture()->name() != nullptr)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(font_->texture()->name(), nctl::strnlen(font_->texture()->name(), Object::MaxNameLength));
	}

	type_ = ObjectType::TEXTNODE;
	renderCommand_->setType(RenderCommand::CommandTypes::TEXT);
	renderCommand_->material().setBlendingEnabled(true);

	const Material::ShaderProgramType shaderProgramType = font_
	                                                          ? fontRenderModeToShaderProgram(font_->renderMode())
	                                                          : Material::ShaderProgramType::TEXTNODE_ALPHA;
	renderCommand_->material().setShaderProgramType(shaderProgramType);
	shaderHasChanged();

	if (font_ && font_->texture())
		renderCommand_->material().setTexture(*font_->texture());

	renderCommand_->geometry().setPrimitiveType(GL_TRIANGLE_STRIP);
	renderCommand_->geometry().setNumElementsPerVertex(sizeof(Vertex) / sizeof(float));
}

void TextNode::calculateBoundaries() const
{
	if (font_ && dirtyBoundaries_)
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
				unsigned int codepoint = nctl::Utf8::InvalidUnicode;
				const int codePointLength = string_.utf8ToCodePoint(i, codepoint);
				const FontGlyph *glyph = (codepoint != nctl::Utf8::InvalidUnicode) ? font_->glyph(codepoint) : nullptr;
				if (glyph)
				{
					xAdvance_ += glyph->xAdvance();
					if (withKerning_)
					{
						// font kerning
						if (i + codePointLength < length)
						{
							unsigned int nextCodepoint = nctl::Utf8::InvalidUnicode;
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
		mutableNode->dirtyBits_.set(DirtyBitPositions::AabbBit);

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

	const Vector2i texSize = font_ && font_->texture() ? font_->texture()->size() : Vector2i::Zero;
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

void TextNode::shaderHasChanged()
{
	renderCommand_->material().reserveUniformsDataMemory();
	instanceBlock_ = renderCommand_->material().uniformBlock(Material::InstanceBlockName);
	GLUniformCache *textureUniform = renderCommand_->material().uniform(Material::TextureUniformName);
	if (textureUniform && textureUniform->intValue(0) != 0)
		textureUniform->setIntValue(0); // GL_TEXTURE0

	dirtyBits_.set(DirtyBitPositions::TransformationBit);
	dirtyBits_.set(DirtyBitPositions::ColorBit);

	renderCommand_->material().setDefaultAttributesParameters();
}

void TextNode::updateRenderCommand()
{
	if (dirtyBits_.test(DirtyBitPositions::TransformationUploadBit))
		renderCommand_->setTransformation(worldMatrix_);

	if (dirtyBits_.test(DirtyBitPositions::ColorUploadBit))
	{
		GLUniformCache *colorUniform = instanceBlock_->uniform(Material::ColorUniformName);
		if (colorUniform)
			colorUniform->setFloatVector(Colorf(absColor()).data());
	}
}

}
