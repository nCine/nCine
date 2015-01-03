#include "TextNode.h"
#include "FontGlyph.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextNode::TextNode(SceneNode* parent, Font* font)
	: DrawableNode(parent, 0, 0), string_(MaxStringLength), dirtyDraw_(true), dirtyBoundaries_(true),
	  withKerning_(true), font_(font), vertices_(16), texCoords_(16),
	  xAdvance_(0.0f), xAdvanceSum_(0.0f), yAdvance_(0.0f), yAdvanceSum_(0.0f), lineLengths_(4), alignment_(ALIGN_LEFT)
{
	type_ = TEXT_TYPE;
	setPriority(DrawableNode::HUD_PRIORITY);
	renderCommand_->setType(RenderCommand::TEXT_TYPE);
	renderCommand_->material().setAlwaysTransparent(true);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns rendered text width
float TextNode::width() const
{
	calculateBoundaries();

	return xAdvanceSum_ * currentAbsScale();
}

/// Returns rendered text height
float TextNode::height() const
{
	calculateBoundaries();

	return yAdvanceSum_ * currentAbsScale();
}

/// Sets the kerning flag for this node renderin
void TextNode::enableKerning(bool withKerning)
{
	if (withKerning != withKerning_)
	{
		withKerning_ = withKerning;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

// Sets the horizontal text alignment
void TextNode::setAlignment(Alignment alignment)
{
	if (alignment != alignment_)
	{
		alignment_ = alignment;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

/// Sets the string to render
void TextNode::setString(const String &string)
{
	if (string_ != string)
	{
		string_ = string;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::draw(RenderQueue& renderQueue)
{
	// Precalculate boundaries for horizontal alignment
	calculateBoundaries();

	if (dirtyDraw_)
	{
		// Clear every previous quad before drawing again
		vertices_.clear();
		texCoords_.clear();

		unsigned int currentLine = 0;
		xAdvance_ = calculateAlignment(currentLine);
		yAdvance_ = 0.0f;
		for (unsigned int i = 0; i < string_.length(); i++)
		{
			if (string_[i] == '\n')
			{
				currentLine++;
				xAdvance_ = calculateAlignment(currentLine);
				yAdvance_ += font_->base();;
			}
			else
			{
				const FontGlyph *glyph = font_->glyph(int(string_[i]));
				if (glyph)
				{
					processGlyph(glyph);
					if (withKerning_)
					{
						// font kerning
						if (i < string_.length() - 1)
						{
							xAdvance_ += glyph->kerning(int(string_[i + 1]));
						}
					}
				}
			}
		}
	}

	DrawableNode::draw(renderQueue);
	dirtyDraw_ = false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Calculates rectangle boundaries for the rendered text
void TextNode::calculateBoundaries() const
{
	if (dirtyBoundaries_)
	{
		lineLengths_.clear();
		float xAdvanceMax = 0.0f; // longest line
		xAdvance_ = 0.0f;
		yAdvance_ = 0.0f;
		for (unsigned int i = 0; i < string_.length(); i++)
		{
			if (string_[i] == '\n')
			{
				lineLengths_.insertBack(xAdvance_);
				if (xAdvance_ > xAdvanceMax)
				{
					xAdvanceMax = xAdvance_;
				}
				xAdvance_ = 0.0f;
				yAdvance_ += font_->base();
			}
			else
			{
				const FontGlyph *glyph = font_->glyph(int(string_[i]));
				if (glyph)
				{
					xAdvance_ += glyph->xAdvance();
					if (withKerning_)
					{
						// font kerning
						if (i < string_.length() - 1)
						{
							xAdvance_ += glyph->kerning(int(string_[i + 1]));
						}
					}
				}
			}
		}
		lineLengths_.insertBack(xAdvance_);
		if (xAdvance_ > xAdvanceMax)
		{
			xAdvanceMax = xAdvance_;
		}

		xAdvanceSum_ = xAdvanceMax;
		yAdvanceSum_ = yAdvance_;

		dirtyBoundaries_ = false;
	}
}

/// Calculates align offset for a particular line
float TextNode::calculateAlignment(unsigned int lineIndex) const
{
	float alignOffset = 0.0f;

	switch (alignment_)
	{
		case ALIGN_LEFT:
			alignOffset = 0.0f;
			break;
		case ALIGN_CENTER:
			alignOffset = (xAdvanceSum_ - lineLengths_[lineIndex]) * 0.5f;
			break;
		case ALIGN_RIGHT:
			alignOffset = xAdvanceSum_ - lineLengths_[lineIndex];
			break;
	}

	return alignOffset;
}

/// Calculates absolute scale factor on the fly
float TextNode::currentAbsScale() const
{
	float absScaleFactor = scaleFactor_;
	if (parent_)
	{
		absScaleFactor *= parent_->absScale();
	}

	return absScaleFactor;
}

/// Fills the batch draw command with data from a glyph
void TextNode::processGlyph(const FontGlyph* glyph)
{
	Point size = glyph->size();
	Point offset = glyph->offset();

	float leftPos = xAdvance_ + offset.x;
	float rightPos = leftPos + size.x;
	float topPos = -yAdvance_ - offset.y;
	float bottomPos = topPos - size.y;

	vertices_.insertBack(leftPos);	vertices_.insertBack(bottomPos);
	vertices_.insertBack(leftPos);	vertices_.insertBack(topPos);
	vertices_.insertBack(rightPos);	vertices_.insertBack(bottomPos);

	vertices_.insertBack(rightPos);	vertices_.insertBack(bottomPos);
	vertices_.insertBack(rightPos);	vertices_.insertBack(topPos);
	vertices_.insertBack(leftPos);	vertices_.insertBack(topPos);


	Point texSize = font_->texture()->size();
	Rect texRect = glyph->texRect();

	float leftCoord = float(texRect.x) / float(texSize.x);
	float rightCoord = float(texRect.x + texRect.w) / float(texSize.x);
	float bottomCoord = float(texRect.y + texRect.h) / float(texSize.y);
	float topCoord = float(texRect.y) / float(texSize.y);

	texCoords_.insertBack(leftCoord);		texCoords_.insertBack(bottomCoord);
	texCoords_.insertBack(leftCoord);		texCoords_.insertBack(topCoord);
	texCoords_.insertBack(rightCoord);	texCoords_.insertBack(bottomCoord);

	texCoords_.insertBack(rightCoord);	texCoords_.insertBack(bottomCoord);
	texCoords_.insertBack(rightCoord);	texCoords_.insertBack(topCoord);
	texCoords_.insertBack(leftCoord);		texCoords_.insertBack(topCoord);

	xAdvance_ += glyph->xAdvance();
}

void TextNode::updateRenderCommand()
{
	renderCommand_->material().setTextureGLId(font_->texture()->gLId());
	renderCommand_->material().setColor(absColor_);
	renderCommand_->transformation().setPosition(absPosition().x, absPosition().y);
	renderCommand_->geometry().setData(GL_TRIANGLES, 0, vertices_.size() / 2, vertices_.data(), texCoords_.data(), NULL);
	renderCommand_->calculateSortKey();

	if (dirtyDraw_)
	{
		applyTransformations();
	}
}

}
