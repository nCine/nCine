#include "ncTextNode.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextNode::ncTextNode(ncSceneNode* parent, ncFont* font)
	: ncDrawableNode(parent, 0, 0), dirtyDraw_(true), dirtyBoundaries_(true),
	  withKerning_(true), font_(font), vertices_(16), texCoords_(16),
	  xAdvance_(0.0f), xAdvanceSum_(0.0f), yAdvance_(0.0f), yAdvanceSum_(0.0f), lineLengths_(4), alignment_(ALIGN_LEFT)
{
	type_ = TEXT_TYPE;
	setPriority(ncDrawableNode::HUD_PRIORITY);
	renderCommand_.setType(ncRenderCommand::TEXT_TYPE);
	renderCommand_.material().setAlwaysTransparent(true);
	memset(string_, 0, MaxStringLength);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns rendered text width
float ncTextNode::width() const
{
	calculateBoundaries();

	return xAdvanceSum_ * currentAbsScale();
}

/// Returns rendered text height
float ncTextNode::height() const
{
	calculateBoundaries();

	return yAdvanceSum_ * currentAbsScale();
}

/// Sets the kerning flag for this node renderin
void ncTextNode::enableKerning(bool withKerning)
{
	if (withKerning != withKerning_)
	{
		withKerning_ = withKerning;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

// Sets the horizontal text alignment
void ncTextNode::setAlignment(Alignment alignment)
{
	if (alignment != alignment_)
	{
		alignment_ = alignment;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

/// Sets the string to render
void ncTextNode::setString(const char *string)
{
	// Setting the dirty flag and updating the string only if different
	if (strncmp(string_, string, MaxStringLength))
	{
		strncpy(string_, string, MaxStringLength);
		// Preventing unterminated string by forcing termination
		string_[MaxStringLength - 1] = '\0';
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void ncTextNode::draw(ncRenderQueue& renderQueue)
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
		for (size_t i = 0; i < strlen(string_); i++)
		{
			if (string_[i] == '\n')
			{
				currentLine++;
				xAdvance_ = calculateAlignment(currentLine);
				yAdvance_ += font_->base();;
			}
			else
			{
				const ncFontGlyph *glyph = font_->glyph(int(string_[i]));
				if (glyph)
				{
					processGlyph(glyph);
					if (withKerning_)
					{
						// font kerning
						if (i < strlen(string_) - 1)
						{
							xAdvance_ += glyph->kerning(int(string_[i + 1]));
						}
					}
				}
			}
		}
	}

	ncDrawableNode::draw(renderQueue);
	dirtyDraw_ = false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Calculates rectangle boundaries for the rendered text
void ncTextNode::calculateBoundaries() const
{
	if (dirtyBoundaries_)
	{
		lineLengths_.clear();
		float xAdvanceMax = 0.0f; // longest line
		xAdvance_ = 0.0f;
		yAdvance_ = 0.0f;
		for (size_t i = 0; i < strlen(string_); i++)
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
				const ncFontGlyph *glyph = font_->glyph(int(string_[i]));
				if (glyph)
				{
					xAdvance_ += glyph->xAdvance();
					if (withKerning_)
					{
						// font kerning
						if (i < strlen(string_) - 1)
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
float ncTextNode::calculateAlignment(unsigned int lineIndex) const
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
float ncTextNode::currentAbsScale() const
{
	float absScaleFactor = scaleFactor_;
	if (parent_)
	{
		absScaleFactor *= parent_->absScale();
	}

	return absScaleFactor;
}

/// Fills the batch draw command with data from a glyph
void ncTextNode::processGlyph(const ncFontGlyph* glyph)
{
	ncPoint size = glyph->size();
	ncPoint offset = glyph->offset();

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


	ncPoint texSize = font_->texture()->size();
	ncRect texRect = glyph->texRect();

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

void ncTextNode::updateRenderCommand()
{
	renderCommand_.material().setTextureGLId(font_->texture()->gLId());
	renderCommand_.material().setColor(absColor_);
	renderCommand_.transformation().setPosition(absPosition().x, absPosition().y);
	renderCommand_.geometry().setData(GL_TRIANGLES, 0, vertices_.size() / 2, vertices_.pointer(), texCoords_.pointer(), NULL);
	renderCommand_.calculateSortKey();

	if (dirtyDraw_)
	{
		applyTransformations();
	}
}
